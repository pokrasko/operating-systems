CC = gcc
NAME = filter2

LIBDIR = ../lib
LIBS_ = libbufio.so
LIBS = $(patsubst %,$(LIBDIR)/%,$(LIBS_))

CFLAGS = -g -std=c99 -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath=../lib -lbufio -lhelpers

all: $(NAME)

clean:
	rm -f $(NAME)
	rm -f *.o
	make clean -C $(LIBDIR)

$(NAME): $(NAME).c $(LIBS)
	$(CC) $(CFLAGS) $< -o $@ -lbufio $(LDFLAGS)

$(LIBS):
	$(MAKE) -C $(dir $@) $(notdir $@)
