#
#	configuration variables for the example

## Main application file
MAIN = coda
DEPH = $(EXSNAPADV)/agm.h $(EXSNAPADV)/agmfit.h $(EXSNAPADV)/agmfast.h $(EXSNAPADV)/agmdirected.h
DEPCPP = $(EXSNAPADV)/agm.cpp $(EXSNAPADV)/agmfit.cpp $(EXSNAPADV)/agmfast.cpp $(EXSNAPADV)/agmdirected.cpp
CXXFLAGS += $(CXXOPENMP)

