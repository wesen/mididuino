#include <MNM.h>
#include <MD.h>
#include <CCHandler.h>

CCHandler ccHandler;

class AutoMDPage : 
public EncoderPage {
public:
  MDEncoder mdEncoders[4];
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
      recEncoders[i].stopRecording();
    }
  }

  void clearRecording(int i) {
    recEncoders[i].clearRecording();
  }

  virtual void setup() {
    for (int i = 0; i < 4; i++) {
      mdEncoders[i].setName("___");
      recEncoders[i].initRecordingEncoder(&mdEncoders[i]);
      encoders[i] = &recEncoders[i];
      ccHandler.addEncoder(&mdEncoders[i]);
    }
  }

  void autoLearnLast4() {
    int8_t ccAssigned[4] = { 
      -1, -1, -1, -1         };
    int8_t encoderAssigned[4] = { 
      -1, -1, -1, -1         };
    incoming_cc_t ccs[4];
    int count = ccHandler.incomingCCs.size();
    for (int i = 0; i < count; i++) {
      ccHandler.incomingCCs.getCopy(i, &ccs[i]);
      incoming_cc_t *cc = &ccs[i];
      for (int j = 0; j < 4; j++) {
        if ((mdEncoders[j].getCC() == cc->cc) &&
          (mdEncoders[j].getChannel() == cc->channel)) {
          ccAssigned[i] = j;
          encoderAssigned[j] = i;
          break;
        }
      }
    }

#if 0
    for (int i = 0; i < count; i++) {
      GUI.setLine(GUI.LINE1);
      //      GUI.flash_put_value(i, ccAssigned[i], 1800);
      GUI.flash_put_value(i, mdEncoders[i].getCC(), 1800);
      GUI.setLine(GUI.LINE2);
      GUI.flash_put_value(i, ccs[i].cc, 1800);
    }
#endif

    for (int i = 0; i < count; i++) {
      incoming_cc_t *cc = &ccs[i];
      if (ccAssigned[i] != -1) {
        if ((mdEncoders[ccAssigned[i]].getChannel() != cc->channel) &&
          (mdEncoders[ccAssigned[i]].getCC() != cc->cc)) {
          mdEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
          mdEncoders[ccAssigned[i]].setValue(cc->value);
          clearRecording(ccAssigned[i]);
        }
      } else {
        for (int j = 0; j < 4; j++) {
          if (encoderAssigned[j] == -1) {
            ccAssigned[i] = j;
            encoderAssigned[j] = i;
//            if ((mdEncoders[ccAssigned[i]].getChannel() != cc->channel) &&
//                (mdEncoders[ccAssigned[i]].getCC() != cc->cc)) {
              mdEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
              mdEncoders[ccAssigned[i]].setValue(cc->value);
              clearRecording(ccAssigned[i]);
//            }
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

