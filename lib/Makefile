CC = gcc
CFLAGS = -c -Wall -I.
LDFLAGS = -shared

all: libhelpers.so

clean:
    rm -f libhelpers.so
    rm -f helpers.o

libhelpers.so: helpers.o
    $(CC) -o $@ $(LDFLAGS) $<

%.o: %.c
    $(CC) -o $@ $(CFLAGS) $<
