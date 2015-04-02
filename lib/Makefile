CC = gcc
NAMES = helpers bufio
LIBS = $(patsubst %,lib%.so,$(NAMES))
CFLAGS = -g -Wall -fpic -I.
LDFLAGS = -shared

all: $(LIBS)

libbufio.so: bufio.c bufio.h libhelpers.so
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

libhelpers.so: helpers.c helpers.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.so
