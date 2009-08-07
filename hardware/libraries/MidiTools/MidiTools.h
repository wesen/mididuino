#ifndef MIDITOOLS_H__
#define MIDITOOLS_H__

#include "WProgram.h"

#define NOTE_C      0
#define NOTE_C_SHARP 1
#define NOTE_D_FLAT  1
#define NOTE_D       2
#define NOTE_D_SHARP 3
#define NOTE_E_FLAT  3
#define NOTE_E       4
#define NOTE_F       5
#define NOTE_F_SHARP 6
#define NOTE_G_FLAT  6
#define NOTE_G       7
#define NOTE_G_SHARP 8
#define NOTE_A_FLAT  8
#define NOTE_A       9
#define NOTE_A_SHARP 10
#define NOTE_B_FLAT  10
#define NOTE_B       11

extern uint8_t minorScale[12];
extern uint8_t majorScale[12];
uint8_t scalePitch(uint8_t pitch, uint8_t root, uint8_t *scale);
void getNotePitch(uint8_t pitch, char *name);

#endif /* MIDITOOLS_H__ */
