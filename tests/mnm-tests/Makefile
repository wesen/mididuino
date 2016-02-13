MIDICTRL_LIBS = MidiTools GUI MNM Elektron Sequencer

include ../../makefiles/Host.mk

all: mnmPatternTest mnmPatternTestFile

mnmTest: mnmTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

mnmPatternTest: mnmPatternTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

mnmPatternTestFile: mnmPatternTestFile.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o mnmTest mnmPatternTest mnmPatternTestFile *.host.o 


