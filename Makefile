CC = gcc
EXECS = cat/cat
EXECS_ = $(patsubst %, %/%, $(NAMES))

all: $(EXECS)

cat/cat:
	$(MAKE) -C $(dir $@) $(notdir $@)
