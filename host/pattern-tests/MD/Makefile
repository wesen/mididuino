CC=gcc
CXX=g++
INCFLAGS=-I../../libraries/CommonTools -I../../libraries/MD
CFLAGS+=$(INCFLAGS)
CXXFLAGS+=$(INCFLAGS)

all: MDPatternTest printMDPattern generateMDPattern bitTest

helpers.o: ../../libraries/CommonTools/helpers.c
	$(CC) $(CFLAGS) -c -o $@ $< 

MDPattern.o: ../../libraries/MD/MDPattern.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Elektron.o: ../../libraries/MD/Elektron.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
MDMessages.o: ../../libraries/MD/MDMessages.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
MDPatternTest.o: MDPatternTest.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

MDPatternTest: MDPatternTest.o helpers.o MDPattern.o Elektron.o MDMessages.o
	$(CXX) $(CXXFLAGS) -o $@ $^
printMDPattern: printMDPattern.o helpers.o MDPattern.o Elektron.o MDMessages.o
	$(CXX) $(CXXFLAGS) -o $@ $^
generateMDPattern: generateMDPattern.o helpers.o MDPattern.o Elektron.o MDMessages.o
	$(CXX) $(CXXFLAGS) -o $@ $^
bitTest: bitTest.o helpers.o MDPattern.o Elektron.o MDMessages.o
	$(CXX) $(CXXFLAGS) -o $@ $^