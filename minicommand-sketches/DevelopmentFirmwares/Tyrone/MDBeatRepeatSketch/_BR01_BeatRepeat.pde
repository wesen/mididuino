#include <MD.h>

class DebugPage : public EncoderPage{
public:
    // Debug Encoder Page
    RangeEncoder debugParam1, debugParam2, debugParam3, debugParam4;
    
    DebugPage() : 
    debugParam1(0, 127, "***"),
    debugParam2(0, 127, "***"), 
    debugParam3(0, 127, "***"),
    debugParam4(0, 127, "***") 
    {
        encoders[0] = &debugParam1;
        encoders[1] = &debugParam2;
        encoders[2] = &debugParam3;
        encoders[3] = &debugParam4;  
    }   
    
//    debugPage.debugParam1.setName(" i ");
//    debugPage.debugParam1.setValue(i);    
//    debugPage.debugParam2.setName("NUM");
//    debugPage.debugParam2.setValue(sliceNumber);                     
//    debugPage.debugParam3.setName("ST");
//    debugPage.debugParam3.setValue(sliceStartValue);                      
//    debugPage.debugParam4.setName("END");
//    debugPage.debugParam4.setValue(sliceEndValue);                      
//    debugPage.update();    
};
DebugPage debugPage;

class BeatRepeatClass : public MidiCallback, ClockCallback {
	
 public:
  static const uint8_t INTVL_1_32_MASK  = 0;
  static const uint8_t INTVL_1_16_MASK  = 1;
  static const uint8_t INTVL_1_8_MASK   = 2;
  static const uint8_t INTVL_1_4_MASK   = 3;
  static const uint8_t INTVL_1_2_MASK   = 4;
  static const uint8_t INTVL_1_BAR_MASK = 5;
  static const uint8_t INTVL_2_BAR_MASK = 6;  
  static const char *intervalNames[7];

  static const uint8_t OFFSET_0_16_MASK = 0;
  static const uint8_t OFFSET_1_16_MASK = 1;
  static const uint8_t OFFSET_2_16_MASK = 2;
  static const uint8_t OFFSET_3_16_MASK = 3;
  static const uint8_t OFFSET_4_16_MASK = 4;
  static const uint8_t OFFSET_5_16_MASK = 5; 
  static const uint8_t OFFSET_6_16_MASK = 6; 
  static const uint8_t OFFSET_7_16_MASK = 7; 
  static const uint8_t OFFSET_8_16_MASK = 8; 
  static const uint8_t OFFSET_9_16_MASK = 9; 
  static const uint8_t OFFSET_10_16_MASK = 10; 
  static const uint8_t OFFSET_11_16_MASK = 11; 
  static const uint8_t OFFSET_12_16_MASK = 12;   
  static const uint8_t OFFSET_13_16_MASK = 13;   
  static const uint8_t OFFSET_14_16_MASK = 14;   
  static const uint8_t OFFSET_15_16_MASK = 15;   
  static const char *offsetNames[16];

//  01/08/2011 - Minicommand currently can't spit out triggers faster than 1/32...
//
//  static const uint8_t GRID_1_96_MASK  = 0;
//  static const uint8_t GRID_1_64_MASK  = 1;
//  static const uint8_t GRID_1_48_MASK  = 2;
  static const uint8_t GRID_1_32_MASK  = 0;
//  static const uint8_t GRID_1_24_MASK  = 1;
  static const uint8_t GRID_1_16_MASK  = 1;
  static const uint8_t GRID_1_12_MASK  = 2;  
  static const uint8_t GRID_1_8_MASK   = 3;  
  static const uint8_t GRID_1_6_MASK   = 4;  
  static const uint8_t GRID_1_4_MASK   = 5;  
  static const uint8_t GRID_1_3_MASK   = 6;  
  static const uint8_t GRID_1_2_MASK   = 7;  
  static const uint8_t GRID_3_4_MASK   = 8; 
  static const uint8_t GRID_1_BAR_MASK = 9;   
  static const char *gridNames[10];  
  static const int grid32Values[10];

