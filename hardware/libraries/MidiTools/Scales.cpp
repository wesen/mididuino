/*
 * MidiCtrl - Scales 
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "WMath.h"
#include "helpers.h"
#include "Scales.h"

uint8_t randomScalePitch(const scale_t *scale, uint8_t octaves) {
  uint8_t pitch = scale->pitches[random(scale->size)];
  if (octaves == 0) {
    return pitch;
  } else {
    return pitch + 12 * random(octaves);
  }
}

uint8_t scalePitch(uint8_t pitch, uint8_t root, const uint8_t *scale) {
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

uint8_t invMajorScale[12] = {
  0, 0, 2, 4, 4, 5, 7, 7, 7, 9, 9, 11
};


/* greek modes */
scale_t ionianScale = {
  "IONIAN",
  "ION",
  7,
  { 0, 2, 4, 5, 7, 9, 11 }
};

scale_t dorianScale = {
  "DORIAN",
  "DOR",
  7,
  { 0, 2, 3, 5, 7, 9, 10 }
};

scale_t phrygianScale = {
  "PHRYG.",
  "PHR",
  7,
  { 0, 1, 3, 5, 7, 8, 10 }
};

scale_t lydianScale = {
  "LYDIAN",
  "LYD",
  7,
  { 0, 2, 4, 6, 7, 9, 11 }
};

scale_t mixolydianScale = {
  "MIXO.",
  "MIX",
  7,
  { 0, 2, 4, 5, 7, 9, 10 }
};

scale_t aeolianScale = {
  "AEOLIAN",
  "AEO",
  7,
  { 0, 2, 3, 5, 7, 8, 10 }
};

scale_t locrianScale = {
  "LOCRIAN",
  "LOC",
  7,
  { 0, 1, 3, 5, 6, 8, 10 }
};

/* harmonic minor modes */
scale_t harmonicMinorScale = {
  "H.MINOR",
  "HM-",
  7,
  { 0, 2, 3, 5, 7, 8, 11 }
};

/* melodic minor modes */

scale_t melodicMinorScale = {
  "M.MINOR",
  "MM-",
  7,
  { 0, 2, 3, 5, 7, 9, 11 }
};

scale_t lydianDominantScale = {
  "LYDDOM",
  "LDO",
  7,
  { 0, 2, 4, 6, 7, 9, 10 }
};

/* symmetric scales */

scale_t wholeToneScale = {
  "WHOLET.",
  "WT ",
  6,
  { 0, 2, 4, 6, 8, 10 }
};

scale_t wholeHalfStepScale = {
  "WSHS",
  "WHT",
  8,
  { 0, 2, 3, 5, 6, 8, 9, 11 }
};

scale_t halfWholeStepScale = {
  "HSWS",
  "HWT",
  8,
  { 0, 1, 3, 4, 6, 7, 9, 10 }
};

/* pentatonic scales */

scale_t majorPentatonicScale = {
  "MAJPENT",
  "P+ ",
  5,
  { 0, 2, 4, 7, 9 }
};

scale_t minorPentatonicScale = {
  "MINPENT",
  "P- ",
  5,
  { 0, 3, 5, 7, 10 }
};

scale_t suspendedPentatonicScale = {
  "SUSPENT",
  "SUS",
  5,
  { 0, 2, 5, 7, 10 }
};

scale_t inSenScale = {
  "INSEN",
  "INS",
  5,
  { 0, 1, 5, 7, 10 }
};

/* derived scales */

scale_t bluesScale = {
  "BLUES",
  "BLS",
  6,
  { 0, 3, 5, 6, 7, 10 }
};

scale_t majorBebopScale = {
  "MAJBEBO",
  "BB+",
  8,
  { 0, 2, 4, 5, 7, 8, 9, 11 }
};

scale_t dominantBebopScale = {
  "DOMBEBO",
  "DBB",
  8,
  { 0, 2, 4, 5, 7, 9, 10, 11 }
};

scale_t minorBebopScale = {
  "MINBEBO",
  "BB-",
  8,
  { 0, 2, 3, 4, 5, 7, 9, 10 }
};

/* arpeggios */

scale_t majorArp = {
  "MAJARP",
  "A+ ",
  3,
  { 0, 4, 7 }
};

scale_t minorArp = {
  "MINARP",
  "a  ",
  3,
  { 0, 3, 7 }
};

scale_t majorMaj7Arp = {
  "MM7ARP",
  "A7+",
  4,
  { 0, 4, 7, 11 }
};

scale_t majorMin7Arp = {
  "Mm7ARP",
  "A7 ",
  4,
  { 0, 4, 7, 10 }
};

scale_t minorMin7Arp = {
  "mm7ARP",
  "a7-",
  4,
  { 0, 3, 7, 10 }
};

scale_t minorMaj7Arp = {
  "mM7ARP",
  "a7+",
  4,
  { 0, 3, 7, 11 }
};

scale_t majorMaj7Arp9 = {
  "MM79ARP",
  "A9+",
  5,
  { 0, 2, 4, 7, 11 }
};

scale_t majorMaj7ArpMin9 = {
  "MM7m9A",
  "A9-",
  5,
  { 0, 1, 4, 7, 11 }
};

scale_t majorMin7Arp9 = {
  "Mm79ARP",
  "79 ",
  5,
  { 0, 2, 4, 7, 10 }
};

scale_t majorMin7ArpMin9 = {
  "Mm7m9A",
  "79-",
  5,
  { 0, 1, 4, 7, 10 }
};

scale_t minorMin7Arp9 = {
  "mm79ARP",
  "a79",
  5,
  { 0, 2, 3, 7, 10 }
};

scale_t minorMin7ArpMin9 = {
  "mm7m9A",
  "a9-",
  5,
  { 0, 1, 3, 7, 10 }
};

scale_t minorMaj7Arp9 = {
  "mM79ARP",
  "a+9",
  5,
  { 0, 2, 3, 7, 11 }
};

scale_t minorMaj7ArpMin9 = {
  "mM7m9A",
  "a-9",
  5,
  { 0, 1, 3, 7, 11 }
};

#ifdef MIDIDUINO

/* scales Encoder */
ScaleEncoder::ScaleEncoder(char *_name, const scale_t *_scales[], uint8_t _numScales) :
  RangeEncoder(0, _numScales, _name) {
  scales = _scales;
  numScales = _numScales;
}

const scale_t *ScaleEncoder::getScale() {
  return scales[getValue()];
}

void ScaleEncoder::displayAt(int i) {
  GUI.put_string_at(i * 4, getScale()->shortName);
}

#endif /* MIDIDUINO */
