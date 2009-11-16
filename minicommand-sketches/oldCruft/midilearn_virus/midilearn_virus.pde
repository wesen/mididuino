RangeEncoder param1(0, 127, "P1");
RangeEncoder param2(0, 127, "P2");
RangeEncoder param3(0, 127, "P3");
RangeEncoder param4(0, 127, "P4");
EncoderPage page(&param1, &param2, &param3, &param4);

#define NUM_HISTORY 4
RangeEncoder historyEncoder0(0, NUM_HISTORY-1, "HS1");
RangeEncoder historyEncoder1(0, NUM_HISTORY-1, "HS2");
RangeEncoder historyEncoder2(0, NUM_HISTORY-1, "HS3");
RangeEncoder historyEncoder3(0, NUM_HISTORY-1, "HS4");
EncoderPage historyPage(&historyEncoder0, &historyEncoder1, &historyEncoder2, &historyEncoder3);

RangeEncoder *encoders[4] = { &param1, &param2, &param3, &param4 };

bool isMidiLearn = false;
int midiLearnEncoder = 0;

int ccs[4] = { 1, 2, 3, 4};
int channels[4] = { 0, 0, 0, 0 };

int8_t record[NUM_HISTORY][4][64];
bool encoderMoved[4] = { false, false, false, false };

void clearRecord(int history, int num) {
  for (int i = 0; i < 64; i++) {
    record[history][num][i] = -1;
  }
}

void initRecord() {
  for (int history = 0; history < NUM_HISTORY; history++) {
    for (int i = 0; i < 4; i++) {
      clearRecord(history, i);
    }
  }
}

void on32Callback() {
  int pos = MidiClock.div32th_counter % 64;
  int currentHistory = historyEncoder.getValue();
  for (int i = 0; i < 4; i++) {
    if (encoderMoved[i] == true && BUTTON_DOWN(Buttons.BUTTON3)) {
       record[currentHistory][i][pos] = encoders[i]->getValue(); 
    } else {
      if (record[currentHistory][i][pos] != -1) {
        encoders[i]->setValue(record[currentHistory][i][pos]);
        MidiUart.sendCC(channels[i], ccs[i], record[currentHistory][i][pos]);
      }
    }
  }
}

void onCCCallback(uint8_t *msg) {
  if (isMidiLearn) {
    GUI.flash_string_fill("CC ");
    GUI.flash_put_value_at(4, msg[0] & 0xF);
    GUI.flash_put_value_at(8, msg[1]);
    ccs[midiLearnEncoder] = msg[1];
    channels[midiLearnEncoder] = msg[0] & 0xF;
    isMidiLearn = false;
  } else {
    for (int i = 0; i < 4; i++) {
      if (((msg[0] & 0xF) == channels[i]) && (msg[1] == ccs[i])) {
        encoders[i]->setValue(msg[2]);
        encoders[i]->redisplay = true;
      }
    }
  }
}

void setup() {
	Midi.setOnControlChangeCallback(onCCCallback);
	GUI.setPage(&page);
   MidiClock.mode = MidiClock.INTERNAL;
   MidiClock.transmit = true;
   MidiClock.setTempo(180);
   MidiClock.setOn32Callback(on32Callback);
   MidiUart.sendNoteOn(48, 100);    
   MidiClock.start();
   initRecord();
   delay(100);
}

void loop() {
	GUI.updatePage();
        for (int i= 0; i < 4; i++) {
           if (encoders[i]->hasChanged()) {
             if (BUTTON_DOWN(Buttons.BUTTON3)) {
               encoderMoved[i] = true;
             }
             MidiUart.sendCC(channels[i], ccs[i], encoders[i]->getValue());
           }
	}
	GUI.update();
}

bool noteSent = false;
void handleGui() {
  for (int i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
    if (BUTTON_PRESSED(i) && BUTTON_DOWN(Buttons.BUTTON1)) {
      isMidiLearn = true;
      midiLearnEncoder = i;
      GUI.flash_string_fill("MIDI LEARN");
      GUI.flash_put_value_at(12, i);
      break;
    }
    if (BUTTON_PRESSED(i) && BUTTON_DOWN(Buttons.BUTTON4)) {
      GUI.flash_string_fill("CLEAR REC");
      GUI.flash_put_value_at(11, i);
      clearRecord(historyEncoder.getValue(),  i);
    }
  }    

  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&historyPage);
  } else if BUTTON_RELEASED(Buttons.BUTTON2) {
    GUI.setPage(&page);
  }
  
  if (BUTTON_RELEASED(Buttons.BUTTON3)) {
    for (int i = 0; i < 4 ; i++) {
      encoderMoved[i] = false;
    }
  }
  
}
