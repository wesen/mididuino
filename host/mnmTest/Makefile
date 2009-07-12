CXX = g++
CC = gcc
CXXFLAGS = -Wall -I../../hardware/libraries/MNM -DHOST_MIDIDUINO \
		 -I../../hardware/libraries/CommonTools \
		 -I../../hardware/libraries/Elektron \
		 -I../../hardware/libraries/Midi \
		 -I. -ffunction-sections

all: mnmTest encoderTest

helpers.o: ../../hardware/libraries/CommonTools/helpers.c
	$(CC) $(CXXFLAGS) -c -o $@ $<


MidiSysex.o: ../../hardware/libraries/Midi/MidiSysex.cpp
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


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	- rm -f *.o mnmTest

encoderTest: encoderTest.o Elektron.o helpers.o
	$(CXX) -o $@ $^

mnmTest: Elektron.o MNMMessages.o MNMPattern.o MidiSysex.o MNMSysex.o MNMParams.o MNM.o helpers.o mnmTest.o
	$(CXX) -o $@ $^