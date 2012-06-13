MIDICTRL_LIBS = MidiTools GUI MD Elektron MNM Sequencer

include ../../makefiles/Host.mk

CPPUNITDIR=$(MIDICTRL_BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I$(CPPUNITDIR)/src
CXXFLAGS += -g
CLDFLAGS += $(CPPUNITDIR)/cppunitlite.a

all: midiTest midiTest2 midiTest3 midiClockTest

midiTest: midiTest.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

midiTest2: midiTest2.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

midiTest3: midiTest3.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

midiClockTest: midiClockTest.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

virtualMidiTest: virtualMidiTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o midiTest midiTest2 midiTest3 midiClockTest virtualMidiTest

