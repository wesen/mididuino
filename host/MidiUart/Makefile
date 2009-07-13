CXX = g++
ARCHFLAGS = -arch i386 -arch ppc  -mmacosx-version-min=10.4
CC = gcc
CXXFLAGS = -Wall -I../../hardware/libraries/MNM -DHOST_MIDIDUINO \
		 -I../../hardware/libraries/CommonTools \
		 -I../../hardware/libraries/Elektron \
		 -I../../hardware/libraries/Midi \
		 -I. -I.. -ffunction-sections $(ARCHFLAGS)
CLDFLAGS =  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)

all: midiTest

helpers.o: ../../hardware/libraries/CommonTools/helpers.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

MidiSysex.o: ../../hardware/libraries/Midi/MidiSysex.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Midi.o: ../../hardware/libraries/Midi/Midi.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MidiClock.o: ../../hardware/libraries/Midi/MidiClock.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNMSysex.o: ../../hardware/libraries/MNM/MNMSysex.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNMParams.o: ../../hardware/libraries/MNM/MNMParams.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


Elektron.o: ../../hardware/libraries/Elektron/Elektron.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNMMessages.o: ../../hardware/libraries/MNM/MNMMessages.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNMPattern.o: ../../hardware/libraries/MNM/MNMPattern.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNM.o: ../../hardware/libraries/MNM/MNM.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MNMTask.o: ../../hardware/libraries/MNM/MNMTask.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


midiTest: MidiUartOSX.o midiTest.o Midi.o helpers.o MidiSysex.o  \
	  MNM.o MNMPattern.o MNMMessages.o Elektron.o MNMParams.o MNMSysex.o \
	  MNMTask.o
	$(CXX) $(CLDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	- rm -f *.o midiTest
