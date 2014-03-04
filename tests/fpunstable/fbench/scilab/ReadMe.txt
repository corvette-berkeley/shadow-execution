
Scilab fbench  (probably suitable for MatLab also)

Note:  This version is not really a fair test of Scilab.
       It's a "straight" conversion of the C version, and
       since it's a standard procedural approach using
       scalars, it's not suited to the matrix-mode
       model used in these packages.

       It runs about 300 times slower than the equivalent
       C version in this form.


       Later I hope to adapt the program to present itself
       apporopriately as a vector/matrix problem.

Instructions:

-->  getf fbench.sci

-->  fbench(1000)         // parameter is no. of iterations to run


