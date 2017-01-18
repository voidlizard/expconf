TOP=..
INCDIRS+=.

include $(TOP)/make/top.mk

# CFLAGS+=-c99

MISCDATA_DIR:=$(TOP)/../../miscdata
CHAOS_PP_DIR:=$(TOP)/../contrib/chaos-pp

MISCDATA_SOURCES+=hash.c
MISCDATA_SOURCES+=slist.c
MISCDATA_SOURCES+=hashfun_murmur.c

EXPCONF_DIR:=..
EXPCONF_SOURCES+= expconf.c
EXPCONF_SOURCES+= exp_tokenize.c
EXPCONF_SOURCES+= strchunk.c

ULISP_SOURCES+=ulisp.c
EX_BIND_SOURCES+=ex_bind.c

EXAMPLES_COMMON_SOURCES+=examples_common.c

TOKENIZE_SOURCES+=tokenize.c $(EXAMPLES_COMMON_SOURCES)
PARSE1_SOURCES+=parse1.c $(EXAMPLES_COMMON_SOURCES)
UCLISP_SOURCES+=ulisp_example.c $(ULISP_SOURCES) $(EXAMPLES_COMMON_SOURCES)

$(eval $(call add_include_dir, $(MISCDATA_DIR)))
$(eval $(call add_include_dir, $(CHAOS_PP_DIR)))
$(eval $(call miscdata_sources, $(MISCDATA_SOURCES)))

$(eval $(call add_include_dir, $(EXPCONF_DIR)))
$(eval $(call add_source_dir,  $(EXPCONF_DIR), $(EXPCONF_SOURCES)))

$(eval $(call ExecFile,tokenize, $(TOKENIZE_SOURCES) $(ALLSOURCES)))
$(eval $(call ExecFile,parse1, $(PARSE1_SOURCES) $(ALLSOURCES)))
$(eval $(call ExecFile,ulisp_example, $(UCLISP_SOURCES) $(ALLSOURCES)))
#$(eval $(call ExecFile,ex_bind, $(EX_BIND_SOURCES) $(ALLSOURCES)))

all: $(ALLEXECUTABLES)

show:
	@echo $(TOP)
	@echo $(MISCDATA_DIR)
	@echo $(CHAOS_PP_DIR)
	@echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

