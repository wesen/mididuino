#include <MD.h>
#include <CCHandler.h>

void onLearnCallback(CCEncoder *enc) {
  uint8_t track, param;
  if (MD.loadedGlobal && MD.loadedKit) {
    MD.parseCC(enc->channel, enc->cc, &track, &param);
    PGM_P name = NULL;
    name = model_param_name(MD.kit.machines[track].model, param);
    char myName[4];
    m_strncpy_p(myName, name, 4);
    enc->setName(myName);
    GUI.currentPage()->redisplay = true;
    if (name != NULL) {
      GUI.flash_p_strings_fill(MD.getMachineName(MD.kit.machines[track].model), name);
    }
  }
}

CCHandler ccHandler;
CCEncoder ccEnc(0, 0, "CC");
CCEncoder ccEnc2(0, 0, "CC");
EncoderPage page(&ccEnc, &ccEnc2);

class CCHandlerTestSketch : 
public Sketch {
public:

  virtual void setup() {
    MDTask.setup();
    MDTask.interval = 3000;
    MDTask.autoLoadKit = true;
    GUI.addTask(&MDTask);
    
    ccHandler.setup();
    ccHandler.setCallback(onLearnCallback);
    ccHandler.addEncoder(&ccEnc);
    ccHandler.addEncoder(&ccEnc2);
    pushPage(&page);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
        ccHandler.midiLearn(&ccEnc);
        return true;
      }
      if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
        ccHandler.midiLearn(&ccEnc2);
        return true;
      }
    }
    return false;
  }

  virtual void loop() {
  }
};

CCHandlerTestSketch sketch;

bool handleTest(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    MidiUart.sendNoteOn(100, 100);
    GUI.flash_strings_fill("HELLO", "DUDE");
  }
}

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
  GUI.addEventHandler(handleTest);
}

void loop() {
}
