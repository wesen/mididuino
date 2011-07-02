MIDICTRL_LIBS = GUI Elektron MidiTools

include ../../Host.mk

all: er1

er1: er1.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o er1