  static const uint8_t GATE_1_16_MASK = 0;
  static const uint8_t GATE_2_16_MASK = 1;
  static const uint8_t GATE_3_16_MASK = 2;
  static const uint8_t GATE_4_16_MASK = 3;
  static const uint8_t GATE_5_16_MASK = 4; 
  static const uint8_t GATE_6_16_MASK = 5; 
  static const uint8_t GATE_7_16_MASK = 6; 
  static const uint8_t GATE_8_16_MASK = 7; 
  static const uint8_t GATE_9_16_MASK = 8; 
  static const uint8_t GATE_10_16_MASK = 9; 
  static const uint8_t GATE_11_16_MASK = 10; 
  static const uint8_t GATE_12_16_MASK = 11;   
  static const uint8_t GATE_13_16_MASK = 12;   
  static const uint8_t GATE_14_16_MASK = 13;   
  static const uint8_t GATE_15_16_MASK = 14; 
  static const uint8_t GATE_1_BAR_MASK = 15; 
  static const uint8_t GATE_2_BAR_MASK = 16; 
  static const char *gateNames[17];
  
  static const char *pitchNames[11];
  static const char *variationNames[11];  
  
  static const uint8_t SMPL_2BTS_MASK   = 0;
  static const uint8_t SMPL_4BTS_MASK   = 1;
  static const uint8_t SMPL_8BTS_MASK   = 2;
  static const uint8_t SMPL_16BTS_MASK  = 3;
  static const uint8_t SMPL_32BTS_MASK  = 4;
  static const uint8_t SMPL_64BTS_MASK  = 5; 
  static const char *numberOfBeatsInSampleNames[6];

  static const uint8_t LEN_1_16_MASK  = 0;
  static const uint8_t LEN_2_16_MASK  = 1;
  static const uint8_t LEN_3_16_MASK  = 2;
  static const uint8_t LEN_4_16_MASK = 3;
  static const char *lengthOfSliceNames[4];

  static const uint8_t DIR_FWD_MASK  = 0;
  static const uint8_t DIR_REV_MASK  = 1;
  static const uint8_t DIR_RND_MASK  = 2;
  static const uint8_t DIR_FWD_PLUS_REV_MASK  = 3;
  static const uint8_t DIR_REV_PLUS_FWD_MASK  = 4;  
  static const char *sliceDirectionNames[5];

  static const uint8_t ORD_FWD_MASK  = 0;
  static const uint8_t ORD_REV_MASK  = 1;
  static const uint8_t ORD_RND_MASK  = 2;
  static const char *sliceOrderNames[3];
  
  // Constructor
  BeatRepeatClass
  (
      uint8_t _interval = INTVL_1_2_MASK,
      uint8_t _offset = OFFSET_0_16_MASK,
      uint8_t _gate = GATE_4_16_MASK,
      uint8_t _grid = GRID_1_16_MASK,
      uint8_t _pitch = 0,
      uint8_t _pitchDecay = 0,
      uint8_t _volume = 110,      
      uint8_t _decay = 0,
      uint8_t _sourceTrack = 0,
      uint8_t _chance = 100,
      uint8_t _variation = 0,
      uint8_t _numberOfBeatsInSample = SMPL_4BTS_MASK,
      uint8_t _sliceOrder = ORD_FWD_MASK,
      uint8_t _sliceDirection = DIR_FWD_MASK,
      uint8_t _lengthOfSlice = LEN_2_16_MASK
  )
  {
      interval = _interval;
      offset = _offset;
      gate = _gate;
      grid = _grid;
      pitch = _pitch;
      pitchDecay = _pitchDecay;
      volume = _volume;
      decay = _decay;
      sourceTrack = _sourceTrack;
      chance = _chance;
      variation = _variation;
      numberOfBeatsInSample = _numberOfBeatsInSample;
      sliceOrder = _sliceOrder;
      sliceDirection = _sliceDirection;
      lengthOfSlice = _lengthOfSlice;
  }
  
