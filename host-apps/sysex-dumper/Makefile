MIDICTRL_LIBS = GUI Elektron MidiTools

include ../../Host.mk

all: dumper

dumper: dumper.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o dumper 

