class Track{
public:
    uint8_t number, level;
    char *name, *shortName;
    bool muted, solo, latchEnabled, muteChanged;
    Track(
        uint8_t _number,
        char * _name, 
        uint8_t _level = 64,
        bool _latchEnabled = false,
        bool _muted = false,
        bool _muteChanged = false,
        bool _solo = false
    )
    {
        number = _number;
        name = _name;
        level = _level;
        latchEnabled = _latchEnabled;
        muted = _muted;
        muteChanged = _muteChanged;        
        solo = _solo;   
    }
        
    void setLevel(uint8_t value){
         level = value; 
    }
        
    // latchEnabled should be set to true (i.e. stored value has been "passed through") before track level is updated
    void setLatchEnabled(bool enabled){
         latchEnabled = enabled; 
    }
    
    void setMuted(bool mute){
         muted = mute; 
    }
    
    void setMuteChanged(bool value){
         muteChanged = value; 
    }    
    
    void setLatchEnabled(uint8_t value){
         if (!latchEnabled){
             if((value >= level - 10) && (value <= level + 10)) {
                 latchEnabled = true; 
             } 
         } 
    }
};


class MDTrack : public Track {
public:
    uint8_t startPoint, sampleWidth;
    uint8_t noteRepeatStartStep;
    bool noteRepeatEnabled;
    note_repeat_speed_type_t noteRepeatSpeed;
    MDTrack(
        uint8_t _number,
        char * _name, 
        uint8_t _level = 64,
        uint8_t _noteRepeatStartStep = 0,
        bool _latchEnabled = false,
        bool _muted = false,
        bool _muteChanged = false,        
        bool _solo = false,
        bool _noteRepeatEnabled = false,
        note_repeat_speed_type_t _noteRepeatSpeed = NOTE_REPEAT_FILL_1        
    ):Track(
        _number,
        _name, 
        _level,
        _latchEnabled,
        _muted,
        _muteChanged,
        _solo
    )
    {
        number = _number;
        name = _name;
        level = _level;
        latchEnabled = _latchEnabled;
        muted = _muted;
        muteChanged = _muteChanged;        
        solo = _solo;   
        noteRepeatSpeed = _noteRepeatSpeed;
        noteRepeatEnabled = _noteRepeatEnabled;
        noteRepeatStartStep = _noteRepeatStartStep;
    }
    
    void setNoteRepeatSpeed(note_repeat_speed_type_t speed){
        noteRepeatSpeed = speed;
    }
    
    void setNoteRepeatStartStep(uint8_t step){
        noteRepeatStartStep = step;
    }        
    
    void toggleNoteRepeatEnabled(){
        noteRepeatEnabled = !noteRepeatEnabled;
        if (noteRepeatEnabled){
            uint8_t step = (MidiClock.div16th_counter) % 16; 
            setNoteRepeatSpeed(noteRepeat.speed);
            setNoteRepeatStartStep(step);
            GUI.flash_strings_fill(name, "NOTE REPEAT: ON"); 
        } else {
            GUI.flash_strings_fill(name, "NOTE REPEAT: OFF");         
        } 
    }
    
    void toggleMute(){
        muted = !muted;     
        muteChanged = true; 
        if (muted){
            MD.muteTrack(number, muted);       
            GUI.flash_strings_fill(name, "TRACK: MUTED"); 
        } else {
            MD.unmuteTrack(number);
            GUI.flash_strings_fill(name, "TRACK: UNMUTED");         
        } 
    }
    
    void setMuteLightRing(uint8_t noteNumber){
        // Turn off the light ring, then set it to the desired on/off value
        MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 0); 
        if(muted){
            MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 0);  
        } else {
            MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 127);            
        }        
    }

    void setNoteRepeatEnabled(bool enabled){
        noteRepeatEnabled = enabled;
    }
    
    note_repeat_speed_type_t getNoteRepeatSpeed(){
        return noteRepeatSpeed;
    }
    
    void setTrackLevel (uint8_t parameterNumber, uint8_t value){       
        if (latchEnabled){          
            setLevel(value);    
            MD.setTrackParam(number, parameterNumber, value);                         
        } else {
            setLatchEnabled(value);
        }  
        GUI.flash_strings_fill(name, "LEVEL");            
    }
};

