/*===============================================================*/
/*                                                               */
/* fbench.java             John Walker's 'fbench' - Java version */
/*                                                               */
/*               Jim White                                       */
/*               u3280625@anu.edu.au                             */
/*               mathimagics@yahoo.co.uk                         */
/*               July 2005                                       */
/*                                                               */
/*===============================================================*/
/* 
/*  This program does exactly the same calculations as fbench.c, */
/*  so it allows you to measure the performance of the Java      */
/*  Runtime "virtual machine" relative to that of native-mode    */
/*  programs when performing similar work (ie, a computationally */
/*  intensive iterative procedure using simple data types).      */
/*                                                               */
/*  The program uses standard Java primitives and classes, and   */
/*  should require no modification. The number of iterations can */
/*  be given on the command line, otherwise the default value of */
/*  10,000 is used.                                              */
/*                                                               */
/*        e.g:    java fbench 5000                               */
/*                                                               */
/*  The program reports elapsed time, and the corresponding time */
/*  inferred for 1000 iterations, which is the standard unit     */
/*  used in the results table given at John's fbench web page.   */
/*                                                               */
/*===============================================================*/
/*  Note:  refer to fbench.c for John's original comments        */
/*         describing the benchmark algorithm and the nature of  */
/*         the task being performed.                             */
/*===============================================================*/

import java.math.*;
import java.util.Date;

public class fbench {

   static final int    INTRIG = 1;
   static final int    max_surfaces = 10;
   static final double PI = 3.1415926535897932;
   static final double twopi      = PI * 2.0;
   static final double piover4    = PI / 4.0;
   static final double fouroverpi = 4.0 / PI;
   static final double piover2    = PI / 2.0;

   static int  current_surfaces = 4;
   static short paraxial;

   static double clear_aperture = 4.0;
   static double aberr_lspher;
   static double aberr_osc;
   static double aberr_lchrom;
   static double max_lspher;
   static double max_osc;
   static double max_lchrom;

   static double radius_of_curvature;
   static double object_distance;
   static double ray_height;
   static double axis_slope_angle;
   static double from_index;
   static double to_index;

   static double spectral_line[] = new double[9];
   static double s[][]           = new double[max_surfaces][5];
   static double testcase[][]={
         {27.05, 1.5137, 63.6, 0.52},
         {-16.68, 1, 0, 0.138},
         {-16.68, 1.6164, 36.7, 0.38},
         {-78.1, 1, 0, 0}
         };
  
   static double od_sa[][]        = new double[2][2];
   
   static int itercount;    
   static final int ITERATIONS = 10000;
   static int niter = ITERATIONS;    

   /* Reference results.  

        "   Marginal ray          47.09479120920   0.04178472683",
        "   Paraxial ray          47.08372160249   0.04177864821",
        "Longitudinal spherical aberration:        -0.01106960671",
        "    (Maximum permissible):                 0.05306749907",
        "Offense against sine condition (coma):     0.00008954761",
        "    (Maximum permissible):                 0.00250000000",
        "Axial chromatic aberration:                0.00448229032",
        "    (Maximum permissible):                 0.05306749907" 
        */

   public static double cot(double x) {
      return(1.0 / Math.tan(x));
      }
   
   public static void Initialise() {
      
      spectral_line[1] = 7621.0;       // A 
      spectral_line[2] = 6869.955;     // B 
      spectral_line[3] = 6562.816;     // C 
      spectral_line[4] = 5895.944;     // D 
      spectral_line[5] = 5269.557;     // E 
      spectral_line[6] = 4861.344;     // F 
      spectral_line[7] = 4340.477;     // G
      spectral_line[8] = 3968.494;     // H
  
      /* Load test case into working array */
      int i, j;
     
      for (i = 0; i < current_surfaces; i++)
         for (j = 0; j < 4; j++)
            {
            s[i + 1][j + 1] = testcase[i][j];
            }
         }

   public static void trace_line(int line, double ray_h) {
      int i;

      object_distance = 0.0;
      ray_height = ray_h;
      from_index = 1.0;

      for (i = 1; i <= current_surfaces; i++) {
         radius_of_curvature = s[i][1];
         to_index = s[i][2];
         if (to_index > 1.0)
          to_index = to_index + ((spectral_line[4] - spectral_line[line]) / (spectral_line[3] - spectral_line[6])) * ((s[i][2] - 1.0) /   s[i][3]);
         transit_surface();
         from_index = to_index;
         if (i < current_surfaces)
            object_distance = object_distance - s[i][4];
         }
      }



