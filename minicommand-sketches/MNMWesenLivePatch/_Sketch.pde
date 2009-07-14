class MNMWesenLivePatchSketch : 
public Sketch {
public:
  AutoMNMPage autoMNMPage;
  SwitchPage switchPage;

  uint8_t ramP1Track;

  void setupPages() {
    autoMNMPage.setup();
    autoMNMPage.setShortName("AUT");
    
    switchPage.initPages(NULL, NULL, NULL, &autoMNMPage);
    switchPage.parent = this;
  }

  virtual void setup() {
    setupPages();

    MNMTask.setup();
    MNMTask.autoLoadKit = true;
    MNMTask.reloadGlobal = true;
    MNMTask.addOnKitChangeCallback(_onKitChanged);
    MNMTask.addOnGlobalChangeCallback(_onGlobalChanged);
    GUI.addTask(&MNMTask);

    ccHandler.setup();
    //    ccHandler.setCallback(onLearnCallback);

//    MidiClock.mode = MidiClock.EXTERNAL_UART2;
//    MidiClock.transmit = true;
    MidiClock.mode = MidiClock.EXTERNAL;
    MidiClock.transmit = false;
    MidiClock.setOn32Callback(on32Callback);
    MidiClock.start();

    
    setPage(&autoMNMPage);
  }

  virtual void destroy() {
    MidiClock.stop();
    GUI.removeTask(&MNMTask);
    MNMTask.destroy();
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
    } else {
      return false;
    }

    return true;
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
        GUI.flash_string_fill(MNM.kit.name);
  }  


  void onGlobalChanged() {
    return;
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("GLOBAL CHANGED");
  }  


  virtual void loop() {
  }    
};

MNMWesenLivePatchSketch sketch;

void on32Callback() {
  sketch.autoMNMPage.on32Callback();
  //  GUI.flash_put_value(0, MidiClock.div32th_counter);
}

void _onKitChanged() {
  sketch.onKitChanged();
}

void _onGlobalChanged() {
  sketch.onGlobalChanged();
}

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