  uint8_t sourceTrack, targetTrack, interval, interval32, offset, offset32, grid, grid32, originalGrid32, variation, chance, gate, gate32, pitch, pitchDecay, volume, decay, decayRange, repeatsRemaining, totalNumberOfRepeats, startingPitch, pitchDecayRange, pitchDecayPerRepeat, decayPerRepeat;
  bool enabled, sliceEnabled;
  char * name;
  void setSourceTrack(uint8_t _sourceTrack);
  void setTargetTrack(uint8_t _targetTrack);  
  void setBeatRepeatEnabled(bool _enabled);
  void setName(char * _name);
  void setActive();
  void toggleEnabled();  
  void setInterval(uint8_t _interval);    
  void setInterval32(uint8_t _interval);  
  void setOffset(uint8_t _offset);    
  void setOffset32(uint8_t _offset);  
  void setGrid(uint8_t _grid);
  void setGrid32(uint8_t _grid);
  void setOriginalGrid32(uint8_t _grid);  
  void restoreGrid32();    
  void setGate(uint8_t _gate);    
  void setGate32(uint8_t _gate);    
  void setTotalNumberOfRepeats();
  void setVariation(uint8_t _variation);
  void setChance(uint8_t _chance);
  void setPitch(uint8_t _pitch);
  void setPitchDecay(uint8_t _pitchDecay);  
  void updatePitchVariables();
  void updateVolumeVariables();  
  void setVolume(uint8_t _volume);  
  void setDecay(uint8_t _decay);    
  uint8_t limitValue(uint8_t _value);

  void setNumberOfBeatsInSample(uint8_t _numberOfBeatsInSample);
  void setSliceNumber(uint8_t _i);  
  void setSliceValues();  
  void setSliceOffsets(uint8_t _sampleSize, uint8_t _threshold);
  void setLengthOfSlice(uint8_t _lengthOfSlice);
  void setSliceDirection(uint8_t _sliceDirection);
  void setSliceOrder(uint8_t _sliceOrder);  
  uint8_t sliceNumber, sliceStartOffset, sliceEndOffset, sliceRevDirOffset;
  uint8_t sliceStartValue, sliceEndValue;   
  uint8_t numberOfBeatsInSample, realNumberOfBeatsInSample;
  uint8_t sampleLengthIn16ths;
  uint8_t lengthOfSlice, lengthOfSlice16;  
  uint8_t sliceDirection; 
  uint8_t sliceOrder;   
  void setSliceEnabled(bool _enabled);

  void setup();
  void on32Callback();    
  void onControlChange(uint8_t *msg);

};

const char *BeatRepeatClass::intervalNames[7] = {
    "1/32",
    "1/16",
    "1/8 ",
    "1/4 ",
    "1/2 ",
    "1BAR",
    "2BAR"  
  };   
  
const char *BeatRepeatClass::offsetNames[16] = {
    " 0 ",
    " 1 ",
    " 2 ",
    " 3 ",    
    " 4 ",
    " 5 ",
    " 6 ",
    " 7 ",
    " 8 ",
    " 9 ",
    " 10",
    " 11",
    " 12",
    " 13",
    " 14",
    " 15"
  };
  
const char *BeatRepeatClass::variationNames[11] = {
    " 0",
    " 1",
    " 2",
    " 3",    
    " 4",
    " 5",
    " 6",
    " 7",
    " 8",
    " 9",
    " 10"
};  
  
const char *BeatRepeatClass::gridNames[10] = {
//    "1/96",
//    "1/64",
//    "1/48",
    "1/32",
//    "1/24",
    "1/16",
    "1/12",
    "1/8 ",
    "1/6 ",
    "1/4 ",
    "1/3 ",
    "1/2 ",
    "3/4 ",
    "1BAR"    
  };  
  
const int BeatRepeatClass::grid32Values[10] = {
   1,
   2,
   3,
   4,
   6,
   8,
   12,
   16,
   24,
   32
}; 

const char *BeatRepeatClass::gateNames[17] = {
    " 1 ",
    " 2 ",
    " 3 ",   
    " 4 ",
    " 5 ",
    " 6 ",
    " 7 ",
    " 8 ",
    " 9 ",
    " 10",
    " 11",
    " 12",
    " 13",
    " 14",
    " 15",
    "1BR",
    "2BR"
  };
  
  const char *BeatRepeatClass::pitchNames[11] = {
    " 0 ",
    "-1 ",
    "-2 ",
    "-3 ",     
    "-4 ",
    "-5 ",
    "-6 ",
    "-7 ",
    "-8 ",
    "-9 ",
    "-10"
  };   
  
  const char *BeatRepeatClass::numberOfBeatsInSampleNames[6] = {
    "2  ",
    "4  ",
    "8  ",
    "16 ",    
    "32 ",
    "64 "
  };

const char *BeatRepeatClass::lengthOfSliceNames[4] = {
    "1/16",
    "2/16",
    "3/16",
    "4/16"
  };  

const char *BeatRepeatClass::sliceDirectionNames[5] = {
    "FWD",
    "REV",
    "RND",
    "F+R",
    "R+F"
  };  
  
