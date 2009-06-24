#ifndef CCHANDLER_H__
#define CCHANDLER_H__

#include "Encoders.hh"

typedef void (*midi_learn_cc_callback_t)(CCEncoder *enc);

void CCHandlerOnCCCallback(uint8_t *msg);

class CCHandler {
 public:
  Vector<CCEncoder *, 32> encoders;
  CCEncoder *midiLearnEnc;
  midi_learn_cc_callback_t callback;

  CCHandler() {
    callback = NULL;
    midiLearnEnc = NULL;
  }

  void setup();
  void onCCCallback(uint8_t *msg);
  void onOutgoingCC(CCEncoder *enc);
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

void CCHandlerEncoderHandle(Encoder *enc);

class CCHandlerEncoder : public CCEncoder {
 public:
 CCHandlerEncoder(int _cc = 0, int _channel = 0, char *_name = NULL, int init = 0) :
  CCEncoder(_cc, _channel, _name, init) {
    handler = CCHandlerEncoderHandle;
  }
};

#endif /* CCHANDLER_H__ */
