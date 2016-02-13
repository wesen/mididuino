typedef enum  { 
  DIR_FWD = 0,
  DIR_REV,
  DIR_COUNT
} sample_direction_type_t;

#define RTIM_COUNT 8 
uint8_t retrigTimes[RTIM_COUNT] = {
  127, 110, 96, 80, 64, 48, 32, 16
};
          
class RomPage : public EncoderPage {
  public:
    static const char *sampleDirectionNames[DIR_COUNT];  
    static const char *retrigEnabledNames[2];          
    static const char *sampleRetrigNames[RTIM_COUNT];    
    RangeEncoder sampleStartEncoder, sampleEndEncoder;
    EnumEncoder retrigTimeEncoder;
    uint8_t sampleDirection, retrigEnabled, retrigTime, retrigAmount, sampleStart, sampleEnd;   
    RomPage() : EncoderPage() {
      sampleStartEncoder.initRangeEncoder(0, 127, "ST", 0);
      sampleEndEncoder.initRangeEncoder(0, 127, "END", 127);      
      retrigTimeEncoder.initEnumEncoder(sampleRetrigNames, RTIM_COUNT, "RTG", 0);
      sampleDirection = 0;
      retrigAmount = 0;
      retrigEnabled = false;
      sampleStart = 0;
      sampleEnd = 127;
      encoders[0] = &sampleStartEncoder;
      encoders[1] = &sampleEndEncoder;
      encoders[2] = &retrigTimeEncoder; 
      encoders[3] = NULL;    
    }
    
    virtual void loop() {
      
          bool changed = false; 
          int delta = 0;
          
          if (sampleStartEncoder.hasChanged()) {
              delta = sampleStartEncoder.getValue() - sampleStart;
              sampleStart = sampleStartEncoder.getValue();
              sampleEnd = constrain (sampleEnd + delta,  0, 127);
              sampleEndEncoder.setValue(sampleEnd);
              changed = true;
          }
          if (sampleEndEncoder.hasChanged()) {
              sampleEnd = sampleEndEncoder.getValue();
              changed = true;              
          }
          if (retrigTimeEncoder.hasChanged()) {
              setRetrigTime (retrigTimeEncoder.getValue());
              changed = true;              
          }
          
          if (changed){
              updateMDTrackParams();            
          }
  
    }
    
    virtual bool handleEvent (gui_event_t *event){
          
          if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                  reverseRomSamples();
                  displaySampleDirection();
          } else if (EVENT_PRESSED(event, Buttons.ENCODER3)){
                  toggleRetrig();
                  displayRetrigStatus();
          } else {                  
                return false;
          }          
    }
    
    void setRetrigTime(uint8_t value){     
         retrigTime = retrigTimes[value];
    }
    
    void updateMDTrackParams() {
        for (int i = 0; i < 16; i++) {
            if ((MD.kit.models[i] >= ROM_01_MODEL) &&  (MD.kit.models[i] <= ROM_24_MODEL)){
                MD.setTrackParam(i, ROM_STRT, sampleStart);    
                MD.setTrackParam(i, ROM_END, sampleEnd);     
                MD.setTrackParam(i, ROM_RTIM, retrigTime);   
                MD.setTrackParam(i, ROM_RTRG, retrigAmount);                   
            }
        }  
    }
    
    void displaySampleDirection() {
      GUI.setLine(GUI.LINE1);
      GUI.flash_string_at(0, "DIRECTION:");      
      GUI.setLine(GUI.LINE2);
      GUI.flash_string_at(0, sampleDirectionNames[sampleDirection]);
    }       
   
   void displayRetrigStatus() {
      GUI.setLine(GUI.LINE1);
      GUI.flash_string_at(0, "RETRIG:");      
      GUI.setLine(GUI.LINE2);
      GUI.flash_string_at(0, retrigEnabledNames[retrigEnabled]);
    }   
    
    void reverseRomSamples (){
         sampleDirection = (sampleDirection + 1) % 2;
         
         // Play forwards
         if (sampleDirection == 0){
             sampleStart = 0;
             sampleEnd = 127;
         } else {
             sampleStart = 64;
             sampleEnd = 0;
         }  
         sampleStartEncoder.setValue(sampleStart);
         sampleEndEncoder.setValue(sampleEnd); 
         updateMDTrackParams();
    }

    void toggleRetrig (){
         retrigEnabled = (retrigEnabled + 1) % 2;
         
         if (retrigEnabled == 0){
             retrigAmount = 8;
         } else {
             retrigAmount = 0;
         }  
         updateMDTrackParams();
    }
};
RomPage romPage;          

const char *RomPage::sampleDirectionNames[DIR_COUNT] = {
      "FORWARD", 
      "REVERSE"
};

const char *RomPage::retrigEnabledNames[2] = {
      "ON ", 
      "OFF "
};

const char *RomPage::sampleRetrigNames[RTIM_COUNT] = {
      "127 ", 
      "110 ",
      "96",
      "80",
      "64",
      "48",
      "32",
      "16"      
};