const char *BeatRepeatClass::sliceOrderNames[3] = {
    "FWD",
    "REV",
    "RND"
  };    


void BeatRepeatClass::setup() {    
    MidiClock.addOn32Callback(this, (midi_clock_callback_ptr_t)&BeatRepeatClass::on32Callback);
    Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&BeatRepeatClass::onControlChange); 
    setInterval32(interval);
    setOffset32(offset);
    setGrid32(grid);
    setOriginalGrid32(grid);
    setGate32(gate);
    setTotalNumberOfRepeats(); 
    updatePitchVariables(); 
    updateVolumeVariables(); 
    setNumberOfBeatsInSample(numberOfBeatsInSample);                     
    setLengthOfSlice(lengthOfSlice);
}

void BeatRepeatClass::on32Callback() {
  
  bool primed = false;

  // if BeatRepeat enabled, or we haven't completed all repeats for the last cycle
  if (enabled || ((repeatsRemaining < totalNumberOfRepeats) && (repeatsRemaining > 0))) {
      
      // Determine the current clock position relative to the [interval]:
      //  1.  Determine the current clock position in 32nd notes.  Convert (32 steps in 16th notes) to 32nds (i.e. multiply by 2) and take the Modulus
      //  2.  Number of 32nds in [interval] = (2 to the power of [interval]).  
      //  3.  Take the modulus of (1) and (2)
      uint8_t val = ((MidiClock.div32th_counter) % (32 * 2)) % interval32;    
    
      // Determine if we are on a 16th note
      bool on16th = (MidiClock.div32th_counter % 2 == 0);
      
      // BeatRepeat start pos = [offset] relative to [interval]
      uint8_t start = (offset32 % interval32);      
            
      // If we are at the start position and BeatRepeat is enabled then reset number of repeats remaining in this cycle 
      if ((val == start) && enabled){
        
          // If variation > 0 then randomize the grid32 value
          if (variation > 0){
              uint8_t rndChance = random(100);
              uint8_t rndDelta = random(variation);
              if (rndChance <= 30){
                 setGrid32(MIN(0, originalGrid32 - rndDelta));  
              } else if (rndChance >= 70){
                 setGrid32(MAX(9,originalGrid32 + rndDelta));  
              }
              //setTotalNumberOfRepeats();            
              updatePitchVariables();                
              updateVolumeVariables();                                          
          }
        
          // Reset number of repeats remaining
          repeatsRemaining = totalNumberOfRepeats;     
          
          // if Chance succeeds, set the primed flag ready to start next cycle of repeats
          if (random(100) <= chance){
              primed = true;
          }          
      }     
      
      // If no repeats left in this cycle then exit
      if (repeatsRemaining == 0){     
          
           // Restore grid32 to the originally stored value
           restoreGrid32();                
          
           // Fire out an "auto slice" trig if enabled
           if (sliceEnabled){
                         
               if (on16th){
                  uint8_t i = MidiClock.div16th_counter % realNumberOfBeatsInSample;
                  if ((i % lengthOfSlice16) == 0){
                      setSliceNumber(i);
                      setSliceValues();
                      MD.sliceTrack32(targetTrack, sliceStartValue, sliceEndValue, true);                       
                  }
                 
               }
           } else {         
               // Restore [source track] volume
               MD.setTrackParam(sourceTrack, MODEL_VOL, MD.kit.params[sourceTrack][MODEL_VOL]);
           }
           return; 
      }
      
      // Exit if current clock position + [offset] relative to the grid setting isn't 0
      if ((val - offset32) % grid32 != 0)  {
          return;
      }       
      
      // [offset] is specified in 16ths so don't need to multiply by 2 to use as the "From" position for the [SLICE TO BE REPEATED]
      uint8_t from = offset;
      
      // The "To" position for [SLICE TO BE REPEATED] = from + (Number of 32nds in [grid] / 2)
      uint8_t to = from + MAX((grid32 / 2), 1);      
      
      //  Pitch & Pitch Decay
      uint8_t thisPitch, pitchDelta;
      pitchDelta = pitchDecayPerRepeat * (totalNumberOfRepeats - repeatsRemaining);
      thisPitch = limitValue(startingPitch - pitchDelta);
      
      // Volume & Decay
      uint8_t thisVolume, volumeDelta;
      volumeDelta = decayPerRepeat * (totalNumberOfRepeats - repeatsRemaining);
      thisVolume = limitValue(volume - volumeDelta);      
    
      //  If primed, or we haven't completed all repeats for the last cycle
      if (primed || repeatsRemaining < totalNumberOfRepeats){              
        
          // Set volume on [source track] to zero (i.e. mute it)
          MD.setTrackParam(sourceTrack, MODEL_VOL, 0);        
          
          // Do the repeat on the [target track]
          MD.sliceTrack32(targetTrack, from, to, thisPitch, thisVolume, true);  
          primed = false;          
          repeatsRemaining-=1;         
          return;
      }
  }
}

