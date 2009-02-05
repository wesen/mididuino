#include "WProgram.h"

#include "EncoderPage.h"

Encoder::Encoder() {
  old = 0;
  cur = 0;
}

void Encoder::clear() {
  old = 0;
  cur = 0;
}

void Encoder::handle(uint8_t val) {
}

void Encoder::update(encoder_t *enc) {
  cur = cur + enc->normal + 5 * enc->button;
}

void RangeEncoder::update(encoder_t *enc) {
  int8_t inc = enc->normal + 5 * enc->button;
  cur = u_limit_value(cur, inc, min, max);
}

void CCEncoder::handle(uint8_t val) {
  MidiUart.sendCC(cc, val);
}

void TempoEncoder::handle(uint8_t val) {
  MidiClock.setTempo(val);
}

/********************/

void EncoderPage::update() {
  for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->update(GUI.Encoders.encoders + i);
  }
}

void EncoderPage::handle() {
  for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->checkHandle();
  }
}

void EncoderPage::clear() {
  for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
    if (encoders[i] != NULL)
      encoders[i]->clear();
  }
}

void EncoderPage::display() {
  for (uint8_t i = 0; i < 4; i++) {
    if (encoders[i] != NULL)
      GUI.put_value(i, encoders[i]->getValue());
  }
}
