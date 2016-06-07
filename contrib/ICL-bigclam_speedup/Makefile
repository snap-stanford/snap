include ./Makefile.config

.SECONDARY:
     
all: bigclam bigclam_parallelised
     
bigclam_parallelised: bigclam.cpp $(BIGCLAM_DEP_PARALLELISED_O) $(SNAPLIB)
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	$(BIGCLAM_DEP_PARALLELISED_O) $(SNAPLIB) \
	$< \
	-o $@

bigclam: bigclam.cpp $(BIGCLAM_DEP_O) $(SNAPLIB)
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	$(BIGCLAM_DEP_O) $(SNAPLIB) \
	$< \
	-o $@

$(SNAPLIB):
	$(MAKE) -C $(SNAPDIR)
	
$(BINDIR)/agmfast_parallelised.o: agmfast_parallelised.cpp
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	-c $< -o $@
	
$(BINDIR)/ag%.o: $(AGMDIR)/ag%.cpp
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	-c $< -o $@

$(BINDIR)/%.o: %.cpp
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	-c $< -o $@

clean:
	rm -rf bin/*.*
	rm bigclam
	rm bigclam_parallelised

deepclean: clean
	$(MAKE) clean -C $(SNAPDIR)

.PHONY: all clean deepclean

