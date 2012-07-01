
class MonomachineLiveSketch : 
public Sketch, public MNMCallback, public ClockCallback, public MidiCallback {
public:  
  MNMEncoderPage autoMNMPages[4];
  SwitchPage switchPage;

  void setupPages() {
    for (int i = 0; i < 4; i++) {
      autoMNMPages[i].setup();
      autoMNMPages[i].setShortName("  ");
      autoMNMPages[i].shortName[2] = '0' + i + 1;
    }

    switchPage.initPages(&autoMNMPages[0], &autoMNMPages[1], &autoMNMPages[2], &autoMNMPages[3]);
    switchPage.parent = this;
  }

  virtual void setup() {
    setupPages();
    MNMTask.addOnKitChangeCallback(this, (mnm_callback_ptr_t)&MonomachineLiveSketch::onKitChanged);    
    ccHandler.setup();
    setPage(&autoMNMPages[0]);
//    Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&MonomachineLiveSketch::on3ByteCallback);
//    Midi.addOnNoteOffCallback(this, (midi_callback_ptr_t)&MonomachineLiveSketch::on3ByteCallback); 
//    Midi.addOnProgramChangeCallback(this, (midi_callback_ptr_t)&MonomachineLiveSketch::on2ByteCallback);   
  }

  void on3ByteCallback(uint8_t *msg) {
    MidiUart.sendMessage(msg[0], msg[1], msg[2]);
  }

  void on2ByteCallback(uint8_t *msg) {
    MidiUart.sendMessage(msg[0], msg[1]);
  }

  virtual void destroy() {
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
    } 
    if (BUTTON_DOWN(Buttons.BUTTON2)) {
      if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
        MNM.revertToCurrentKit(true);
        GUI.flash_strings_fill("REVERT TO KIT:", MNM.kit.name);
      } 
      else if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
        MNM.revertToCurrentTrack(true);
        GUI.flash_strings_fill("REVERT TO TRK ", "");
        GUI.setLine(GUI.LINE1);
        GUI.flash_put_value_at(14, MNM.currentTrack + 1);
        GUI.setLine(GUI.LINE2);
        GUI.flash_p_string_fill(MNM.getMachineName(MNM.kit.models[MNM.currentTrack]));
      }
      return true;
    } 

    return true;
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("LOADED MNM KIT:"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MNM.kit.name);
  }  

  void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("MNM "), 5);
    m_strncpy_p(n2, PSTR("LIV "), 5);
  }

  virtual void show() {
    if (currentPage() == NULL)
      setPage(&autoMNMPages[0]);
  }

  virtual void hide() {
      if (currentPage() == &switchPage){
          popPage(&switchPage);
      }
  }

  virtual void mute(bool pressed) {
  }

  virtual void doExtra(bool pressed) {
  }

  virtual Page *getPage(uint8_t i) {
    if (i < 4) {
      return &autoMNMPages[i];
    } else {
      return NULL;
    }
  }
  
};


