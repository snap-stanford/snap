SNAPDIR=../../../../snap
INCLUDES= -I $(SNAPDIR)/snap-core -I $(SNAPDIR)/glib-core
CC=g++
CFLAGS=-c -O3 

SOURCES= risnapds.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=risnapds
SNAPLIB=$(SNAPDIR)/snap-core/Snap.o

all:	$(SNAPLIB) $(SOURCES) $(EXECUTABLE)

$(SNAPLIB):	
	$(MAKE) -C $(SNAPDIR)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(SNAPLIB)

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $@
clean:
	rm -f $(EXECUTABLE) *.o
