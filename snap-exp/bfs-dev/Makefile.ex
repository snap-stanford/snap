#
#	configuration variables for the example

## Main application file
MAIN = testgraph
DEPH = 
DEPCPP = 

%: %.cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o 
	$(CC) $(CXXFLAGS) $(CXXOPENMP) -o $@ $@.cpp $(DEPCPP) $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(EXSNAPADV) -I$(EXGLIB) -I$(EXSNAPEXP) $(LDFLAGS) $(LIBS) 

