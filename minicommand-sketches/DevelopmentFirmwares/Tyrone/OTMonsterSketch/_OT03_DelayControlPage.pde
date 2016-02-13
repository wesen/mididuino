#define DELAY_TIMES_COUNT 5
#define CYCLE_MODE_COUNT 3

class DelayControlPage : public EncoderPage, MidiCallback, ClockCallback {

  public: 
	RangeEncoder cycleIntervalEncoder, debugEncoder;
        EnumEncoder delayTimeEncoder, maxDelayTimeEncoder, cycleModeEncoder;
        uint8_t delayTime, maxDelayTime, cycleMode, interval;
        bool cycleEnabled, tripletsEnabled;
        
        static const uint8_t delayTimeValues[DELAY_TIMES_COUNT];             
        static const char *delayTimeNames[DELAY_TIMES_COUNT]; 
        static const uint8_t delayTimeTripletValues[DELAY_TIMES_COUNT];             
        static const char *delayTimeTripletNames[DELAY_TIMES_COUNT]; 
        static const char *cycleModeNames[CYCLE_MODE_COUNT];       
       
   
        DelayControlPage():
        delayTimeEncoder(delayTimeNames, DELAY_TIMES_COUNT, "TIM", 0), 
        maxDelayTimeEncoder(delayTimeNames, DELAY_TIMES_COUNT, "MAX", 0), 
        cycleModeEncoder(cycleModeNames, CYCLE_MODE_COUNT, "CYC", 0),  
        cycleIntervalEncoder(1, 64, "INT", 4),
        debugEncoder(0, 127, "DBG", 0)
        {
	    encoders[0] = &cycleModeEncoder;
	    encoders[1] = &cycleIntervalEncoder;
	    encoders[2] = &delayTimeEncoder;
            encoders[3] = &maxDelayTimeEncoder;
            //encoders[3] = &debugEncoder;
	}

	virtual void loop() {
            if (delayTimeEncoder.hasChanged()) {
                delayTime = MIN(delayTimeEncoder.getValue(), maxDelayTime);
                delayTimeEncoder.setValue(delayTime, true);
                setDelayTime();                       
	    }
            if (maxDelayTimeEncoder.hasChanged()) {
                maxDelayTime = maxDelayTimeEncoder.getValue();                       
	    }
            if (cycleModeEncoder.hasChanged()) {
                cycleMode = cycleModeEncoder.getValue();                       
	    }
            if (cycleIntervalEncoder.hasChanged()) {
                interval = cycleIntervalEncoder.getValue();                       
	    }           
	}

        void setup(){         
            delayTime = 0;
            maxDelayTime = DELAY_TIMES_COUNT;
            cycleMode = 0;
            interval = 4;
            cycleEnabled = false;
            tripletsEnabled = false;
            updateEncoders();
            setDelayTime(); 
            MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&DelayControlPage::on16Callback);
        }      

        void updateEncoders(){
            delayTimeEncoder.setValue(delayTime, true);     
            maxDelayTimeEncoder.setValue(maxDelayTime, true);     
            cycleModeEncoder.setValue(cycleMode, true);
            cycleIntervalEncoder.setValue(interval, true);                 
        }        
    
	bool handleEvent(gui_event_t *event) {
  
            // Encoder 1 button
            if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                toggleCycleEnabled();
                return true;
            }

            // Encoder 3 button
            if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
                toggleTripletsEnabled();
                return true;
            }
	   
            return false;
	}

        void toggleCycleEnabled(){
            cycleEnabled = !cycleEnabled;
            if (cycleEnabled){
                GUI.flash_strings_fill("DELAY TIME CYCLE", "ENABLED");
            } else {
                GUI.flash_strings_fill("DELAY TIME CYCLE", "DISABLED");     
            }
        }

        void toggleTripletsEnabled(){
            tripletsEnabled = !tripletsEnabled;
            if (tripletsEnabled){
                delayTimeEncoder.initEnumEncoder(delayTimeTripletNames, DELAY_TIMES_COUNT, "TIM", delayTime);
                maxDelayTimeEncoder.initEnumEncoder(delayTimeTripletNames, DELAY_TIMES_COUNT, "MAX", maxDelayTime);
                GUI.flash_strings_fill("DELAY TIMES:", "TRIPLETS ON");
            } else {
                delayTimeEncoder.initEnumEncoder(delayTimeNames, DELAY_TIMES_COUNT, "TIM", delayTime);
                maxDelayTimeEncoder.initEnumEncoder(delayTimeNames, DELAY_TIMES_COUNT, "MAX", maxDelayTime);
                GUI.flash_strings_fill("DELAY TIMES:", "TRIPLETS OFF"); 
            }          
            setDelayTime();  
            GUI.redisplay();
        }
        
        void setDelayTime(){          
          for (int chan=0; chan < 8; chan++){
              if (tripletsEnabled){
                  MidiUart.sendCC(chan, CC_DELAY_TIME, delayTimeTripletValues[delayTime]);
              } else {
                  MidiUart.sendCC(chan, CC_DELAY_TIME, delayTimeValues[delayTime]);
              }
          }  
        }
        
        void setCycleMode(uint8_t _cycleMode){
            cycleMode = _cycleMode;
        }
        
        void setInterval(uint8_t _interval){
            interval = _interval;
        }
        
        void on16Callback(){
        
            if (!cycleEnabled){
                return;
            }
        
            uint8_t currentPos = (MidiClock.div16th_counter % interval);  
            
            if (currentPos == 0){
        
                switch(cycleMode){
                 
                    // UP
                    case 0: 
                         delayTime = (delayTime + 1) % (maxDelayTime + 1);
                         break;
                    
                    // DOWN
                    case 1:
                         delayTime = (delayTime + maxDelayTime) % (maxDelayTime + 1);
                         break;
                        
                    // RANDOM    
                    case 2: 
                        delayTime = random(0, (maxDelayTime + 1));
                        break;
                }                
                setDelayTime();
                updateEncoders();                   
            }
        }

};

const char *DelayControlPage::delayTimeNames[DELAY_TIMES_COUNT] = {
    "/2 ",
    "/4 ",
    "/8 ",
    "/16",
    "/32"   
  };

const uint8_t DelayControlPage::delayTimeValues[DELAY_TIMES_COUNT] = {
    128 -1,
    64 - 1,
    32 - 1,
    16 - 1,
    8 - 1
};  

const char *DelayControlPage::delayTimeTripletNames[DELAY_TIMES_COUNT] = {
    "/3 ",
    "/6 ",
    "/12",
    "/24",
    "/48"   
  };

const uint8_t DelayControlPage::delayTimeTripletValues[DELAY_TIMES_COUNT] = {
    96 - 1,
    48 - 1,
    24 - 1,
    12 - 1,
    6 - 1
};  
  
const char *DelayControlPage::cycleModeNames[CYCLE_MODE_COUNT] = {  
    "UP ",
    "DWN",
    "RND"
};
  




