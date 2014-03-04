{-  John Walker's Floating Point Benchmark, derived from...

        Marinchip Interactive Lens Design System
               John Walker   December 1980

                   by John Walker
               http://www.fourmilab.ch/

    This program may be used, distributed, and modified freely as
    long as the origin information is preserved.

    This is a complete optical design raytracing algorithm,
    stripped of its user interface and recast into Haskell.
    It not only determines execution speed on an extremely
    floating point (including trig function) intensive
    real-world application, it checks accuracy on an algorithm
    that is exquisitely sensitive to errors. The performance of
    this program is typically far more sensitive to changes in
    the efficiency of the trigonometric library routines than the
    average floating point program.

    Implemented in September 2012 by John Walker.  -}

    module Main where

    import Text.Printf
    import Data.Time.Clock (getCurrentTime, diffUTCTime)
    import System.Environment (getArgs)
    import System.IO
    import Control.DeepSeq
    import System.Random
  --  import Debug.Trace    -- Usage: traceShow <object> $

--  The cot function defined in terms of tan

    cot :: Double -> Double
    cot x = 1 / tan x

--      Wavelengths of standard spectral lines in Angstroms
--              (Not all are used in this program)

    type Wavelength = Double

    data SpectralLine = SpectralLine {
                            waveLength:: !Wavelength
                        } deriving (Show)

    a_line, b_line, c_line, d_line, e_line,
            f_line, g'_line, h_line :: SpectralLine

    a_line  = SpectralLine 7621.0       -- A
    b_line  = SpectralLine 6869.955     -- B
    c_line  = SpectralLine 6562.816     -- C
    d_line  = SpectralLine 5895.944     -- D
    e_line  = SpectralLine 5269.557     -- E
    f_line  = SpectralLine 4861.344     -- F
    g'_line = SpectralLine 4340.477     -- G'
    h_line  = SpectralLine 3968.494     -- H

{-      The  test case used in this program is the design for a 4 inch
        f/12 achromatic telescope objective used as the example in Wyld's
        classic work on ray tracing by hand, given in Amateur Telescope
        Making, Volume 3 (Volume 2 in the 1996 reprint edition).  -}

    type Curvature_Radius = Double      -- Radius of curvature
                                        --   (+ if convex to light source)
    type Index_Of_Refraction = Double   -- Index of refraction (1 for air space)
    type Dispersion = Double            -- Dispersion (Abbe number (V))
    type Edge_Thickness = Double        -- Edge thickness (0 for last surface)

    data Surface_Property = Surface_Property {
                                curvature_Radius :: !Curvature_Radius,
                                index_Of_Refraction :: !Index_Of_Refraction,
                                dispersion :: !Dispersion,
                                edge_Thickness :: !Edge_Thickness
                            } deriving (Show)

    type LensClearAperture = Double
    type LensDesign = [Surface_Property]

    wyldClearAperture :: LensClearAperture
    wyldClearAperture = 4.0   -- Clear aperture of the following lens design
    wyldLens :: LensDesign
    wyldLens =  [   --                CurRad  Index   Disp  Edge
                    Surface_Property   27.05  1.5137  63.6  0.52 ,
                    Surface_Property (-16.68) 1.0      0.0  0.138,
                    Surface_Property (-16.68) 1.6164  36.7  0.38 ,
                    Surface_Property (-78.1)  1.0      0.0  0.0
                ]

--  The Axial_Incidence type specifies whether we're tracing a
--  marginal or paraxial ray in transit_surface.

    data Axial_Incidence = Marginal_Ray |
                           Paraxial_Ray
                           deriving (Eq, Show)

--  These type synonyms elucidate the components of the
--  Trace_Context, defined below.

    type Object_distance = Double
    type Ray_height = Double
    type Axis_slope_angle = Double

