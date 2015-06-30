CC = gcc
NAMES = forking polling

LIBDIR = ../lib
LIBS_ = libbufio.so
LIBS = $(patsubst %,$(LIBDIR)/%,$(LIBS_))

CFLAGS = -g -Wall -std=c99 -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath=../lib -lbufio -lhelpers

all: $(NAMES)

clean:
	rm -f $(NAMES)
	rm -f $(patsubst %,%.o,$(NAMES))
	make clean -C $(LIBDIR)

forking: forking.c $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

polling: polling.c $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(LIBS):
	$(MAKE) -C $(dir $@) $(notdir $@)
