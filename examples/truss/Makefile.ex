# Configuration variables for the example. 

## Main application file
MAIN = truss
DEPH = $(EXSNAPADV)/truss.h
DEPCPP = $(EXSNAPADV)/truss.cpp
CXXFLAGS += $(CXXOPENMP)
