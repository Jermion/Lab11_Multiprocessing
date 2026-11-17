
CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
SOURCES= mandel.c jpegrw.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mandel

all: $(EXECUTABLE) mandelmovie

# Pull in dependency info for existing object files
-include $(OBJECTS:.o=.d)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

# Compile the multiprocessing program
mandelmovie: mandelmovie.c
	$(CC) -Wall -g mandelmovie.c -o mandelmovie

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) mandelmovie *.d

.PHONY: all clean
