
DIRS += linux osx windows

SUBALL = $(addsuffix .all,$(DIRS))
SUBCLEAN = $(addsuffix .clean,$(DIRS))
SUBINSTALL = $(addsuffix .install,$(DIRS))

.PHONY: all clean install

all: $(SUBALL)
clean: $(SUBCLEAN)
install: $(SUBINSTALL)

$(SUBALL):
	$(MAKE) -C $* all

$(SUBCLEAN):
	$(MAKE) -C $* clean

$(SUBINSTALL):
	$(MAKE) -C $* install
