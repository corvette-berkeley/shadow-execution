# Makefile for LLVMInstrument pass

LEVEL = ../../..
LIBRARYNAME = LLVMMonitorPass
BUILD_ARCHIVE = 1

include $(LEVEL)/Makefile.common

# Path to top level of LLVM hierarchy
LEVEL = ../../..

# Name of the library to build
LIBRARYNAME = LLVMMonitorPass

# Make the shared library become a loadable module so the tools can 
# dlopen/dlsym on the resulting library.
LOADABLE_MODULE = 1

SHARED_LIBRARY = 1
ARCHIVE_LIBRARY = 1

BUILD_ACHIVE = 1

# Include the makefile implementation stuff
include $(LEVEL)/Makefile.common
