TOP=..

INCDIRS+=.

include $(TOP)/make/top.mk

SIMPLE1_SOURCES+=simple1.c


$(eval $(call ExecFile,simple1, $(SIMPLE1_SOURCES) $(ALLSOURCES)))


all: $(ALLEXECUTABLES)

show:
	echo $(ALLEXECUTABLES)

clean_extra:
	@echo clean extra...
	$(RM) $(patsubst %.rl,%.c,$(RAGEL_SOURCES))

