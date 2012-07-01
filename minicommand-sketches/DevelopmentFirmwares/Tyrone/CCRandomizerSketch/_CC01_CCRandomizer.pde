#define MAX_LENGTH 64 // = 64 steps in 16th notes
#define CC_PLAYBACK_PARAM_1 16 // lower boundary of CC parameters
#define CC_FX_2_PARAM_6 45 // upper boundary of CC parameters

class CCRandomizerClass: public MidiCallback, ClockCallback {
  
  public:

      // Constructor
      CCRandomizerClass
      (
          uint8_t _track = 1, 
          uint8_t _parameter = 0,
          uint8_t _parameterCC = CC_PLAYBACK_PARAM_1,
          uint8_t _length = 16, 
          uint8_t _offset = 0, 
          uint8_t _interval = 4, 
          uint8_t _ccMin = 0, 
          uint8_t _ccMax = 127
      )
      {
          track = _track;
          parameter = _parameter;          
          parameterCC = _parameterCC;
          length = _length;
          offset = _offset;
          interval = _interval;
          ccMin = _ccMin;
          ccMax = _ccMax;
      }
      
      uint8_t values[MAX_LENGTH];
      
      uint8_t track, parameter, parameterCC, length, offset, interval, ccMin, ccMax;
      bool enabled;
      CCEncoder outputEncoder;
      char * name;
      char * shortName;      
      static const char *parameterNames[30]; 
      static const char *parameterShortNames[30];       
      static const uint8_t parameterCCs[30];
      void setName(char * _name);
      void setShortName(char * _name);      
      void setActive();
      void toggleEnabled();  
      void setTrack(uint8_t _track);
      void setParameterCC(uint8_t _parameter);
      void setParameter(uint8_t _parameter);      
      void setLength(uint8_t _length);        
      void setInterval(uint8_t _interval);    
      void setOffset(uint8_t _offset);    
      void setCCMin(uint8_t _ccMin);
      void setCCMax(uint8_t _ccMax);
      void updateOutputEncoder();
      void randomizeValues();
      CCEncoder* getOutputEncoder();

      void setup();
      void on16Callback();   
};

const char *CCRandomizerClass::parameterNames[30] = {
    "PLAYBACK PARAM 1",
    "PLAYBACK PARAM 2",
    "PLAYBACK PARAM 3",
    "PLAYBACK PARAM 4",
    "PLAYBACK PARAM 5",
    "PLAYBACK PARAM 6",
    "ATTACK",
    "HOLD",
    "RELEASE",
    "VOLUME",
    "BALANCE",
    "N/A",
    "LFO 1 SPEED",
    "LFO 2 SPEED",
    "LFO 3 SPEED",
    "LFO 1 DEPTH",
    "LFO 2 DEPTH",
    "LFO 3 DEPTH",
    "FX 1 PARAM 1",
    "FX 1 PARAM 2",
    "FX 1 PARAM 3",
    "FX 1 PARAM 4",
    "FX 1 PARAM 5",
    "FX 1 PARAM 6",    
    "FX 2 PARAM 1",
    "FX 2 PARAM 2",
    "FX 2 PARAM 3",
    "FX 2 PARAM 4",
    "FX 2 PARAM 5",
    "FX 2 PARAM 6"        
  };
  
const char *CCRandomizerClass::parameterShortNames[30] = {  
    "PL1",
    "PL2",
    "PL3",
    "PL4",
    "PL5",
    "PL6",
    "AM1",
    "AM2",
    "AM3",
    "AM4",
    "AM5",
    "N/A",
    "L1S",
    "L2S",
    "L3S",
    "L1D",
    "L2D",
    "L3D",
    "1X1",
    "1X2",
    "1X3",
    "1X4",
    "1X5",
    "1X6",
    "2X1",
    "2X2",
    "2X3",
    "2X4",
    "2X5",
    "2X6"
};
  
const uint8_t CCRandomizerClass::parameterCCs[30] = {
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45    
  };  

void CCRandomizerClass::setup() {    
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&CCRandomizerClass::on16Callback);
    randomizeValues();
    updateOutputEncoder();
}

void CCRandomizerClass::randomizeValues() {
    for (uint8_t i = 0; i < MAX_LENGTH; i++) {
	values[i] = random(ccMin, ccMax);
    }
    GUI.flash_strings_fill(name, "RANDOMIZED");
}

void CCRandomizerClass::updateOutputEncoder(){
    outputEncoder.initCCEncoder((track - 1), parameterCC);
}

void CCRandomizerClass::setName(char * _name) {
    name = _name;                
}

void CCRandomizerClass::setShortName(char * _shortName) {
    shortName = _shortName;                
}

void CCRandomizerClass::setTrack(uint8_t _track){
    track = _track;
}

void CCRandomizerClass::setParameter(uint8_t _parameter){
    parameter = _parameter;
}

void CCRandomizerClass::setParameterCC(uint8_t _parameter){
    parameterCC = parameterCCs[_parameter];
}

void CCRandomizerClass::setLength(uint8_t _length){
    length = _length;
}  

void CCRandomizerClass::setInterval(uint8_t _interval){
    interval = _interval;
}

void CCRandomizerClass::setOffset(uint8_t _offset){
    offset = _offset;
}

void CCRandomizerClass::setCCMin(uint8_t _ccMin){
    ccMin = _ccMin;
}

void CCRandomizerClass::setCCMax(uint8_t _ccMax){
    ccMax = _ccMax;
}

void CCRandomizerClass::toggleEnabled() {
    enabled = !enabled;
    if (enabled) {
	  GUI.flash_strings_fill(name, "ENABLED");
    } else {	
          GUI.flash_strings_fill(name, "DISABLED");          
    } 
}

void CCRandomizerClass::setActive() {
    GUI.flash_strings_fill(name, "SELECTED");
}

CCEncoder* CCRandomizerClass::getOutputEncoder(){
    return &outputEncoder;  
}

void CCRandomizerClass::on16Callback(){

    if (!enabled){
        return;
    }

    uint8_t currentPos = ((MidiClock.div32th_counter % MAX_LENGTH) % length);  
    
    if (currentPos % interval == 0){

        currentPos += offset;
        outputEncoder.setValue(values[currentPos], true);
        outputEncoder.redisplay = true;
      
    }
}
