#include <SDCard.h>

namespace MidiController {
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

void initEncoders();
void updateCCEncoder(uint8_t cc, uint8_t channel, uint8_t value);
void showPageConfigPage(int page);
void showConfigPage();
void showEncoderConfigPage(int encoder);
void showNameEditPage();

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

bool writeSettings() {
  SDCardFile file;
  if (!file.create("/MidiController/settings")) {
    return false;
  }
  
  if (file.write((unsigned char *)ccs, sizeof(ccs)) != sizeof(ccs))
    goto end;
  if (file.write((unsigned char *)channels, sizeof(channels)) != sizeof(channels))
    goto end;
  if (file.write((unsigned char *)mins, sizeof(mins)) != sizeof(mins))
    goto end;
  if (file.write((unsigned char *)maxs, sizeof(maxs)) != sizeof(maxs))
    goto end;
  if (file.write((unsigned char *)names, sizeof(names)) != sizeof(names))
    goto end;
  file.close();
  return true;

end:
  file.close();
  return false;
}

bool loadSettings() {
  SDCardFile file;
  if (!file.open("/MidiController/settings")) {
    initSettings();
    writeSettings();
    return false;
  }
  
  if (file.read((unsigned char *)ccs, sizeof(ccs)) != sizeof(ccs))
    goto end;
  if (file.read((unsigned char *)channels, sizeof(channels)) != sizeof(channels))
    goto end;
  if (file.read((unsigned char *)mins, sizeof(mins)) != sizeof(mins))
    goto end;
  if (file.read((unsigned char *)maxs, sizeof(maxs)) != sizeof(maxs))
    goto end;
  if (file.read((unsigned char *)names, sizeof(names)) != sizeof(names))
    goto end;
  
  file.close();
  initEncoders();
  return true;
  
end:
  file.close();
  initSettings();
  writeSettings();
  return false;
}

void writeConfigSettings() {
  writeSettings();
  showCCPage(currentCCPage);
  GUI.flash_p_strings_fill(PSTR(""), PSTR("WRITE SETTINGS"));
}

void quitConfig() {
  showCCPage(currentCCPage);
  GUI.flash_p_strings_fill(PSTR("QUIT UNSAVED"), PSTR(""));
}

class ConfigPage : public EncoderPage {
  public:
  ConfigPage() {
  }
  
  virtual void display(bool redisplay = true) {
    if (resetSettingsFlag) {
      int key = showModalGui_p(PSTR("RESET ALL CCS?"), PSTR("YES           NO"));
      if (key == 0) {
        initSettings();
        writeConfigSettings();
        showCCPage(currentCCPage);
        GUI.flash_p_strings_fill(PSTR("RESETTING"), PSTR(""));
      }
      resetSettingsFlag = false;
    }
    if (loadSettingsFlag) {
      int key = showModalGui_p(PSTR("LOAD SETTINGS?"), PSTR("YES           NO"));
      if (key == 0) {
        loadSettings();
        showCCPage(currentCCPage);
        GUI.flash_p_strings_fill(PSTR("LOADING"), PSTR(""));
      }
      loadSettingsFlag = false;
    }
    
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("SELECT PAGE"));
      GUI.setLine(GUI.LINE2);
      GUI.put_p_string_fill(PSTR("PG1 PG2 PG3 PG4"));
    }
  }
  
  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON4)) {
      writeConfigSettings();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      quitConfig();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      resetSettingsFlag = true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadSettingsFlag = true;
    }
    
    for (int i = 0; i < 4; i++) {
      if (BUTTON_PRESSED(Buttons.ENCODER1 + i)) {
        showPageConfigPage(i);
      }
    }
    
    return true;
  }
};

ConfigPage configPage;

void showConfigPage() {
  GUI.flash_p_strings_fill(PSTR("CONFIG PAGE"), PSTR(""));
  GUI.setPage(&configPage);
}

class PageConfigPage : public EncoderPage {
  public:
  PageConfigPage() {
  }

  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("SELECT ENCODER"));
      GUI.setLine(GUI.LINE2);
      char buf[17];
      buf[16] = '\0';
      for (int i = 0; i < 4; i++) {
        m_strncpy(buf + i * 4, ccEncoders[currentEditedPage][i].getName(), 3);
        buf[i*4 + 3] = ' ';
      }
      GUI.put_string_fill(buf);
    }
  }
  
  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON4)) {
      writeConfigSettings();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      quitConfig();
      return true;
    }

    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      showConfigPage();      
      return true;
    }
    
    for (int i = 0; i < 4; i++) {
      if (BUTTON_PRESSED(Buttons.ENCODER1 + i)) {
        showEncoderConfigPage(i);
      }
    }
    
    return true;
  }
};