void BeatRepeatClass::onControlChange(uint8_t *msg) {
    
    uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
    uint8_t cc = msg[1];
    uint8_t value = msg[2];
    uint8_t track, param;
    MD.parseCC(channel, cc, &track, &param);

    // If sourceTrack params edited via MD UI, then keep targetTrack params synced
    if (track == sourceTrack) {
       MD.kit.params[sourceTrack][param] = value;
       MD.setTrackParam(targetTrack, param, value);
       if (param = MODEL_VOL){
          updateVolumeVariables();
          return;
       }
       if (param = ROM_PTCH){
          updatePitchVariables();
       }
    }

    // If targetTrack params edited via MD UI, then keep sourceTrack params synced    
    if (track == targetTrack) {
       MD.setTrackParam(sourceTrack, param, value);
    }
    
}

void BeatRepeatClass::setName(char * _name) {
    name = _name;                
}

void BeatRepeatClass::setSourceTrack(uint8_t _sourceTrack) {
    sourceTrack = _sourceTrack;       
}

void BeatRepeatClass::setTargetTrack(uint8_t _targetTrack) {
    targetTrack = _targetTrack;          
}

void BeatRepeatClass::setBeatRepeatEnabled(bool _enabled) {
    enabled = _enabled;
}

void BeatRepeatClass::toggleEnabled() {
    enabled = !enabled;
    if (enabled) {
	  GUI.flash_strings_fill(name, "ENABLED");
    } else {	
          GUI.flash_strings_fill(name, "DISABLED");          
          
          // Restore [source track] volume
          MD.setTrackParam(sourceTrack, MODEL_VOL, MD.kit.params[sourceTrack][MODEL_VOL]);  
          repeatsRemaining = 0;
    } 
}

void BeatRepeatClass::setActive() {
    GUI.flash_strings_fill(name, "SELECTED");
}

// Number of 32nds in [interval] = (2 to the power of[interval])
void BeatRepeatClass::setInterval32(uint8_t _interval) {
    interval32 = (1<<_interval);
}

void BeatRepeatClass::setInterval(uint8_t _interval) {
    interval = _interval;
}

// [offset] is specified in 16ths so offset32 = (offset * 2)
void BeatRepeatClass::setOffset32(uint8_t _offset) {
    offset32 = (_offset * 2);
}

void BeatRepeatClass::setOffset(uint8_t _offset) {
    offset = _offset;
}

// Number of 32nds in [grid] = (2 to the power of[grid])
void BeatRepeatClass::setGrid32(uint8_t _grid) {
//    grid32 = (1<<_grid);    
    grid32 = grid32Values[_grid];
}

void BeatRepeatClass::setGrid(uint8_t _grid) {
    grid = _grid;
}

void BeatRepeatClass::setOriginalGrid32(uint8_t _grid) {
//    grid32 = (1<<_grid);    
    originalGrid32 = grid32Values[_grid];
}

void BeatRepeatClass::restoreGrid32() {
    grid32 = originalGrid32;
}

void BeatRepeatClass::setTotalNumberOfRepeats(){
    totalNumberOfRepeats = MAX((gate32 / grid32), 1); 
    repeatsRemaining = totalNumberOfRepeats;  
}

void BeatRepeatClass::setVariation(uint8_t _variation) {
    variation = _variation;
}

void BeatRepeatClass::setChance(uint8_t _chance) {
    chance = _chance;
}

// [gate] values less than 16 are for 1-16 16th notes, then increment up by a whole bar at a time
void BeatRepeatClass::setGate32(uint8_t _gate) {
    if (_gate <=15){
    	gate32 = (_gate + 1) * 2;
    } else {
    	gate32 = (_gate - 14) * 32;
    }
}

void BeatRepeatClass::setGate(uint8_t _gate) {
    gate = _gate;
}