class SynthTrack : public Track {
public:
    bool transposeEnabled;
    SynthTrack(
        uint8_t _number,
        char * _name, 
        uint8_t _level = 64,
        bool _latchEnabled = false,
        bool _muted = false,
        bool _muteChanged = false,        
        bool _solo = false,
        bool _transposeEnabled = true
    ):Track(
        _number,
        _name, 
        _level,
        _latchEnabled,
        _muted,
        _muteChanged,
        _solo
    )
    {
        number = _number;
        name = _name;
        level = _level;
        latchEnabled = _latchEnabled;
        muted = _muted;
        muteChanged = _muteChanged;                
        solo = _solo;   
        transposeEnabled = _transposeEnabled;
    }
    
    void setTransposeEnabled(bool enabled){
         transposeEnabled = enabled; 
    }
    
    void toggleTransposeEnabled(){
         transposeEnabled = !transposeEnabled; 
         if (transposeEnabled){
            GUI.flash_strings_fill(name, "TRANSPOSE: ON"); 
        } else {
            GUI.flash_strings_fill(name, "TRANSPOSE: OFF");         
        } 
    }
    
    void setTrackLevel(uint8_t parameterNumber, uint8_t value){   
    
        if (latchEnabled){        
            setLevel(value);              
            if (number <= MNM_TRACK_6){
                MidiUart.sendCC(number + mnmBaseMidiChannel, parameterNumber, value); 
            } else if (number == OB_TRACK_1){
                MidiUart.sendCC(oberheimMidiChannel, parameterNumber, value); 
            } else if (number == TETRA_TRACK_1){
                MidiUart.sendCC(tetraMidiChannel, parameterNumber, value); 
            }                    
        } else {
            setLatchEnabled(value);
        }  
        GUI.flash_strings_fill(name, "LEVEL");          
    }
    
    void toggleMute() {  
        muted = !muted;
        muteChanged = true;          
        if(muted){
            MidiUart.sendCC(mnmBaseMidiChannel + number, MNM_TRACK_MUTE, 127);
            GUI.flash_strings_fill(name, "TRACK: MUTED");             
        }else{
            MidiUart.sendCC(mnmBaseMidiChannel + number, MNM_TRACK_MUTE, 0);
            GUI.flash_strings_fill(name, "TRACK: UNMUTED");             
        }  
    }
     
    void setMuteLightRing(uint8_t noteNumber){
          // Turn off the light ring, then set it to the desired on/off value
          MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 0); 
          if(muted){
              MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 0);  
          } else {
              MidiUart.sendNoteOn(xone4DMidiChannel, noteNumber, 127);            
          }        
    }
};


// Instantiate Tracks
MDTrack mdTrack1(MD_TRACK_1, "MD TRACK 1");
MDTrack mdTrack2(MD_TRACK_2, "MD TRACK 2");
MDTrack mdTrack3(MD_TRACK_3, "MD TRACK 3");
MDTrack mdTrack4(MD_TRACK_4, "MD TRACK 4");
MDTrack mdTrack5(MD_TRACK_5, "MD TRACK 5");
MDTrack mdTrack6(MD_TRACK_6, "MD TRACK 6");
MDTrack mdTrack7(MD_TRACK_7, "MD TRACK 7");
MDTrack mdTrack8(MD_TRACK_8, "MD TRACK 8");
MDTrack mdTrack9(MD_TRACK_9, "MD TRACK 9");
MDTrack mdTrack10(MD_TRACK_10, "MD TRACK 10");
MDTrack mdTrack11(MD_TRACK_11, "MD TRACK 11");
MDTrack mdTrack12(MD_TRACK_12, "MD TRACK 12");
MDTrack mdTrack13(MD_TRACK_13, "MD TRACK 13");
MDTrack mdTrack14(MD_TRACK_14, "MD TRACK 14");
MDTrack mdTrack15(MD_TRACK_15, "MD TRACK 15");
MDTrack mdTrack16(MD_TRACK_16, "MD TRACK 16");
SynthTrack mnmTrack1(MNM_TRACK_1, "MNM TRACK 1");
SynthTrack mnmTrack2(MNM_TRACK_2, "MNM TRACK 2");
SynthTrack mnmTrack3(MNM_TRACK_3, "MNM TRACK 3");
SynthTrack mnmTrack4(MNM_TRACK_4, "MNM TRACK 4");
SynthTrack mnmTrack5(MNM_TRACK_5, "MNM TRACK 5");
SynthTrack mnmTrack6(MNM_TRACK_6, "MNM TRACK 6");
SynthTrack obTrack1(OB_TRACK_1, "OBERHEIM TRACK 1");
SynthTrack tetraTrack1(TETRA_TRACK_1, "TETRA TRACK 1");

