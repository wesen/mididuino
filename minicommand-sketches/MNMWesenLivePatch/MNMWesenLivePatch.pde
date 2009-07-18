#include <MNM.h>
#include <CCHandler.h>

CCHandler ccHandler;

class MagicMNMPage : 
public EncoderPage {
public:
  uint8_t track;
  MNMEncoder mnmEncoders[4];
  RecordingEncoder<64> recEncoders[4];
  uint8_t params[4];

  void on32Callback() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].playback(MidiClock.div32th_counter);
    }
  }

  void startRecording() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].startRecording();
    }
  }

  void stopRecording() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].startRecording();
    }
  }

  void clearRecording(int i) {
    recEncoders[i].clearRecording();
  }

  virtual void show() {
    if (track != 255) {
      GUI.flash_strings_fill("MAGIC PAGE ", "");
      GUI.setLine(GUI.LINE2);
      GUI.flash_p_string_fill(MNM.getMachineName(MNM.kit.machines[track].model));
      GUI.setLine(GUI.LINE1);
      GUI.flash_put_value_at(11, track + 1);
    }
  }

  virtual void setup() {
    track = 255;
    for (int i = 0; i < 4; i++) {
      mnmEncoders[i].setName("___");
      recEncoders[i].initRecordingEncoder(&mnmEncoders[i]);
      encoders[i] = &recEncoders[i];
      ccHandler.addEncoder(&mnmEncoders[i]);
    }
  }

  void setup(uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) {
    setup();
    params[0] = param1;
    params[1] = param2;
    params[2] = param3;
    params[3] = param4;
  }

  void setTrack(uint8_t _track) {
    track = _track;
    for (int i = 0; i < 4; i++) {
      mnmEncoders[i].initMNMEncoder(track, params[i], NULL);
    }
  }

  void setToCurrentTrack() {
    uint8_t currentTrack = MNM.getCurrentTrack();
    if (currentTrack == 255) {
      GUI.flash_strings_fill("MNM TIMEOUT", "");
    } 
    else {
      show();
      setTrack(currentTrack);
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      setToCurrentTrack();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      startRecording();
      return true;
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
      stopRecording();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4) || EVENT_RELEASED(event, Buttons.BUTTON4)) {
      return true;
    }

    if (BUTTON_DOWN(Buttons.BUTTON4)) {
      for (int i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
        if (EVENT_PRESSED(event, i)) {
          GUI.setLine(GUI.LINE1);
          GUI.flash_string_fill("CLEAR");
          GUI.setLine(GUI.LINE2);
          GUI.flash_put_value(0, i);
          clearRecording(i);
        }
      }
    }
    return false;
  }
};

class MagicSwitchPage : 
public SwitchPage {
public:
  MagicMNMPage magicPages[4];
  Page *currentPage;
  bool selectPage;

  MagicSwitchPage() : 
  SwitchPage("MAGIC PAGE:", 
  &magicPages[0], &magicPages[1], &magicPages[2], &magicPages[3]) {
    currentPage = NULL;

    magicPages[0].setup(MNM_MODEL_AMP_DEC, MNM_MODEL_FILT_WDTH, MNM_MODEL_FX_DSND, MNM_MODEL_FX_DFB);
    magicPages[0].setShortName("STD");
    magicPages[1].setup(MNM_MODEL_FX_DSND, MNM_MODEL_FX_DFB, MNM_MODEL_FX_DBAS, MNM_MODEL_FX_DWID);
    magicPages[1].setShortName("DEL");
    magicPages[2].setup(MNM_MODEL_FILT_BASE, MNM_MODEL_FILT_WDTH, MNM_MODEL_FILT_DEC, MNM_MODEL_FILT_WOFS);
    magicPages[2].setShortName("FLT");
    magicPages[3].setup(MNM_MODEL_AMP_DEC, MNM_MODEL_LFO1_DPTH, MNM_MODEL_LFO2_DPTH, MNM_MODEL_LFO3_DPTH);
    magicPages[3].setShortName("LFO");

    setPage(&magicPages[0]);
    selectPage = false;
  }
  
  void setup() {
  }

  void setPage(Page *page) {
    if (currentPage != page) {
      currentPage = page;
      currentPage->redisplayPage();
      show();
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      selectPage = true;
      redisplayPage();
      return true;
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
      selectPage = false;
      if (currentPage != NULL) {
        redisplayPage();
      }
      return true;
    }
    if (selectPage) {
      for (int i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
        if (pages[i] != NULL && EVENT_PRESSED(event, i)) {
          setPage(pages[i]);
          return true;
        }
      }
      if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
        setToCurrentTrack();
        return true;
      }
    }

    if (currentPage != NULL) {
      return currentPage->handleEvent(event);
    } else {
      return false;
    }
  }

  void setToCurrentTrack() {
    uint8_t currentTrack = MNM.getCurrentTrack();
    if (currentTrack == 255) {
      GUI.flash_strings_fill("MNM TIMEOUT", "");
    } 
    else {
      GUI.flash_strings_fill("MAGIC PAGE ", "");
      GUI.setLine(GUI.LINE2);
      GUI.flash_p_string_fill(MNM.getMachineName(MNM.kit.machines[currentTrack].model));
      GUI.setLine(GUI.LINE1);
      GUI.flash_put_value_at(11, currentTrack + 1);

      for (int i = 0; i < 4; i++) {
        magicPages[i].setTrack(currentTrack);
      }
    }
  }

  virtual void display() {
    if (currentPage != NULL && !selectPage) {
      currentPage->display();
    } else {
      SwitchPage::display();
    }
  }
  
  virtual void show() {
    if (currentPage != NULL) {
      currentPage->show();
    }
  }
  
  virtual void hide() {
    if (currentPage != NULL) {
      currentPage->show();
    }
  }
  
};

