CC = gcc-4.0
ARCHFLAGS = -arch i386 -arch ppc -mmacosx-version-min=10.4
#ARCHFLAGS = -arch i386 
#ARCHFLAGS = -arch ppc
#CFLAGS += -mno-cygwin
CFLAGS += -W -Wall $(ARCHFLAGS) -g
#LDFLAGS += -lwinmm
DLLTOOL = dlltool
CFLAGS +=  -isysroot /Developer/SDKs/MacOSX10.4u.sdk

all: midi-send

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

midi-send: midi-send.o midi-apple.o logging.o
	$(CC) $(CFLAGS) -framework CoreAudio -framework CoreMIDI -framework Carbon -o $@ $+

midi-merge: midi-merge.c
	$(CC) $(CFLAGS) -framework CoreAudio -framework CoreMIDI -framework Carbon -o $@ $+

midi-console: midi-console.c
	$(CC) -framework CoreAudio -framework CoreMIDI -framework Carbon -o $@ $+

clean:
	- rm -f *.o *.exe

winmm.lib:
	cp /c/WINDOWS/SYSTEM32/WINMM.DLL winmm.dll
	$(DLLTOOL) -l winmm.lib -z winmm.def --export-all-symbols winmm.dll
#	$(DLLTOOL) -e winmm.def winmm..dll
	rm -f winmm.dll

mmsystem.lib:
	cp /c/WINDOWS/SYSTEM32/mmsystem.dll mmsystem.dll
	$(DLLTOOL) -l mmsystem.lib -z mmsystem.def --export-all-symbols mmsystem.dll
#	$(DLLTOOL) -e mmsystem.def mmsystem.dll
	rm -f mmsystem.dll