   public static void transit_surface() {
      double iang,     /* Incidence angle */
             rang,     /* Refraction angle */
             iang_sin,    /* Incidence angle sin */
             rang_sin,    /* Refraction angle sin */
             old_axis_slope_angle, sagitta;

      if (paraxial != 0) {
         if (radius_of_curvature != 0.0) {
            if (object_distance == 0.0) {
               axis_slope_angle = 0.0;
               iang_sin = ray_height / radius_of_curvature;
               } 
            else
               iang_sin = ((object_distance -      radius_of_curvature) / radius_of_curvature) *         axis_slope_angle;

            rang_sin = (from_index / to_index) * iang_sin;
            old_axis_slope_angle = axis_slope_angle;
            axis_slope_angle = axis_slope_angle +  iang_sin - rang_sin;
            if (object_distance != 0.0) ray_height = object_distance * old_axis_slope_angle;
            object_distance = ray_height / axis_slope_angle;
            return;
            }
         object_distance = object_distance * (to_index / from_index);
         axis_slope_angle = axis_slope_angle * (from_index / to_index);
         return;
         }

      if (radius_of_curvature != 0.0) {
         if (object_distance == 0.0) {
            axis_slope_angle = 0.0;
            iang_sin = ray_height / radius_of_curvature;
            }
         else {
            iang_sin = ((object_distance -  radius_of_curvature) / radius_of_curvature) * Math.sin(axis_slope_angle);
            }
         iang = Math.asin(iang_sin);
         rang_sin = (from_index / to_index) *   iang_sin;
         old_axis_slope_angle = axis_slope_angle;
         axis_slope_angle = axis_slope_angle +       iang - Math.asin(rang_sin);
         sagitta = Math.sin((old_axis_slope_angle + iang) / 2.0);
         sagitta = 2.0 * radius_of_curvature*sagitta*sagitta;
         object_distance = ((radius_of_curvature * Math.sin(old_axis_slope_angle + iang)) * cot(axis_slope_angle)) + sagitta;
         return;
         }

      rang = -Math.asin((from_index / to_index) * Math.sin(axis_slope_angle));
      object_distance = object_distance * ((to_index * Math.cos(-rang)) / (from_index *  Math.cos(axis_slope_angle)));
      axis_slope_angle = -rang;
      }


   public static void main(String[] args) {

      Date    Now;
      long    time;
      double  od_cline, od_fline;
      boolean GotParam = false;


      /* check command line for iteration count parameter */

      try                     {if (args[0].length() > 0) GotParam = true;}
      catch (ArrayIndexOutOfBoundsException exception) {GotParam = false;}
      if (GotParam) niter = Integer.parseInt(args[0]);

      Initialise();

      System.out.printf("\nReady to begin John Walker's floating point accuracy\n");
      System.out.printf("and performance benchmark.  %d iterations will be made.\n\n", niter);

      System.out.printf("\nBegin ... ");

      Now = new Date(); time = Now.getTime();

      for (itercount = 0; itercount < niter; itercount++) {
         for (paraxial = 0; paraxial <= 1; paraxial++) {

         /* Do main trace in D light */

            trace_line(4, clear_aperture / 2.0);
            od_sa[paraxial][0] = object_distance;
            od_sa[paraxial][1] = axis_slope_angle;
            }
         paraxial = 0;

         /* Trace marginal ray in C */

         trace_line(3, clear_aperture / 2.0);
         od_cline = object_distance;

         /* Trace marginal ray in F */

         trace_line(6, clear_aperture / 2.0);
         od_fline = object_distance;

         aberr_lspher = od_sa[1][0] - od_sa[0][0];
         aberr_osc = 1.0 - (od_sa[1][0] * od_sa[1][1]) / (Math.sin(od_sa[0][1]) * od_sa[0][0]);
         aberr_lchrom = od_fline - od_cline;
         max_lspher = Math.sin(od_sa[0][1]);

         /* D light */

         max_lspher = 0.0000926 / (max_lspher * max_lspher);
         max_osc = 0.0025;
         max_lchrom = max_lspher;
         }

      Now = new Date(); time = Now.getTime() - time;
      System.out.printf("\nFinished, elapsed =  %d msecs\n", time);
       /* Now evaluate the accuracy of the results from the last ray trace */

      String z = new String("");
      System.out.printf("\n\nResults\n\n");
      System.out.printf("Marginal ray      %21.11f  %14.11f \n", od_sa[0][0], od_sa[0][1]);
      System.out.printf("Paraxial ray      %21.11f  %14.11f \n", od_sa[1][0], od_sa[1][1]);
      System.out.printf("Longitudinal spherical aberration:      %16.11f \n", aberr_lspher);
      System.out.printf("    (Maximum permissible):              %16.11f \n", max_lspher);
      System.out.printf("Offense against sine condition (coma):  %16.11f \n", aberr_osc);
      System.out.printf("    (Maximum permissible):              %16.11f \n", max_osc);
      System.out.printf("Axial chromatic aberration:             %16.11f \n", aberr_lchrom);
      System.out.printf("    (Maximum permissible):              %16.11f \n", max_lchrom);
      System.out.printf("Finished, clock =  %d\n", time);
      /* note that time unit is msecs, so the following calculation is correct */
      System.out.printf("Time for 1000 iterations = %8.4f\n",(time / (double) niter)); 
      }
   }
