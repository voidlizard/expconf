.PHONY: all clean executables
.PRECIOUS: %.c %.h

ifeq "$(.DEFAULT_GOAL)" ""
.DEFAULT_GOAL=all
endif

ALLEXECUTABLES=

ifeq "$(MISCDATA_DIR)" ""
MISCDATA_DIR=$(TOP)/contrib/miscdata
endif

INCDIRS+=$(TOP)

INC=$(addprefix -I, $(INCDIRS))

BUILDDIR=_build

CFLAGS+=$(INC) $(DEBUG) --std=gnu99

DEBUG=-g
CC=gcc -O2

RAGELOPTS=-G2

objfiles=$(addprefix $(BUILDDIR)/,$(patsubst %.c,%.o,$(1)))

define add_source_dir
ALLSOURCES+=$2
vpath %.c $1
endef

define miscdata_sources
ALLSOURCES+=$(1)
vpath %.c $(MISCDATA_DIR)
endef

define add_include_dir
INCDIRS+=$1
endef

define ExecFile
ALLEXECUTABLES+=$(BUILDDIR)/$1
$(BUILDDIR)/$1: $$(call objfiles,$2)
	$(CC) $$^ -o $$@ $(LDFLAGS)
endef

vpath %.o $(BUILDDIR)
vpath %.c .

.SECONDEXPANSION: all

all: $(ALLEXECUTABLES) ;

.PHONY: builddir
%: builddir ;

builddir: $(BUILDDIR) ;

%.c: %.rl
	ragel $(RAGELOPTS) $< -o $@

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) \
		-MT $(addprefix $(BUILDDIR)/, $(patsubst %.c,%.o,$<)) \
		-MM $< > $(patsubst %.o,%.d,$@)

.PHONY: $(BUILDDIR)
$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

clean: clean_extra
clean:
	@echo Cleaning up...
	@rm  -rf $(BUILDDIR)/*

.PHONY: ctags
ctags: $(OBJECTFILES)
	cat $(BUILDDIR)/*.d | sed 's/^.*://g' | sed 's/\s/\n/g' | egrep '^(\w|\.\.)' | sort -u | xargs ctags
	ctags -a $(MISCDATA_DIR)/*.c


ifneq "$(wildcard $(BUILDDIR))" ""
-include $(OBJECTFILES:.o=.d)
endif
