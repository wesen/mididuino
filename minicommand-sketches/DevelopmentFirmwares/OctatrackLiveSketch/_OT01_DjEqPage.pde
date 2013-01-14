class DjEqPage : public EncoderPage {

  public: 
	CCEncoder mdEqLowEncoder, mdEqHighEncoder, mnmEqLowEncoder, mnmEqHighEncoder; 
        uint8_t mdEqLow, mdEqHigh, mnmEqLow, mnmEqHigh;
        bool mdEqLowKill, mdEqHighKill, mnmEqLowKill, mnmEqHighKill;
   
        DjEqPage():
        mdEqLowEncoder(CC_FX_2_PARAM_4, MD_THRU_MIDI_CHANNEL, "LOW", 64), 
        mdEqHighEncoder(CC_FX_2_PARAM_6, MD_THRU_MIDI_CHANNEL, "HI", 64),  
        mnmEqLowEncoder(CC_FX_2_PARAM_4, MNM_THRU_MIDI_CHANNEL, "LOW", 64),  
        mnmEqHighEncoder(CC_FX_2_PARAM_6, MNM_THRU_MIDI_CHANNEL, "HI", 64)
        {
	    encoders[0] = &mdEqLowEncoder;
	    encoders[1] = &mdEqHighEncoder;
	    encoders[2] = &mnmEqLowEncoder;
	    encoders[3] = &mnmEqHighEncoder;
	}

	virtual void loop() {
//            if (mdEqLowEncoder.hasChanged()) {
//                mdEqLowEncoder.setValue(mdEqLowEncoder.getValue(), true);                       
//	    }
//            if (mdEqHighEncoder.hasChanged()) {
//                mdEqHighEncoder.setValue(mdEqHighEncoder.getValue(), true);    
//	    }
//            if (mnmEqLowEncoder.hasChanged()) {
//                mnmEqLowEncoder.setValue(mnmEqLowEncoder.getValue(), true);    
//	    }
//            if (mnmEqHighEncoder.hasChanged()) {
//                mnmEqHighEncoder.setValue(mnmEqHighEncoder.getValue(), true);    
//	    }
	}

        void setup(){         
            mdEqLow = mdEqLowEncoder.getValue();
            mdEqHigh = mdEqHighEncoder.getValue();
            mnmEqLow = mnmEqLowEncoder.getValue();
            mnmEqHigh = mnmEqHighEncoder.getValue();
            mdEqLowKill = false;
            mdEqHighKill = false;
            mnmEqLowKill = false;
            mnmEqHighKill = false;
        }      
      
        void toggleEqKill(CCEncoder *encoder, bool& killEnabled, uint8_t& value){
            uint8_t channel = encoder->getChannel();
            uint8_t cc = encoder->getCC();
            killEnabled = !killEnabled;

            if(killEnabled){
                value = encoder->getValue();  
                encoder->setValue(0, true); 
                encoder->name[3] = '*';      
            } else {
                encoder->setValue(value, true);
                encoder->name[3] = ' ';      
            }           
            redisplayPage ();
        }  
        
        void centreEq(CCEncoder *encoder, bool& killEnabled){
            killEnabled = false;
            encoder->setValue(64, true);  
            encoder->name[3] = ' ';        
            redisplayPage ();            
        }
    
	bool handleEvent(gui_event_t *event) {
   
            if (BUTTON_DOWN(Buttons.BUTTON2)) {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    centreEq(&mdEqLowEncoder, mdEqLowKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    centreEq(&mdEqHighEncoder, mdEqHighKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
                    centreEq(&mnmEqLowEncoder, mnmEqLowKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
                    centreEq(&mnmEqHighEncoder, mnmEqHighKill);
                    return true;
                }
            } else {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    toggleEqKill(&mdEqLowEncoder, mdEqLowKill, mdEqLow);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    toggleEqKill(&mdEqHighEncoder, mdEqHighKill, mdEqHigh);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
                    toggleEqKill(&mnmEqLowEncoder, mnmEqLowKill, mnmEqLow);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
                    toggleEqKill(&mnmEqHighEncoder, mnmEqHighKill, mnmEqHigh);
                    return true;
                }  
            }
            return false;

	}

};
