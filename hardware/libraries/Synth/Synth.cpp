#include "Synth.h"

#define MYCLI() uint8_t mycli_tmp = SREG; cli()
#define MYSEI() SREG = mycli_tmp;

const uint16_t freqtable[91] = {
  69,    73,   77,   82,   86,   92,   97,  103,  109,  115,  122,  129, // 12
  137,  145,  154,  163,  173,  183,  194,  206,  218,  231,  244,  259, // 24
  274,  291,  308,  326,  346,  366,  388,  411,  435,  461,  489,  518,  // 36
  549,  581,  616,  652,  691,  732,  776,  822,  871,  923,  978,  1036, // 48
  1097, 1163, 1232, 1305, 1383, 1465, 1552, 1644, 1742, 1845, 1955, 2071, // 60
  2195, 2325, 2463, 2610, 2765, 2930, 3104, 3288, 3484, 3691, 3910, 4143, // 72
  4389, 4650, 4927, 5220, 5530, 5859, 6207, 6577, 6968, 7382, 7821, 8286, // 84
  8779, 9301, 9854, 10440, 11060, 11718, 12415 // 91
};


static uint8_t sample = 0;
note_t notes[MAX_POLYPHONY];
uint8_t polyphony = 1;

asm("__note_phase__ = 0\n\t"
    "__note_speed__ = 2\n\t"
    "__note_sample__ = 4\n\t"
    "__note_length__ = 5\n\t"
    "__note_struct_size = 7\n\t"
    );

SIGNAL(TIMER1_OVF_vect) {
  //  asm("out %0, %1\n\t" : : "I" (OCR1A), "r" (sample) );
  //  asm("in %0, %1\n\t" : "=r" (value) : "I" (OCR1AL));

  PORTB |= _BV(5);
#if 0
  asm(
      "sts OCR1AL, %0          \n\t"
      "ldi r30, lo8(notes)     \n\t"
      "ldi r31, hi8(notes)     \n\t"

      "ldi r21, lo8(-1)         \n\t" /* 255 for sample */

"_L_next_note:\n\t"
      "ldd r24, Z+__note_speed__   \n\t"
      "ldd r25, Z+__note_speed__+1 \n\t"

      "ldd r18, Z+__note_phase__   \n\t"
      "ldd r19, Z+__note_phase__+1 \n\t"
      "add r24, r18                \n\t"
      "adc r25, r19                \n\t"
      "std Z+1, r25                \n\t"
      "st  Z,   r24                \n\t"

      "sbrs r24, 7                 \n\t" /* vergleich mit 32768 */
      "rjmp _L_sample_zero         \n\t"
      "std  Z+__note_sample__, r21 \n\t"
      "rjmp _L_note_length         \n\t"
"_L_sample_zero:\n\t"
      "std Z+__note_sample__, __zero_reg__ \n\t"

"_L_note_length:\n\t"
      "ldd r24, Z+__note_length__   \n\t"
      "ldd r25, Z+__note_length__+1 \n\t"
      "sbiw r24, 0                  \n\t"
      "breq _L_loop_next            \n\t"

      "sbiw r24, 1                  \n\t" /* length-- */
      "std Z+__note_length__+1, r25 \n\t"
      "std Z+__note_length__, r24   \n\t"
      "ldd r24, Z+__note_sample__   \n\t"
      "lsr r24                      \n\t"
      "lsr r24                      \n\t"
      "add r20, r24                 \n\t"

"_L_loop_next:\n\t"
      "adiw r30, __note_struct_size__ \n\t"
      "ldi r24, hi8(notes+28)\n\t"
      "cpi r30, lo8(notes+28)\n\t"
      "cpc r31, r24\n\t"
      "brne _L_next_note\n\t"

      "sts %0, r20\n\t"

      : : "r" (sample));
  #endif
 WRITE_AUDIO(sample);
 
 sample = 0;
  for (uint8_t i = 0; i < 4; i++) {
    notes[i].phase += notes[i].speed;
    if (notes[i].phase >= 32768)
      notes[i].sample = 255;
    else
      notes[i].sample = 0;
    if (notes[i].length > 0) {
      notes[i].length--;
      sample += (notes[i].sample >> 2);
    }
  }

  PORTB &= ~_BV(5);
}

SynthClass::SynthClass() {
  polyphony = 1;
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    notes[i].phase = notes[i].speed = notes[i].sample = notes[i].length = 0;
  }
}

void SynthClass::init() {
  pinMode(AUDIO, OUTPUT);
  TCCR1A = _BV(WGM10) | _BV(COM1A1);
  TCCR1B = _BV(CS10) | _BV(WGM12);
  TIMSK1 |= _BV(TOIE1);
}

void SynthClass::setPolyphony(uint8_t numNotes) {
  if (numNotes > MAX_POLYPHONY)
    polyphony = MAX_POLYPHONY;
  else
    polyphony = numNotes;
}

uint8_t SynthClass::getPolyphony() {
  return polyphony;
}

bool SynthClass::isPlaying(uint8_t note) {
  return (notes[note].length != 0);
}

uint8_t SynthClass::nextNote() {
  for (int i = 0; i < polyphony; i++) {
    if (!isPlaying(i))
      return i;
  }
  return 0; // default return first note
}

void SynthClass::play(uint8_t pitch, uint8_t length, uint8_t note) {
  MYCLI();
  if (note == 0xFF)
    note = nextNote();
  notes[note].speed = freqtable[pitch];
  notes[note].phase = 0;
  notes[note].length = (uint16_t)length << 8;
  MYSEI();
}

static uint32_t val = 2343453;

void fast_srand(uint32_t bla) {
  val = bla;
}

uint32_t fast_random(uint16_t max) {
  val <<= 1;
  if (!(val & 0x8000))
    goto end;
  else
    val ^= 0x20aa95b5;

 end:
  if (max == 0)
    return val;
  else
    return (val % max);
}

SynthClass Synth;