PageConfigPage pageConfigPage;

void showPageConfigPage(int page) {
  GUI.flash_p_strings_fill(PSTR("EDIT PAGE"), PSTR(""));
  GUI.setLine(GUI.LINE1);
  GUI.flash_put_value_at(10, page);
  currentEditedPage = page;
  GUI.setPage(&pageConfigPage);
}

char encoderToChar(uint8_t val) {
  if (val < 26) 
    return val + 'A';
  else
    return (val - 26) + '0';
}

uint8_t charToEncoder(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A';
  }
  if (c >= '0' && c <= '9') {
    return c - '0' + 26;
  } else {
    return '0';
  }
}

class NameEditPage : public EncoderPage {
public:
  NameEditPage() {
    encoders[0] = &char1Encoder;
    encoders[1] = &char2Encoder;
    encoders[2] = &char3Encoder;
    encoders[3] = &char4Encoder;
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("EDIT NAME "));
      GUI.put_value_at(10, currentEditedEncoder);
    }
    
    bool refresh = false;
    for (int i = 0; i < 3; i++) {
      if (encoders[i]->hasChanged()) {
        refresh = true;
      }
    }
    
    if (refresh) {
      for (int i = 0; i < 3; i++) {
        names[currentEditedPage][currentEditedEncoder][i] = encoderToChar(encoders[i]->getValue());
      }
      ccEncoders[currentEditedPage][currentEditedEncoder].setName(
          names[currentEditedPage][currentEditedEncoder]);
    }
    
    if (refresh || redisplay) {
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(ccEncoders[currentEditedPage][currentEditedEncoder].getName());
    }
  }
  
  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON4)) {
      writeConfigSettings();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      quitConfig();
      return true;
    }

    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      showEncoderConfigPage(currentEditedEncoder);
    }
    return true;
  }
};

NameEditPage nameEditPage;

void showNameEditPage() {
  for (int i = 0; i < 3; i++) {
    nameEditPage.encoders[i]->setValue(
        charToEncoder(names[currentEditedPage][currentEditedEncoder][i]));
  }
  GUI.setPage(&nameEditPage);
}

class EncoderConfigPage : public EncoderPage {
  public:
  EncoderConfigPage() {
    encoders[0] = &configCCEncoder;
    encoders[1] = &configChannelEncoder;
    encoders[2] = &configMinEncoder;
    encoders[3] = &configMaxEncoder;
  }

  virtual void display(bool redisplay = true) {
    EncoderPage::display(redisplay);
    if (configCCEncoder.hasChanged()) {
      ccs[currentEditedPage][currentEditedEncoder] = configCCEncoder.getValue();      
    }
    if (configChannelEncoder.hasChanged()) {
      channels[currentEditedPage][currentEditedEncoder] = configChannelEncoder.getValue();      
    }
    if (configMinEncoder.hasChanged()) {
      mins[currentEditedPage][currentEditedEncoder] = configMinEncoder.getValue();      
    }
    if (configMaxEncoder.hasChanged()) {
      maxs[currentEditedPage][currentEditedEncoder] = configMaxEncoder.getValue();      
    }
    initEncoder(currentEditedPage, currentEditedEncoder);
  }
  
  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON4)) {
      writeConfigSettings();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      quitConfig();
      return true;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      showNameEditPage();
    }
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      showPageConfigPage(currentEditedPage);
    }
    
    return true;
  }
};

EncoderConfigPage encoderConfigPage;

void showEncoderConfigPage(int encoder) {
  GUI.flash_p_strings_fill(PSTR("EDIT ENC"), PSTR(""));
  GUI.setLine(GUI.LINE1);
  GUI.flash_put_value_at(9, encoder);
  currentEditedEncoder = encoder;
  encoderConfigPage.encoders[0]->setValue(ccs[currentEditedPage][currentEditedEncoder]);
  encoderConfigPage.encoders[1]->setValue(channels[currentEditedPage][currentEditedEncoder]);
  encoderConfigPage.encoders[2]->setValue(mins[currentEditedPage][currentEditedEncoder]);
  encoderConfigPage.encoders[3]->setValue(maxs[currentEditedPage][currentEditedEncoder]);
  GUI.setPage(&encoderConfigPage);
}



  class MidiControllerSketch : public SubSketch {
    public:

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

void destroy() {
  Midi.setOnControlChangeCallback(NULL);
}
PGM_P getName() {
  return PSTR("MIDI CONTROLLER");
}
  };
  
  MidiControllerSketch sketch;
}
