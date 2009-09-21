#include "Encoders.hh"
#include "MidiTools.h"
#include "Midi.h"

/* handlers */

void CCEncoderHandle(Encoder *enc) {
  CCEncoder *ccEnc = (CCEncoder *)enc;
  MidiUart.sendCC(ccEnc->getChannel(), ccEnc->getCC(), ccEnc->getValue());
}

void VarRangeEncoderHandle(Encoder *enc) {
  VarRangeEncoder *rEnc = (VarRangeEncoder *)enc;
  *(rEnc->var) = rEnc->getValue();
}

#ifndef HOST_MIDIDUINO
#include <MidiClock.h>

void TempoEncoderHandle(Encoder *enc) {
  MidiClock.setTempo(enc->getValue());
}
#endif

/* Encoder */
Encoder::Encoder(const char *_name, encoder_handle_t _handler) {
  old = 0;
  cur = 0;
  redisplay = false;
  setName(_name);
  handler = _handler;
  fastmode = true;
  pressmode = false;
}

void Encoder::checkHandle() {
  if (cur != old) {
    if (handler != NULL)
      handler(this);
  }
  
  old = cur;
}

void Encoder::setName(const char *_name) {
  if (_name != NULL)
    m_strncpy_fill(name, _name, 4);
  name[3] = '\0';
}

void Encoder::setValue(int value, bool handle) {
  if (handle) {
    cur = value;
    checkHandle();
  } else {
    old = cur = value;
  }
  redisplay = true;
}

void Encoder::displayAt(int i) {
  GUI.put_value(i, getValue());
  redisplay = false;
}

bool Encoder::hasChanged() {
  return old != cur;
}

void Encoder::clear() {
  old = 0;
  cur = 0;
}

int Encoder::update(encoder_t *enc) {
  cur = cur + enc->normal + (pressmode ? 0 : (fastmode ? 5 * enc->button : enc->button));
  return cur;
}

/* EnumEncoder */
void EnumEncoder::displayAt(int i) {
  GUI.put_string_at(i * 4, enumStrings[getValue()]);
  redisplay = false;
}

void PEnumEncoder::displayAt(int i) {
  //  GUI.put_p_string_at_fill(i * 4, (PGM_P)(pgm_read_word(enumStrings[getValue()])));
  GUI.put_p_string_at(i * 4, (PGM_P)(enumStrings[getValue()]));
  redisplay = false;
}


/* RangeEncoder */

int RangeEncoder::update(encoder_t *enc) {
  int inc = enc->normal + (pressmode ? 0 : (fastmode ? 5 * enc->button : enc->button));
  
  cur = limit_value(cur, inc, min, max);
  return cur;
}

/* CharEncoder */
CharEncoder::CharEncoder()  : RangeEncoder(0, 37) {
}

char CharEncoder::getChar() {
  uint8_t val = getValue();
  if (val == 0) {
    return ' ';
  }
  if (val < 27) 
    return val - 1+ 'A';
  else
    return (val - 27) + '0';
}

void CharEncoder::setChar(char c) {
  if (c >= 'A' && c <= 'Z') {
    setValue(c - 'A' + 1);
  } else if (c >= '0' && c <= '9') {
    setValue(c - '0' + 26 + 1);
  } else {
    setValue(0);
  }
}

/* notePitchEncoder */
NotePitchEncoder::NotePitchEncoder(char *_name) : RangeEncoder(0, 127, _name) {
}

void NotePitchEncoder::displayAt(int i) {
  char name[5];
  getNotePitch(getValue(), name);
  GUI.put_string_at(i * 4, name);
}

void MidiTrackEncoder::displayAt(int i) {
  GUI.put_value(i, getValue() + 1);
}

void AutoNameCCEncoder::initCCEncoder(uint8_t _channel, uint8_t _cc) {
  CCEncoder::initCCEncoder(_channel, _cc);
  setCCName();
  GUI.redisplay();
}
