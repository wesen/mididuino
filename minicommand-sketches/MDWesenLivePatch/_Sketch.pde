class MDWesenLivePatchSketch : 
public Sketch {
public:
  MDFXEncoder flfEncoder, flwEncoder, fbEncoder, levEncoder;
  EncoderPage page;

  MDFXEncoder timEncoder, frqEncoder, modEncoder;  
  EncoderPage page2;

  MDEncoder pFlfEncoder, pFlwEncoder, pSrrEncoder, pVolEncoder;
  EncoderPage page4;
  
  BreakdownPage breakPage;
  AutoMDPage autoMDPage;
  SwitchPage switchPage;

  uint8_t ramP1Track;

  void setupPages() {
    flfEncoder.initMDFXEncoder(MD_ECHO_FLTF, MD_FX_ECHO, "FLF", 0);
    flwEncoder.initMDFXEncoder(MD_ECHO_FLTW, MD_FX_ECHO, "FLW", 127);
    fbEncoder.initMDFXEncoder( MD_ECHO_FB,   MD_FX_ECHO, "FB",  32);
    levEncoder.initMDFXEncoder(MD_ECHO_LEV,  MD_FX_ECHO, "LEV", 100);
    page.setShortName("DL1");
    page.setEncoders(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

    timEncoder.initMDFXEncoder(MD_ECHO_TIME, MD_FX_ECHO, "TIM", 24);
    frqEncoder.initMDFXEncoder(MD_ECHO_MFRQ, MD_FX_ECHO, "FRQ", 0);
    modEncoder.initMDFXEncoder(MD_ECHO_MOD,  MD_FX_ECHO, "MOD", 0);
    page2.setEncoders(&timEncoder, &frqEncoder, &modEncoder, &fbEncoder);
    page2.setShortName("DL2");

    pFlfEncoder.initMDEncoder(ramP1Track, MODEL_FLTF, "FLF", 0);
    pFlwEncoder.initMDEncoder(ramP1Track, MODEL_FLTW, "FLW", 127);
    pSrrEncoder.initMDEncoder(ramP1Track, MODEL_SRR, "SRR", 0);
    pVolEncoder.initMDEncoder(ramP1Track, MODEL_VOL, "VOL", 100);
    page4.setEncoders(&pFlfEncoder, &pFlwEncoder, &pSrrEncoder, &pVolEncoder);
    page4.setShortName("RAM");

    autoMDPage.setup();
    autoMDPage.setShortName("AUT");
    
    switchPage.initPages(&page, &page2, &page4, &autoMDPage);
    switchPage.parent = this;
  }

  virtual void setup() {
    setupPages();

    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(_onKitChanged);
    MDTask.addOnGlobalChangeCallback(_onGlobalChanged);
    GUI.addTask(&MDTask);

    for (int i = 0; i < 4; i++) {
      ccHandler.addEncoder((CCEncoder *)page4.encoders[i]);
    }
    ccHandler.setup();
    //    ccHandler.setCallback(onLearnCallback);

    MidiClock.mode = MidiClock.EXTERNAL_UART2;
    MidiClock.transmit = true;
//    MidiClock.mode = MidiClock.EXTERNAL;
//    MidiClock.transmit = false;
    
    MidiClock.setOn16Callback(_on16Callback);
    MidiClock.setOn32Callback(on32Callback);
    MidiClock.start();
    setPage(&page);
  }

  virtual void destroy() {
    MidiClock.stop();
    GUI.removeTask(&MDTask);
    MDTask.destroy();
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
    } else {
      if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
        pushPage(&breakPage);
      } 
      else if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
        popPage(&breakPage);
      } 
      else if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
        breakPage.startSupatrigga();
      } 
      else if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
        breakPage.stopSupatrigga();
      } 
      else {
        return false;
      }
    }

    return true;
  }

  void onKitChanged() {
    for (int i = 0; i < 16; i++) {
      if (MD.kit.machines[i].model == RAM_P1_MODEL) {
        GUI.setLine(GUI.LINE1);
        GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
//        GUI.flash_put_value(3, (uint8_t)i);
        GUI.setLine(GUI.LINE2);
        GUI.flash_string_fill(MD.kit.name);
        ramP1Track = i;
        breakPage.ramP1Track = i;
        break;
      }
    }
    for (int i = 0; i < 4; i++) {
      ((MDEncoder *)page4.encoders[i])->track = ramP1Track;
    }
    for (int i = 0; i < 4; i++) {
      ((MDFXEncoder *)page.encoders[i])->loadFromKit();
      ((MDFXEncoder *)page2.encoders[i])->loadFromKit();
      ((MDEncoder *)page4.encoders[i])->loadFromKit();
    }
  }  


  void onGlobalChanged() {
    return;
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("GLOBAL CHANGED");
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, MD.global.keyMap[36]);
    GUI.flash_put_value(1, MD.global.drumMapping[0]);
  }  


  virtual void loop() {
  }    
};

MDWesenLivePatchSketch sketch;

void on32Callback() {
  sketch.autoMDPage.on32Callback();
  //  GUI.flash_put_value(0, MidiClock.div32th_counter);
}

void _on16Callback() {
  sketch.breakPage.on16Callback();
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
