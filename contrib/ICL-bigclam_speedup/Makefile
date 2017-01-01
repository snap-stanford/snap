include ./Makefile.config

.SECONDARY:
     
all: bigclam bigclam_old cmtycompare
     
bigclam_old: src/bigclam.cpp $(BIGCLAM_OLD_DEP_O) $(SNAPLIB)
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	$(BIGCLAM_OLD_DEP_O) $(SNAPLIB) \
	$< \
	-o $@

bigclam: src/bigclam.cpp $(BIGCLAM_DEP_O) $(SNAPLIB)
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	$(BIGCLAM_DEP_O) $(SNAPLIB) \
	$< \
	-o $@

cmtycompare: src/cmtycompare.cpp $(SNAPLIB)
	$(CC) $(CXXFLAGS) $(CXXOPENMP) $(LDFLAGS) $(LIBS) \
	$(BIGCLAM_HEADERFLAGS) \
	$(SNAPLIB) \
	$< \
	-o $@

$(SNAPLIB):
	$(MAKE) -C $(SNAPDIR)
	
$(BINDIR)/agmfast_old.o: src/agmfast_old.cpp
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
	rm bigclam_old
	rm cmtycompare

deepclean: clean
	$(MAKE) clean -C $(SNAPDIR)

.PHONY: all clean deepclean

