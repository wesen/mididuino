
class RecorderPage : public EncoderPage, public ClockCallback {

  public: 
	CCEncoder recEqLowEncoder, recEqHighEncoder, recFilterBaseEncoder, recFilterWidthEncoder; 
        uint8_t recEqLow, recEqHigh, recFilterBase, recFilterWidth;
        bool recEqLowKill, recEqHighKill, recFilterBaseKill, recFilterWidthKill, restorePlayback, supaTriggaActive;
   
        RecorderPage():
        recEqLowEncoder(CC_EQ_LOW_GAIN, RECORDER_MIDI_CHANNEL, "LOW", 64), 
        recEqHighEncoder(CC_EQ_HI_GAIN, RECORDER_MIDI_CHANNEL, "HI", 64)  
//        recFilterBaseEncoder(CC_FX_1_PARAM_1, RECORDER_MIDI_CHANNEL, "BAS", 0), 
//        recFilterWidthEncoder(CC_FX_1_PARAM_2, RECORDER_MIDI_CHANNEL, "WDT", 127)          
        {
	    encoders[0] = &recEqLowEncoder;
	    encoders[1] = &recEqHighEncoder;
//	    encoders[2] = &recFilterBaseEncoder;
//	    encoders[3] = &recFilterWidthEncoder;
	}

	virtual void loop() {
//            displaySupaTriggerActive();
	}

//        void displaySupaTriggerActive(){
//            GUI.setLine(GUI.LINE1);
//            GUI.put_p_string_at(12, PSTR("SUP"));           
//            if (supaTriggaActive) {
//                GUI.setLine(GUI.LINE2);
//                GUI.put_p_string_at(12, PSTR("ON "));            
//            } else {
//                GUI.setLine(GUI.LINE2);
//                GUI.put_p_string_at(12, PSTR("OFF"));            
//            } 
//        }            

        void setup(){         
            recEqLow = recEqLowEncoder.getValue();
            recEqHigh = recEqHighEncoder.getValue();
            recEqLowKill = false;
            recEqHighKill = false;
            recFilterBase = recFilterBaseEncoder.getValue();
            recFilterWidth = recFilterWidthEncoder.getValue();
            recFilterBaseKill = false;
            recFilterWidthKill = false;            
            MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&RecorderPage::on16Callback);
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
        
        void centreEq(CCEncoder *encoder, bool& killEnabled){
            killEnabled = false;
            encoder->setValue(64, true);  
            encoder->name[3] = ' ';       
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
        
        void sliceTrack32(uint8_t midiChannel, uint8_t from, uint8_t to, bool correct = true) {
            uint8_t pfrom, pto;
            if (from > to) {
                pfrom = MIN(127, from * 4 + 1);
                //pto = MIN(127, to * 4);
                pto = 0;
            } else {
                pfrom = MIN(127, from * 4);
                //pto = MIN(127, to * 4);
                pto = 127;
                if (correct && pfrom >= 64)
                  pfrom++;
            }
            MidiUart.sendCC(midiChannel, CC_PLAYBACK_PARAM_2, pfrom);
            MidiUart.sendCC(midiChannel, CC_PLAYBACK_PARAM_4, pto);
            MidiUart.sendNoteOn(midiChannel, MIDI_NOTE_C3, 127);
        }
        
        void doSupatrigga() {
            uint8_t val = (MidiClock.div16th_counter) % 32;
            if ((val % 4) == 0) {
                uint8_t from = 0, to = 0;
                if (random(100) > 50) {
                  from = random(0, 6);
                  to = random(from + 2, 8);
                } 
                else {
                  from = random(2, 8);
                  to = random(0, from - 2);
                }
                sliceTrack32(RECORDER_MIDI_CHANNEL, from * 4, to * 4);
            }
        }
        
        void on16Callback() {

            if (restorePlayback) {
                uint8_t val = (MidiClock.div16th_counter) % 32;
                if ((val % 4) == 0) {
                  restorePlayback = false;
                  sliceTrack32(RECORDER_MIDI_CHANNEL, val, 127, true);
                  return;
                }
            }
            if (supaTriggaActive) {
                doSupatrigga();
            }                    
        }
        
        void startSupatrigga() {
            supaTriggaActive = true;
        }
        
        void stopSupatrigga() {
            restorePlayback = true;
            supaTriggaActive = false;
        }

	bool handleEvent(gui_event_t *event) {
   
            if (BUTTON_DOWN(Buttons.BUTTON2)) {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    centreEq(&recEqLowEncoder, recEqLowKill);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    centreEq(&recEqHighEncoder, recEqHighKill);
                    return true;
                }    
//                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
//                    resetFilter(&recFilterBaseEncoder, recFilterBaseKill);
//                    return true;
//                }    
//                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
//                    resetFilter(&recFilterWidthEncoder, recFilterWidthKill, true);
//                    return true;
//                }                  
            } else {
                if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                    toggleKill(&recEqLowEncoder, recEqLowKill, recEqLow);
                    return true;
                }    
                if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
                    toggleKill(&recEqHighEncoder, recEqHighKill, recEqHigh);
                    return true;
                }    
//                if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
//                    toggleKill(&recFilterBaseEncoder, recFilterBaseKill, recFilterBase);
//                    return true;
//                }    
//                if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
//                    toggleKill(&recFilterWidthEncoder, recFilterWidthKill, recFilterWidth, true);
//                    return true;
//                }                  
            }
            
            return false;

	}

};




