TOP=..
INCDIRS+=.

include $(TOP)/make/top.mk

EXPCONF_DIR:=..
EXPCONF_SOURCES+= expconf.c
EXPCONF_SOURCES+= exp_tokenize.c
EXPCONF_SOURCES+= strchunk.c

EXAMPLES_COMMON_SOURCES+=examples_common.c

TOKENIZE_SOURCES+=tokenize.c $(EXAMPLES_COMMON_SOURCES)

$(eval $(call add_include_dir, $(EXPCONF_DIR)))
$(eval $(call add_source_dir,  $(EXPCONF_DIR), $(EXPCONF_SOURCES)))

$(eval $(call ExecFile,tokenize, $(TOKENIZE_SOURCES) $(ALLSOURCES)))

all: $(ALLEXECUTABLES)

show:
	echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

