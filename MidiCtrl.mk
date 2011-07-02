# Get the directory of this makefile
CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))

MIDICTRL_BASE_DIR  = $(CURDIR)
MIDICTRL_LIB_DIR   ?= $(MIDICTRL_BASE_DIR)/hardware/libraries
MIDICTRL_JAR_DIR   ?= $(MIDICTRL_BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(MIDICTRL_JAR_DIR)/*.jar))

MIDICTRL_LIBS       += CommonTools Midi
MIDICTRL_LIB_DIRS   = $(foreach lib,$(MIDICTRL_LIBS),$(MIDICTRL_LIB_DIR)/$(lib))
MIDICTRL_INC_FLAGS  = $(foreach dir,$(MIDICTRL_LIB_DIRS),-I$(dir))
MIDICTRL_OBJS       += $(foreach dir,$(MIDICTRL_LIB_DIRS),$(foreach file,$(filter-out $(MIDICTRL_HOST_EXCLUDE),$(wildcard $(dir)/*.c)),$(subst .c,.o,$(file))))
MIDICTRL_DEPS       = $(subst .o,.d,$(MIDICTRL_HOST_OBJS))

CXXFLAGS += $(MIDICTRL_INC_FLAGS)
CFLAGS += $(MIDICTRL_INC_FLAGS)

default: all

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.d:%.cpp
	set -e; $(CXX) -MM $(CXXFLAGS) $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ ; \
	[ -s $@ ] || rm -f $@

printlibs:
	echo $(MIDICTRL_LIBS)