// Instantiate MD Tracks Collection array
MDTrack *mdTracks[MD_TRACK_COUNT] = {
   &mdTrack1, &mdTrack2, &mdTrack3, &mdTrack4, &mdTrack5, &mdTrack6, &mdTrack7, &mdTrack8, &mdTrack9, &mdTrack10, &mdTrack11, &mdTrack12, &mdTrack13, &mdTrack14, &mdTrack15, &mdTrack16
};

// Instantiate MD Track Groups
MDTrack *mdTracksLowMidHigh[FADER_COUNT] = {
   &mdTrack1, &mdTrack5, &mdTrack9, &mdTrack10
};

MDTrack *mdTracksLow[FADER_COUNT] = {
   &mdTrack1, &mdTrack2, &mdTrack3, &mdTrack4
};

MDTrack *mdTracksMid[FADER_COUNT] = {
   &mdTrack5, &mdTrack6, &mdTrack7, &mdTrack8
};

MDTrack *mdTracksHigh[FADER_COUNT] = {
   &mdTrack9, &mdTrack10, &mdTrack11, &mdTrack12
};

// Instantiate Synth Tracks Collection array
SynthTrack *synthTracks[SYNTH_TRACK_COUNT] = {
   &mnmTrack1, &mnmTrack2, &mnmTrack3, &mnmTrack4, &mnmTrack5, &mnmTrack6, &obTrack1, &tetraTrack1
};

// Instantiate Synth Track Groups
SynthTrack *synthTracksCombo[FADER_COUNT] = {
   &mnmTrack1, &mnmTrack2, &obTrack1, &tetraTrack1
};

SynthTrack *synthTracks1To4[FADER_COUNT] = {
   &mnmTrack1, &mnmTrack2, &mnmTrack3, &mnmTrack4
};

SynthTrack *synthTracks5To8[FADER_COUNT] = {
   &mnmTrack5, &mnmTrack6, &obTrack1, &tetraTrack1
};

uint8_t faderMuteNotesLeft[FADER_COUNT] = {
    noteBottomLeftLightRing1, noteBottomLeftLightRing2, noteBottomLeftLightRing3, noteBottomLeftLightRing4
};
uint8_t faderMuteNotesRight[FADER_COUNT] = { 
    noteBottomRightLightRing1, noteBottomRightLightRing2, noteBottomRightLightRing3, noteBottomRightLightRing4
};


// Xone4d Controller Track Type Modes
enum xone4DControllerTrackType_t {
MACHINE_DRUM,
SYNTH,
XONE_4D_CTRLR_TRACK_TYPE_COUNT
};

const char *xone4DControllerTrackTypeNames[XONE_4D_CTRLR_TRACK_TYPE_COUNT] = {
  "MACHINEDRUM TRKS",
  "MNM OB TETRA TRK"
};

// Xone4d Rotary Pot Functions
enum potFunctionMode_t {
    EQ_FREQ_GAIN_EDIT,
    FILTER_FREQ_WIDTH_EDIT,
    DELAY_REVERB_SEND,
    XONE_4D_POT_FN_MODE_COUNT
};

const char *xone4DPotFunctionModeNames[XONE_4D_POT_FN_MODE_COUNT] = {
  "EQ FRQ GAIN EDIT",
  "FLTW FLTF EDIT",  
  "DELAY RVERB SEND"
};


// The actual edit parameters (CC's) for pot function mode, by Track Type
uint8_t potFunctionParameterNumbers[XONE_4D_CTRLR_TRACK_TYPE_COUNT][XONE_4D_POT_FN_MODE_COUNT][2] = {
    // Track Type = MACHINE_DRUM
    {
        {MODEL_EQF, MODEL_EQG},
        {MODEL_FLTF, MODEL_FLTW}, 
        {MODEL_DEL, MODEL_REV}
    },  
    // Track Type = SYNTH    
    {
        {80, 81},
        {72, 73}, 
        {84, 85} 
    }
};

