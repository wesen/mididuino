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

midiTest: MidiUartOSX.o midiTest.o Midi.o helpers.o MidiSysex.o 
	$(CXX) $(CLDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	- rm -f *.o midiTest
