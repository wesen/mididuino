#include "WProgram.h"
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
  7,
  { 0, 2, 4, 5, 7, 9, 11 }
};

scale_t dorianScale = {
  "DORIAN",
  7,
  { 0, 2, 3, 5, 7, 9, 10 }
};

scale_t phrygianScale = {
  "PHRYG.",
  7,
  { 0, 1, 3, 5, 7, 8, 10 }
};

scale_t lydianScale = {
  "LYDIAN",
  7,
  { 0, 2, 4, 6, 7, 9, 11 }
};

scale_t mixolydianScale = {
  "MIXO.",
  7,
  { 0, 2, 4, 5, 7, 9, 10 }
};

scale_t aeolianScale = {
  "AEOLIAN",
  7,
  { 0, 2, 3, 5, 7, 8, 10 }
};

scale_t locrianScale = {
  "LOCRIAN",
  7,
  { 0, 1, 3, 5, 6, 8, 10 }
};

/* harmonic minor modes */
scale_t harmonicMinorScale = {
  "H.MINOR",
  7,
  { 0, 2, 3, 5, 7, 8, 11 }
};

/* melodic minor modes */

scale_t melodicMinorScale = {
  "M.MINOR",
  7,
  { 0, 2, 3, 5, 7, 9, 11 }
};

scale_t lydianDominantScale = {
  "LYDDOM",
  7,
  { 0, 2, 4, 6, 7, 9, 10 }
};

/* symmetric scales */

scale_t wholeToneScale = {
  "WHOLET.",
  6,
  { 0, 2, 4, 6, 8, 10 }
};

scale_t wholeHalfStepScale = {
  "WSHS",
  8,
  { 0, 2, 3, 5, 6, 8, 9, 11 }
};

scale_t halfWholeStepScale = {
  "HSWS",
  8,
  { 0, 1, 3, 4, 6, 7, 9, 10 }
};

/* pentatonic scales */

scale_t majorPentatonicScale = {
  "MAJPENT",
  5,
  { 0, 2, 4, 7, 9 }
};

scale_t minorPentatonicScale = {
  "MINPENT",
  5,
  { 0, 3, 5, 7, 10 }
};

scale_t suspendedPentatonicScale = {
  "SUSPENT",
  5,
  { 0, 2, 5, 7, 10 }
};

scale_t inSenScale = {
  "INSEN",
  5,
  { 0, 1, 5, 7, 10 }
};

/* derived scales */

scale_t bluesScale = {
  "BLUES",
  6,
  { 0, 3, 5, 6, 7, 10 }
};

scale_t majorBebopScale = {
  "MAJBEBO",
  8,
  { 0, 2, 4, 5, 7, 8, 9, 11 }
};

scale_t dominantBebopScale = {
  "DOMBEBO",
  8,
  { 0, 2, 4, 5, 7, 9, 10, 11 }
};

scale_t minorBebopScale = {
  "MINBEBO",
  8,
  { 0, 2, 3, 4, 5, 7, 9, 10 }
};

/* arpeggios */

scale_t majorArp = {
  "MAJARP",
  3,
  { 0, 4, 7 }
};

scale_t minorArp = {
  "MINARP",
  3,
  { 0, 3, 7 }
};

scale_t majorMaj7Arp = {
  "MM7ARP",
  4,
  { 0, 4, 7, 11 }
};

scale_t majorMin7Arp = {
  "Mm7ARP",
  4,
  { 0, 4, 7, 10 }
};

scale_t minorMin7Arp = {
  "mm7ARP",
  4,
  { 0, 3, 7, 10 }
};

scale_t minorMaj7Arp = {
  "mM7ARP",
  4,
  { 0, 3, 7, 11 }
};

scale_t majorMaj7Arp9 = {
  "MM79ARP",
  5,
  { 0, 2, 4, 7, 11 }
};

scale_t majorMaj7ArpMin9 = {
  "MM7m9A",
  5,
  { 0, 1, 4, 7, 11 }
};

scale_t majorMin7Arp9 = {
  "Mm79ARP",
  5,
  { 0, 2, 4, 7, 10 }
};

scale_t majorMin7ArpMin9 = {
  "Mm7m9A",
  5,
  { 0, 1, 4, 7, 10 }
};

scale_t minorMin7Arp9 = {
  "mm79ARP",
  5,
  { 0, 2, 3, 7, 10 }
};

scale_t minorMin7ArpMin9 = {
  "mm7m9A",
  5,
  { 0, 1, 3, 7, 10 }
};

scale_t minorMaj7Arp9 = {
  "mM79ARP",
  5,
  { 0, 2, 3, 7, 11 }
};

scale_t minorMaj7ArpMin9 = {
  "mM7m9A",
  5,
  { 0, 1, 3, 7, 11 }
};

#ifdef MIDIDUINO

/* scales Encoder */
ScaleEncoder::ScaleEncoder(char *_name, scale_t *_scales[], uint8_t _numScales) :
  RangeEncoder(0, _numScales, _name) {
  scales = _scales;
  numScales = _numScales;
}

scale_t *ScaleEncoder::getScale() {
  return scales[getValue()];
}

void ScaleEncoder::displayAt(int i) {
  GUI.put_string_at(i * 4, getScale()->name);
}

#endif /* MIDIDUINO */
