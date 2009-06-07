#include <SDCard.h>

uint8_t ccs[4][4];
uint8_t channels[4][4];
uint8_t mins[4][4];
uint8_t maxs[4][4];
char names[4][4][4];

CCEncoder ccEncoders[4][4];
EncoderPage pages[4];

bool midiLearnActive = false;
uint8_t midiLearnPage = 0;
uint8_t midiLearnEncoder = 0;

int currentCCPage = 0;
uint8_t currentEditedPage = 0;
uint8_t currentEditedEncoder = 0;

bool resetSettingsFlag = false;
bool loadSettingsFlag = false;

RangeEncoder configCCEncoder(0, 127, "CC");
RangeEncoder configChannelEncoder(0, 15, "CHN");
RangeEncoder configMinEncoder(0, 127, "MIN");
RangeEncoder configMaxEncoder(0, 127, "MAX");

RangeEncoder char1Encoder(0, 35);
RangeEncoder char2Encoder(0, 35);
RangeEncoder char3Encoder(0, 35);
RangeEncoder char4Encoder(0, 35);

void initSettings() {
  for (int page = 0; page < 4; page++) {
    for (int encoder = 0; encoder < 4; encoder++) {
      int cc = page * 4 + encoder;
      ccs[page][encoder] = cc;
      channels[page][encoder] = 0;
      names[page][encoder][0] = 'C';
      names[page][encoder][1] = (cc / 10) + '0' ;
      names[page][encoder][2] = (cc % 10) + '0';
      names[page][encoder][3] = '\0';
      mins[page][encoder] = 0;
      maxs[page][encoder] = 127;
    }
  }
  initEncoders();
}

void initEncoder(int page, int encoder) {
      ccEncoders[page][encoder].cc = ccs[page][encoder];
      ccEncoders[page][encoder].channel = channels[page][encoder];
      ccEncoders[page][encoder].setName(names[page][encoder]);
      ccEncoders[page][encoder].min = mins[page][encoder];
      ccEncoders[page][encoder].max = maxs[page][encoder];
      int value = ccEncoders[page][encoder].getValue();
      value = constrain(value, mins[page][encoder], maxs[page][encoder]);
      ccEncoders[page][encoder].setValue(value);
}

void initEncoders() {
 for (int page = 0; page < 4; page++) {
    for (int encoder = 0; encoder < 4; encoder++) {
      initEncoder(page, encoder);
      pages[page].encoders[encoder] = &ccEncoders[page][encoder];
    }
  }
}

void onCCCallback(uint8_t *msg) {
  if (midiLearnActive) {
    ccs[midiLearnPage][midiLearnEncoder] = msg[1];
    channels[midiLearnPage][midiLearnEncoder] = MIDI_VOICE_CHANNEL(msg[0]);
    ccEncoders[midiLearnPage][midiLearnEncoder].setValue(msg[2]);
    initEncoder(midiLearnPage, midiLearnEncoder);
    midiLearnActive = false;
    GUI.flash_p_strings_fill(PSTR("LEARNED ENC "), PSTR("CHN    , CC "));
    GUI.setLine(GUI.LINE1);
    GUI.flash_put_value_at(12, midiLearnEncoder);
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value_at(4, MIDI_VOICE_CHANNEL(msg[0]));
    GUI.flash_put_value_at(12, msg[1]);
    return;
  } 
 
 updateCCEncoder(msg[1], MIDI_VOICE_CHANNEL(msg[0]), msg[2]);
  
}

void updateCCEncoder(uint8_t cc, uint8_t channel, uint8_t value) {
  for (int page = 0; page < 4; page++) {
    for (int encoder = 0; encoder < 4; encoder++) {
      if (channel == channels[page][encoder] &&
            cc == ccs[page][encoder]) {
        ccEncoders[page][encoder].setValue(constrain(value, ccEncoders[page][encoder].min,
        ccEncoders[page][encoder].max));
      }
    }
  }
}

void showCCPage(int page) {
  GUI.flash_p_strings_fill(PSTR("PAGE"), PSTR(""));
  GUI.setLine(GUI.LINE1);
  GUI.flash_put_value_at(5, page);
  currentCCPage = page;
  GUI.setPage(&pages[page]);
}

void setup() {
  SDCard.init();
  initSettings();
  loadSettings();
  Midi.setOnControlChangeCallback(onCCCallback);
  showCCPage(currentCCPage);
}

void loop() {
  GUI.updatePage();

  for (int page = 0; page < 4; page++) {
    for (int encoder = 0; encoder < 4; encoder++) {
      if (ccEncoders[page][encoder].hasChanged()) {
        ccEncoders[page][encoder].checkHandle();
        updateCCEncoder(ccs[page][encoder], channels[page][encoder], ccEncoders[page][encoder].getValue());
      }
    }
  }
  
  GUI.update();
}

void handleGui() {
  if (!GUI.handleGui()) {
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      for (int i = 0; i < 4; i++) {
        if (BUTTON_PRESSED(Buttons.ENCODER1 + i)) {
          if (i != currentCCPage) {
            midiLearnActive = false;
          }
          showCCPage(i);
        }

        if (BUTTON_PRESSED(Buttons.BUTTON4)) {
          midiLearnActive = false;
          showConfigPage();
        }
      }
      return;
    }
    
    if (BUTTON_DOWN(Buttons.BUTTON3)) {
      for (int i = 0; i < 4; i++) {
        if (BUTTON_PRESSED(Buttons.ENCODER1 + i)) {
          if (midiLearnActive && midiLearnEncoder == i && midiLearnPage == currentCCPage) {
            midiLearnActive = false;
            GUI.flash_p_strings_fill(PSTR("MIDI LEARN OFF"), PSTR(""));
            return;
          }
          midiLearnActive = true;
          midiLearnEncoder = i;
          midiLearnPage = currentCCPage;
          GUI.flash_p_strings_fill(PSTR("MIDI LEARN ON"), PSTR("ENC "));
          GUI.flash_put_value_at(5, i);
        }
      }
    }
    
    if (BUTTON_DOWN(Buttons.BUTTON4)) {
      for (int i = 0; i < 4; i++) {
        if (BUTTON_PRESSED(Buttons.ENCODER1 + i)) {
          currentEditedPage = currentCCPage;
          midiLearnActive = false;
          showEncoderConfigPage(i);
        }
      }
    }
  }
}
