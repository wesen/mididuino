/*
 * MidiCtrl - Juno 106 sys encoders
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef JUNO_ENCODERS_H__
#define JUNO_ENCODERS_H__

#include <Juno.h>
#include <GUI.h>

#ifdef MIDIDUINO_USE_GUI

class JunoEncoder : public RangeEncoder {
public:
  uint8_t channel;
  uint8_t param;

  void initJunoEncoder(uint8_t channel = 0, uint8_t param = JUNO_LFO_DELAY, char *_name = NULL, uint8_t init = 0) {
    this->channel = channel;
    this->param = param;
    setName(_name);
    setValue(init);
  }

  JunoEncoder(uint8_t channel = 0, uint8_t param = JUNO_LFO_DELAY, char *_name = NULL, uint8_t init = 0);
};

#endif /* MIDIDUINO_USE_GUI */

#endif /** JUNO_ENCODERS_H__ */
