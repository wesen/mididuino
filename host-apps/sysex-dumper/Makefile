MIDICTRL_LIBS = GUI Elektron MidiTools

include ../../makefiles/Host.mk

all: dumper

dumper: dumper.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o dumper 

