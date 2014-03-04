
      *   John Walker's Floating Point Benchmark, derived from...
      *
      *        Marinchip Interactive Lens Design System
      *              John Walker   December 1980
      *
      *                  by John Walker
      *              http://www.fourmilab.ch/
      *
      *   This program may be used, distributed, and modified freely as
      *   long as the origin information is preserved.
      *
      *   This is a complete optical design raytracing algorithm,
      *   stripped of its user interface and recast into COBOL.
      *   It not only determines execution speed on an extremely
      *   floating point (including trig function) intensive
      *   real-world application, it checks accuracy on an algorithm
      *   that is exquisitely sensitive to errors. The performance of
      *   this program is typically far more sensitive to changes in
      *   the efficiency of the trigonometric library routines than the
      *   average floating point program.
      *
      *        Implemented in September 2012 by John Walker.

        identification division.
        program-id.  fbench.

        environment division.
            configuration section.
                source-computer.  GNU-Linux.

        data division.

        working-storage section.

        77 confirmation picture is x(80).

        01 spectral-lines.
            10 specific-lines.
                20 a-line usage is computational-2 value is 7621.0.
                20 b-line usage is computational-2 value is 6869.955.
                20 c-line usage is computational-2 value is 6562.816.
                20 d-line usage is computational-2 value is 5895.944.
                20 e-line usage is computational-2 value is 5269.557.
                20 f-line usage is computational-2 value is 4861.344.
                20 g-prime-line usage is computational-2
                        value is 4340.477.
                20 h-line usage is computational-2 value is 3968.494.
            10 spectral-line redefines specific-lines.
                20 wavelength usage is computational-2 occurs 8 times.

        01 wyld-design.
            10 clear-aperture usage is computational-2 value is 4.
            10 number-of-surfaces picture is 99 usage is computational
                    value is 4.
            10 surface-data.
                20 surface-1.
                    30 radius usage is computational-2 value is 27.05.
                    30 index-of-refraction usage is computational-2
                            value is 1.5137.
                    30 dispersion usage is computational-2
                            value is 63.6.
                    30 edge-thickness usage is computational-2
                            value is 0.52.
                20 surface-2.
                    30 radius usage is computational-2 value is -16.68.
                    30 index-of-refraction usage is computational-2
                            value is 1.0.
                    30 dispersion usage is computational-2 value is 0.0.
                    30 edge-thickness usage is computational-2
                            value is 0.138.
                20 surface-3.
                    30 radius usage is computational-2 value is -16.68.
                    30 index-of-refraction usage is computational-2
                            value is 1.6164.
                    30 dispersion usage is computational-2
                            value is 36.7.
                    30 edge-thickness usage is computational-2
                            value is 0.38.
                20 surface-4.
                    30 radius usage is computational-2 value is -78.1.
                    30 index-of-refraction usage is computational-2
                            value is 1.0.
                    30 dispersion usage is computational-2 value is 0.0.
                    30 edge-thickness usage is computational-2
                            value is 0.0.
            10 surfaces redefines surface-data.
                20 surface occurs 4 times.
                    30 radius usage is computational-2.
                    30 index-of-refraction usage is computational-2.
                    30 dispersion usage is computational-2.
                    30 edge-thickness usage is computational-2.

        01 transit-surface-locals.
            10 iang usage is computational-2.
            10 rang usage is computational-2.
            10 iang-sin usage is computational-2.
            10 rang-sin usage is computational-2.
            10 old-axis-slope-angle usage is computational-2.
            10 sagitta usage is computational-2.

        01 ray-trace-context.
            10 paraxial picture is 9 usage is computational.
                88 marginal-ray value is 0.
                88 paraxial-ray value is 1.

            10 aberr-lspher usage is computational-2.
            10 aberr-osc usage is computational-2.
            10 aberr-lchrom usage is computational-2.

            10 max-lchrom usage is computational-2.
            10 max-lspher usage is computational-2.
            10 max-osc usage is computational-2.

            10 radius-of-curvature usage is computational-2.
            10 object-distance usage is computational-2.
            10 ray-height usage is computational-2.
            10 axis-slope-angle usage is computational-2.
            10 from-index usage is computational-2.
            10 to-index usage is computational-2.

        01 trace-line-arguments-and-locals.
            10 which-line picture is 99 usage is computational.
            10 ray-h usage is computational-2.
            10 i picture is 99 usage is computational.

        01 ray-trace-results.
            10 d-marginal-ray.
                20 objdist usage is computational-2.
                20 axisang usage is computational-2.
            10 d-paraxial-ray.
                20 objdist usage is computational-2.
                20 axisang usage is computational-2.
            10 c-marginal-ray.
                20 objdist usage is computational-2.
            10 f-marginal-ray.
                20 objdist usage is computational-2.

        01 evaluation-report.
            10 marginal-ray-report.
                20 filler picture is x(21) value is
                    "   Marginal ray      ".
                20 objdist picture is -ZZZZ9.9(11).
                20 filler picture is x(2) value is "  ".
                20 axisang picture is -9.9(11).
            10 paraxial-ray-report.
                20 filler picture is x(21) value is
                    "   Paraxial ray      ".
                20 objdist picture is -ZZZZ9.9(11).
                20 filler picture is x(2) value is "  ".
                20 axisang picture is -9.9(11).
            10 lspher-report.
                20 filler picture is x(38) value is
                    "Longitudinal spherical aberration:    ".
                20 aberr picture is -ZZZZ9.9(11).
            10 lspher-max-report.
                20 filler picture is x(38) value is
                    "    (Maximum permissible):            ".
                20 aberr picture is -ZZZZ9.9(11).
            10 osc-report.
                20 filler picture is x(38) value is
                    "Offense against sine condition (coma):".
                20 aberr picture is -ZZZZ9.9(11).
            10 osc-max-report.
                20 filler picture is x(38) value is
                    "    (Maximum permissible):            ".
                20 aberr picture is -ZZZZ9.9(11).
            10 lchrom-report.
                20 filler picture is x(38) value is
                    "Axial chromatic aberration:           ".
                20 aberr picture is -ZZZZ9.9(11).
            10 lchrom-max-report.
                20 filler picture is x(38) value is
                    "    (Maximum permissible):            ".
                20 aberr picture is -ZZZZ9.9(11).

        01 benchmark-parameters.
            10 number-of-iterations
                     picture is S9(12) usage is computational
                     value is -1000.
            10 abs-number-of-iterations
                picture is 9(10) usage is computational.
            10 display-abs-iterations picture is Z(9)9.
            10 normalisation-factor
                picture is 9(10) usage is computational.
            10 display-norm-factor picture is Z(7)9.
            10 argc picture is 9999 usage is computational.
            10 argv1 picture is S9(12) usage is computational.

         01 dump-d.
            10 d-tag picture is x(20) value is spaces.
            10 d-value picture is -ZZZZ9.9(11).

        procedure division.

        initialisation.

            move 10000 to number-of-iterations.

            compute abs-number-of-iterations =
                    function abs (number-of-iterations).
            divide abs-number-of-iterations by 1000 giving
                    normalisation-factor rounded.
            move abs-number-of-iterations to display-abs-iterations.
            move normalisation-factor to display-norm-factor.

            display "Ready to begin John Walker's floating point"
            display "accuracy and performance benchmark.  ".
            display display-abs-iterations with
                        no advancing.
            display " iterations will be made.".
            display " ".
            display "Measured run time in seconds should be divided by "
                        with no advancing.
            display display-norm-factor.
            display "to normalise for reporting results.  For archival "
            display "results, adjust iteration count so the benchmark "
            display "runs about five minutes.".
            display " ".

            if number-of-iterations is greater than zero
                display "Press return to begin benchmark: "
                            with no advancing
                accept confirmation.

            perform evaluate-design abs-number-of-iterations times.

            if number-of-iterations is greater than zero
                display "Stop the timer: " with no advancing
                accept confirmation.

            perform display-evaluation.

            stop run.

      *       Calculate passage through surface
      *
      *       If the variable paraxial is paraxial-ray, the trace
      *       through the surface will be done using the paraxial
      *       approximations.  Otherwise, the normal trigonometric
      *       trace will be done.
      *
      *       This subroutine takes the following global inputs:
      *
      *       radius-of-curvature     Radius of curvature of surface
      *                               being crossed.  If 0, surface is
      *                               plane.
      *
      *       object-distance         Distance of object focus from
      *                               lens vertex.  If 0, incoming
      *                               rays are parallel and
      *                               the following must be specified:
      *
      *       ray-height              Height of ray from axis.  Only
      *                               relevant if object-distance is 0
      *
      *       axis-slope-angle        Angle incoming ray makes with axis
      *                               at intercept
      *
      *       from-index              Refractive index of medium being
      *                               left
      *
      *       to-index                Refractive index of medium being
      *                               entered
      *
      *       The outputs are the following global variables:
      *
      *       object-distance         Distance from vertex to object
      *                               focus after refraction
      *
      *       axis-slope-angle        Angle incoming ray makes with axis
      *                               at intercept after refraction

        transit-surface.
            if paraxial-ray then
                if radius-of-curvature is not equal to zero then
                    if object-distance is equal to zero then
                        move zero to axis-slope-angle
                        divide ray-height by radius-of-curvature giving
                               iang-sin
                    else
                        compute iang-sin = ((object-distance -
                            radius-of-curvature) /
                            radius-of-curvature) *
                            axis-slope-angle
                    end-if
                    compute rang-sin = (from-index / to-index) *
                        iang-sin
                    move axis-slope-angle to old-axis-slope-angle
                    compute axis-slope-angle = axis-slope-angle +
                        iang-sin - rang-sin
                    if object-distance not equal to zero then
                        multiply object-distance by old-axis-slope-angle
                                 giving ray-height
                    end-if
                    divide ray-height by axis-slope-angle giving
                           object-distance
                    exit paragraph
                end-if
                compute object-distance = object-distance *
                    (to-index / from-index)
                compute axis-slope-angle = axis-slope-angle *
                    (from-index / to-index)
                exit paragraph.

            if radius-of-curvature is not equal to zero then
                if object-distance is equal to zero then
                    move zero to axis-slope-angle
                    divide ray-height by radius-of-curvature giving
                           iang-sin
                else
                    compute iang-sin = ((object-distance -
                        radius-of-curvature) / radius-of-curvature) *
                        function sin(axis-slope-angle)
                end-if
                compute iang = function asin(iang-sin)
                compute rang-sin = (from-index / to-index) * iang-sin
                move axis-slope-angle to old-axis-slope-angle
                compute axis-slope-angle = axis-slope-angle +
                    iang - function asin(rang-sin)
                compute sagitta = function sin((old-axis-slope-angle +
                    iang) / 2.0)
                compute sagitta = 2.0 * radius-of-curvature *
                    sagitta * sagitta
                compute object-distance = ((radius-of-curvature *
                    function sin(old-axis-slope-angle + iang)) *
                    (1 / function tan(axis-slope-angle))) + sagitta
                exit paragraph.

            compute rang = - (function asin((from-index / to-index) *
                function sin(axis-slope-angle)))
            compute object-distance = object-distance * ((to-index *
                function cos(- rang)) / (from-index *
                function cos(axis-slope-angle)))
            compute axis-slope-angle = - rang.

      *      Perform ray trace in specific spectral line
      *      Store arguments into which-line and ray-h before calling.

        trace-line.
            move zero to object-distance
            move ray-h to ray-height
            move 1 to from-index

            perform trace-one-line varying i from 1 by 1
                until i is greater than number-of-surfaces.

        trace-one-line.
            move radius of surface(i) to radius-of-curvature
            move index-of-refraction of surface(i) to to-index
            if to-index is greater than 1.0 then
                compute to-index = to-index + ((d-line -
                    (wavelength of spectral-line(which-line))) /
                    (c-line - f-line)) *
                    (((index-of-refraction of surface(i)) - 1.0) /
                    (dispersion of surface(i))).
            perform transit-surface
            move to-index to from-index
            if i is less than number-of-surfaces then
                subtract edge-thickness of surface(i) from
                         object-distance giving object-distance.

      *  Ray trace design and compute aberrations

        evaluate-design.
            compute ray-h = (clear-aperture of wyld-design) / 2

            move zero to paraxial
            move 4 to which-line
            perform trace-line
            move object-distance to objdist of d-marginal-ray
            move axis-slope-angle to axisang of d-marginal-ray

            move 1 to paraxial
            move 4 to which-line
            perform trace-line
            move object-distance to objdist of d-paraxial-ray
            move axis-slope-angle to axisang of d-paraxial-ray

            move zero to paraxial
            move 3 to which-line
            perform trace-line
            move object-distance to objdist of c-marginal-ray

            move 6 to which-line
            perform trace-line
            move object-distance to objdist of f-marginal-ray

      *     Ray tracing complete; evaluate aberrations in design

            subtract objdist of d-marginal-ray from
                        objdist of d-paraxial-ray giving aberr-lspher
            compute aberr-osc = 1.0 - ((objdist of d-paraxial-ray) *
                (axisang of d-paraxial-ray)) /
               (function sin(axisang of d-marginal-ray) *
               objdist of d-marginal-ray)
            subtract objdist of c-marginal-ray from
                        objdist of f-marginal-ray giving aberr-lchrom

      *    Compute maximum acceptable aberratiions

            compute max-lspher = function sin(axisang of d-marginal-ray)
            compute max-lspher = 0.0000926 / (max-lspher * max-lspher)
            move 0.0025 to max-osc
            move max-lspher to max-lchrom.

      *     Display design evaluation

        display-evaluation.
            add objdist of d-marginal-ray to zero
               giving objdist of marginal-ray-report rounded.
            add axisang of d-marginal-ray to zero
               giving axisang of marginal-ray-report rounded.
            display marginal-ray-report.

            add objdist of d-paraxial-ray to zero
               giving objdist of paraxial-ray-report rounded.
            add axisang of d-paraxial-ray to zero
               giving axisang of paraxial-ray-report rounded.
            display paraxial-ray-report.

            add aberr-lspher to zero giving
               aberr of lspher-report rounded.
            display lspher-report.
            add max-lspher to zero giving
               aberr of lspher-max-report rounded.
            display lspher-max-report.

            add aberr-osc to zero giving aberr of osc-report rounded.
            display osc-report.
            add max-osc to zero giving aberr of osc-max-report rounded.
            display osc-max-report.

            add aberr-lchrom to zero giving
               aberr of lchrom-report rounded.
            display lchrom-report.
            add max-lchrom to zero giving
               aberr of lchrom-max-report rounded.
            display lchrom-max-report.
