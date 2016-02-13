class Xone4DControllerSketch : public Sketch, public MDCallback {
public:
    bool muted;
    Xone4DController xone4DController;
    
    // Debug Page
//    void setupDebugPage(){
//        debugPage.setShortName("DBG");
//    }    
        
    // NoteRepeat Page    
    void setupNoteRepeatPage(){
        noteRepeatPage.setShortName("NTR");
    }
    
    // Rom Page
    void setupRomPage(){
        romPage.setShortName("ROM"); 
    }
            
    // Breakdown Page
    BreakdownPage breakPage;
    void setupBreakPage(){
       breakPage.setShortName("BRK"); 
       mdBreakdown.ramP1Track = ramP1Track; 
    }    
            
    // Delay Control, Ram P1 Control Pages
//    MDFXEncoder flfEncoder, flwEncoder, fbEncoder, levEncoder, timEncoder, frqEncoder, modEncoder;
//    EncoderPage mdDelayControlPage, mdDelayControlPage2;
    MDEncoder pFlfEncoder, pFlwEncoder, pSrrEncoder, pVolEncoder;
    EncoderPage ramP1ControlPage;
    void setupDelayRamP1ControlPages(){
//       flfEncoder.initMDFXEncoder(MD_ECHO_FLTF, MD_FX_ECHO, "FLF", 0);
//       flwEncoder.initMDFXEncoder(MD_ECHO_FLTW, MD_FX_ECHO, "FLW", 127);
//       fbEncoder.initMDFXEncoder( MD_ECHO_FB,   MD_FX_ECHO, "FB",  32);
//       levEncoder.initMDFXEncoder(MD_ECHO_LEV,  MD_FX_ECHO, "LEV", 100);
//       mdDelayControlPage.setShortName("DL1");
//       mdDelayControlPage.setEncoders(&flfEncoder, &flwEncoder, &levEncoder, &fbEncoder);
//       
//       // MD Delay Control Page 2
//       timEncoder.initMDFXEncoder(MD_ECHO_TIME, MD_FX_ECHO, "TIM", 24);
//       frqEncoder.initMDFXEncoder(MD_ECHO_MFRQ, MD_FX_ECHO, "FRQ", 0);
//       modEncoder.initMDFXEncoder(MD_ECHO_MOD,  MD_FX_ECHO, "MOD", 0);
//       mdDelayControlPage2.setShortName("DL2");
//       mdDelayControlPage2.setEncoders(&timEncoder, &frqEncoder, &modEncoder, &fbEncoder);
    
       // Ram P1 Page
       pFlfEncoder.initMDEncoder(ramP1Track, MODEL_FLTF, "FLF", 0);
       pFlwEncoder.initMDEncoder(ramP1Track, MODEL_FLTW, "FLW", 127);
       pSrrEncoder.initMDEncoder(ramP1Track, MODEL_SRR, "SRR", 0);
       pVolEncoder.initMDEncoder(ramP1Track, MODEL_VOL, "VOL", 127);
       ramP1ControlPage.setEncoders(&pFlfEncoder, &pFlwEncoder, &pSrrEncoder, &pVolEncoder);
       ramP1ControlPage.setShortName("RAM");
    }
    
    // Switch Pages
    SwitchPage switchPage;
    void setupSwitchPages(){
       switchPage.initPages(&noteRepeatPage, &romPage, &ramP1ControlPage, &breakPage);
//       switchPage.initPages(&noteRepeatPage, &debugPage, &ramP1ControlPage, &breakPage);       
//       switchPage.initPages(&noteRepeatPage, &mdDelayControlPage, &mdDelayControlPage2, &breakPage);
       switchPage.parent = this;

    }
    
    void getName(char *n1, char *n2) {
        m_strncpy_p(n1, PSTR("X4D "), 5);
        m_strncpy_p(n2, PSTR("CTR "), 5);
      }
    
    
    void setup() {
       muted = false;
       xone4DController.setup();
       MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&Xone4DControllerSketch::onKitChanged);
       setupDelayRamP1ControlPages();
       setupNoteRepeatPage();
       setupBreakPage();
//       setupDebugPage();
       setupSwitchPages(); 
       setupRomPage();
    }
    
      virtual void show() {
          if (currentPage() == &switchPage){
  		popPage(&switchPage);
          } 
          if (currentPage() == NULL){
  		setPage(&breakPage);
          }
      }
      
      virtual void hide() {
	  mdBreakdown.stopSupatrigga();
          if (currentPage() == &switchPage){
  		popPage(&switchPage);
          }
      }
      
      virtual void mute(bool pressed) {
        if (pressed) {
              muted = !muted;
              mdBreakdown.muted = muted;
              if (muted) {
        	  GUI.flash_strings_fill("X4D CONTROLLER:", "MUTED");
              } else {
        	  GUI.flash_strings_fill("X4D CONTROLLER:", "UNMUTED");
              }
        }
      }
      
      virtual void doExtra(bool pressed) {
        if (pressed) {
          mdBreakdown.startSupatrigga();
        } else {
          mdBreakdown.stopSupatrigga();
        }
      }
    
      virtual Page *getPage(uint8_t i) {
        if (i == 0) {
          return &breakPage;
        } else if (i == 1) {
          return &noteRepeatPage;
        } else if (i == 2) {
          return &ramP1ControlPage;
//        } else if (i == 3) {
//          return &mdDelayControlPage;
//        } else if (i == 4) {
//          return &mdDelayControlPage2;
        }else {
          return NULL;
        }
      }
    
    bool handleEvent(gui_event_t *event) {   
       
       if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
          GUI.pushPage(&switchPage);
       } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {   
          GUI.popPage(&switchPage);         
       } else {
           if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
                GUI.pushPage(&switchPage);
           } else if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
                GUI.popPage(&switchPage);
           }  else {
                if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
                  xone4DController.toggleMdRamP1TrackMute(mdRamP1TrackMuted);
//                  xone4DController.updateMuteLightRings();
                } else {
                    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
                        mdBreakdown.startSupatrigga();
                    } else if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
                        mdBreakdown.stopSupatrigga();
                    } else {
                        if (EVENT_PRESSED(event, Buttons.ENCODER4)) {          
                            triggerRamR1TrackFlag = true;                
                            GUI.flash_strings_fill("RAM R1 MACHINE", "TRIGGED ON BT 1");
                        } else {
                            return false;
                        }
                    }
                 }
           }
       }
       return true;
    }
    
    void onKitChanged() {
          GUI.setLine(GUI.LINE1);
          GUI.flash_p_string_fill(PSTR("LOADED MD KIT:"));
          GUI.setLine(GUI.LINE2);
          GUI.flash_string_fill(MD.kit.name);
          for (int i = 0; i < 16; i++) {
            if (MD.kit.models[i] == RAM_P1_MODEL) {
              ramP1Track = i;
              mdBreakdown.ramP1Track = i;
              continue;
            }
            if (MD.kit.models[i] == RAM_R1_MODEL) {
              ramR1Track = i;
              continue;
            }
          }          
          for (int i = 0; i < 4; i++) {
            ((MDEncoder *)ramP1ControlPage.encoders[i])->track = ramP1Track;
          }
//          for (int i = 0; i < 4; i++) {
////            ((MDFXEncoder *)mdDelayControlPage.encoders[i])->loadFromKit();
////            ((MDFXEncoder *)mdDelayControlPage2.encoders[i])->loadFromKit();
//            ((MDEncoder *)ramP1ControlPage.encoders[i])->loadFromKit();
//          }
      }              


};

