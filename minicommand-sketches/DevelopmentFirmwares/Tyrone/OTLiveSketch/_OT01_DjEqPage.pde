#define RECORDER_MIDI_CHANNEL (5 - 1)  // OT Track / Midi Channel 5
#define MD_THRU_MIDI_CHANNEL (6 - 1)  // OT Track / Midi Channel 6 
#define MNM_THRU_MIDI_CHANNEL (7 - 1) // OT Track / Midi Channel 7 
#define TETRA_MIDI_CHANNEL (12 - 1) // Midi Channel 12

#define CC_EQ_LOW_GAIN 37  //FX 1 PARAM 4
#define CC_EQ_HI_GAIN 39  //FX 1 PARAM 6

#define CC_DELAY_TIME 40  //FX 2 PARAM 1


#define CC_PLAYBACK_PARAM_2 17  //RECORDER START
#define CC_PLAYBACK_PARAM_4 19  //RECORDER RATE

class DjEqPage : public EncoderPage {

  public: 
	CCEncoder mdEqLowEncoder, mdEqHighEncoder, mnmEqLowEncoder, mnmEqHighEncoder; 
        uint8_t mdEqLow, mdEqHigh, mnmEqLow, mnmEqHigh;
        bool mdEqLowKill, mdEqHighKill, mnmEqLowKill, mnmEqHighKill;
   
        DjEqPage():
        mdEqLowEncoder(CC_EQ_LOW_GAIN, MD_THRU_MIDI_CHANNEL, "LOW", 64), 
        mdEqHighEncoder(CC_EQ_HI_GAIN, MD_THRU_MIDI_CHANNEL, "HI", 64),  
        mnmEqLowEncoder(CC_EQ_LOW_GAIN, MNM_THRU_MIDI_CHANNEL, "LOW", 64),  
        mnmEqHighEncoder(CC_EQ_HI_GAIN, MNM_THRU_MIDI_CHANNEL, "HI", 64)
        {
	    encoders[0] = &mdEqLowEncoder;
	    encoders[1] = &mdEqHighEncoder;
	    encoders[2] = &mnmEqLowEncoder;
	    encoders[3] = &mnmEqHighEncoder;
	}

	virtual void loop() {
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
