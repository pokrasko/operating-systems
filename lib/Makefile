CC = gcc
CFLAGS = -g -Wall -fpic -I.
LDFLAGS = -shared

all: libhelpers.so

clean:
	rm -f libhelpers.so
	rm -f helpers.o

libhelpers.so: helpers.c helpers.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
