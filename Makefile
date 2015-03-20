MAKE_ = make -C
DIRS_ = lib/ cat/ revwords/
MAKE = $(patsubst %,$(MAKE_) %\n,$(DIRS_))
CLEAN_ = make clean - C

all:
	make -C lib/
	make -C cat/
	make -C revwords/

clean:
	make clean -C lib/
	make clean -C cat/
	make clean -C revwords/
