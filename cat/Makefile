CC = gcc
NAME = cat

LIBDIR = ../lib
LIBS_ = libhelpers.so
LIBS = $(patsubst %,$(LIBDIR)/%,$(LIBS_))

CFLAGS = -c -Wall -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -lhelpers

all: $(NAME)

clean:
	rm -f $(NAME)
	rm -f $(NAME).o

$(NAME): $(NAME).o $(LIBS)
	$(CC) $(LDFLAGS) $< -o $@

$(LIBS):
	$(MAKE) -C $(dir $@) $(notdir $@)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
