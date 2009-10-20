#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include "MidiSysex.hh"
#include "CCHandler.h"
#include "Encoders.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiClass Midi;

char *file = NULL;

CCEncoder mnmEncoders[4];
CCHandler ccHandler;

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

void hexDump(uint8_t *data, uint16_t len) {
  uint8_t cnt = 0;
  for (int i = 0; i < len; i++) {
    if (cnt == 0) {
      printf("%.4x: ", i & 0xFFF0);
    }
    printf("%.2x ", data[i]);
    cnt++;
    if (cnt == 8) {
      printf(" ");
    }
    if (cnt == 16) {
      printf("\n");
      cnt = 0;
    }
  }
  if (cnt != 0) {
    printf("\n");
  }
}

void handleIncomingMidi() {
  //    printf("foo\n");
  //    fflush(stdout);
  MidiUart.runLoop();
  while (MidiUart.avail()) {
    uint8_t c = MidiUart.getc();
    //      printf("%x\n", c);
    //      fflush(stdout);
    Midi.handleByte(c);
  }
  usleep(1000);
}

void onCCCallback(uint8_t *msg) {
  //  printf("%x %x %x\n", msg[0], msg[1], msg[2]);
}

void onNoteOnCallback(uint8_t *msg) {
  autoLearnLast4();
}

int main(int argc, char *argv[]) {
  // signal(SIGINT, SIG_IGN);
  MidiUart.init(1, 1);
  ccHandler.setup();
  Midi.addOnControlChangeCallback(onCCCallback);
  Midi.addOnNoteOnCallback(onNoteOnCallback);
  
  for (;;) {
    handleIncomingMidi();
  }

  return 0;
}
