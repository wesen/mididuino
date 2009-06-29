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
  incoming_cc_t cc;
  cc.channel = MIDI_VOICE_CHANNEL(msg[0]);
  cc.cc = msg[1];
  cc.value = msg[2];

  bool found = false;
  for (int i = 0; i < incomingCCs.size(); i++) {
    incoming_cc_t *cc2 = incomingCCs.getp(i);
    if (cc2->channel == cc.channel && cc2->cc == cc.cc) {
      //      cc2->value = cc.value;
      found = true;

      /* swap to top */
      //      incoming_cc_t *topcc = incomingCCs.getp(incomingCCs.size() - 1);
      for (int j = i; j < incomingCCs.size() - 1; j++) {
	incoming_cc_t *botcc = incomingCCs.getp(j);
	incoming_cc_t *topcc = incomingCCs.getp(j+1);
	botcc->value = topcc->value;
	botcc->channel = topcc->channel;
	botcc->cc = topcc->cc;
	topcc->value = cc.value;
	topcc->cc = cc.cc;
	topcc->channel = cc.channel;
      }
      break;
    }
  }
  if (!found) 
    incomingCCs.putp(&cc);
    
  if (midiLearnEnc != NULL) {
    midiLearnEnc->initCCEncoder(cc.channel, cc.cc);
    midiLearnEnc->setValue(cc.value);
    if (callback != NULL)
      callback(midiLearnEnc);
    midiLearnEnc = NULL;
  }

  for (int i = 0; i < encoders.size; i++) {
    if (encoders.arr[i] != NULL) {
      if (encoders.arr[i]->getChannel() == cc.channel &&
	  encoders.arr[i]->getCC() == cc.cc) {
	encoders.arr[i]->setValue(cc.value);
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
