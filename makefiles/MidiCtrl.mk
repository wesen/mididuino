# Get the directory of this makefile
CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))

MIDICTRL_BASE_DIR  ?= $(abspath $(CURDIR)..)
MIDICTRL_LIB_DIR   ?= $(MIDICTRL_BASE_DIR)/hardware/libraries

MIDICTRL_LIBS       += CommonTools Midi
MIDICTRL_LIB_DIRS   += $(foreach lib,$(MIDICTRL_LIBS),$(MIDICTRL_LIB_DIR)/$(lib))
MIDICTRL_INC_FLAGS  += $(foreach dir,$(MIDICTRL_LIB_DIRS),-I$(dir))
MIDICTRL_OBJS       += $(foreach dir,$(MIDICTRL_LIB_DIRS),$(foreach file,$(wildcard $(dir)/*.cpp),$(subst .cpp,.avr.o,$(file))))
MIDICTRL_OBJS       += $(foreach dir,$(MIDICTRL_LIB_DIRS),$(foreach file,$(filter-out $(MIDICTRL_HOST_EXCLUDE),$(wildcard $(dir)/*.c)),$(subst .c,.avr.o,$(file))))
MIDICTRL_DEPS       += $(subst .o,.d,$(MIDICTRL_HOST_OBJS))

CXXFLAGS += $(MIDICTRL_INC_FLAGS)
CFLAGS += $(MIDICTRL_INC_FLAGS)

CXXFlags += -Werror -Wall
CFLAGS += -Werror -Wall

default: all

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.d:%.cpp
	set -e; $(CXX) -MM $(CXXFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.host.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.host.d:%.cpp
	set -e; $(CXX) -MM $(CXXFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.avr.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.avr.d:%.cpp
	set -e; $(CXX) -MM $(CXXFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.host.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.host.d:%.cpp
	set -e; $(CXX) -MM $(CXXFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

printlibs:
	echo $(MIDICTRL_LIBS)
