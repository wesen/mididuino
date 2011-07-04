MIDICTRL_LIBS = GUI Elektron MidiTools Midi

include ../../makefiles/Host.mk

all: profiler merger

profiler: profiler.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

merger: merger.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o profiler merger
