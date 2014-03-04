;;;
;;;  John Walker's Floating Point Benchmark, derived from...
;;;
;;;  Marinchip Interactive Lens Design System
;;;
;;;  By John Walker
;;;     http://www.fourmilab.ch/
;;;
;;;  This program may be used, distributed, and modified freely as
;;;  long as the origin information is preserved.
;;;
;;;  This is a complete optical design raytracing algorithm,
;;;  stripped of its user interface and recast into Common Lisp.
;;;  It not only determines execution speed on an extremely
;;;  floating point (including trig function) intensive
;;;  real-world application, it checks accuracy on an algorithm
;;;  that is exquisitely sensitive to errors.  The performance of
;;;  this program is typically far more sensitive to changes in
;;;  the efficiency of the trigonometric library routines than the
;;;  average floating point program.
;;;
;;;  Ported from the C language implementation in September 2005
;;;  by John Walker.

    ;;      Wavelengths of standard spectral lines in Angstroms
    ;;              (Not all are used in this program)

    (setf spectral_line
    	    '(
    		( A_line . 7621.0 )	    ; A
    		( B_line . 6869.955 )       ; B
    		( C_line . 6562.816 )       ; C
    		( D_line . 5895.944 )       ; D
    		( E_line . 5269.557 )       ; E
    		( F_line . 4861.344 )       ; F
    		( Gprime_line . 4340.477 )  ; G'
    		( H_line . 3968.494 )       ; H
	    )
    )

    (setf D_light (cdr (assoc 'D_line spectral_line)))
    (setf C_light (cdr (assoc 'C_line spectral_line)))
    (setf F_light (cdr (assoc 'F_line spectral_line)))

    ;;      The test case used in this program is the design for a 4 inch
    ;;      f/12 achromatic telescope objective used as the example in Wyld's
    ;;      classic work on ray tracing by hand, given in Amateur Telescope
    ;;      Making, Volume 3 (Volume 2 in the 1996 reprint edition).

    (setf testcase
	'(
	    ( 27.05 1.5137 63.6 0.52 )
	    ( -16.68 1 0 0.138 )
	    ( -16.68 1.6164 36.7 0.38 )
	    ( -78.1 1 0 0 )
	)
    )

    (setf default_iteration_count 100000)
    
   ;;      Reference results.  These happen to be derived from a
   ;;      run on Microsoft Quick BASIC on the IBM PC/AT.

    (setf reference_results
    	'(
            "   Marginal ray          47.09479120920   0.04178472683"
            "   Paraxial ray          47.08372160249   0.04177864821"
            "Longitudinal spherical aberration:        -0.01106960671"
            "    (Maximum permissible):                 0.05306749907"
            "Offense against sine condition (coma):     0.00008954761"
            "    (Maximum permissible):                 0.00250000000"
            "Axial chromatic aberration:                0.00448229032"
            "    (Maximum permissible):                 0.05306749907"
    	)
    )

    (setf current_surfaces 0
	  paraxial 0
	  clear_aperture 0
	  aberr_lspher 0
	  aberr_osc 0
    	  aberr_lchrom 0
	  max_lspher 0
	  max_osc 0
	  max_lchrom 0
	  radius_of_curvature 0
    	  object_distance 0
	  ray_height 0
	  axis_slope_angle 0
	  from_index 0
	  to_index 0
    )

    ;;      Calculate passage through surface
    ;;
    ;;      If the variable paraxial is 1, the trace through the
    ;;      surface will be done using the paraxial approximations.
    ;;      Otherwise, the normal trigonometric trace will be done.
    ;;
    ;;      This subroutine takes the following global inputs:
    ;;
    ;;      radius_of_curvature     Radius of curvature of surface
    ;;                              being crossed.  If 0, surface is
    ;;                              plane.
    ;;
    ;;      object_distance         Distance of object focus from
    ;;                              lens vertex.  If 0, incoming
    ;;                              rays are parallel and
    ;;                              the following must be specified:
    ;;
    ;;      ray_height              Height of ray from axis.  Only
    ;;                              relevant if $object_distance == 0
    ;;
    ;;      axis_slope_angle        Angle incoming ray makes with axis
    ;;                              at intercept
    ;;
    ;;      from_index              Refractive index of medium being left
    ;;
    ;;      to_index                Refractive index of medium being
    ;;                              entered.
    ;;
    ;;      The outputs are the following global variables:
    ;;
    ;;      object_distance         Distance from vertex to object focus
    ;;                              after refraction.
    ;;
    ;;      axis_slope_angle        Angle incoming ray makes with axis
    ;;                              at intercept after refraction.
    
    (defun transit_surface ()
    	(if (= paraxial 1)
	    (progn
		(if (/= radius_of_curvature 0)
	    	    (progn
	    		(if (= object_distance 0)
			    (setf axis_slope_angle 0
		    		iang_sin (/ ray_height radius_of_curvature)
			    )
			    (setf iang_sin
		    		(*
				    (/
			    		(- object_distance radius_of_curvature)
					radius_of_curvature
				    )
				    axis_slope_angle
				)
			    )
			)
			(setf rang_sin (* (/ from_index to_index) iang_sin))
			(setf old_axis_slope_angle axis_slope_angle)
			(setf axis_slope_angle (- (+ axis_slope_angle iang_sin) rang_sin))
			(if (/= object_distance 0)
		    	    (setf ray_height (* object_distance old_axis_slope_angle))
			)
			(setf object_distance (/ ray_height axis_slope_angle))
		    )
		    
		    (setf object_distance (* object_distance (/ to_index from_index))
			  axis_slope_angle (* axis_slope_angle (/ from_index to_index))
		    )
		)
	    )
    
    	    (progn
    		(if (/= radius_of_curvature 0)
		    (progn
	    		(if (= object_distance 0)
			    (setf axis_slope_angle 0
		    		  iang_sin (/ ray_height radius_of_curvature)
			    )
			    (setf iang_sin
		    		(*
    	    	    		    (/
			    		(- object_distance radius_of_curvature)
					radius_of_curvature
				    )
				    (sin axis_slope_angle)
				)
			    )
			)
			(setf iang (asin iang_sin)
			      rang_sin (* (/ from_index to_index) iang_sin)
			      old_axis_slope_angle axis_slope_angle
			      axis_slope_angle (+ axis_slope_angle (- iang (asin rang_sin)))
			      sagitta (sin (/ (+ old_axis_slope_angle iang) 2.0))
			      sagitta (* 2 radius_of_curvature sagitta sagitta)
			      object_distance (+
		      	    	    		(*  radius_of_curvature
						    (sin (+ old_axis_slope_angle iang))
	      	    	    	    		    (/ 1 (tan axis_slope_angle))
						)
			    	    		sagitta
			    		      )
			)
		    )

		    (setf rang (- (asin (* (/ from_index to_index) (sin axis_slope_angle))))
			  object_distance (* object_distance
	      	    	    		     (/ (* to_index (cos (- rang)))
						(* from_index (cos axis_slope_angle))
					     )
	      	    			  )
			  axis_slope_angle (- rang)
		    )
		)
	    )
	)
    )

    ;;	Perform ray trace in specific spectral line
    
    (defun trace_line (line ray_h)
    	(setf object_distance 0
	      ray_height ray_h
	      from_index 1
	)
	
	(do ((surfaces s (cdr surfaces)))
	    ((null surfaces))
    	    (setf surface (car surfaces))
	    (setf radius_of_curvature (car surface)
	    	to_index (second surface)
	    )
	    
	    (if (> to_index 1)
        	(setf to_index
		    (+ to_index
		       (*
			   (/
			       (- D_light line)
			       (- C_light F_light )
		           )
			   (/ (1- (second surface))
			      (third surface)
			   )
		       )
		    )
		)
	    )
	    (transit_surface)
	    (setf from_index to_index)
	    (if (cdr surface)
		(setf object_distance
		    (- object_distance (fourth surface))
		)
	    )
	)
    )
    
    ;;	Set parameters for the design to be traced

    (setf clear_aperture 4
    	  current_surfaces 4
	  s testcase
    )
    
    ;;	Run the benchmark
    
    (defun fbench (&optional iteration_count)
    
    	(if (null iteration_count)
	    (setf iteration_count default_iteration_count)
	)
	
        (format t "Starting John Walker's floating point accuracy and~%")
        (format t "performance benchmark.  ~D iterations will be made.~%" iteration_count)
    
	(setf start_time (GET-INTERNAL-RUN-TIME))

	;;	Perform iteration_count iterations of the ray trace

	(do ((iteration 0 (1+ iteration)))
	    ((> iteration iteration_count))

    	    (setf od_sa nil)
	    (do ((jp 0 (1+ jp)))
    		((> jp 1))

		;;  Do main trace in D light

		(setf paraxial jp)
    		(trace_line D_light (/ clear_aperture 2))
    		(setf od_sa (append od_sa (list (list object_distance axis_slope_angle))))
	    )
	    (setf paraxial 0)

	    ;;	Trace marginal ray in C

	    (trace_line C_light (/ clear_aperture 2))
	    (setf od_cline object_distance)

	    ;;	Trace marginal ray in F

	    (trace_line F_light (/ clear_aperture 2))
	    (setf od_fline object_distance)

	    (setf aberr_lspher (- (caadr od_sa) (caar od_sa))
		  aberr_osc (- 1 (/ (* (caadr od_sa) (cadadr od_sa))
	    	    		    (* (sin (cadar od_sa)) (caar od_sa))
				 )
			    )
		  aberr_lchrom (- od_fline od_cline)
		  max_lspher (sin (cadar od_sa))

		  ;;  D light

		  max_lspher (/ 0.0000926 (* max_lspher max_lspher))
		  max_osc 0.0025
		  max_lchrom max_lspher
	    )
	)

	(setf end_time (GET-INTERNAL-RUN-TIME))

	(setf results
    	    (list
		(format nil "   Marginal ray          ~14,11F  ~14,11F" (caar od_sa) (cadar od_sa))
		(format nil "   Paraxial ray          ~14,11F  ~14,11F" (caadr od_sa) (cadadr od_sa))
		(format nil "Longitudinal spherical aberration:      ~16,11F" aberr_lspher)
		(format nil "    (Maximum permissible):              ~16,11F" max_lspher)
		(format nil "Offense against sine condition (coma):  ~16,11F" aberr_osc)
		(format nil "    (Maximum permissible):              ~16,11F" max_osc)
		(format nil "Axial chromatic aberration:             ~16,11F" aberr_lchrom)
		(format nil "    (Maximum permissible):              ~16,11F" max_lchrom)
	    )
	)

	(if (equal results reference_results)
    	    (format t "No errors in results.~%")
	    (progn
		(setf errors 0)
		(do ((received results (cdr received))
	    	     (expected reference_results (cdr expected))
		     (line 1 (1+ line))
	    	    )
	    	    ((null received))
		    (if (not (equal (car expected) (car received)))
			(progn
		    	    (setf errors (1+ errors))
			    (format t "Error in results in line ~D...~%" line)
			    (format t "Expected: ~A~%" (car expected))
			    (format t "Received: ~A~%" (car received))
			)
		    )
		)
		(format t "~D error~A in results.  This is VERY SERIOUS.~%"
	    	    errors (if (> errors 1) "s" ""));
	    )
	)

	(format t "Elapsed time: ~,2F seconds.~%"
    	    (setf etime (/ (- end_time start_time) INTERNAL-TIME-UNITS-PER-SECOND)))
	(format t "Normalised time for 1000 iterations: ~,4F~%" (* etime (/ 1000 iteration_count)))

    )
    
    ;;	Build compiled version of the program
    
    (defun make ()
    	(compile-file "fbench.lsp" :system-p t)
	(compiler::link (list "fbench.o") "fbench")
    )

    (format t "This is John Walker's floating point accuracy and~%")
    (format t "performance benchmark program.  You start it with~%")
    (format t "~%(fbench <itercount>)~%~%")
    (format t "where <itercount> is the number of iterations~%")
    (format t "to be executed (default ~D).  Archival timings~%" default_iteration_count)
    (format t "should be made with the iteration count set so~%")
    (format t "that roughly five minutes of execution is timed.~%")
    (format t "~%Use (make) to build compiled executable with~%")
    (format t "GNU Common Lisp.~%")
