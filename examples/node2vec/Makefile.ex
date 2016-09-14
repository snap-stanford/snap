MAIN = node2vec
DEPH = $(EXSNAPADV)/n2v.h $(EXSNAPADV)/word2vec.h $(EXSNAPADV)/biasedrandomwalk.h
DEPCPP = $(EXSNAPADV)/n2v.cpp $(EXSNAPADV)/word2vec.cpp $(EXSNAPADV)/biasedrandomwalk.cpp
CXXFLAGS += $(CXXOPENMP)
