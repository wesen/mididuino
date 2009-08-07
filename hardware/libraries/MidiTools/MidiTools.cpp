#include "helpers.h"
#include "MidiTools.h"

/*
uint8_t minorScale[12] = {
//0  1  2  3  4  5  6   7  8   9  10  11
//1  9b 2  3b 3  4  5b  5  6b  6  7b  7
  0, 0, 2, 3, 3, 5, 7,  7, 8,  8, 10, 10
};

uint8_t majorScale[12] = {
//0  1  2  3  4  5  6   7  8   9  10  11
//1  9b 2  3b 3  4  5b  5  6b  6  7b  7
  0, 0, 2, 4, 4, 5, 7,  7, 9,  9, 11, 11
};
*/

const char *noteNames[] = {
  "C ",
  "Db",
  "D ",
  "Eb",
  "E ",
  "F ",
  "Gb",
  "G ",
  "Ab",
  "A ",
  "Bb",
  "B "
};


void getNotePitch(uint8_t pitch, char *name) {
  int8_t octave = pitch / 12 - 1;
  uint8_t note = pitch % 12;
  m_strncpy(name, noteNames[note], 2);
  if (octave < 0) {
    name[2] = '-';
    name[3] = '0' - octave;
    name[4] = '\0';
  } else {
    name[2] = octave + '0';
    name[3] = '\0';
  }
}

uint8_t scalePitch(uint8_t pitch, uint8_t root, uint8_t *scale) {
  uint8_t scaleRoot;
  if (pitch < root) {
    scaleRoot = 12 + pitch - root;
  } else {
    scaleRoot = pitch - root;
  }
  uint8_t octave = scaleRoot / 12;
  scaleRoot %= 12;
  return octave * 12 + root + scale[scaleRoot];
}