// The edit parameter names for pot function mode, by Track Type
const char * potFunctionParameterNames[XONE_4D_CTRLR_TRACK_TYPE_COUNT][XONE_4D_POT_FN_MODE_COUNT][2] = {
    // Track Type = MACHINE_DRUM
    {
        {"EQ FREQ", "EQ GAIN"},
        {"FILTER FREQ", "FILTER WIDTH"}, 
        {"DELAY SEND", "REVERB SEND"}
    },  
    // Track Type = SYNTH    
    {
        {"EQ FREQ", "EQ GAIN"},
        {"FILTER FREQ", "FILTER WIDTH"}, 
        {"DELAY SEND", "DELAY FEEDBACK"}
    }
};
                                




// Xone4d Controller Modes
enum xone4DControllerMode_t {
MACHINE_DRUM_LMH,
MACHINE_DRUM_L,
MACHINE_DRUM_M,
MACHINE_DRUM_H,
SYNTH_COMBO,
SYNTH_1_4,
SYNTH_5_8,
XONE_4D_NATIVE,
XONE_4D_CTRLR_MODE_COUNT
};

const char *xone4DControllerModeNames[XONE_4D_CTRLR_MODE_COUNT] = {
  "MD LOW MID HIGH ",
  "MD LOW (TRK 1-4)",
  "MD MID (TRK 5-8)",
  "MD HIGH (TK9-12)",  
  "MNM 1-2 OB TETRA", 
  "MNM TRACKS 1-4  ",
  "MNM 5-6 OB TETRA",  
  "XONE 4D NATIVE  "
};

class Xone4DFaderBank {
public:
  char *name;
  MDTrack *mdTrackGroup[FADER_COUNT];
  SynthTrack *synthTrackGroup[FADER_COUNT];
  uint8_t faderMuteNoteGroup[FADER_COUNT], number, trackGroup;
  xone4DControllerTrackType_t trackType;
  xone4DControllerMode_t controllerMode;
  potFunctionMode_t potFunctionMode;
  bool encoderDown;
  
  Xone4DFaderBank(
      uint8_t _number,
      char * _name,
      MDTrack *_mdTrackGroup[FADER_COUNT],
      SynthTrack *_synthTrackGroup[FADER_COUNT],
      uint8_t _faderMuteNoteGroup[FADER_COUNT], 
      xone4DControllerTrackType_t _trackType = MACHINE_DRUM,
      uint8_t _trackGroup = 0,
      xone4DControllerMode_t _controllerMode = MACHINE_DRUM_LMH,
      potFunctionMode_t _potFunctionMode = EQ_FREQ_GAIN_EDIT,
      bool _encoderDown = false
  ){      
      number = _number;
      name = _name;
      setMDTracks(_mdTrackGroup);
      setSynthTracks(_synthTrackGroup);
      setFaderMuteNotes(_faderMuteNoteGroup);      
      trackType = _trackType; 
      trackGroup = _trackGroup;
      controllerMode = _controllerMode;
      potFunctionMode = _potFunctionMode;    
      encoderDown = _encoderDown;    
  }
  
  void setEncoderDown(bool value){
      encoderDown = value;
  }  
  
  void setLatchEnabled(bool enabled){
     for (int i=0; i<FADER_COUNT; i++){
         mdTrackGroup[i]->setLatchEnabled(enabled);
         synthTrackGroup[i]->setLatchEnabled(enabled);
     }
  }
  
  void setTrackTypeAndGroup(uint8_t value){

     setLatchEnabled(false);
    
     if (encoderDown){    
         trackType = (xone4DControllerTrackType_t) value;      
     } else {
         trackGroup = value;  
     }

     // Now update the controllerMode 
     switch(trackType){       
         case MACHINE_DRUM:
             switch (trackGroup){                 
                 case 0:
                     setControllerMode(MACHINE_DRUM_L);
                     return;                     
                 case 1:
                     setControllerMode(MACHINE_DRUM_M);
                     return;                     
                 case 2:
                     setControllerMode(MACHINE_DRUM_H);
                     return;                     
                 case 3:  
                     setControllerMode(MACHINE_DRUM_LMH);
                     return;                                    
             }
             return;
             
         case SYNTH:
             switch (trackGroup){                 
                 case 0:
                     setControllerMode(SYNTH_1_4);
                     return;                     
                 case 1:
                     setControllerMode(SYNTH_5_8);
                     return;                     
                 // set when trackGroup = 2 or 3
                 default:
                     setControllerMode(SYNTH_COMBO);
                     return;                     
             }
             return;       
       
     }
    
  }
  
