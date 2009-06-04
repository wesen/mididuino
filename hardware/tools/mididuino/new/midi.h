#ifndef MIDI_H__
#define MIDI_H__

extern int canSendSysex;
extern int waitingForBootloader;
extern int exitMainLoop;

void listInputMidiDevices(void);
void listOutputMidiDevices(void);
void midiSendLong(unsigned char *buf, unsigned long len);
void midiSendShort(unsigned char status,
		   unsigned char byte1, unsigned char byte2);

void midiInitialize(char *inputDevice, char *outputDevice);
void midiReceive(unsigned char c);
void midiMainLoop(void);
void midiTimeout(void);
void midiClose(void);

#endif /* MIDI_H__ */
