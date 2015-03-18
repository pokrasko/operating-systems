CC = gcc
CFLAGS = -c -Wall -fpic -I.
LDFLAGS = -shared

all: libhelpers.so

clean:
	rm -f libhelpers.so
	rm -f helpers.o

libhelpers.so: helpers.o
	$(CC) $(LDFLAGS) $< -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@
