(   John Walker's Floating Point Benchmark, derived from...

        Marinchip Interactive Lens Design System
               John Walker   December 1980

                   by John Walker
               http://www.fourmilab.ch/

    This program may be used, distributed, and modified freely as
    long as the origin information is preserved.

    This is a complete optical design raytracing algorithm, stripped
    of its user interface and recast into ANS Forth. It not only
    determines execution speed on an extremely floating point
    [including trig function] intensive real-world application, it
    allows you to check accuracy on an algorithm that is exquisitely
    sensitive to errors. The performance of this program is
    typically far more sensitive to changes in the efficiency of the
    trigonometric library routines than the average floating point
    program.

    Implemented in October 2012 by John Walker.
)

    \   Iterations of benchmark to run (< 0: no user interface)
    ( -20749758 ) 100000 constant number_of_iterations

     \	Required for Forth, Inc. SwiftForth.  You will need to change
     \  the path in the following include to wherever fpmath.f is
     \  installed on your machine.
     \ s" ../forth_dev/swiftforth/fpmath.f" included
     \ make-round

    s" fpout.fs" included           \ Floating point output editing

    \   Wavelengths of standard spectral lines in Angstroms
    \           (Not all are used in this program)

    7621.0e0   fconstant a-line
    6869.955e0 fconstant b-line
    6562.816e0 fconstant c-line
    5895.944e0 fconstant d-line
    5269.557e0 fconstant e-line
    4861.344e0 fconstant f-line
    4340.477e0 fconstant g'-line
    3968.494e0 fconstant h-line

    \   Design array and access words

    falign

    4.0e0 fconstant design_clear_aperture
    4 constant number_of_surfaces
    create design number_of_surfaces 4 * floats allot

    : curvature_radius{n}           ( n -- addr )
        4 floats * design +
    ;

    : index_of_refraction{n}        ( n -- addr )
        4 floats * 1 floats + design +
    ;

    : dispersion{n}                 ( n -- addr )
        4 floats * 2 floats + design +
    ;

    : edge_thickness{n}             ( n -- addr )
        4 floats * 3 floats + design +
    ;

    \   Surface 0
        27.05e0   0 curvature_radius{n} f!
        1.5137e0  0 index_of_refraction{n} f!
        63.6e0    0 dispersion{n} f!
        0.52e0    0 edge_thickness{n} f!
    \   Surface 1
        -16.68e0  1 curvature_radius{n} f!
        1.0e0     1 index_of_refraction{n} f!
        0.0e0     1 dispersion{n} f!
        0.138e0   1 edge_thickness{n} f!
    \   Surface 2
        -16.68e0  2 curvature_radius{n} f!
        1.6164e0  2 index_of_refraction{n} f!
        36.7e0    2 dispersion{n} f!
        0.38e0    2 edge_thickness{n} f!
    \   Surface 3
        -78.1e0   3 curvature_radius{n} f!
        1.0e0     3 index_of_refraction{n} f!
        0.0e0     3 dispersion{n} f!
        0.0e0     3 edge_thickness{n} f!

    \   Define fcot in terms of ftan

    : fcot                          ( r1 -- r2 )
        ftan 1.0e0 fswap f/
    ;

    \      Calculate passage through surface
    \
    \      If the variable paraxial is Paraxial_Ray, the trace through
    \      the surface will be done using the paraxial approximations.
    \      Otherwise, the normal trigonometric trace will be done.
    \
    \      This subroutine takes the following global inputs:
    \
    \      radius_of_curvature     Radius of curvature of surface
    \                              being crossed.  If 0, surface is
    \                              plane.
    \
    \      object_distance         Distance of object focus from
    \                              lens vertex.  If 0, incoming
    \                              rays are parallel and
    \                              the following must be specified:
    \
    \      ray_height              Height of ray from axis.  Only
    \                              relevant if $object_distance == 0
    \
    \      axis_slope_angle        Angle incoming ray makes with axis
    \                              at intercept
    \
    \      from_index              Refractive index of medium being left
    \
    \      to_index                Refractive index of medium being
    \                              entered.
    \
    \      The outputs are the following global variables:
    \
    \      object_distance         Distance from vertex to object focus
    \                              after refraction.
    \
    \      axis_slope_angle        Angle incoming ray makes with axis
    \                              at intercept after refraction.

    fvariable aberr_lspher
    fvariable aberr_osc
    fvariable aberr_lchrom
    fvariable max_lspher
    fvariable max_osc
    fvariable max_lchrom
    fvariable radius_of_curvature
    fvariable object_distance
    fvariable ray_height
    fvariable axis_slope_angle
    fvariable from_index
    fvariable to_index

    variable paraxial

    \   Local variables in transit_surface

    fvariable iang                      \ Incidence angle
    fvariable rang                      \ Refraction angle
    fvariable iang_sin                  \ Incidence angle sin
    fvariable rang_sin                  \ Refraction angle sin
    fvariable old_axis_slope_angle      \ Incoming axis slope angle
    fvariable sagitta                   \ Sagitta (radius from axis)

    : transit_surface_paraxial          ( -- )
        radius_of_curvature f@ f0= invert if
            object_distance f@ f0= if
                axis_slope_angle 0.0e0 f!
                ray_height f@ radius_of_curvature f@ f/ iang_sin f!
            else
                object_distance f@ radius_of_curvature f@ f-
                    radius_of_curvature f@ f/ axis_slope_angle f@ f*
                    iang_sin f!
            then
            from_index f@ to_index f@ f/ iang_sin f@ f* rang_sin f!
            axis_slope_angle f@ old_axis_slope_angle f!
            axis_slope_angle f@ iang_sin f@ f+
                rang_sin f@ f- axis_slope_angle f!
            object_distance f@ f0= invert if
                object_distance f@ old_axis_slope_angle f@ f*
                    ray_height f!
            then
            ray_height f@ axis_slope_angle f@ f/ object_distance f!
        else
            object_distance f@
                to_index f@ from_index f@ f/ f* object_distance f!
            axis_slope_angle f@
                from_index f@ to_index f@ f/ f* axis_slope_angle f!
        then
    ;

    : transit_surface_marginal          ( -- )
        radius_of_curvature f@ f0= invert if
            object_distance f@ f0= if
                0.0e0 axis_slope_angle f!
                ray_height f@ radius_of_curvature f@ f/ iang_sin f!
            else
                object_distance f@ radius_of_curvature f@ f-
                    radius_of_curvature f@ f/
                    axis_slope_angle f@ fsin f* iang_sin f!
            then
            iang_sin f@ fasin iang f!
            from_index f@ to_index f@ f/ iang_sin f@ f* rang_sin f!
            axis_slope_angle f@ old_axis_slope_angle f!
            axis_slope_angle f@
                iang f@ f+ rang_sin f@ fasin f- axis_slope_angle f!
            old_axis_slope_angle f@ iang f@ f+ 0.5e0 f* fsin sagitta f!
            radius_of_curvature f@ sagitta f@ fdup f* f* 2.0e0 f*
                sagitta f!
            radius_of_curvature f@
                old_axis_slope_angle f@ iang f@ f+ fsin f*
                axis_slope_angle f@ fcot f* sagitta f@ f+
                object_distance f!
        else
            from_index f@ to_index f@ f/ fasin fnegate
                axis_slope_angle fsin f* rang f!
            object_distance f@
                to_index f@
                rang f@ fnegate fcos f*
                from_index f@ axis_slope_angle f@ fcos f* f/ f*
                object_distance f!
            rang f@ fnegate axis_slope_angle f!
        then
    ;

    : transit_surface                   ( -- )
        paraxial @ if
            transit_surface_paraxial
        else
            transit_surface_marginal
        then
    ;

    \   Perform ray trace in specific spectral line

    fvariable sline                     \ Spectral line in angstroms
    variable csurf                      \ Surface counter

    : trace_line                        ( r r -- )
        ray_height f!
        sline f!
        0.0e0 object_distance f!
        1.0e0 from_index f!

        0 csurf !
        begin
            csurf @ number_of_surfaces <
        while
            csurf @ curvature_radius{n} f@ radius_of_curvature f!
            csurf @ index_of_refraction{n} f@ to_index f!

                1.0e0 to_index f@ f< if
                to_index f@ d-line sline f@ f-
                c-line f-line f- f/
                to_index f@ 1.0e0 f- csurf @ dispersion{n} f@ f/ f* f+
                    to_index f!
            then

            transit_surface

            to_index f@ from_index f!

            csurf @ number_of_surfaces 1- < if
                object_distance f@ csurf @ edge_thickness{n} f@ f-
                    object_distance f!
            then

            1 csurf +!
       repeat
    ;

    \   evaluate_design performs ray traces on a given design with a
    \   specified clear aperture and calculates spherical aberration,
    \   coma, and chromatic aberration, along with the conventional
    \   acceptable upper bounds for these quantities.

    fvariable ed_sagitta
    fvariable ed_d_marginal_od
    fvariable ed_d_marginal_sa
    fvariable ed_d_paraxial_od
    fvariable ed_d_paraxial_sa
    fvariable ed_c_marginal_od
    fvariable ed_f_marginal_od

    : evaluate_design
        design_clear_aperture 0.5e0 f* ed_sagitta f!

        \   Marginal ray in D
        0 paraxial !
        d-line ed_sagitta f@ trace_line
        object_distance f@ ed_d_marginal_od f!
        axis_slope_angle f@ ed_d_marginal_sa f!

        \   Paraxial ray in D
        1 paraxial !
        d-line ed_sagitta f@ trace_line
        object_distance f@ ed_d_paraxial_od f!
        axis_slope_angle f@ ed_d_paraxial_sa f!

        \   Marginal ray in C
        0 paraxial !
        c-line ed_sagitta f@ trace_line
        object_distance f@ ed_c_marginal_od f!

        \   Marginal ray in F
        f-line ed_sagitta f@ trace_line
        object_distance f@ ed_f_marginal_od f!

        \   Compute aberrations of the design

        \   The longitudinal spherical aberration is just the
        \   difference between where the D line comes to focus
        \   for paraxial and marginal rays.
        ed_d_paraxial_od f@ ed_d_marginal_od f@ f- aberr_lspher f!

        \   The offense against the sine condition is a measure
        \   of the degree of coma in the design.  We compute it
        \   as the lateral distance in the focal plane between
        \   where a paraxial ray and marginal ray in the D line
        \   come to focus.
        1.0e0 ed_d_paraxial_od f@ ed_d_paraxial_sa f@ f*
              ed_d_marginal_sa f@ fsin ed_d_marginal_od f@ f* f/ f-
        aberr_osc f!

        \   The axial chromatic aberration is the distance between
        \   where marginal rays in the C and F lines come to focus.
        ed_f_marginal_od f@ ed_c_marginal_od f@ f- aberr_lchrom f!

        \   Compute maximum acceptable values for each aberration

        \   Maximum longitudinal spherical aberration, which is
        \   also the maximum for axial chromatic aberration.  This
        \   is computed for the D line.
        0.0000926e0 ed_d_marginal_sa f@ fsin fdup f* f/
            fdup max_lspher f! max_lchrom f!

        \   Maximum offense against the sine correction is a constant.
        0.0025e0 max_osc f!
    ;

    \   Print the results of the design evaluation

    : evaluation_report
        CR

        ."    Marginal ray   " ed_d_marginal_od f@ 11 21 z_F.R
            ed_d_marginal_sa f@ 11 16 z_F.R CR

        ."    Paraxial ray   " ed_d_paraxial_od f@ 11 21 z_F.R
            ed_d_paraxial_sa f@ 11 16 z_F.R CR

        ." Longitudinal spherical aberration:      "
            aberr_lspher f@ 11 16 z_F.R CR
        ."     (Maximum permissible):              "
            max_lspher f@ 11 16 z_F.R CR

        ." Offense against sine condition (coma):  "
            aberr_osc f@ 11 16 z_F.R CR
        ."     (Maximum permissible):              "
            max_osc f@ 11 16 z_F.R CR

        ." Axial chromatic aberration:             "
            aberr_lchrom f@ 11 16 z_F.R CR
        ."     (Maximum permissible):              "
            max_lchrom f@ 11 16 z_F.R CR
    ;

    number_of_iterations abs constant iterations
    variable ni
    create ibuf 80 chars allot

    : run_benchmark
        number_of_iterations 0 > if
            CR
            ." Ready to begin John Walker's floating point accuracy" CR
            ." and performance benchmark.  " iterations .
                ." iterations will be made." CR
            CR
            ." Measured run time in seconds should be divided by "
                iterations 1000 / . CR
            ." to normalise for reporting results.  For archival" CR
            ." results, adjust iteration count so the benchmark runs" CR
            ." about five minutes." CR
            CR ." Start the timer: "
            ibuf 79 accept
            CR
        then

        0 ni !
        begin
            ni @ iterations <
        while
            evaluate_design
            1 ni +!
        repeat

        number_of_iterations 0 > if
            ." Stop the timer: "
            ibuf 79 accept
        then

        evaluation_report
    ;

    \   And now, let's run the benchmark!

    run_benchmark bye
