CC = gcc
NAME = revwords

LIBDIR = ../lib
LIBS_ = libhelpers.so
LIBS = $(patsubst %,$(LIBDIR)/%,$(LIBS_))

CFLAGS = -std=c99 -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath=../lib -lhelpers

all: $(NAME)

clean:
	rm -f $(NAME)
	rm -f $(NAME).o
	make clean -C $(LIBDIR)

$(NAME): $(NAME).c $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(LIBS):
	$(MAKE) -C $(dir $@) $(notdir $@)
