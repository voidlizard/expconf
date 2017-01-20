TOP=..
INCDIRS+=.

include $(TOP)/make/top.mk

CFLAGS+=-Wall


MISCDATA_DIR:=$(TOP)/../../miscdata
CHAOS_PP_DIR:=$(TOP)/../contrib/chaos-pp

MISCDATA_SOURCES+=hash.c
MISCDATA_SOURCES+=slist.c
MISCDATA_SOURCES+=hashfun_murmur.c
MISCDATA_SOURCES+=static_mem_pool.c

EXPCONF_DIR:=..
EXPCONF_SOURCES+= expconf.c
EXPCONF_SOURCES+= exp_tokenize.c
EXPCONF_SOURCES+= strchunk.c
EXPCONF_SOURCES+=$(ULISP_SOURCES)

ULISP_SOURCES+=ulisp.c ulisp_parser.c ulisp_primop_string.c

EXAMPLES_COMMON_SOURCES+=examples_common.c

ULISP_EXAMPLE_SOURCES+=ulisp_example.c $(ULISP_SOURCES) $(EXAMPLES_COMMON_SOURCES)

$(eval $(call add_include_dir, $(MISCDATA_DIR)))
$(eval $(call add_include_dir, $(CHAOS_PP_DIR)))
$(eval $(call miscdata_sources, $(MISCDATA_SOURCES)))

$(eval $(call add_include_dir, $(EXPCONF_DIR)))
$(eval $(call add_source_dir,  $(EXPCONF_DIR), $(EXPCONF_SOURCES)))

$(eval $(call ExecFile,ulisp_example, $(ULISP_EXAMPLE_SOURCES) $(ALLSOURCES)))

all: $(ALLEXECUTABLES)

show:
	@echo $(TOP)
	@echo $(MISCDATA_DIR)
	@echo $(CHAOS_PP_DIR)
	@echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

