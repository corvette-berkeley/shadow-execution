c
c    John Walker's Floating Point Benchmark
c  
c    FORTRAN 77 version
c  
c    When the test case is run, the results should
c    agree with those that follow to the last digit, or the
c    accuracy of the host system is suspect.
c  
c                               Focal Length       Angle to Axis
c   Marginal ray               47.09479120920      0.04178472683
c   Paraxial ray               47.08372160249      0.04177864821
c  
c   Longitudinal spherical aberration:       -0.01106960671
c       (Maximum permissible):                0.05306749907
c                                               Acceptable
c  
c    Offense against sine condition (coma):   0.00008954761
c       (Maximum permissible):                0.00250000000
c                                               Acceptable
c  
c   Axial chromatic aberration:               0.00448229032
c       (Maximum permissible):                0.05306749907
c                                               Acceptable
c  
c
c    This version was translated from a Microsoft BASIC version of the
c    benchmark.  It is logically equivalent to the original C version,
c    and generates the same results to the last decimal place.
c
        double precision c1, h2, o1, a2, o2, o3, b1
        double precision b2, b3, e1, e2, e3, x1, x2
        double precision c2
        integer p 
c
        double precision S3(2,2)
        integer tbfr(80)
c
        common /dat/ h2, o1, a2,
     1               x1, x2,
     2               c2,
     3               p

        data c1 / 4.0d0 /
c
c       The variable I9 specifies the number of iterations to be
c       executed.  Change it and recompile to adjust for reasonable
c       timing on your system.
c
        i9 = 10000
        i9 = 35000000
c
        print 1111, i9
1111    format ('Press return to begin ', i8, ' iterations:')
        read (5, 1000) tbfr
1000    format (80A1)
        do 102 i8 = 1, i9
        do 101 P = 1,2
c          Do main trace in D light
           call tline(4, c1 / 2)
           S3(P,1) = O1
           S3(P,2) = A2
101     continue
        P = 1
c  
c   Trace marginal ray in C
c
        call tline(3, c1 / 2)
        O2 = O1
c  
c   Trace marginal ray in F
c
        call tline(6, c1 / 2)
        O3 = O1
c  
        B1 = S3(2,1) - S3(1,1)
        B3 = 1 - (S3(2,1) * S3(2,2)) / (SIN(S3(1,2)) * S3(1,1))
        B2 = O3 - O2
        E1 = SIN(S3(1,2))
c
c   D light
c
        E1 = .0000926d0 / (E1 * E1)
        E3 = .0025d0
        E2 = E1
102     continue
        print *, 'Stop the timer:'
        read (5, 1000) tbfr
c  
c  
        print 1108
1108    format (23x, 'Focal Length          Angle to Axis')
        print 1100, S3(1,1), S3(1,2)
1100    format ('Marginal ray  ',f21.11,'  ',f21.11)
        print 1101, S3(2,1), S3(2,2)
1101    format ('Paraxial ray  ',f21.11,'  ',f21.11,/)
        print 1102, B1
1102    format ('Longitudinal spherical aberration:      ', f16.11)
        print 1103, E1
1103    format ('  (Maximum permissible):                ', f16.11)
1109    format (42x, '   Acceptable', /)
1110    format (42x, '** Excessive **', /)
        IF (ABS(B1) .le. E1) THEN 
           print 1109
        ELSE
           print 1110
        end if
c  
        print 1104, B3
1104    format ('Offense against sine condition (coma):  ', f16.11)
        print 1105, E3
1105    format ('    (Maximum permissible):              ', f16.11)
        IF (ABS(B3) .le. E3) THEN
           print 1109
        ELSE
           print 1110
        end if
        print 1106, B2
1106    format ('Axial chromatic aberration:             ', f16.11)
        print 1107, E2
1107    format ('    (Maximum permissible):              ', f16.11)
        IF (ABS(B2) .le. E2) THEN
           print 1109
        ELSE
           print 1110
        end if
c  
        END
c  
c    Perform ray trace in specific spectral line
c  
c  
        subroutine tline(z, h)
        integer z
        double precision h
c
        double precision h2, o1, a2
        double precision x1, x2
        double precision c2
        integer i1, p 
c
        double precision S1(8)
        double precision S(4, 4)
c
        common /dat/ h2, o1, a2,
     1               x1, x2,
     2               c2,
     3               p
c
c       The design being traced
c
        data i1 / 4 /
        data s /
     1        27.05d0, 1.5137d0, 63.6d0, .52d0,
     2       -16.68d0, 1d0, 0d0, .138d0,
     3       -16.68d0, 1.6164d0, 36.7d0, .38d0,
     4       -78.1d0, 1d0, 0d0, 0d0 /
c
c       Table of spectral lines
c
        data s1 / 7621d0, 6869.955d0, 6562.816d0, 5895.944d0,
     1             5269.557d0, 4861.344d0, 4340.477d0, 3968.494d0 /
c
        O1 = 0
        H2 = H
        X1 = 1
c  
        do 201 i = 1, i1
           C2 = S(1,i)
           X2 = S(2,i)
           IF (X2 .gt. 1) THEN
              X2 = X2 + ((S1(4) - S1(Z)) / (S1(3) - S1(6))) *
     1        ((S(2,i) - 1) / S(3,i))
           end if
           call tsurf
           X1 = X2
           IF (I .lt. I1) O1 = O1 - S(4,i)
201     continue
        return
        end
c  
c              Calculate passage through surface
c  
c               If the variable P is true, the trace through the
c               surface will be done using the paraxial approximations.
c               Otherwise, the normal trigonometric trace will be done.
c  
c                 v3   Incidence angle
c                 v4   Refraction angle
c                 v1   Incidence angle sin
c                 v2   Refraction angle sin
c  
        subroutine tsurf
c
        double precision h2, o1, a2
        double precision x1, x2, v1, v2, v3    
        double precision v4, c2, g1, a3
        integer p 
c
        common /dat/ h2, o1, a2,
     1               x1, x2,
     2               c2,
     3               p
c
        IF (P .ne. 1) THEN
           IF (C2 .ne. 0) THEN
              IF (O1 .eq. 0) THEN
                 A2 = 0
                 V1 = H2 / C2
              ELSE
                 V1 = ((O1 - C2) / C2) * A2
              end if
              V2 = (X1 / X2) * V1
              A3 = A2
              A2 = A2 + V1 - V2
              IF (O1 .ne. 0) H2 = O1 * A3
              O1 = H2 / A2
              RETURN
           end if
           O1 = O1 * (X2 / X1)
           A2 = A2 * (X1 / X2)
           RETURN
c
        end if
c  
        IF (C2 .ne. 0) THEN
           IF (O1 .eq. 0) THEN
              A2 = 0
              V1 = H2 / C2
           ELSE
              V1 = ((O1 - C2) / C2) * SIN(A2)
           end if
           V3 = asin(V1)
c  
           V2 = (X1 / X2) * V1
           A3 = A2
           A2 = A2 + V3 - asin(V2)
           G1 = SIN((A3 + V3) / 2)
           G1 = 2 * C2*G1*G1
           O1 = ((C2 * SIN(A3 + V3)) * (1 / TAN(A2))) + G1
           RETURN
        end if
        V4 = - asin((X1 / X2) * SIN(A2))
        O1 = O1 * ((X2 * COS(-V4)) / (X1 * COS(A2)))
        A2 = -V4
        RETURN
        END
