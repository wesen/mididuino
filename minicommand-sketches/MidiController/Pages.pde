void writeConfigSettings() {
  GUI.flash_p_strings_fill(PSTR(""), PSTR("WRITE SETTINGS"));
  writeSettings();
  showCCPage(currentCCPage);
}

void quitConfig() {
  GUI.flash_p_strings_fill(PSTR("QUIT UNSAVED"), PSTR(""));
  showCCPage(currentCCPage);
}

class ConfigPage : public EncoderPage {
  public:
  ConfigPage() {
  }
  
  virtual void display(bool redisplay = true) {
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
      GUI.put_p_string_fill(PSTR("EC1 EC2 EC3 EC4"));
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

RangeEncoder char1Encoder(0, 35);
RangeEncoder char2Encoder(0, 35);
RangeEncoder char3Encoder(0, 35);
RangeEncoder char4Encoder(0, 35);

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

RangeEncoder configCCEncoder(0, 127, "CC");
RangeEncoder configChannelEncoder(0, 15, "CHN");
RangeEncoder configMinEncoder(0, 127, "MIN");
RangeEncoder configMaxEncoder(0, 127, "MAX");

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
