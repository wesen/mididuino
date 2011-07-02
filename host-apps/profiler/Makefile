MIDICTRL_LIBS = GUI Elektron MidiTools

include ../../makefiles/Host.mk

all: profiler

profiler: profiler.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

merger: merger.host.o $(OBJS)
	$(CLDCXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o profiler merger
