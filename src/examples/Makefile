TOP=..
INCDIRS+=.

include $(TOP)/make/top.mk

EXPCONF_DIR:=..
EXPCONF_SOURCES+= expconf.c
EXPCONF_SOURCES+= strchunk.c

EXAMPLES_COMMON_SOURCES+=dump_token.c examples_common.c

SIMPLE1_SOURCES+=simple1.c $(EXAMPLES_COMMON_SOURCES)
FILE1_SOURCES+=file1.c $(EXAMPLES_COMMON_SOURCES)

$(eval $(call add_include_dir, $(EXPCONF_DIR)))
$(eval $(call add_source_dir,  $(EXPCONF_DIR), $(EXPCONF_SOURCES)))

$(eval $(call ExecFile,simple1, $(SIMPLE1_SOURCES) $(ALLSOURCES)))
$(eval $(call ExecFile,file1, $(FILE1_SOURCES) $(ALLSOURCES)))

all: $(ALLEXECUTABLES)

show:
	echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

