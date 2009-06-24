#include "CCHandler.h"
#include "Midi.h"

CCHandler *activeCCHandler = NULL;

void CCHandlerOnCCCallback(uint8_t *msg) {
  if (activeCCHandler != NULL) {
    activeCCHandler->onCCCallback(msg);
  }
}

void CCHandlerEncoderHandle(Encoder *enc) {
  CCEncoder *ccEnc = (CCEncoder *)enc;
  if (activeCCHandler != NULL)
    activeCCHandler->onOutgoingCC(ccEnc);
  MidiUart.sendCC(ccEnc->channel, ccEnc->cc, ccEnc->getValue());
}

void CCHandler::setup() {
  activeCCHandler = this;
  Midi.addOnControlChangeCallback(CCHandlerOnCCCallback);
}

void CCHandler::onCCCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t cc = msg[1];
  uint8_t value = msg[2];
    
  if (midiLearnEnc != NULL) {
    midiLearnEnc->channel = channel;
    midiLearnEnc->cc = cc;
    midiLearnEnc->setValue(value);
    if (callback != NULL)
      callback(midiLearnEnc);
    midiLearnEnc = NULL;
  }

  for (int i = 0; i < encoders.size; i++) {
    if (encoders.arr[i] != NULL) {
      if (encoders.arr[i]->channel == channel &&
	  encoders.arr[i]->cc == cc) {
	encoders.arr[i]->setValue(value);
      }
    }
  }
}

void CCHandler::onOutgoingCC(CCEncoder *enc) {
  for (int i = 0; i < encoders.size; i++) {
    if (encoders.arr[i] != NULL) {
      if (encoders.arr[i]->channel == enc->channel &&
	  encoders.arr[i]->cc == enc->cc) {
	encoders.arr[i]->setValue(enc->getValue());
      }
    }
  }
}
