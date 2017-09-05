#
#	configuration variables for the example

## Main application file
MAIN = localmotifclustermain
DEPH = $(EXSNAPADV)/localmotifcluster.h
DEPCPP = $(EXSNAPADV)/localmotifcluster.cpp

# Set the suffix _ if the fortran77 routines are named that way
CXXFLAGS += -DF77_POST
