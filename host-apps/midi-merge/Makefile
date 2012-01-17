all: midi-merge

midi-merge: midi-merge.c
	$(CC) $(CFLAGS) -framework CoreAudio -framework CoreMIDI -framework Carbon -o $@ $+

clean:
	rm -f midi-merge