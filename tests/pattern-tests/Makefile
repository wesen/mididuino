MIDICTRL_LIBS = MidiTools GUI Elektron MD Sequencer

include ../../makefiles/Host.mk

CPPUNITDIR=$(MIDICTRL_BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I$(CPPUNITDIR)/src
CLDFLAGS += $(CPPUNITDIR)/cppunitlite.a

all: MDPatternTest generateMDPattern printMDPattern

MDPatternTest: MDPatternTest.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

generateMDPattern: generateMDPattern.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

printMDPattern: printMDPattern.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o test


