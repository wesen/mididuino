#
# MidiCtrl - Host specific makefile settings
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(CURDIR)MidiCtrl.mk

# set default compilers and flags
CXX      ?= g++-4.7
CC       ?= gcc-4.7
CLDCXX   ?= $(CXX)
CLDC     ?= $(CC)

# ARCHFLAGS = -arch i386 -arch ppc  -mmacosx-version-min=10.4
HOST_BASE_DIR  = $(abspath $(CURDIR)../host)
HOST_LIBS      += MidiUart GUI Tools
HOST_LIB_DIRS  = $(foreach lib,$(HOST_LIBS),$(HOST_BASE_DIR)/$(lib))
HOST_INC_FLAGS = $(foreach dir,$(HOST_LIB_DIRS),-I$(dir)) -I$(HOST_BASE_DIR)
HOST_OBJS      = $(foreach dir,$(HOST_LIB_DIRS),$(foreach file,$(wildcard $(dir)/*.cpp),$(subst .cpp,.host.o,$(file))))
HOST_OBJS      += $(foreach dir,$(HOST_LIB_DIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))
HOST_DEPS      = $(subst .host.o,.host.d,$(HOST_OBJS))

MIDICTRL_HOST_EXCLUDE +=  %MDArpeggiator.cpp 
MIDICTRL_HOST_OBJS    = $(foreach dir,$(MIDICTRL_LIB_DIRS),$(foreach file,$(filter-out $(MIDICTRL_HOST_EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
MIDICTRL_HOST_OBJS    += $(foreach dir,$(MIDICTRL_LIB_DIRS),$(foreach file,$(filter-out $(MIDICTRL_HOST_EXCLUDE),$(wildcard $(dir)/*.c)),$(subst .c,.host.o,$(file))))
MIDICTRL_HOST_DEPS    = $(subst .host.o,.host.d,$(MIDICTRL_HOST_OBJS))

CXXFLAGS += -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g -std=c++0x
CFLAGS   += -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g
CLDFLAGS += -g -O0
#CLDFLAGS += -lreadline -L/opt/local/lib
#CLDFLAGS += --gc-sections

UNAME=$(shell uname)
ISWIN=$(findstring CYGWIN,$(UNAME))
ISMAC=$(findstring Darwin,$(UNAME))

ifneq (,$(ISMAC))
CFLAGS += 
CXXFLAGS += 
CLDFLAGS +=  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)
CLDFLAGS += 
endif

ifneq (,$(ISWIN))
LIBS += -lwinmm
endif

CXXFLAGS += $(HOST_INC_FLAGS)
CFLAGS += $(HOST_INC_FLAGS)

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.test.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.test.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(MIDICTRL_HOST_DEPS)
include $(HOST_DEPS)

OBJS = $(MIDICTRL_HOST_OBJS) $(HOST_OBJS)
TEST_OBJS = $(foreach file,$(OBJS),$(subst .host.o,.test.o,$(file)))

libclean:
	rm -rf $(MIDICTRL_HOST_OBJS) $(HOST_OBJS) $(MIDICTRL_HOST_DEPS) $(HOST_DEPS) $(TEST_OBJS)

