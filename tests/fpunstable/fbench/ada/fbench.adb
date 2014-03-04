--  John Walker's Floating Point Benchmark, derived from...
--
--  Marinchip Interactive Lens Design System
--
--  By John Walker
--     http://www.fourmilab.ch/
--
--  This program may be used, distributed, and modified freely as
--  long as the origin information is preserved.
--
--  This is a complete optical design raytracing algorithm,
--  stripped of its user interface and recast into Ada 95.
--  It not only determines execution speed on an extremely
--  floating point (including trig function) intensive
--  real-world application, it checks accuracy on an algorithm
--  that is exquisitely sensitive to errors. The performance of
--  this program is typically far more sensitive to changes in
--  the efficiency of the trigonometric library routines than the
--  average floating point program.
--
--  Ported from the C language implementation in August 2005
--  by John Walker.

    with Ada.Text_IO, Ada.Numerics.Long_Elementary_Functions,
         Ada.Integer_Text_IO, Ada.Long_Float_Text_IO, Ada.Command_Line;

    use Ada.Text_IO, Ada.Numerics.Long_Elementary_Functions,
         Ada.Integer_Text_IO, Ada.Long_Float_Text_IO, Ada.Command_Line;

    procedure Fbench is

        max_surfaces : constant Integer := 10;

--      Wavelengths of standard spectral lines in Angstroms
--              (Not all are used in this program)

        type Spectral_Line_Name is (
            A_line, B_line, C_line, D_line, E_line, F_line,
            Gprime_line, H_line
        );

        spectral_line : constant array (Spectral_Line_Name) of Long_Float := (
            7621.0,         -- A
            6869.955,       -- B
            6562.816,       -- C
            5895.944,       -- D
            5269.557,       -- E
            4861.344,       -- F
            4340.477,       -- G'
            3968.494        -- H
        );

        niter : Positive := 1000;   -- Number of iterations to perform

--      The  test case used in this program is the design for a 4 inch
--      f/12 achromatic telescope objective used as the example in Wyld's
--      classic work on ray tracing by hand, given in Amateur Telescope
--      Making, Volume 3 (Volume 2 in the 1996 reprint edition).

        type Surface_Property is (
            Curvature_Radius,       -- Radius of curvature
                                    --   (+ if convex to light source)
            Index_Of_Refraction,    -- Index of refraction (1 for air space)
            Dispersion,             -- Dispersion (Abbe number (V))
            Edge_Thickness          -- Edge thickness (0 for last surface)
        );

        testcase : constant array (1.. 4, Surface_Property) of Long_Float := (
            (  27.05, 1.5137, 63.6, 0.52  ),
            ( -16.68, 1.0,     0.0, 0.138 ),
            ( -16.68, 1.6164, 36.7, 0.38  ),
            ( -78.1,  1.0,     0.0, 0.0   )
        );

        s : array(0 .. max_surfaces - 1, Surface_Property) of Long_Float;

        subtype Results_Row is Positive range 1 .. 8;
        subtype Results_Column is Positive range 1 .. 56;

--      Reference results.  These happen to be derived from a
--      run on Microsoft Quick BASIC on the IBM PC/AT.

        Reference_Results : constant array (Results_Row) of String (Results_Column) := (
            "   Marginal ray          47.09479120920   0.04178472683 ",
            "   Paraxial ray          47.08372160249   0.04177864821 ",
            "Longitudinal spherical aberration:        -0.01106960671",
            "    (Maximum permissible):                 0.05306749907",
            "Offense against sine condition (coma):     0.00008954761",
            "    (Maximum permissible):                 0.00250000000",
            "Axial chromatic aberration:                0.00448229032",
            "    (Maximum permissible):                 0.05306749907"
        );

        Calculated_Results : array (Results_Row) of String (Results_Column) := (
            "   Marginal ray          ______________   _____________ ",
            "   Paraxial ray          ______________   _____________ ",
            "Longitudinal spherical aberration:        ______________",
            "    (Maximum permissible):                ______________",
            "Offense against sine condition (coma):    ______________",
            "    (Maximum permissible):                ______________",
            "Axial chromatic aberration:               ______________",
            "    (Maximum permissible):                ______________"
        );

        current_surfaces : Positive;

        type Axial_Incidence is ( Marginal_Ray, Paraxial_Ray );
        paraxial : Axial_Incidence;

        clear_aperture, aberr_lspher, aberr_osc, aberr_lchrom,
        max_lspher, max_osc, max_lchrom, radius_of_curvature,
        object_distance, ray_height, axis_slope_angle,
        from_index, to_index : Long_Float;

        od_sa : array(Axial_Incidence, 0 .. 1) of Long_Float;

