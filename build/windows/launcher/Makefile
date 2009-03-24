CXXFLAGS = -mwindows -mno-cygwin -O2 -Wall
OBJS = launcher.o launcher-rc.o

mididuino.exe: $(OBJS)
	$(LINK.cc) $(CXXFLAGS) -o $@ $(OBJS)
	cp mididuino.exe ../work/

$(OBJS): Makefile

launcher-rc.o: launcher.rc
	windres -i $< -o $@

clean:
	$(RM) $(OBJS) mididuino.exe
