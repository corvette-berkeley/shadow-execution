cd .libs
extract-bc libgsl.so
mv libgsl.so.bc ../libgsl-3.0.so.bc
cd ../cblas/.libs
extract-bc libgslcblas.so
mv libgslcblas.so.bc ../../libgslcblas-3.0.so.bc
cd ../../