--      Calculate passage through surface
--
--      If the variable paraxial is Paraxial_Ray, the trace through the
--      surface will be done using the paraxial approximations.
--      Otherwise, the normal trigonometric trace will be done.
--
--      This subroutine takes the following global inputs:
--
--      radius_of_curvature     Radius of curvature of surface
--                              being crossed.  If 0, surface is
--                              plane.
--
--      object_distance         Distance of object focus from
--                              lens vertex.  If 0, incoming
--                              rays are parallel and
--                              the following must be specified:
--
--      ray_height              Height of ray from axis.  Only
--                              relevant if $object_distance == 0
--
--      axis_slope_angle        Angle incoming ray makes with axis
--                              at intercept
--
--      from_index              Refractive index of medium being left
--
--      to_index                Refractive index of medium being
--                              entered.
--
--      The outputs are the following global variables:
--
--      object_distance         Distance from vertex to object focus
--                              after refraction.
--
--      axis_slope_angle        Angle incoming ray makes with axis
--                              at intercept after refraction.

        procedure transit_surface is
            iang,                   -- Incidence angle
            rang,                   -- Refraction angle
            iang_sin,               -- Incidence angle sin
            rang_sin,               -- Refraction angle sin
            old_axis_slope_angle, sagitta : Long_Float;
        begin
            if paraxial = Paraxial_Ray then
                if radius_of_curvature /= 0.0 then
                    if object_distance = 0.0  then
                        axis_slope_angle := 0.0;
                        iang_sin := ray_height / radius_of_curvature;
                    else
                        iang_sin := ((object_distance -
                        radius_of_curvature) / radius_of_curvature) *
                        axis_slope_angle;
                    end if;
                    rang_sin := (from_index / to_index) * iang_sin;
                    old_axis_slope_angle := axis_slope_angle;
                    axis_slope_angle := axis_slope_angle +
                        iang_sin - rang_sin;
                    if object_distance /= 0.0 then
                        ray_height := object_distance * old_axis_slope_angle;
                    end if;
                    object_distance := ray_height / axis_slope_angle;
                    return;
                end if;
                object_distance := object_distance * (to_index / from_index);
                axis_slope_angle := axis_slope_angle * (from_index / to_index);
                return;
            end if;

            if radius_of_curvature /= 0.0 then
                if object_distance = 0.0 then
                    axis_slope_angle := 0.0;
                    iang_sin := ray_height / radius_of_curvature;
                else
                    iang_sin := ((object_distance -
                        radius_of_curvature) / radius_of_curvature) *
                        Sin(axis_slope_angle);
                end if;
                iang := Arcsin(iang_sin);
                rang_sin := (from_index / to_index) * iang_sin;
                old_axis_slope_angle := axis_slope_angle;
                axis_slope_angle := axis_slope_angle +
                    iang - Arcsin(rang_sin);
                sagitta := Sin((old_axis_slope_angle + iang) / 2.0);
                sagitta := 2.0 * radius_of_curvature * sagitta * sagitta;
                object_distance := ((radius_of_curvature *
                    Sin(old_axis_slope_angle + iang)) *
                    Cot(axis_slope_angle)) + sagitta;
                return;
            end if;

            rang := -Arcsin((from_index / to_index) *
                Sin(axis_slope_angle));
            object_distance := object_distance * ((to_index *
                Cos(-rang)) / (from_index *
                Cos(axis_slope_angle)));
            axis_slope_angle := -rang;
        end transit_surface;

