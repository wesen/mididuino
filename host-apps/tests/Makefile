MIDICTRL_LIBS = GUI Elektron MidiTools

include ../../makefiles/Host.mk

all: SendProgramChange

SendProgramChange: SendProgramChange.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o SendProgramChange
