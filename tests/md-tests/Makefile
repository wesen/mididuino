MIDICTRL_LIBS = MidiTools GUI MD Elektron Sequencer

include ../../makefiles/Host.mk

CPPUNITDIR=$(MIDICTRL_BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I$(CPPUNITDIR)/src
CLDFLAGS += $(CPPUNITDIR)/cppunitlite.a

all: mdPatternTest

mdPatternTest: mdPatternTest.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o mdPatternTest
