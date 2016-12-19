TOP=..
INCDIRS+=.

include $(TOP)/make/top.mk

EXPCONF_DIR:=..
EXPCONF_SOURCES+= expconf.c
EXPCONF_SOURCES+= strchunk.c

SIMPLE1_SOURCES+=simple1.c


$(eval $(call add_include_dir, $(EXPCONF_DIR)))
$(eval $(call add_source_dir,  $(EXPCONF_DIR), $(EXPCONF_SOURCES)))

$(eval $(call ExecFile,simple1, $(SIMPLE1_SOURCES) $(ALLSOURCES)))


all: $(ALLEXECUTABLES)

show:
	echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

