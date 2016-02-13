class Xone4DController : public ClockCallback, public MidiCallback {
public:  
    
  void doTranspose(uint8_t rawNoteNumber, uint8_t midiChannel){     
       int scaledNoteNumber = (rawNoteNumber + 12) % 24; 
       int pitchbendAmt = 16384 / 24; 
       if ((rawNoteNumber == 48) || (rawNoteNumber == 0) || (rawNoteNumber == 96)){
           // Hack to allow transpose down by one octave.  Novation Remote must be set to oct range -4, 0 or +4 to work properly.
           MidiUart.sendPitchBend(midiChannel, 8192);    
//           uint8_t val = 51;
//           MidiUart.sendNRPN(mnmBaseMidiChannel, 116, val);           
       } else {           
           MidiUart.sendPitchBend(midiChannel, scaledNoteNumber * pitchbendAmt);    
//           uint8_t val = 51 + scaledNoteNumber;
//           MidiUart.sendNRPN(mnmBaseMidiChannel, 116, val);           
       }                
  }   
       
  void on16Callback(uint32_t pos) {
    
      uint8_t val = (MidiClock.div16th_counter) % 16;   
      uint8_t val2 = (MidiClock.div16th_counter) % 32; 
      
      if (triggerRamR1TrackFlag && (val == 0)){
          MD.triggerTrack(ramR1Track, 127); 
          triggerRamR1TrackFlag = false;
      }
      
      if (triggerRamP1TrackFlag){
          MD.sliceTrack32(ramP1Track, val2, 32, true);    
          triggerRamP1TrackFlag = false;
      }
              
      for (int i = 0; i < 12; i++) {     
          if (mdTracks[i]->noteRepeatEnabled){ 
              doNoteRepeat(mdTracks[i]); 
          }         
      }      
  }
  
  void doNoteRepeat(MDTrack *track){
    
    uint8_t val = (MidiClock.div16th_counter) % 16;    
    uint8_t val2 = (MidiClock.div16th_counter + track->noteRepeatStartStep) % 12;
    uint8_t val3 = (MidiClock.div16th_counter + track->noteRepeatStartStep) % 6;
    
    // Set note repeat speed.  Use track setting first, but change it if an "override" speed is currently set in the noteRepeat object
    note_repeat_speed_type_t speed;        
    if (noteRepeat.overrideSpeed){ 
        speed = NOTE_REPEAT_16TH;
    } else {
        speed = track->noteRepeatSpeed; 
    }
              
    if (random(100) <= noteRepeat.probability){
        
        switch(speed){
              
              case NOTE_REPEAT_8TH:
                  if((val%2) == 0){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
              
              case NOTE_REPEAT_FILL_1:
                  if((val == 13) || (val == 15)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_FILL_2:
                  if(val == 13){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
              
              case NOTE_REPEAT_FILL_3:
                  if((val == 12) || (val == 13) || (val == 14) || (val == 15)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_FILL_4:
                  if((val == 9) || (val == 10) || (val == 14)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_FILL_5:
                  if((val == 11) || (val == 14)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_1_6TH:
                  if(val2 % 6 == 0){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;

              case NOTE_REPEAT_2_6TH:
                  if((val2 == 0) || (val2 == 2)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_3_6TH:
                  if((val2 == 0) || (val2 == 2) || (val2 == 4)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;

              case NOTE_REPEAT_4_6TH:
                  if((val2 == 0) || (val2 == 2) || (val2 == 4) || (val2 == 6)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                                    
              case NOTE_REPEAT_5_6TH:
                  if((val2 == 0) || (val2 == 2) || (val2 == 4) || (val2 == 6) || (val2 == 8)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
  
              case NOTE_REPEAT_2_16TH:
                  if((val3 == 0) || (val3 == 1)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_3_16TH:
                  if((val3 == 0) || (val3 == 1) || (val3 == 2)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;

              case NOTE_REPEAT_4_16TH:
                  if((val3 == 0) || (val3 == 1) || (val3 == 3) || (val3 == 4)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_5_16TH:
                  if((val3 == 1) || (val3 == 2) || (val3 == 3) || (val3 == 4) || (val3 == 5)){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
              
              case NOTE_REPEAT_1_3RD:
                  if(val2 % 3 == 0){
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                  
              case NOTE_REPEAT_2_3RD:
                  if((val2 % 3 == 0) || (val2 % 3 == 1)) {
                      MD.triggerTrack(track->number, noteRepeat.velocity); 
                  }
                  return;
                         
              case NOTE_REPEAT_16TH:
                  MD.triggerTrack(track->number, noteRepeat.velocity); 
                  return;          
              
              default:
                  return;    
          }
          
      }

  }
  

//   void onControlChange(uint8_t *msg) {
//        
//        // Debug - print a message to the screen to show the input data type received
////        debugPage.displayMessageDetails(msg, "CC ", "IN1"); 
//          
//        uint8_t midiChannel = MIDI_VOICE_CHANNEL(msg[0]) ;
//        uint8_t ccNumber = msg[1];
//        uint8_t ccValue = msg[2];
//        uint8_t track = 100;
//        uint8_t param = 100;
//        char *trackgroup, *tracknumber;
//        
//        MD.parseCC(midiChannel, ccNumber, &track, &param);
//        debugPage.debugParam1.setName("TRK");
//        debugPage.debugParam1.setValue(track);    
//        debugPage.debugParam2.setName("PRM");
//        debugPage.debugParam2.setValue(param);                     
//        debugPage.update();      
//        
//        // Echo the CC out on the same midi channel
//        MidiUart.sendCC(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);
//
//  }  
  
  // Method for tracking manual changes to MD mutes - not working as MD no longer transmits CCs when muting through the MD UI?
//   void onControlChange(uint8_t *msg) {
//        
//        // Debug - print a message to the screen to show the input data type received
////        debugPage.displayMessageDetails(msg, "CC ", "IN1"); 
//          
//        int midiChannel = MIDI_VOICE_CHANNEL(msg[0]) ;
//        int ccNumber = msg[1];
//        int ccValue = msg[2];
//        int track = -100;
//        char *trackgroup, *tracknumber;
//
//        // If we have a "MachineDrum" midi channel then assign a non-negative track value
//        switch (midiChannel){
//            case mdBaseMidiChannel:
//                track = 0;
//                trackgroup = "TRK 1-4";
//                break;                    
//            case (mdBaseMidiChannel + 1):
//                track = 4;
//                trackgroup = "TRK 5-8";
//                break;                
//            case (mdBaseMidiChannel + 2):
//                track = 8;
//                trackgroup = "TRK 9-13";                    
//                break;                
//            case (mdBaseMidiChannel + 3):
//                track = 12;
//                trackgroup = "TRK 14-16";                                        
//                break;    
//            default:
//                break;                
//        }
//  
//        // If we have a "Mute" CC value then increment the track number
//        switch (ccNumber){                                              
//              case MD_TRACK_MUTE_1:                        
//                  track += 1;   
//                  tracknumber = "TRACK 1";                                          
//                  break;   
//              case MD_TRACK_MUTE_2:                        
//                  track += 2;   
//                  tracknumber = "TRACK 2";                                                                
//                  break;   
//              case MD_TRACK_MUTE_3:                        
//                  track += 3;   
//                  tracknumber = "TRACK 3";                                                                
//                  break;   
//              case MD_TRACK_MUTE_4:                        
//                  tracknumber = "TRACK 4";                                                            
//                  track += 4;   
//                  break;                
//              default:
//                  break;                
//        }   
//            
//        if (track > 0){
//            // If ccValue = 0 then the track has been muted, so update the mdTrack.muted flag 
//            if (ccValue > 0){
//                mdTracks.getMDTrack(track)->setMuted(true);    
//            } else {
//                mdTracks.getMDTrack(track)->setMuted(false);                             
//            }
//        }  
//        
//        // Echo the CC out on the same midi channel
//        MidiUart.sendCC(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);
//
//  }  
 
void onControlChange2(uint8_t *msg) {
          
  	if (MIDI_VOICE_CHANNEL(msg[0]) == xone4DMidiChannel) {

            int ccNumber = msg[1];
            int ccValue = msg[2];
    
            // If Xone is in "native" mode then echo to the Midi Out on ableton midichannel 
            if (xone4D.xone4dNativeControlMode){
                MidiUart.sendCC(abletonMidiChannel, ccNumber, ccValue);
                return;
            }

            switch (ccNumber){                               
              
                case ccLeftFader1:                        
                    xone4DFaderBank1.mapFaderControlChange(0, ccValue);   
                    return; 
                
                case ccLeftFader2:
                    xone4DFaderBank1.mapFaderControlChange(1, ccValue);   
                    return;                       
                
                case ccLeftFader3:
                    xone4DFaderBank1.mapFaderControlChange(2, ccValue);   
                    return; 
                
                case ccLeftFader4:
                    xone4DFaderBank1.mapFaderControlChange(3, ccValue);   
                    return;      
    
                case ccRightFader1:
                    xone4DFaderBank2.mapFaderControlChange(0, ccValue);  
                    return; 
                
                case ccRightFader2:
                    xone4DFaderBank2.mapFaderControlChange(1, ccValue);  
                    return;                       
                
                case ccRightFader3:
                    xone4DFaderBank2.mapFaderControlChange(2, ccValue);  
                    return; 
                
                case ccRightFader4:
                    xone4DFaderBank2.mapFaderControlChange(3, ccValue);  
                    return; 

                case ccTopLeftPot1:  
                    xone4DFaderBank1.mapPotControlChange(0, ccValue, 0); 
                    return;    
                    
                case ccBottomLeftPot1:                    
                    xone4DFaderBank1.mapPotControlChange(0, ccValue, 1);               
                    return;    
    
                case ccTopLeftPot2:
                    xone4DFaderBank1.mapPotControlChange(1, ccValue, 0);
                    return;          
    
                case ccBottomLeftPot2:
                    xone4DFaderBank1.mapPotControlChange(1, ccValue, 1);
                    return;                            
    
                case ccTopLeftPot3:
                    xone4DFaderBank1.mapPotControlChange(2, ccValue, 0);
                    return;               
    
                case ccBottomLeftPot3:
                    xone4DFaderBank1.mapPotControlChange(2, ccValue, 1);
                    return;                         
                
                case ccTopLeftPot4:
                    xone4DFaderBank1.mapPotControlChange(3, ccValue, 0);
                    return;        

                case ccBottomLeftPot4:
                    xone4DFaderBank1.mapPotControlChange(3, ccValue, 1);
                    return;                         
                
                case ccTopRightPot1:
                    xone4DFaderBank2.mapPotControlChange(0, ccValue, 0); 
                    return;      
    
                case ccBottomRightPot1:
                    xone4DFaderBank2.mapPotControlChange(0, ccValue, 1); 
                    return;                        
                
                case ccTopRightPot2:
                    xone4DFaderBank2.mapPotControlChange(1, ccValue, 0); 
                    return;        
    
                case ccBottomRightPot2:
                    xone4DFaderBank2.mapPotControlChange(1, ccValue, 1); 
                    return;                         
                
                case ccTopRightPot3:
                    xone4DFaderBank2.mapPotControlChange(2, ccValue, 0); 
                    return;        
    
                case ccBottomRightPot3:
                    xone4DFaderBank2.mapPotControlChange(2, ccValue, 1);
                    return;                        
                
                case ccTopRightPot4:
                    xone4DFaderBank2.mapPotControlChange(3, ccValue, 0);
                    return;     
    
                case ccBottomRightPot4:
                    xone4DFaderBank2.mapPotControlChange(3, ccValue, 1);
                    return;                         
/*
                case ccTopLeftEncoder1:
                    return;                         
                    
                case ccTopLeftEncoder2:
                    return;                         
                    
                case ccTopLeftEncoder3:
                    return;                         
                    
                case ccTopLeftEncoder4:
                    return;                         
                    
                case ccTopRightEncoder1:                
                    return;                         
                    
                case ccTopRightEncoder2:                
                    return;                         
                    
                case ccTopRightEncoder3:                
                    return;                         
                    
                case ccTopRightEncoder4:                                
                    return;                         
                */    
                default:
                    return;    
             
            }   
        }

        // Echo the CC out on the same midi channel
        MidiUart.sendCC(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);

  }
  
  void onNoteOn2(uint8_t *msg) {
        
        uint8_t noteNumber = msg[1];
        uint8_t noteValue = msg[2];
    
//        if (MIDI_VOICE_CHANNEL(msg[0]) == mnmTransposeMidiChannel){
//           if (mnmTrack1.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack1.number);
//           if (mnmTrack2.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack2.number);
//           if (mnmTrack3.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack3.number);
//           if (mnmTrack4.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack4.number);
//           if (mnmTrack5.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack5.number);
//           if (mnmTrack6.transposeEnabled)
//               doTranspose(noteNumber, mnmBaseMidiChannel + mnmTrack6.number);
//           if (obTrack1.transposeEnabled)
//               doTranspose(noteNumber, oberheimMidiChannel);
//           return;           
//                  
//        }
    
        if (MIDI_VOICE_CHANNEL(msg[0]) == xone4DMidiChannel) {                      

            // If Xone is in "native" mode and note is not a "Xone Controller Mode Change" then echo to the Midi Out but on abletonMidiChannel
            if ((xone4D.xone4dNativeControlMode) && (noteNumber!= noteBottomRightButton4) && (noteNumber!= noteBottomRightEncoder1)){
                MidiUart.sendNoteOn(abletonMidiChannel, noteNumber, noteValue);     
                return;
            }
                      
            switch (noteNumber){

//                case noteTopLeftEncoder1:
//                case noteTopLeftEncoder2:                           
//                case noteTopLeftEncoder3:
//                case noteTopLeftEncoder4:                
//                case noteTopRightEncoder1:
//                case noteTopRightEncoder2:                
//                case noteTopRightEncoder3:
//                case noteTopRightEncoder4:                

                case noteTopLeftButton1:
                    xone4DFaderBank1.toggleNoteRepeatTranspose(0);
                    return;
                    
                case noteTopLeftButton2:
                    xone4DFaderBank1.toggleNoteRepeatTranspose(1);
                    return;

                case noteTopLeftButton3:
                    xone4DFaderBank1.toggleNoteRepeatTranspose(2);
                    return;

                case noteTopLeftButton4:
                    xone4DFaderBank1.toggleNoteRepeatTranspose(3);
                    return;
                    
                case noteTopRightButton1:
                    xone4DFaderBank2.toggleNoteRepeatTranspose(0);
                    return;
                    
                case noteTopRightButton2:
                    xone4DFaderBank2.toggleNoteRepeatTranspose(1);
                    return;

                case noteTopRightButton3:
                    xone4DFaderBank2.toggleNoteRepeatTranspose(2);
                    return;

                case noteTopRightButton4:
                    xone4DFaderBank2.toggleNoteRepeatTranspose(3);
                    return;
                    
                case noteMiddleLeftButton1:
                    xone4DFaderBank1.setPotFunctionMode(EQ_FREQ_GAIN_EDIT);
                    return;

                case noteMiddleLeftButton2:
                    xone4DFaderBank1.setPotFunctionMode(FILTER_FREQ_WIDTH_EDIT);
                    return;
                    
                case noteMiddleLeftButton3:  
                    xone4DFaderBank1.setPotFunctionMode(DELAY_REVERB_SEND);
                    return;

//                case noteMiddleLeftButton4:  

                case noteMiddleRightButton1:
                    xone4DFaderBank2.setPotFunctionMode(EQ_FREQ_GAIN_EDIT);
                    return;

                case noteMiddleRightButton2:
                    xone4DFaderBank2.setPotFunctionMode(FILTER_FREQ_WIDTH_EDIT);
                    return;
                    
                case noteMiddleRightButton3:  
                    xone4DFaderBank2.setPotFunctionMode(DELAY_REVERB_SEND);
                    return;

//                case noteMiddleRightButton4: 
//                case noteBottomLeftButton1:
//                case noteBottomLeftButton2:
//                case noteBottomLeftButton3:
//                case noteBottomLeftButton4:                    
//                case noteBottomRightButton1:
//                case noteBottomRightButton2:
//                case noteBottomRightButton3:

                case noteBottomRightButton4:       
                   xone4D.toggleXone4DNativeControllerMode();
                   return;
                   
//                // not used as this toggles "shift mode" on the Xone4D itself   
//                case noteBottomLeftEncoder1:
//                    return; 
                    
                case noteBottomRightEncoder1:
                    xone4DFaderBank1.setEncoderDown(true);    
                    xone4DFaderBank2.setEncoderDown(true);           
                    return;              
             
                case noteLeftJogLeftClick:
                    xone4DFaderBank1.setTrackTypeAndGroup(0);
                    return;             
             
                case noteLeftJogUpClick:
                    xone4DFaderBank1.setTrackTypeAndGroup(1);
                    return;

                case noteLeftJogRightClick:
                    xone4DFaderBank1.setTrackTypeAndGroup(2);
                    return;

                case noteLeftJogDownClick:
                    xone4DFaderBank1.setTrackTypeAndGroup(3);
                    return;

                case noteRightJogLeftClick:      
                    xone4DFaderBank2.setTrackTypeAndGroup(0);
                    return;             

                case noteRightJogUpClick:
                    xone4DFaderBank2.setTrackTypeAndGroup(1);
                    return;             

                case noteRightJogRightClick:
                    xone4DFaderBank2.setTrackTypeAndGroup(2);
                    return;             

                case noteRightJogDownClick:
                    xone4DFaderBank2.setTrackTypeAndGroup(3);
                    return;                                                                        

                case noteBottomLeftLightRing1:   
                    xone4DFaderBank1.mapLightRingNote(XONE4D_FADER_1);
                    xone4DFaderBank2.syncLightRings();
                    return;
                 
                case noteBottomLeftLightRing2:    
                    xone4DFaderBank1.mapLightRingNote(XONE4D_FADER_2);
                    xone4DFaderBank2.syncLightRings();
                    return;
                  
                case noteBottomLeftLightRing3:                        
                    xone4DFaderBank1.mapLightRingNote(XONE4D_FADER_3);
                    xone4DFaderBank2.syncLightRings();
                    return;
                
                case noteBottomLeftLightRing4:                        
                    xone4DFaderBank1.mapLightRingNote(XONE4D_FADER_4);
                    xone4DFaderBank2.syncLightRings();
                    return;
                
                case noteBottomRightLightRing1:              
                    xone4DFaderBank2.mapLightRingNote(XONE4D_FADER_1);
                    xone4DFaderBank1.syncLightRings();
                    return;
                 
                case noteBottomRightLightRing2:                        
                    xone4DFaderBank2.mapLightRingNote(XONE4D_FADER_2);
                    xone4DFaderBank1.syncLightRings();
                    return;
                  
                case noteBottomRightLightRing3:                        
                    xone4DFaderBank2.mapLightRingNote(XONE4D_FADER_3);
                    xone4DFaderBank1.syncLightRings();
                    return;
                
                case noteBottomRightLightRing4:                        
                    xone4DFaderBank2.mapLightRingNote(XONE4D_FADER_4);
                    xone4DFaderBank1.syncLightRings();
                    return;
                          
                default:
                     return; 
              
            }               
         
        }
        
        // Echo the note out on the same midi channel
        MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);
        
  }
  
  void onNoteOff2(uint8_t *msg) {  
    
        // Debug - print a message to the screen to show the input data type received
        //debugPage.displayMessageDetails(msg, "NOF", "IN2");

        uint8_t noteNumber = msg[1];
        
        if ((MIDI_VOICE_CHANNEL(msg[0]) == mnmTransposeMidiChannel) || (MIDI_VOICE_CHANNEL(msg[0]) == xone4DMidiChannel)) {     
            
            switch (noteNumber){

//                // not used as this toggles "shift mode" on the Xone4D itself  
//                case noteBottomLeftEncoder1:
//                    return;    
                                 
                case noteBottomRightEncoder1:
                    xone4DFaderBank1.setEncoderDown(false);
                    xone4DFaderBank2.setEncoderDown(false);
                    return;
                                                                  
                default:
                     return; 
              
            }  
        }        
        
        // Echo the note out on the same midi channel
        MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);    
        
  }  
  
  void toggleMdRamP1TrackMute(bool& muteStatus){

     MDTrack * mdTrack;   
      
     for (int i = 0; i < 16; i++) {
  
          mdTrack = mdTracks[i];
          
          if (i == ramP1Track) {     
              if(muteStatus){                        
                  MD.unmuteTrack(i); 
                  MD.setTrackParam(i, MODEL_VOL, 127);
                  triggerRamP1TrackFlag = true;
              } else {
                  MD.muteTrack(i, true);
                  MD.setTrackParam(i, MODEL_VOL, 0);
              }                 
          } else {
              if(muteStatus){        
                  MD.muteTrack(i, muteStatus);    
              } else {
                  if(!mdTrack->muted){                  
                      MD.unmuteTrack(i); 
                  }
              }
          }
      }  
//      if(muteStatus){
//          GUI.flash_strings_fill("MD LOW/MID/HIGH:", "MUTED");  
//      }else{
//          GUI.flash_strings_fill("MD LOW/MID/HIGH:", "UNMUTED");  
//      }
      muteStatus = !muteStatus;
  }
  
  void initialiseMDMutes(){      
      // Unmute tracks 1-15
      for (int i = 0; i < 15; i++) {       
         mdTracks[i]->setMuted(false);
         MD.unmuteTrack(i);                          
      }  
      // Mute track 16
      mdTracks[15]->setMuted(true);
      MD.muteTrack(15, true);                          
  }
  
  void setup() {
      MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&Xone4DController::on16Callback);
      Midi2.addOnNoteOnCallback(this, (midi_callback_ptr_t)&Xone4DController::onNoteOn2);
      Midi2.addOnNoteOffCallback(this, (midi_callback_ptr_t)&Xone4DController::onNoteOff2);      
//      Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&Xone4DController::onControlChange);      
      Midi2.addOnControlChangeCallback(this, (midi_callback_ptr_t)&Xone4DController::onControlChange2);
      initialiseMDMutes();      
      xone4DFaderBank1.updateLightRings();
      xone4DFaderBank2.updateLightRings();      
  }
  
};


  
        

