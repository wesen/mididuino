#
# MidiCtrl - Makefile for unit test suite
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

MIDICTRL_LIBS = Midi MidiTools GUI Elektron MD MNM Sequencer GUI

include ../../makefiles/Host.mk

CPPUNITDIR=$(MIDICTRL_BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I../../thirdparty/CppUnitLite2/src -std=c++0x
CPPSOURCES := $(shell find . -name '*.cpp')
HOSTOBJFILES = $(foreach file,$(CPPSOURCES),$(subst .cpp,.test.o,$(file)))

CFLAGS += -DTEST_SUITE
CXXFLAGS += -DTEST_SUITE

all: test

test: $(HOSTOBJFILES) $(TEST_OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o test
	- find . -name \*.o -print0 | xargs -0 rm

