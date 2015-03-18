CC = gcc
EXECS = cat/cat revwords/revwords

all: $(EXECS)

cat/cat:
	$(MAKE) -C $(dir $@) $(notdir $@)

revwords/revwords:
	$(MAKE) -C $(dir $@) $(notdir $@)