  void setControllerMode(xone4DControllerMode_t mode){
      controllerMode = mode;
      GUI.flash_strings_fill("X4D CTRL MODE:", xone4DControllerModeNames[mode]);
      
      switch (controllerMode){
        
          case MACHINE_DRUM_LMH:
              setMDTracks(mdTracksLowMidHigh);
              break;
              
          case MACHINE_DRUM_L:
              setMDTracks(mdTracksLow);
              break;
              
          case MACHINE_DRUM_M:
              setMDTracks(mdTracksMid);
              break;
              
          case MACHINE_DRUM_H:
              setMDTracks(mdTracksHigh);
              break;
              
          case SYNTH_COMBO:
              setSynthTracks(synthTracksCombo);
              break;
              
          case SYNTH_1_4:
              setSynthTracks(synthTracks1To4);
              break;
              
          case SYNTH_5_8:
              setSynthTracks(synthTracks5To8);
              break;
                            
          default:
              return;          
      }
      updateLightRings();
  }  
  
  void setMDTracks(MDTrack *trackGroup[4]){
     for (int i=0; i<FADER_COUNT; i++){
         mdTrackGroup[i] = trackGroup[i];
     }
  }
   
  void setSynthTracks(SynthTrack *trackGroup[4]){
      for (int i=0; i<FADER_COUNT; i++){
          synthTrackGroup[i] = trackGroup[i];
      }
  }  
    
  void setFaderMuteNotes(uint8_t noteGroup[4]){
      for (int i=0; i<FADER_COUNT; i++){    
          faderMuteNoteGroup[i] = noteGroup[i];
      }
  }    
  
  void mapFaderControlChange(uint8_t faderNumber, uint8_t ccAmount){
            
      switch (trackType){

        case MACHINE_DRUM:
              {
                MDTrack *track = mdTrackGroup[faderNumber];          
                track->setTrackLevel(TRACK_VOL, ccAmount);  
              }
              break;
              
          case SYNTH:
              {
                  SynthTrack *track = synthTrackGroup[faderNumber]; 
                  track->setTrackLevel(MIDI_TRACK_VOL, ccAmount);  
              }
              break;
              
          default:
              return;              
      }     
  }
  

  
  void mapPotControlChange(uint8_t faderNumber, uint8_t ccAmount, uint8_t upperEncoder){
    
      char *trackName;
      
      switch(trackType){       
          case MACHINE_DRUM:
              {
                  MDTrack *track = mdTrackGroup[faderNumber];
                  trackName = track->name;                
                  MD.setTrackParam(track->number, potFunctionParameterNumbers[trackType][potFunctionMode][upperEncoder], ccAmount);  
              }
              break;
             
          // TODO:  need to handle OB / TETRA correctly...
          case SYNTH:
              {
                  SynthTrack *track = synthTrackGroup[faderNumber]; 
                  trackName = track->name;                
                  MidiUart.sendCC(track->number + mnmBaseMidiChannel, potFunctionParameterNumbers[trackType][potFunctionMode][upperEncoder], ccAmount);                    
              }
              break;
                   
          default:
              return;              
  
      }
    
      GUI.flash_strings_fill(trackName, potFunctionParameterNames[trackType][potFunctionMode][upperEncoder]);          
      
  }    
  
  void setPotFunctionMode(potFunctionMode_t mode){
      potFunctionMode = mode;
      GUI.flash_strings_fill("POT FUNCTN MODE:", xone4DPotFunctionModeNames[mode]);
  }
  
  void toggleNoteRepeatTranspose(uint8_t faderNumber){  
      
      switch(trackType){       
          case MACHINE_DRUM:
              {
                  MDTrack *track = mdTrackGroup[faderNumber];             
                  track->toggleNoteRepeatEnabled();
              }
              return;
             
          case SYNTH:
              {
                  SynthTrack *track = synthTrackGroup[faderNumber];              
                  track->toggleTransposeEnabled();
              }
              return;
            
          default:
              return;                
      }      
  }
  
