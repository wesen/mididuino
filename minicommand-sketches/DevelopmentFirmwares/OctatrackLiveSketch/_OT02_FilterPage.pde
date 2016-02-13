class FilterPage : public EncoderPage {

  public: 
	CCEncoder mdFilterBaseEncoder, mdFilterWidthEncoder, mnmFilterBaseEncoder, mnmFilterWidthEncoder; 
        uint8_t mdFilterBase, mdFilterWidth, mnmFilterBase, mnmFilterWidth;
        bool mdFilterBaseKill, mdFilterWidthKill, mnmFilterBaseKill, mnmFilterWidthKill;
   
        FilterPage():
        mdFilterBaseEncoder(CC_FX_1_PARAM_1, MD_THRU_MIDI_CHANNEL, "BAS", 0), 
        mdFilterWidthEncoder(CC_FX_1_PARAM_2, MD_THRU_MIDI_CHANNEL, "WDT", 127),  
        mnmFilterBaseEncoder(CC_FX_1_PARAM_1, MNM_THRU_MIDI_CHANNEL, "BAS", 0),  
        mnmFilterWidthEncoder(CC_FX_1_PARAM_2, MNM_THRU_MIDI_CHANNEL, "WDT", 127)
        {
	    encoders[0] = &mdFilterBaseEncoder;
	    encoders[1] = &mdFilterWidthEncoder;
	    encoders[2] = &mnmFilterBaseEncoder;
	    encoders[3] = &mnmFilterWidthEncoder;
	}

	virtual void loop() {
//            if (mdFilterBaseEncoder.hasChanged()) {
//                mdFilterBaseEncoder.setValue(mdFilterBaseEncoder.getValue(), true);                       
//	    }
//            if (mdFilterWidthEncoder.hasChanged()) {
//                mdFilterWidthEncoder.setValue(mdFilterWidthEncoder.getValue(), true);    
//	    }
//            if (mnmFilterBaseEncoder.hasChanged()) {
//                mnmFilterBaseEncoder.setValue(mnmFilterBaseEncoder.getValue(), true);    
//	    }
//            if (mnmFilterWidthEncoder.hasChanged()) {
//                mnmFilterWidthEncoder.setValue(mnmFilterWidthEncoder.getValue(), true);    
//	    }
	}

        void setup(){         
            mdFilterBase = mdFilterBaseEncoder.getValue();
            mdFilterWidth = mdFilterWidthEncoder.getValue();
            mnmFilterBase = mnmFilterBaseEncoder.getValue();
            mnmFilterWidth = mnmFilterWidthEncoder.getValue();
            mdFilterBaseKill = false;
            mdFilterWidthKill = false;
            mnmFilterBaseKill = false;
            mnmFilterWidthKill = false;
        }      

        // filterWidth must be passed in as True when we are manipulating the filter Width param    
        void toggleKill(CCEncoder *encoder, bool& killEnabled, uint8_t& value, bool filterWidth = false){
            uint8_t channel = encoder->getChannel();
            uint8_t cc = encoder->getCC();
            killEnabled = !killEnabled;   

            if(killEnabled){
                value = encoder->getValue();
                encoder->name[3] = '*';      
                
                // set to 127 when "killing" / manipulating filter Width, otherwise set to 0
                if (filterWidth){  
                    encoder->setValue(127, true); 
                } else {
                    encoder->setValue(0, true); 
                }                
            } else {
                encoder->setValue(value, true);
                encoder->name[3] = ' ';                
            }
            redisplayPage ();            
        }          
        
        void resetFilter(CCEncoder *encoder, bool& killEnabled, bool filterWidth = false){
            killEnabled = false;
            if (filterWidth){  
                encoder->setValue(127, true);  
            } else {
                encoder->setValue(0, true);                
            }
            encoder->name[3] = ' ';       
            redisplayPage ();                
        }          
    
	bool handleEvent(gui_event_t *event) {
  
            if (BUTTON_DOWN(Buttons.BUTTON2)) {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    resetFilter(&mdFilterBaseEncoder, mdFilterBaseKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    resetFilter(&mdFilterWidthEncoder, mdFilterWidthKill, true);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
                    resetFilter(&mnmFilterBaseEncoder, mnmFilterBaseKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
                    resetFilter(&mnmFilterWidthEncoder, mnmFilterWidthKill, true);
                    return true;
                }                  
            } else {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    toggleKill(&mdFilterBaseEncoder, mdFilterBaseKill, mdFilterBase);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    toggleKill(&mdFilterWidthEncoder, mdFilterWidthKill, mdFilterWidth, true);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
                    toggleKill(&mnmFilterBaseEncoder, mnmFilterBaseKill, mnmFilterBase);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
                    toggleKill(&mnmFilterWidthEncoder, mnmFilterWidthKill, mnmFilterWidth, true);
                    return true;
                }   
            }
            return false;

	}

};