class AutoMNMPage : 
public EncoderPage {
public:
  MNMEncoder mnmEncoders[4];
  RecordingEncoder<64> recEncoders[4];

  void on32Callback() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].playback(MidiClock.div32th_counter);
    }
  }

  void startRecording() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].startRecording();
    }
  }

  void stopRecording() {
    for (int i = 0; i < 4; i++) {
      recEncoders[i].startRecording();
    }
  }

  void clearRecording(int i) {
    recEncoders[i].clearRecording();
  }

  virtual void setup() {
    for (int i = 0; i < 4; i++) {
      mnmEncoders[i].setName("___");
      recEncoders[i].initRecordingEncoder(&mnmEncoders[i]);
      encoders[i] = &recEncoders[i];
      ccHandler.addEncoder(&mnmEncoders[i]);
    }
  }

  void autoLearnLast4() {
    int8_t ccAssigned[4] = { 
      -1, -1, -1, -1             };
    int8_t encoderAssigned[4] = { 
      -1, -1, -1, -1             };
    incoming_cc_t ccs[4];
    for (int i = 0; i < 4; i++) {
      ccHandler.incomingCCs.getCopy(i, &ccs[i]);
      incoming_cc_t *cc = &ccs[i];
      for (int j = 0; j < 4; j++) {
        if ((mnmEncoders[j].getCC() == cc->cc) &&
          (mnmEncoders[j].getChannel() == cc->channel)) {
          ccAssigned[i] = j;
          encoderAssigned[j] = i;
          break;
        }
      }
    }

#if 0
    for (int i = 0; i < 4; i++) {
      GUI.setLine(GUI.LINE1);
      //      GUI.flash_put_value(i, ccAssigned[i], 1800);
      GUI.flash_put_value(i, mnmEncoders[i].getCC(), 1800);
      GUI.setLine(GUI.LINE2);
      GUI.flash_put_value(i, ccs[i].cc, 1800);
    }
#endif

    for (int i = 0; i < 4; i++) {
      incoming_cc_t *cc = &ccs[i];
      if (ccAssigned[i] != -1) {
        mnmEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
        mnmEncoders[ccAssigned[i]].setValue(cc->value);
        clearRecording(ccAssigned[i]);
      } 
      else {
        for (int j = 0; j < 4; j++) {
          if (encoderAssigned[j] == -1) {
            ccAssigned[i] = j;
            encoderAssigned[j] = i;
            mnmEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
            mnmEncoders[ccAssigned[i]].setValue(cc->value);
            clearRecording(ccAssigned[i]);
            break;
          }
        }
      }
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      autoLearnLast4();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      startRecording();
      return true;
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
      stopRecording();
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4) || EVENT_RELEASED(event, Buttons.BUTTON4)) {
      return true;
    }
    if (BUTTON_DOWN(Buttons.BUTTON4)) {
      for (int i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
        if (EVENT_PRESSED(event, i)) {
          GUI.setLine(GUI.LINE1);
          GUI.flash_string_fill("CLEAR");
          GUI.setLine(GUI.LINE2);
          GUI.flash_put_value(0, i);
          clearRecording(i);
        }
      }
    }
    return false;
  }
};