  void mapLightRingNote(uint8_t faderNumber){  
      
      switch(trackType){       
          case MACHINE_DRUM:
              {
                  MDTrack *track = mdTrackGroup[faderNumber];             
                  track->toggleMute();
                  track->setMuteLightRing(faderMuteNoteGroup[faderNumber]);
              }
              return;
             
          case SYNTH:
              {
                  SynthTrack *track =  synthTrackGroup[faderNumber];              
                  track->toggleMute();
                  track->setMuteLightRing(faderMuteNoteGroup[faderNumber]);                  
              }
              return;
            
          default:
              return;                
      }      
  }    
  
  void updateLightRings(){    

      switch(trackType){       
          case MACHINE_DRUM:
              for (int i=0; i<FADER_COUNT; i++){
                   MDTrack *track = mdTrackGroup[i];
                       track->setMuteLightRing(faderMuteNoteGroup[i]);                    
              }
              return;
             
          case SYNTH:
              for (int i=0; i<FADER_COUNT; i++){
                  SynthTrack *track = synthTrackGroup[i];    
                  track->setMuteLightRing(faderMuteNoteGroup[i]);                 
              }
              return;            
      }               
  }
  
  
  // 09/10/11 - TODO:  fix this function.  Seems to break the firmware when called..?
  void syncLightRings(){    

      switch(trackType){       
          case MACHINE_DRUM:
              for (int i=0; i<FADER_COUNT; i++){
                   MDTrack *track = mdTrackGroup[i];
                   if(track->muteChanged){  
                       track->setMuteLightRing(faderMuteNoteGroup[i]); 
                       break;                   
                   }
              }
              break;
             
          case SYNTH:
              for (int i=0; i<FADER_COUNT; i++){
                  SynthTrack *track = synthTrackGroup[i];    
                  if(track->muteChanged){            
                      track->setMuteLightRing(faderMuteNoteGroup[i]);       
                      break;                      
                  }
              }
              break;            
      }   
      
      for (int i=0; i<MD_TRACK_COUNT; i++){
          MDTrack *track = mdTracks[i];
          track->setMuteChanged(false);
      }
      
      for (int i=0; i<SYNTH_TRACK_COUNT; i++){
          SynthTrack *track = synthTracks[i]; 
          track->setMuteChanged(false);
      }
      
  }
  

};

// Instantiate X4D Fader Banks
Xone4DFaderBank xone4DFaderBank1(1, "X4D LEFT FADERS ", mdTracksLowMidHigh, synthTracksCombo, faderMuteNotesLeft);
Xone4DFaderBank xone4DFaderBank2(2, "X4D RIGHT FADERS", mdTracksLowMidHigh, synthTracksCombo, faderMuteNotesRight);


class Xone4D {
public:
  Xone4DFaderBank *xone4DFaderBank1, *xone4DFaderBank2;   
  bool xone4dNativeControlMode;

  Xone4D (
      Xone4DFaderBank *_xone4DFaderBank1, 
      Xone4DFaderBank *_xone4DFaderBank2, 
      bool _xone4dNativeControlMode = false
  ){      
      xone4DFaderBank1 = _xone4DFaderBank1; 
      xone4DFaderBank2 = _xone4DFaderBank2; 
      xone4dNativeControlMode = _xone4dNativeControlMode;
  }
       
  // need to push encoder AND button to toggle controller mode
  void toggleXone4DNativeControllerMode() {

    if (xone4DFaderBank2->encoderDown){
        xone4dNativeControlMode = !xone4dNativeControlMode;
        xone4DFaderBank1->setLatchEnabled(false);                  
        xone4DFaderBank2->setLatchEnabled(false);          
        if (xone4dNativeControlMode){
            GUI.flash_strings_fill(xone4DControllerModeNames[XONE_4D_NATIVE], "MODE: ON");
        } else {
            GUI.flash_strings_fill(xone4DControllerModeNames[XONE_4D_NATIVE], "MODE: OFF");
        }
    }     
  }  
  
};

// Instantiate X4D object
Xone4D xone4D(&xone4DFaderBank1, &xone4DFaderBank2);
