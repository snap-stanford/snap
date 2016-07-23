#
#	configuration variables for the example

## Main application file
MAIN = motifclustermain
DEPH = $(EXSNAPADV)/motifcluster.h
DEPCPP = $(EXSNAPADV)/motifcluster.cpp
LDFLAGS += -larpack

# Set the suffix _ if the fortran77 routines are named that way
CXXFLAGS += -DF77_POST