--      Perform ray trace in specific spectral line

        procedure trace_line(line : in Spectral_Line_Name; ray_h : in Long_Float) is
        begin
            object_distance := 0.0;
            ray_height := ray_h;
            from_index := 1.0;

            for i in 1 .. current_surfaces loop
                radius_of_curvature := s(i, Curvature_Radius);
                to_index := s(i, Index_Of_Refraction);
                if to_index > 1.0 then
                    to_index := to_index + ((spectral_line(D_line) -
                        spectral_line(line)) /
                        (spectral_line(C_line) - spectral_line(F_line))) *
                        ((s(i, Index_Of_Refraction) - 1.0) / s(i, Dispersion));
                end if;
                transit_surface;
                from_index := to_index;
                if i < current_surfaces then
                    object_distance := object_distance - s(i, Edge_Thickness);
                end if;
            end loop;
        end trace_line;

        od_fline, od_cline : Long_Float;
        junk : Integer;
        Text_Buffer : String(1 .. 132);
        Errors : Integer := 0;

--      Edit number into field in Calculated_Results array

        procedure enum(num : in Long_Float;
                       row : in Positive; column : in Positive) is
        begin
            Put(Calculated_Results(row)(column .. column + 14), num,
                Aft => 11, Exp => 0);
        end enum;

     begin

--      Take iteration count from command line, if specified

        if Argument_Count > 0 then
            Get(Argument(1), niter, junk);
        end if;

--      Load test case into working array

        clear_aperture := 4.0;
        current_surfaces := 4;
        for i in 1 .. current_surfaces loop
            for j in Surface_Property loop
                s(i, j) := testcase(i, j);
            end loop;
        end loop;

        Put("Press return to begin benchmark: ");
        Flush;
        Get_Line(Text_Buffer, junk);

--      Timing begins here

        for itercount in 1 .. niter loop
            for p in Axial_Incidence loop

                -- Do main trace in D light

                paraxial := p;
                trace_line(D_line, clear_aperture / 2.0);
                od_sa(paraxial, 0) := object_distance;
                od_sa(paraxial, 1) := axis_slope_angle;
            end loop;
            paraxial := Marginal_Ray;

--          Trace marginal ray in C

            trace_line(C_line, clear_aperture / 2.0);
            od_cline := object_distance;

--          Trace marginal ray in F

            trace_line(F_line, clear_aperture / 2.0);
            od_fline := object_distance;

            aberr_lspher := od_sa(Paraxial_Ray, 0) - od_sa(Marginal_Ray, 0);
            aberr_osc := 1.0 - (od_sa(Paraxial_Ray, 0) * od_sa(Paraxial_Ray, 1)) /
               (Sin(od_sa(Marginal_Ray, 1)) * od_sa(Marginal_Ray, 0));
            aberr_lchrom := od_fline - od_cline;
            max_lspher := Sin(od_sa(Marginal_Ray, 1));

--          D light

            max_lspher := 0.0000926 / (max_lspher * max_lspher);
            max_osc := 0.0025;
            max_lchrom := max_lspher;

        end loop;

--      Timing ends here

        Put("Stop the timer: ");
        Flush;
        Get_Line(Text_Buffer, junk);

--      Edit results into Calculated_Results array

        enum(od_sa(Marginal_Ray, 0), 1, 25);
        enum(od_sa(Marginal_Ray, 1), 1, 41);

        enum(od_sa(Paraxial_Ray, 0), 2, 25);
        enum(od_sa(Paraxial_Ray, 1), 2, 41);

        enum(aberr_lspher, 3, 42);
        enum(max_lspher, 4, 42);

        enum(aberr_osc, 5, 42);
        enum(max_osc, 6, 42);

        enum(aberr_lchrom, 7, 42);
        enum(max_lchrom, 8, 42);

--      Check for errors and report any we find

        for i in Results_Row loop
            if Calculated_Results(i) /= Reference_Results(i) then
                Errors := Errors + 1;
                New_Line;
                Put("Error in results on line ");
                Put(i, Width => 1);
                Put_Line("...");

                Put("Expected:  ");
                Put_Line(Calculated_Results(i));

                Put("Received:  ");
                Put_Line(Reference_Results(i));

                Put("(Errors)   ");
                for j in Results_Column loop
                    if Calculated_Results(i)(j) /=
                       Reference_Results(i)(j) then
                        Put("^");
                    else
                        Put(" ");
                    end if;
                end loop;
                New_Line;
            end if;
        end loop;

--      Print error summary

        New_Line;
        if Errors = 0 then
            Put_Line("No errors in results.");
        else
            Put(Errors, Width => 1);
            Put(" error");
            if Errors > 1 then
                Put("s");
            end if;
            Put_Line(" in results.  This is VERY SERIOUS.");
        end if;

     end Fbench;
