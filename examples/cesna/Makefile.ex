#
#	configuration variables for the example

## Main application file
MAIN = cesna
DEPH = $(EXSNAPADV)/agm.h $(EXSNAPADV)/agmfit.h $(EXSNAPADV)/agmfast.h $(EXSNAPADV)/agmattr.h
DEPCPP = $(EXSNAPADV)/agm.cpp $(EXSNAPADV)/agmfit.cpp $(EXSNAPADV)/agmfast.cpp $(EXSNAPADV)/agmattr.cpp
CXXFLAGS += $(CXXOPENMP)

