include Makefile.macros
include Makefile.def

TOP=$(shell pwd)
BRANCH = $(shell git branch | grep '\*' | cut -d ' ' -f 2)
CFLAGS += -I$(TOP)/include -I$(TOP)/bdwgc/include
LFLAGS += -L$(TOP)/lib -L$(TOP)/bdwgc/gc-inst/lib -lgc -l$(PRODNAME)

CORE	+=	aggregator	\
		collector	\
		distributor	\
		publisher	\
		subscriber	\
		tools		\

EXTERN	+=	bdwgc

LIB	+=	lib
PRE	+=	$(EXTERN) $(LIB)

ALL	=	$(PRE) $(CORE)

SUBCORE = $(addsuffix .core,$(CORE))
SUBPRE = $(addsuffix .pre,$(PRE))
SUBCLEAN = $(addsuffix .clean,$(ALL))
SUBALL = $(addsuffix .all,$(ALL))
SUBINSTALL = $(addsuffix .install,$(ALL))

.PHONY: all clean install

all: $(SUBCORE)
clean: $(SUBCLEAN)
install: $(SUBINSTALL) postinstall

$(SUBCORE): $(SUBPRE)
	$(MAKE) -C $* all

$(SUBPRE):
	$(MAKE) -C $* all

$(SUBCLEAN):
	$(MAKE) -C $* clean

$(SUBINSTALL):
	$(MAKE) -C $* install

postinstall:
	$(AT)echo "Build Done"