{-
        radius_of_curvature     Radius of curvature of surface
                                being crossed.  If 0, surface is
                                plane.

        object_distance         Distance of object focus from
                                lens vertex.  If 0, incoming
                                rays are parallel and
                                the following must be specified:

        ray_height              Height of ray from axis.  Only
                                relevant if object_distance == 0

        axis_slope_angle        Angle incoming ray makes with axis
                                at intercept

        from_index              Refractive index of medium being left


        to_index                Refractive index of medium being
                                entered.

        The result of a call on transit_surface is a Trace_Context
        with the following components modified to reflect the geometry
        of the ray as it exits the surface.

        object_distance         Distance from vertex to object focus
                                after refraction.

        ray_height              Height of ray from axis.

        axis_slope_angle        Angle incoming ray makes with axis
                                at intercept after refraction.
-}

    data Trace_Context = Trace_Context {
                                axial_incidence :: !Axial_Incidence,
                                radius_of_curvature :: !Curvature_Radius,
                                object_distance :: !Object_distance,
                                ray_height :: !Ray_height,
                                axis_slope_angle :: !Axis_slope_angle,
                                from_index :: !Index_Of_Refraction,
                                to_index :: !Index_Of_Refraction
                            } deriving (Show)

    transit_surface :: Trace_Context -> Trace_Context

    --  Paraxial ray with flat radius of curvature
    transit_surface (Trace_Context Paraxial_Ray 0.0 object_distance_s
        ray_height_s axis_slope_angle_s from_index_s to_index_s) =
            Trace_Context Paraxial_Ray 0.0
                (object_distance_s * (to_index_s / from_index_s))
                ray_height_s
                (axis_slope_angle_s * (from_index_s / to_index_s))
                from_index_s to_index_s

    --  Paraxial ray with curved surface
    transit_surface (Trace_Context Paraxial_Ray radius_of_curvature_s
        object_distance_s ray_height_s axis_slope_angle_s
        from_index_s to_index_s) =
        let axis_slope_angle' = if odz then 0 
                                       else axis_slope_angle_s
            iang_sin = if odz then ray_height_s / radius_of_curvature_s
                              else ((object_distance_s -
                                    radius_of_curvature_s) /
                                    radius_of_curvature_s) *
                                    axis_slope_angle_s
            rang_sin = (from_index_s / to_index_s) * iang_sin
            axis_slope_angle'' = axis_slope_angle' + iang_sin - rang_sin
            ray_height' = if odz then ray_height_s
                                 else object_distance_s * axis_slope_angle'
            object_distance' = ray_height' / axis_slope_angle''
        in
            Trace_Context Paraxial_Ray
                radius_of_curvature_s
                object_distance'
                ray_height'
                axis_slope_angle''
                from_index_s to_index_s
        where odz = object_distance_s == 0

    --  Marginal ray with flat radius of curvature
    transit_surface (Trace_Context Marginal_Ray 0.0 object_distance_s
        ray_height_s axis_slope_angle_s from_index_s to_index_s) =
        let rang = (-(asin((from_index_s / to_index_s))) *
                    sin(axis_slope_angle_s))
        in
            Trace_Context Marginal_Ray 0.0
                (object_distance_s * ((to_index_s *
                    cos((-rang))) / (from_index_s *
                    cos(axis_slope_angle_s))))
                ray_height_s
                (-rang)
                from_index_s to_index_s

    --  Marginal ray with curved surface
    transit_surface (Trace_Context Marginal_Ray radius_of_curvature_s
        object_distance_s ray_height_s axis_slope_angle_s
        from_index_s to_index_s) =
        let axis_slope_angle' = if odz then 0 
                                       else axis_slope_angle_s
            iang_sin = if odz then ray_height_s / radius_of_curvature_s
                              else ((object_distance_s -
                                    radius_of_curvature_s) /
                                    radius_of_curvature_s) *
                                    sin(axis_slope_angle_s)
            iang = asin(iang_sin)
            rang_sin = (from_index_s / to_index_s) * iang_sin
            axis_slope_angle'' = axis_slope_angle' + iang - asin(rang_sin)
            sagitta = 2 * radius_of_curvature_s *
                      sin((axis_slope_angle' + iang) / 2) ^ (2 :: Int)
            ray_height' = if odz then ray_height_s
                                 else object_distance_s * axis_slope_angle'
            object_distance' = ((radius_of_curvature_s *
                                sin(axis_slope_angle' + iang)) *
                                cot(axis_slope_angle'')) + sagitta
        in
            Trace_Context Marginal_Ray
                radius_of_curvature_s
                object_distance'
                ray_height'
                axis_slope_angle''
                from_index_s to_index_s
        where odz = object_distance_s == 0

{-      Perform ray trace for a given design for a specific
        spectral line and ray height.  The caller passes in the
        design and desired spectral line, along with a Trace_Context
        initialised with the required axial_incidence,
        radius_of_curvature (0), object_distance (0),
        ray_height, axis_slope_angle (0), and from_index (1).
        We walk through the design list, applying transit_surface
        for each surface encountered, and return a Trace_Context
        whose object_distance and axis_slope_angle contain the
        results of the ray trace.  -}

    trace_line :: LensDesign ->  SpectralLine -> Trace_Context -> Trace_Context

    trace_line [] _ context = context

    trace_line (surface:design) spectral_line context =
        let
            to_index' = if (index_Of_Refraction surface) > 1 then
                                (index_Of_Refraction surface) +
                                (((waveLength d_line) -
                                  (waveLength spectral_line)) /
                                    ((waveLength c_line) -
                                     (waveLength f_line))) *
                                (((index_Of_Refraction surface) - 1) /
                                    (dispersion surface))
                        else
                            (index_Of_Refraction surface)

            context' = transit_surface (Trace_Context
                                    (axial_incidence context)   -- axial_incidence
                                    (curvature_Radius surface)  -- radius_of_curvature
                                    (object_distance context)   -- object_distance
                                    (ray_height context)        -- ray_height
                                    (axis_slope_angle context)  -- axis_slope_angle
                                    (from_index context)        -- from_index
                                    to_index')                  -- to_index
        in
        trace_line design spectral_line (Trace_Context
                                    (axial_incidence context')      -- axial_incidence
                                    (radius_of_curvature context')  -- radius_of_curvature
                                    (object_distance context' -
                                        (edge_Thickness surface))   -- object_distance
                                    (ray_height context')           -- ray_height
                                    (axis_slope_angle context')     -- axis_slope_angle
                                    (to_index context')             -- from_index
                                    0)                              -- to_index

{-      The trace_lens function is a little bit of syntactic sugar
        to simplify invoking trace_line.  It creates an initial
        Trace_Context for trace_line which specifies the axial_incidence
        and ray_height (computed from the clear aperture of the
        design), invokes trace_line with the supplied design and
        spectral line, then returns a tuple containing the
        object_distance and axis_slope_angle resulting from the ray
        trace.  -}

    data RayResult = RayResult {
                        ray_object_distance :: !Object_distance,  -- object_distance
                        ray_axis_slope_angle :: !Axis_slope_angle -- axis_slope_angle
                     } deriving (Show)

    instance NFData RayResult where
        rnf (RayResult a b) = rnf a `seq` rnf b

    trace_lens :: LensDesign ->  LensClearAperture -> SpectralLine ->
                  Axial_Incidence -> RayResult

    trace_lens design clear_aperture spectral_line axial_incidence_s =
        let context = trace_line design spectral_line
                        (Trace_Context
                            axial_incidence_s       -- axial_incidence
                            0                       -- radius_of_curvature
                            0                       -- object_distance
                            (clear_aperture / 2)    -- ray_height
                            0                       -- axis_slope_angle
                            1                       -- from_index
                            0)                      -- to_index
        in
        RayResult (object_distance context) (axis_slope_angle context)
 
{-  The evaluate_design function performs a ray trace on a given design
    with a specified clear aperture and returns a DesignEvaluation
    which includes the results for the D line and calculation of
    spherical aberration, coma, and chromatic aberration, along with
    the conventional acceptable upper bounds for these quantities.  -}

    data DesignEvaluation = DesignEvaluation {
                            -- Results from ray trace
                            marginal_ray :: !RayResult,
                            paraxial_ray :: !RayResult,
                            longitudinal_spherical_aberration :: !Double,
                            offense_against_sine_condition :: !Double,
                            axial_chromatic_aberration :: !Double,

                            -- Acceptable maxima for aberrations
                            max_longitudinal_spherical_aberration :: !Double,
                            max_offense_against_sine_condition :: !Double,
                            max_axial_chromatic_aberration :: !Double
                        } deriving (Show)

    instance NFData DesignEvaluation where
        rnf (DesignEvaluation a b c d e f g h) =
            rnf a `seq` rnf b `seq` rnf c `seq` rnf d `seq`
            rnf e `seq` rnf f `seq` rnf g `seq` rnf h

    evaluate_design :: LensDesign ->  LensClearAperture -> DesignEvaluation

    evaluate_design design clear_aperture =
        let
                -- Trace the rays upon which we base the evaluation
                d_marginal = trace_lens design clear_aperture d_line Marginal_Ray
                d_paraxial = trace_lens design clear_aperture d_line Paraxial_Ray

                c_marginal = trace_lens design clear_aperture c_line Marginal_Ray

                f_marginal = trace_lens design clear_aperture f_line Marginal_Ray

                -- Compute aberrations of the design
                
                -- The longitudinal spherical aberration is just the
                -- difference between where the D line comes to focus
                -- for paraxial and marginal rays.
                aberr_lspher = (ray_object_distance d_paraxial) -
                                    (ray_object_distance d_marginal)

                -- The offense against the sine condition is a measure
                -- of the degree of coma in the design.  We compute it
                -- as the lateral distance in the focal plane between
                -- where a paraxial ray and marginal ray in the D line
                -- come to focus.
                aberr_osc = 1 - ((ray_object_distance d_paraxial) *
                                    (ray_axis_slope_angle d_paraxial)) /
                                (sin(ray_axis_slope_angle d_marginal) *
                                    (ray_object_distance d_marginal))

                -- The axial chromatic aberration is the distance between
                -- where marginal rays in the C and F lines come to focus.
                aberr_lchrom = (ray_object_distance f_marginal) -
                               (ray_object_distance c_marginal)

                -- Compute maximum acceptable values for each aberration

                -- Maximum longitudinal spherical aberration, which is
                -- also the maximum for axial chromatic aberration.  This
                -- is computed for the D line.
                max_lspher = 0.0000926 /
                    (sin(ray_axis_slope_angle d_marginal) ^ (2 :: Int))
        in
        DesignEvaluation d_marginal         -- marginal_ray
                         d_paraxial         -- paraxial_ray
                         aberr_lspher       -- longitudinal_spherical_aberration
                         aberr_osc          -- offense_against_sine_condition
                         aberr_lchrom       -- axial_chromatic_aberration
                         max_lspher         -- max_longitudinal_spherical_aberration
                         0.0025             -- max_offense_against_sine_condition
                         max_lspher         -- max_axial_chromatic_aberration
                                            --   (NOT an error: see above)

{-  The evaluation_report function takes a DesignEvaluation and returns a
    list of strings containing a primate-readable version of the evaluation.
    For the purposes of the benchmark, this function is not timed; it serves
    only to create textual results which can be compared against those from
    the reference implementation.  -}

    evaluation_report :: DesignEvaluation -> [String]

    evaluation_report de =
        [
            (printf "%15s   %21.11f  %14.11f" "Marginal ray"
                (ray_object_distance (marginal_ray de))
                (ray_axis_slope_angle (marginal_ray de))),

            (printf "%15s   %21.11f  %14.11f" "Paraxial ray"
                (ray_object_distance (paraxial_ray de))
                (ray_axis_slope_angle (paraxial_ray de))),

            (printf "Longitudinal spherical aberration:      %16.11f"
                (longitudinal_spherical_aberration de)),
            (printf max_permissible
                (max_longitudinal_spherical_aberration de)),

            (printf "Offense against sine condition (coma):  %16.11f"
                (offense_against_sine_condition de)),
            (printf max_permissible
                (max_offense_against_sine_condition de)),

            (printf "Axial chromatic aberration:             %16.11f"
                (axial_chromatic_aberration de)),
            (printf max_permissible
                (max_axial_chromatic_aberration de))            
        ]
        where
            max_permissible = "    (Maximum permissible):              %16.11f"

{-  The validate_results function compares a primate-readable
    report from evaluation_report with the archival results from
    the reference implementation (which all language implementations
    must reproduce character-by-character [apart from trivia such as
    end of line conventions and trailing white space]).  It returns
    a Bool indicating whether the results compared and a list of Bools
    in which False indicates a miscompare for that line of the results.
    This function is not timed in the benchmark.  -}

    {- Reference results.  These happen to be derived from a run
       on Microsoft Quick BASIC on the IBM PC/AT.  -}
    results_expected :: [String]
    results_expected =
        [
            "   Marginal ray          47.09479120920   0.04178472683",
            "   Paraxial ray          47.08372160249   0.04177864821",
            "Longitudinal spherical aberration:        -0.01106960671",
            "    (Maximum permissible):                 0.05306749907",
            "Offense against sine condition (coma):     0.00008954761",
            "    (Maximum permissible):                 0.00250000000",
            "Axial chromatic aberration:                0.00448229032",
            "    (Maximum permissible):                 0.05306749907"
        ]

    validate_results :: [String] -> [String] -> (Bool, [Bool])

    validate_results expected received =
        let
            matches = zipWith (==) expected received

            exact = and matches
        in
            (exact, matches)

{-  The report_validation_results function reports the results of
    validate_results to a user via standard output.  -}
    report_validation_results :: (Bool, [Bool]) -> [String] -> [String] -> String

    report_validation_results (True, _) _ _ =
        "No errors in results."
    
    report_validation_results (False, matches) expected received =
        let
            num_errors = length (filter not matches)
        in
        (printf "%d error%s in results.  This is VERY SERIOUS.\n"
                    num_errors (if num_errors == 1 then "" else "s")) ++
            (list_discrepancies matches expected received "" 0)
        where
        --  Edit a discrepancy report and return a string containing it.
        list_discrepancies :: [Bool] -> [String] -> [String] ->
                              String -> Int -> String

        list_discrepancies (m:matched) (e:expecteds) (r:receiveds) s n =
            let  d | not m = ("Error in results on line " ++ (show n) ++
                            "...\n    Expected: \"" ++ e ++
                            "\"\n    Received: \"" ++ r ++ "\"\n")
                   | otherwise = ""
            in
            list_discrepancies matched expecteds receiveds (s ++ d) (n + 1)

        list_discrepancies [] _ _ s _ =
            s

        list_discrepancies _ _ _ _ _ = ""   -- Can't happen; avoid compiler warning

{-  The run_benchmark function executes the ray trace and evaluation
    for one iteration, making sure that all values are actually strictly
    calculated.  To avoid having all of the successive iterations of
    the benchmark be optimised out by lazy evaluation, we are passed a
    seeded pseudorandom generator from which we obtain successive
    Doubles R uniformly distributed within 0 <= R < 1.  These are
    added to the clear aperture of the design for each successive
    iteration until the last, making each trace unique.  On the last
    trace we use the proper clear aperture to produce the results
    we'll verify for accuracy.  Since the ray tracing algorithms run
    in constant time regardless of the value of the clear aperture
    (or for that matter, the surfaces), this randomisation does not
    affect the run time we measure.  -}

    run_benchmark :: Int -> StdGen -> DesignEvaluation-> IO (DesignEvaluation)
    
    run_benchmark n gen dev = do
        if n == 0 then do
            return (dev)
         else do
            let nrand = random gen :: (Double, StdGen)
            let de = evaluate_design wyldLens (wyldClearAperture +
                     (if n == 1 then 0 else (fst nrand)))
            _ <- de `deepseq` return (0 :: Int)
            run_benchmark (n - 1) (snd nrand) de

{-  The time_benchmark function runs the ray trace and evaluation
    of the reference design the number of times given by the argument,
    invoking the run_benchmark function to actually do the work.
    It returns the DesignEvaluation from the last ray trace, and the
    elapsed time in seconds (as returned by getCurrentTime) as a tuple.
    We pass a random generator seed to run_benchmark so that it can
    randomise successive traces to prevent them from being optimised
    out by lazy evaluation.  Lazy evaluation is usually wonderful,
    but it is the bane of benchmark writers.  -}

    seed :: StdGen
    seed = (mkStdGen 1234)

    time_benchmark :: Int -> IO (DesignEvaluation, Double)
            
    time_benchmark n = do
        start <- getCurrentTime
        de <- run_benchmark n seed (DesignEvaluation
                                        (RayResult 0 0)
                                        (RayResult 0 0)
                                        0 0 0 0 0 0)
        end <- getCurrentTime
        return (de, (realToFrac (diffUTCTime end start) :: Double))

{-                          Main program

    When run from the command line, the optional first argument
    specifies the number of iterations to perform.  If this
    number is negative, the prompts to start the benchmark
    and stop the timer will be omitted, and the benchmark run
    in batch mode.  -}
    
    main :: IO ()

    main = do
        args <- getArgs
        --  Gimmick: if the iteration is count is negative,
        --  suppress user interaction, allowing batch timing
        --  runs.
        let iterations | null args = -1000
                       | otherwise = read (head args)
        let absiter = abs iterations
        let interactive = iterations > 0
        let norm = (realToFrac absiter :: Double) / 1000
        
        putStr
            ("Ready to begin John Walker's floating point accuracy\n\
            \and performance benchmark.  " ++
             (show absiter) ++ " iterations will be made.\n\
            \\n\
            \Measured run time in seconds should be divided by " ++
            (printf "%.0f" norm) ++ "\n\
            \to normalise for reporting results.  For archival results,\n\
            \adjust iteration count so the benchmark runs about five minutes.\n\
            \\n")

        if interactive
            then
                putStr "Press return to begin benchmark: "
           else
                return ()
            
        hFlush stdout
        _ <- if interactive then getLine else return ""
        
        bresult <- time_benchmark absiter

        if interactive
            then
                putStr "Stop the timer: \a"
           else
                return ()
   
        hFlush stdout
        _ <- if interactive then getLine else return ""
    
        putStrLn (printf "Measured elapsed time: %.4f seconds." (snd bresult))

        let report = evaluation_report (fst bresult)
        let validation = validate_results results_expected report
        let vresults = report_validation_results validation results_expected report

        putStrLn vresults
        
