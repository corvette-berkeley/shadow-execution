1000 REM  John Walker's Optical Bench
1010 REM
1020 REM  Microsoft BASIC version
1030 REM
1040 REM  This must be run on a BASIC which provides double
1050 REM  precision, and in which all trig functions are done
1060 REM  in double precision.  If not, the results are utterly
1070 REM  useless.  When the test case is run, the results should
1080 REM  agree with those that follow to the last digit, or the
1090 REM  accuracy of the host system is suspect.
1100 REM
1110 REM                             Focal Length       Angle to Axis
1120 REM Marginal ray               47.09479120920      0.04178472683
1130 REM Paraxial ray               47.08372160249      0.04177864821
1140 REM
1150 REM Longitudinal spherical aberration:       -0.01106960671
1160 REM     (Maximum permissible):                0.05306749907    Acceptable
1170 REM
1180 REM  Offense against sine condition (coma):   0.00008954761
1190 REM     (Maximum permissible):                0.00250000000    Acceptable
1200 REM
1210 REM Axial chromatic aberration:               0.00448229032
1220 REM     (Maximum permissible):                0.05306749907    Acceptable
1230 REM
1240 REM  Correspondence of variable names in this program to
1250 REM  variable names in the original QBASIC program.
1260 REM
1270 REM S1#     spectral.line
1280 REM Z%      zline%
1290 REM S3#     od.sa
1300 REM D$      dname$
1310 REM I1%     icurrent.surfaces
1320 REM C1#     clear.aperture
1330 REM P%      iparaxial
1340 REM H#      ray.h
1350 REM H2#     ray.height
1360 REM O1#     object.distance
1370 REM A2#     axis.slope.angle
1380 REM O2#     od.cline
1390 REM O3#     od.fline
1400 REM B1#     aberr.lspher
1410 REM B2#     aberr.lchrom
1420 REM B3#     aberr.osc
1430 REM E1#     max.lspher
1440 REM E2#     max.lchrom
1450 REM E3#     max.osc
1460 REM X1#     from.index
1470 REM X2#     to.index
1480 REM V1#     liang.sin
1490 REM V2#     rang.sin
1500 REM V3#     liang
1510 REM V4#     rang
1520 REM C2#     radius.of.curvature
1530 REM G1#     sagitta
1540 REM A3#     old.axis.slope.angle
1550 DIM S1#(9)
1560 DIM S#(10, 5)
1570 DIM S3#(2,2)
1580 REM
1590 DEF FNA#(X#) = ATN(X# / SQR(1# - X# * X#))
1600 REM
1610 REM  Initialise when called the first time 
1620 REM
1630 S1#(1) = 7621#    :  REM A
1640 S1#(2) = 6869.955#  :  REM B
1650 S1#(3) = 6562.816#  :  REM C
1660 S1#(4) = 5895.944#  :  REM D
1670 S1#(5) = 5269.557#  :  REM E
1680 S1#(6) = 4861.344#  :  REM F
1690 S1#(7) = 4340.477#  :  REM G
1700 S1#(8) = 3968.494#  :  REM H
1710 REM
1720 READ D$
1730 READ I1%
1740 READ C1#
1750 FOR I% = 1 TO I1%
1760    READ S#(I%, 1), S#(I%, 2), S#(I%, 3), S#(I%, 4)
1770 NEXT I%
1780 REM
1790 PRINT "Surface         Radius       Ref. Index    Dispersion   Thickness"
1800 PRINT
1810 FOR I% = 1 TO I1%
1820    PRINT USING " ###      ########.#### #########.####  #######.#  #######.####"; I%, S#(I%,1), S#(I%,2), S#(I%,3), S#(I%,4)
1830 NEXT I%
1832 input "How many iterations"; i9%
1840 REM
1841 line input "Press return to begin: "; s9$
1844 for i8% = 1 to i9%
1850 FOR P% = 0 TO 1
1860    REM Do main trace in D light
1870    Z% = 4
1880    H# = C1# / 2#
1890    GOSUB 2410
1900    S3#(P%,0) = O1#
1910    S3#(P%,1) = A2#
1920 NEXT P%
1930 P% = 0
1940 REM
1950 REM Trace marginal ray in C
1960 Z% = 3
1970 H# = C1# / 2#
1980 GOSUB 2410
1990 O2# = O1#
2000 REM
2010 REM Trace marginal ray in F
2020 Z% = 6
2030 H# = C1# / 2#
2040 GOSUB 2410
2050 O3# = O1#
2060 REM
2070 B1# = S3#(1,0) - S3#(0,0)
2080 B3# = 1# - (S3#(1,0) * S3#(1,1)) / (SIN(S3#(0,1)) * S3#(0,0))
2090 B2# = O3# - O2#
2100 E1# = SIN(S3#(0,1))
2110 REM D light
2120 E1# = .0000926# / (E1# * E1#)
2130 E3# = .0025#
2140 E2# = E1#
2141 next i8%
2143 line input "Stop the timer: "; s9$
2150 REM
2160 FMT$ = "\               \     #######.########### ######.###########"
2170 REM
2180 PRINT
2190 PRINT "                            Focal Length       Angle to Axis"
2200 PRINT USING FMT$; "Marginal ray", S3#(0,0), S3#(0,1)
2210 PRINT USING FMT$; "Paraxial ray", S3#(1,0), S3#(1,1)
2220 PRINT
2230 PRINT USING "Longitudinal spherical aberration:      ######.###########"; B1#
2240 PRINT USING "    (Maximum permissible):              ######.###########    "; E1#;
2250 IF ABS(B1#) <= E1# THEN PRINT "Acceptable" ELSE PRINT "** Excessive **"
2260 PRINT
2270 REM
2280 PRINT USING "Offense against sine condition (coma):    ####.###########"; B3#
2290 PRINT USING "    (Maximum permissible):                  ##.###########    "; E3#;
2300 IF ABS(B3#) <= E3# THEN PRINT "Acceptable" ELSE PRINT "** Excessive **"
2310 PRINT
2320 REM
2330 PRINT USING "Axial chromatic aberration:             ######.###########"; B2#
2340 PRINT USING "    (Maximum permissible):              ######.###########    "; E2#;
2350 IF ABS(B2#) <= E2# THEN PRINT "Acceptable" ELSE PRINT "** Excessive **"
2360 REM
2370 END
2380 REM
2390 REM  Perform ray trace in specific spectral l
2400 REM
2410 REM trace.l(z%, h#)
2420 REM
2430         O1# = 0#
2440         H2# = H#
2450         X1# = 1#
2460 REM
2470         FOR I% = 1 TO I1%
2480            C2# = S#(I%,1)
2490            X2# = S#(I%,2)
2500            IF X2# > 1# THEN X2# = X2# + ((S1#(4) - S1#(Z%)) / (S1#(3) - S1#(6))) * ((S#(I%,2) - 1#) / S#(I%,3))
2510            GOSUB 2630
2520            X1# = X2#
2530            IF I% < I1% THEN O1# = O1# - S#(I%,4)
2540         NEXT I%
2550         RETURN
2560 REM
2570 REM            Calculate passage through surface
2580 REM
2590 REM             If the variable p% is true, the trace through the
2600 REM             surface will be done using the p% approximations.
2610 REM             Otherwise, the normal trigonometric trace will be done.
2620 REM
2630 REM transit.surface()
2640 REM               v3#   Incidence angle
2650 REM               v4#   Refraction angle
2660 REM               v1#   Incidence angle sin
2670 REM               v2#   Refraction angle sin
2680 REM
2690         IF P% = 0 THEN GOTO 2820
2700            IF C2# = 0# THEN GOTO 2780
2710               IF O1# = 0# THEN A2# = 0# : V1# = H2# / C2# ELSE V1# = ((O1# - C2#) / C2#) * A2#
2720               V2# = (X1# / X2#) * V1#
2730               A3# = A2#
2740               A2# = A2# + V1# - V2#
2750               IF O1# <> 0# THEN H2# = O1# * A3#
2760               O1# = H2# / A2#
2770               RETURN
2780            O1# = O1# * (X2# / X1#)
2790            A2# = A2# * (X1# / X2#)
2800            RETURN
2810 REM
2820         IF C2# = 0# THEN GOTO 2930
2830            IF  O1# = 0# THEN A2# = 0# : V1# = H2# / C2# ELSE V1# = ((O1# - C2#) / C2#) * SIN(A2#)
2840            V3# = FNA#(V1#)
2850 REM
2860            V2# = (X1# / X2#) * V1#
2870            A3# = A2#
2880            A2# = A2# + V3# - FNA#(V2#)
2890            G1# = SIN((A3# + V3#) / 2#)
2900            G1# = 2# * C2#*G1#*G1#
2910            O1# = ((C2# * SIN(A3# + V3#)) * (1# / TAN(A2#))) + G1#
2920            RETURN
2930         V4# = -FNA#((X1# / X2#) * SIN(A2#))
2940         O1# = O1# * ((X2# * COS(-V4#)) / (X1# * COS(A2#)))
2950         A2# = -V4#
2960         RETURN
2970 REM
2980 REM       The design is defined in the following data statements
2990 REM
3000 REM                  Design name
3010         DATA "4 inch f/12 achromatic objective"
3020 REM          Number of surfaces, clear aperture
3030         DATA 4, 4.0
3040 REM  For each surface:  Radius of curvature (+ if convex to light source)
3050 REM                     Index of refraction (1 for air space)
3060 REM                     Dispersion (Abbe number (V))
3070 REM                     Edge thickness (0 for last surface)
3080         DATA  27.05, 1.5137, 63.6, .52
3090         DATA -16.68, 1, 0, .138
3100         DATA -16.68, 1.6164, 36.7, .38
3110         DATA -78.1, 1, 0, 0
3120 END
