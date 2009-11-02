#ifndef MIDI_H__
#define MIDI_H__

#include <inttypes.h>

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

typedef void(*midi_ack_callback_t)(uint8_t *ptr);
extern midi_ack_callback_t midi_ack_callback;

#endif /* MIDI_H__ */
