#
#	configuration variables for the example

## Main application file
MAIN = zydemo
DEPH = 
DEPCPP = 

SNAPDIR = $(EXSNAP)
GLIBDIR = $(EXGLIB)

ZYDIR = ../../..
LIBZYGOTE = $(ZYDIR)/libzygote
CXXFLAGS += -I$(LIBZYGOTE)
LDFLAGS += -L$(LIBZYGOTE)

LIBS += -lzygote

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	# Linux flags
	CXXLIBFLAGS = $(CXXFLAGS) -fPIC -D__STDC_LIMIT_MACROS
	LDLIBFLAGS = $(LDFLAGS) -shared
	LIBSUFFIX = so
else ifeq ($(UNAME), Darwin)
	# OS X flags
	CXXLIBFLAGS += $(CXXFLAGS)
	LDLIBFLAGS = $(LDFLAGS) -dynamiclib
	LIBSUFFIX = dylib
else ifeq ($(shell uname -o), Cygwin)
	# Cygwin flags
	CXXLIBFLAGS += $(CXXFLAGS) -shared -D__STDC_LIMIT_MACROS
endif

all: $(MAIN).$(LIBSUFFIX)

%.$(LIBSUFFIX): %.o $(DEPH) $(DEPCPP) Snap.o
	$(CXX) $(CXXFLAGS) $(CXXLIBFLAGS) $(LDLIBFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXXLIBFLAGS) -I$(EXSNAP) -I$(EXSNAPADV) -I$(EXGLIB) -I$(EXSNAPEXP)

Snap.o: 
	$(CXX) $(CXXLIBFLAGS) -c $(SNAPDIR)/Snap.cpp -I$(SNAPDIR) -I$(GLIBDIR)

clean: clean-so

clean-so:
	rm -f $(MAIN).so $(MAIN).dylib

