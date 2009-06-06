#include "Encoders.hh"

#include "Midi.h"

Encoder::Encoder(char *_name) {
  old = 0;
  cur = 0;
  redisplay = false;
  setName(_name);
}

void Encoder::clear() {
  old = 0;
  cur = 0;
}

void Encoder::handle(int val) {
}

void Encoder::update(encoder_t *enc) {
  cur = cur + enc->normal + 5 * enc->button;
}

void RangeEncoder::update(encoder_t *enc) {
  int inc = enc->normal + 5 * enc->button;
  
  cur = limit_value(cur, inc, min, max);
}

void CCEncoder::handle(int val) {
  MidiUart.sendCC(channel, cc, val);
}

void TempoEncoder::handle(int val) {
  MidiClock.setTempo(val);
}