void BeatRepeatClass::setPitch(uint8_t _pitch) {
    pitch = _pitch;
}

void BeatRepeatClass::setPitchDecay(uint8_t _pitchDecay) {
    pitchDecay = _pitchDecay;
}

void BeatRepeatClass::updatePitchVariables() {
      startingPitch  = MD.kit.params[sourceTrack][ROM_PTCH] - (pitch * 4);
      pitchDecayRange = startingPitch * pitchDecay / 100;
      pitchDecayPerRepeat = pitchDecayRange / totalNumberOfRepeats;
}

void BeatRepeatClass::setVolume(uint8_t _volume) {
    volume = _volume;
}

void BeatRepeatClass::setDecay(uint8_t _decay) {
    decay = _decay;
}

// TODO:  Logarithmic decay?
void BeatRepeatClass::updateVolumeVariables() {
      decayRange = volume * decay / 100;
      decayPerRepeat =  decayRange / totalNumberOfRepeats; 
}

uint8_t BeatRepeatClass::limitValue(uint8_t _value){
    if (((_value % 128)==0) && (_value > 0)){
        return 127;
    } else {
        return (_value % 128);
    }  
}


void BeatRepeatClass::setNumberOfBeatsInSample(uint8_t _numberOfBeatsInSample) {
    numberOfBeatsInSample = _numberOfBeatsInSample;
    realNumberOfBeatsInSample = (1<<(numberOfBeatsInSample+1));    
    sampleLengthIn16ths = numberOfBeatsInSample * 4;        
}

void BeatRepeatClass::setSliceValues() {
    switch(sliceDirection){
              
        case DIR_REV_MASK:
            setSliceOffsets(100, 101);
            break;

        case DIR_RND_MASK:
            setSliceOffsets(100, 50);
            break;

        case DIR_FWD_PLUS_REV_MASK:
            setSliceOffsets(100, 25);
            break;

        case DIR_REV_PLUS_FWD_MASK:
            setSliceOffsets(100, 75);
            break;

        // DIR_FWD_MASK:              
        default:
            setSliceOffsets(100, 0);
            break;
      
    }
    
    sliceStartValue = limitValue(((128/sampleLengthIn16ths) * ((sliceNumber/lengthOfSlice16) + sliceStartOffset) * (lengthOfSlice16)) - sliceRevDirOffset);  
    sliceEndValue = limitValue(((128/sampleLengthIn16ths) * ((sliceNumber/lengthOfSlice16) + sliceEndOffset) * (lengthOfSlice16)) - sliceRevDirOffset);    
}

void BeatRepeatClass::setSliceOffsets(uint8_t _sampleSize, uint8_t _threshold){

    // Slice will play forwards
    if (random(_sampleSize) >= _threshold) {
        sliceStartOffset = 0;
        sliceEndOffset = 1;
        sliceRevDirOffset = 0;
    // Slice will play in Reverse
    } else {
        sliceStartOffset = 1;
        sliceEndOffset = 0;   
        sliceRevDirOffset = ((128/sampleLengthIn16ths) / 2) * lengthOfSlice16;           
    }
}

void BeatRepeatClass::setSliceNumber(uint8_t _i) {
    switch(sliceOrder){
                 
        case ORD_REV_MASK:
            sliceNumber = ((sampleLengthIn16ths - ((_i+1) % sampleLengthIn16ths)) % sampleLengthIn16ths);
            return;

        case ORD_RND_MASK:
            if (random(100) > 50){
                sliceNumber = random(0, (sampleLengthIn16ths - 1));
            } else {
                sliceNumber = (_i % sampleLengthIn16ths);  
            }
            return;

        // ORD_FWD_MASK:              
        default:
            sliceNumber = (_i % sampleLengthIn16ths);
            return;
      
    }
}

void BeatRepeatClass::setLengthOfSlice(uint8_t _lengthOfSlice) {
    lengthOfSlice = _lengthOfSlice;
    lengthOfSlice16 = lengthOfSlice + 1;
}

void BeatRepeatClass::setSliceDirection(uint8_t _sliceDirection) {
    sliceDirection = _sliceDirection;
}

void BeatRepeatClass::setSliceOrder(uint8_t _sliceOrder) {
    sliceOrder = _sliceOrder;
}

void BeatRepeatClass::setSliceEnabled(bool _enabled){
    sliceEnabled = _enabled;
}
