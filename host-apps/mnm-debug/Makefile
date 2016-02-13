MIDICTRL_LIBS = GUI Elektron MidiTools Midi MNM Sequencer

include ../../makefiles/Host.mk

all: mnm-debug

mnm-debug: mnm-debug.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o mnm-debug
