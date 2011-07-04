MIDICTRL_LIBS = MidiTools GUI
MIDICTRL_HOST_EXCLUDE = %ModalGui.cpp %MDArpeggiator.cpp

include ../../makefiles/Host.mk

all: ccHandler

ccHandler: ccHandler.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o ccHandler

