#ifndef CCHANDLER_H__
#define CCHANDLER_H__

#include "Encoders.hh"
#include "Circular.hh"

typedef void (*midi_learn_cc_callback_t)(CCEncoder *enc);

void CCHandlerOnCCCallback(uint8_t *msg);

typedef struct incoming_cc_s {
  uint8_t channel;
  uint8_t cc;
  uint8_t value;
} incoming_cc_t;

class CCHandler {
 public:
  CircularBuffer<incoming_cc_t, 4> incomingCCs;
  Vector<CCEncoder *, 32> encoders;
  CCEncoder *midiLearnEnc;
  midi_learn_cc_callback_t callback;

  CCHandler() {
    callback = NULL;
    midiLearnEnc = NULL;
  }

  void setup();
  void destroy();
  void onCCCallback(uint8_t *msg);
  void onOutgoingCC(uint8_t channel, uint8_t cc, uint8_t num);
  void midiLearn(CCEncoder *enc) {
    midiLearnEnc = enc;
  }
  void addEncoder(CCEncoder *enc) {
    encoders.add(enc);
  }
  void removeEncoder(CCEncoder *enc) {
    encoders.remove(enc);
  }
  void setCallback(midi_learn_cc_callback_t _callback) {
    callback = _callback;
  }
};

#endif /* CCHANDLER_H__ */
