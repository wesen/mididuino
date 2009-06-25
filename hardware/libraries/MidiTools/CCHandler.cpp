#include "CCHandler.h"
#include "Midi.h"

CCHandler *activeCCHandler = NULL;

void CCHandlerOnCCCallback(uint8_t *msg) {
  if (activeCCHandler != NULL) {
    activeCCHandler->onCCCallback(msg);
  }
}

void onOutgoingCCCallback(uint8_t *msg) {
  if (activeCCHandler != NULL) {
    activeCCHandler->onOutgoingCC(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);
  }
}

void CCHandler::setup() {
  activeCCHandler = this;
  Midi.addOnControlChangeCallback(CCHandlerOnCCCallback);
  MidiUart.addOnControlChangeCallback(onOutgoingCCCallback);
}

void CCHandler::destroy() {
  Midi.removeOnControlChangeCallback(CCHandlerOnCCCallback);
  MidiUart.removeOnControlChangeCallback(onOutgoingCCCallback);
}

void CCHandler::onCCCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t cc = msg[1];
  uint8_t value = msg[2];
    
  if (midiLearnEnc != NULL) {
    midiLearnEnc->initCCEncoder(cc, channel);
    midiLearnEnc->setValue(value);
    if (callback != NULL)
      callback(midiLearnEnc);
    midiLearnEnc = NULL;
  }

  for (int i = 0; i < encoders.size; i++) {
    if (encoders.arr[i] != NULL) {
      if (encoders.arr[i]->getChannel() == channel &&
	  encoders.arr[i]->getCC() == cc) {
	encoders.arr[i]->setValue(value);
      }
    }
  }
}

void CCHandler::onOutgoingCC(uint8_t channel, uint8_t cc, uint8_t value) {
  for (int i = 0; i < encoders.size; i++) {
    if (encoders.arr[i] != NULL) {
      if (encoders.arr[i]->getChannel() == channel &&
	  encoders.arr[i]->getCC() == cc) {
	encoders.arr[i]->setValue(value);
      }
    }
  }
}
