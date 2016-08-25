MAIN = node2vec
DEPH = $(EXSNAPADV)/n2v.h $(EXSNAPADV)/word2vec.h $(EXSNAPADV)/randomwalk.h
DEPCPP = $(EXSNAPADV)/n2v.cpp $(EXSNAPADV)/word2vec.cpp $(EXSNAPADV)/randomwalk.cpp
CXXFLAGS += $(CXXOPENMP)
