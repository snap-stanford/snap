#
#	configuration variables for the example

## Main application file
MAIN = bigclam
DEPH = $(EXSNAPADV)/agm.h $(EXSNAPADV)/agmfit.h $(EXSNAPADV)/agmfast.h
DEPCPP = $(EXSNAPADV)/agm.cpp $(EXSNAPADV)/agmfit.cpp $(EXSNAPADV)/agmfast.cpp
CXXFLAGS += $(CXXOPENMP)

