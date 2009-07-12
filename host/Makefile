CXX = g++
CC = gcc
CXXFLAGS = -Wall -I../hardware/libraries/MNM -DHOST_MIDIDUINO \
		 -I../hardware/libraries/CommonTools \
		 -I../hardware/libraries/Elektron \
		 -I../hardware/libraries/Midi \
		 -I. -ffunction-sections

all: schedTest

schedTest: schedTest.o
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	- rm -f *.o schedTest
