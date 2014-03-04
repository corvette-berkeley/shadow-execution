
function fbench(niter)

   global  object_distance;
   global  axis_slope_angle;

   // FBench for SciLab :  
   //   original C code by John Walker
   //                      (Autodesk, inc) 
   //                      www.fourmilab.ch/fbench/fbench.html
   //
   //   SciLab adaapt'n by Jim White 
   //                      (u3280625@anu.edu.au)

   max_surfaces = 10
   current_surfaces = 0
   paraxial = 0
   clear_aperture = 0

   aberr_lspher = 0
   aberr_osc = 0
   aberr_lchrom = 0

   max_lspher = 0
   max_osc = 0
   max_lchrom = 0
   radius_of_curvature = 0
   object_distance = 0
   ray_height = 0
   axis_slope_angle = 0
   from_index = 0
   to_index = 0
   itercount =0

   spectral_line = [7621 6869.955 6562.816 5895.944 5269.557 4861.344 4340.477 3968.494]
   s = [ 0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0;  0 0 0 0 0 0 ]
   od_sa = [0 0 0; 0 0 0; 0 0 0]
   testcase= [27.05 1.5137 63.6 0.52; -16.68  1  0  0.138; -16.68 1.6164 36.7 0.38; -78.1  1 0 0]
   

   // Load test case into working array
   clear_aperture = 4
   current_surfaces = 4
   for i = 1:4;
      for j = 1:4
         s(i, j) = testcase(i, j);
         end;
      end;

//
// BEGIN
//
   printf("Start of test - %d iterations will be made.\n", niter);
   tic();
//  
// Perform ray trace the specified number of times.
//
   for itercount = 1:niter
      for paraxial = 1:2
         trace_line(4, clear_aperture / 2);
         od_sa(paraxial, 1) = object_distance;
         od_sa(paraxial, 2) = axis_slope_angle;
         end;
      paraxial = 1;
// Trace marginal ray in C
      trace_line(3, clear_aperture / 2);
      od_cline = object_distance;
// Trace marginal ray in F
      trace_line(6, clear_aperture / 2);
      od_fline = object_distance;

      aberr_lspher = od_sa(2, 1) - od_sa(1, 1);
      aberr_osc = 1 - (od_sa(2, 1) * od_sa(2, 2)) / (sin(od_sa(1, 2)) * od_sa(1, 1));
      aberr_lchrom = od_fline - od_cline;
      max_lspher = sin(od_sa(1, 2));
// D light
      max_lspher = 0.0000926 / (max_lspher * max_lspher);
      max_osc = 0.0025;
      max_lchrom = max_lspher;
      end;
   tfinish = toc();
   printf("   Test completed, clock = %8.4f \n", tfinish);
   printf("%15s   %21.11f  %14.11f \n", "Marginal ray", od_sa(1,1), od_sa(1,2));
   printf("%15s   %21.11f  %14.11f \n", "Paraxial ray", od_sa(2,1), od_sa(2,2));
  	printf("Longitudinal spherical aberration:      %16.11f \n",	   aberr_lspher);
   printf("    (Maximum permissible):              %16.11f \n",	   max_lspher);
   printf("Offense against sine condition (coma):  %16.11f \n",	   aberr_osc);
   printf("    (Maximum permissible):              %16.11f \n",	   max_osc);
   printf("Axial chromatic aberration:             %16.11f \n",	   aberr_lchrom);
   printf("    (Maximum permissible):              %16.11f \n",	   max_lchrom);
   printf("\nTime for 1000 iterations = %10.4f \n", tfinish * niter / 1000.0);
   endfunction

function trace_line(line, ray_h)

   global  object_distance;         // critical!  (JW)
   global  axis_slope_angle;        // critical!  (JW)

//
//  Perform ray trace in specific spectral line
//
   object_distance = 0;
   ray_height = ray_h;
   from_index = 1;
   
   for i = 1:current_surfaces;
      radius_of_curvature = s(i, 1);
      to_index = s(i, 2);
      if (to_index > 1) then
         to_index = to_index + ((spectral_line(4) - spectral_line(line)) / (spectral_line(3) - spectral_line(6))) * ((s(i, 2) - 1) / s(i, 3));
         end; 
      transit_surface();
      from_index = to_index;
      if (i < current_surfaces) then 
         object_distance = object_distance - s(i, 4);
         end;
      end;
   endfunction

function transit_surface()
   
   global  object_distance;
   global  axis_slope_angle;
   
   iang = 0;      // Incidence angle
   rang = 0;      //  Refraction angle
   iang_sin  = 0; // Incidence angle sin
   rang_sin = 0;  // Refraction angle sin
   old_axis_slope_angle = 0;
   sagitta = 0;
  
   
  if (paraxial > 1) then
      if (radius_of_curvature <> 0) then
         
         if (object_distance == 0) then
            axis_slope_angle = 0;
            iang_sin = ray_height / radius_of_curvature;
         else
            iang_sin = ((object_distance - radius_of_curvature) / radius_of_curvature) * axis_slope_angle;
            end;
         rang_sin = (from_index / to_index) * iang_sin;
         old_axis_slope_angle = axis_slope_angle;
         axis_slope_angle = axis_slope_angle + iang_sin - rang_sin;
         if object_distance ~= 0 then 
            ray_height = object_distance * old_axis_slope_angle;
            end;
         object_distance = ray_height / axis_slope_angle;
         return;
         end; 
      object_distance = object_distance * (to_index / from_index);
      axis_slope_angle = axis_slope_angle * (from_index / to_index);
      return;
      end;
   
   if (radius_of_curvature ~= 0) then
      if (object_distance == 0) then
         axis_slope_angle = 0;
         iang_sin = ray_height / radius_of_curvature;
      else
         iang_sin = ((object_distance - radius_of_curvature) / radius_of_curvature) * sin(axis_slope_angle);
         end;
      iang = asin(iang_sin);
      rang_sin = (from_index / to_index) * iang_sin;
      old_axis_slope_angle = axis_slope_angle;
      axis_slope_angle = axis_slope_angle + iang - asin(rang_sin);
      sagitta = sin((old_axis_slope_angle + iang) / 2);
      sagitta = 2 * radius_of_curvature * sagitta * sagitta;
      object_distance = ((radius_of_curvature * sin(old_axis_slope_angle + iang)) * cotg(axis_slope_angle)) + sagitta;
      return;
      end; 

   rang = -asin((from_index / to_index) * sin(axis_slope_angle));
   object_distance = object_distance * ((to_index * cos(-rang)) / (from_index * cos(axis_slope_angle)));
   axis_slope_angle = -rang;
   endfunction

