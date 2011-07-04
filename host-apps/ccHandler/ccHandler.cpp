/*
 * MidiCtrl - Simple test program for testing the CC Handler
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "Platform.h"
#include "MidiSysex.hh"
#include "CCHandler.h"
#include "Encoders.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

char *file = NULL;

CCEncoder mnmEncoders[4];

void autoLearnLast4() {
  printf("AUTOLEARN\n");
  
  int8_t ccAssigned[4] = { 
    -1, -1, -1, -1             };
  int8_t encoderAssigned[4] = {
    
    -1, -1, -1, -1             };
  incoming_cc_t ccs[4];
  int count = ccHandler.incomingCCs.size();

  printf("count: %d\n", count);
  
  for (int i = 0; i < count; i++) {
    ccHandler.incomingCCs.getCopy(i, &ccs[i]);
    
    incoming_cc_t *cc = &ccs[i];
    //    printf("%d: channel :%d, cc: %d, value: %d\n", i,
    //	   cc->channel, cc->cc, cc->value);
	   
    for (int j = 0; j < 4; j++) {
      if ((mnmEncoders[j].getCC() == cc->cc) &&
          (mnmEncoders[j].getChannel() == cc->channel)) {
	ccAssigned[i] = j;
	encoderAssigned[j] = i;
	break;
      }
    }
  }

  for (int i = 0; i < count; i++) {
    printf("%x ", ccs[i].cc);
  }
  printf("\n");

  for (int i = 0; i < count; i++) {
    incoming_cc_t *cc = &ccs[i];
    if (ccAssigned[i] != -1) {
      if ((mnmEncoders[ccAssigned[i]].getChannel() != cc->channel) &&
	  (mnmEncoders[ccAssigned[i]].getCC() != cc->cc)) {
	mnmEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
	mnmEncoders[ccAssigned[i]].setValue(cc->value);
	//	clearRecording(ccAssigned[i]);
      }
    } 
    else {
      for (int j = 0; j < 4; j++) {
	if (encoderAssigned[j] == -1) {
	  ccAssigned[i] = j;
	  encoderAssigned[j] = i;
	  mnmEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
	  mnmEncoders[ccAssigned[i]].setValue(cc->value);
	  //	  clearRecording(ccAssigned[i]);
	  break;
	}
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    printf("ENCODER %d: channel %d, cc %d, value %d\n",
	   i, mnmEncoders[i].getChannel(), mnmEncoders[i].getCC(), mnmEncoders[i].getValue());
  }
}

class Callbacks : public MidiCallback {
  
public:
  Callbacks() {
  }

  void onControlChange(uint8_t *msg) {
  }

  void onNoteOn(uint8_t *msg) {
    autoLearnLast4();
  }
};

Callbacks callbacks;

int main(int argc, char *argv[]) {
  // signal(SIGINT, SIG_IGN);
  MidiUart.init(1, 1);
  ccHandler.setup();
  Midi.addOnControlChangeCallback(&callbacks, (midi_callback_ptr_t)&Callbacks::onControlChange);
  Midi.addOnNoteOnCallback(&callbacks, (midi_callback_ptr_t)&Callbacks::onNoteOn);
  
  for (;;) {
    handleIncomingMidi();
  }

  return 0;
}
