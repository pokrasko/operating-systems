MAKE_ = make -C
DIRS_ = lib/ cat/ revwords/ filter/
WILDCARD = $(wildcard $(VAR))
CLEAN_ = make clean -C
MAKE = $(foreach VAR,$(DIRS_),$(MAKE_) $(WILDCARD);)
CLEAN = $(foreach VAR,$(DIRS_),$(CLEAN_) $(WILDCARD);)

all:
	$(MAKE)

clean:
	$(CLEAN)
