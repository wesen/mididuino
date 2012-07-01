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

#define INTERNAL_TRACK_1_MIDI_CHANNEL (1 - 1)  // OT Track / Midi Channel 1
#define INTERNAL_TRACK_2_MIDI_CHANNEL (2 - 1)  // OT Track / Midi Channel 2
#define INTERNAL_TRACK_3_MIDI_CHANNEL (3 - 1)  // OT Track / Midi Channel 3
#define INTERNAL_TRACK_4_MIDI_CHANNEL (4 - 1)  // OT Track / Midi Channel 4
#define INTERNAL_TRACK_5_MIDI_CHANNEL (5 - 1)  // OT Track / Midi Channel 5
#define INTERNAL_TRACK_6_MIDI_CHANNEL (6 - 1)  // OT Track / Midi Channel 6
#define INTERNAL_TRACK_7_MIDI_CHANNEL (7 - 1)  // OT Track / Midi Channel 7
#define INTERNAL_TRACK_8_MIDI_CHANNEL (8 - 1)  // OT Track / Midi Channel 8
#define OCTATRACK_TRANSPOSE_MIDI_CHANNEL (15 - 1)  // Midi Channel 15
#define MIDI_TRACK_1_MIDI_CHANNEL (5 - 1)  // MIDI Track 1 / Midi Channel 5  (MNM-1)
#define MIDI_TRACK_2_MIDI_CHANNEL (6 - 1)  // MIDI Track 2 / Midi Channel 6  (MNM-2)
#define MIDI_TRACK_3_MIDI_CHANNEL (7 - 1)  // MIDI Track 3 / Midi Channel 7  (MNM-3)
#define MIDI_TRACK_4_MIDI_CHANNEL (8 - 1)  // MIDI Track 4 / Midi Channel 8  (MNM-4)
#define MIDI_TRACK_5_MIDI_CHANNEL (9 - 1)  // MIDI Track 5 / Midi Channel 9  (MNM-5)
#define MIDI_TRACK_6_MIDI_CHANNEL (10 - 1)  // MIDI Track 6 / Midi Channel 10  (MNM-6)
#define MIDI_TRACK_7_MIDI_CHANNEL (11 - 1)  // MIDI Track 7 / Midi Channel 11  (MNM-MULTI ENV)
#define MIDI_TRACK_8_MIDI_CHANNEL (13 - 1)  // MIDI Track 8 / Midi Channel 13  (TETRA)
#define CC_TRANSPOSE_INTERNAL_TRACK 16  //Playback param 1
#define CC_TRANSPOSE_MIDI_TRACK 22  //Arpeggiator param 1
#define INTERNAL_TRACKS 0  //Array index
#define MIDI_TRACKS 1  //Array index


class OctatrackTransposeClass: public MidiCallback{

  public: 
        int offset[2][8];
        uint8_t transposeCCs[2];
        uint8_t midiChannels[2][8];
        uint8_t transposeTrackEnabled[2][8];
        bool transposeEnabled[2];
   
        OctatrackTransposeClass
        (
        )
        {
	}

        void setup(){      
            setupOffset();   
            setupTransposeCCs();  
            setupTransposeTrackEnabled();
            setupTransposeEnabled();
            setupMidiChannels();
            Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&OctatrackTransposeClass::onNoteOn);
        }      
        
        void setupOffset(){
            for (int i=INTERNAL_TRACKS; i<=MIDI_TRACKS; i++){
                for (int j=0; j<8; j++){
                    offset[i][j] = 0;
                }
            }                        
        }
        
        void setupTransposeCCs(){
            transposeCCs[INTERNAL_TRACKS] = CC_TRANSPOSE_INTERNAL_TRACK;
            transposeCCs[MIDI_TRACKS] = CC_TRANSPOSE_MIDI_TRACK;
        }
        
        void setupTransposeEnabled(){
            transposeEnabled[INTERNAL_TRACKS] = true;
            transposeEnabled[MIDI_TRACKS] = true;
        }
        
        void setTransposeTrackEnabled(uint8_t _trackType, uint8_t _trackNumber, bool _value){
            transposeTrackEnabled[_trackType][_trackNumber] = _value;  
        }
        
        void setOffset(uint8_t _trackType, uint8_t _trackNumber, int _value){
            offset[_trackType][_trackNumber] = _value;  
        }        
        
        bool getTransposeTrackEnabled(uint8_t _trackType, uint8_t _trackNumber){
            return transposeTrackEnabled[_trackType][_trackNumber];  
        }
        
        int getOffset(uint8_t _trackType, uint8_t _trackNumber){
            return offset[_trackType][_trackNumber];  
        }        
      
        void setupTransposeTrackEnabled(){
            // OT Internal Tracks
            transposeTrackEnabled[INTERNAL_TRACKS][0] = true;  //Track 1    
            transposeTrackEnabled[INTERNAL_TRACKS][1] = true;    
            transposeTrackEnabled[INTERNAL_TRACKS][2] = true;    
            transposeTrackEnabled[INTERNAL_TRACKS][3] = true;    
            transposeTrackEnabled[INTERNAL_TRACKS][4] = false;    
            transposeTrackEnabled[INTERNAL_TRACKS][5] = false;    
            transposeTrackEnabled[INTERNAL_TRACKS][6] = false;    
            transposeTrackEnabled[INTERNAL_TRACKS][7] = false;  //Track 8
        
            // Midi Tracks    
            transposeTrackEnabled[MIDI_TRACKS][0] = true;  //Track 1    
            transposeTrackEnabled[MIDI_TRACKS][1] = true;    
            transposeTrackEnabled[MIDI_TRACKS][2] = true;    
            transposeTrackEnabled[MIDI_TRACKS][3] = true;    
            transposeTrackEnabled[MIDI_TRACKS][4] = true;    
            transposeTrackEnabled[MIDI_TRACKS][5] = true;    
            transposeTrackEnabled[MIDI_TRACKS][6] = false;    
            transposeTrackEnabled[MIDI_TRACKS][7] = true;  //Track 8            
        }
        
        void setupMidiChannels(){
            // OT Internal Tracks
            midiChannels[INTERNAL_TRACKS][0] = INTERNAL_TRACK_1_MIDI_CHANNEL;  //Track 1    
            midiChannels[INTERNAL_TRACKS][1] = INTERNAL_TRACK_2_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][2] = INTERNAL_TRACK_3_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][3] = INTERNAL_TRACK_4_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][4] = INTERNAL_TRACK_5_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][5] = INTERNAL_TRACK_6_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][6] = INTERNAL_TRACK_7_MIDI_CHANNEL;    
            midiChannels[INTERNAL_TRACKS][7] = INTERNAL_TRACK_8_MIDI_CHANNEL;  //Track 8
        
            // Midi Tracks    
            midiChannels[MIDI_TRACKS][0] = MIDI_TRACK_1_MIDI_CHANNEL;  //Track 1    
            midiChannels[MIDI_TRACKS][1] = MIDI_TRACK_2_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][2] = MIDI_TRACK_3_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][3] = MIDI_TRACK_4_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][4] = MIDI_TRACK_5_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][5] = MIDI_TRACK_6_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][6] = MIDI_TRACK_7_MIDI_CHANNEL;    
            midiChannels[MIDI_TRACKS][7] = MIDI_TRACK_8_MIDI_CHANNEL;  //Track 8            
        }
        
        
        void onNoteOn(uint8_t *msg) {
            
            uint8_t noteNumber = msg[1];
            uint8_t noteValue = msg[2];
        
            if (MIDI_VOICE_CHANNEL(msg[0]) == OCTATRACK_TRANSPOSE_MIDI_CHANNEL){
                doTranspose(noteNumber);                          
            } 
            
            // Echo Not required when using the MOTU MTP AV USB            
//            else {              
//                // Echo the note out on the same midi channel
//                MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]), msg[1], msg[2]);
//            }
    
        }
        
        void doTranspose(uint8_t rawNoteNumber){    
          
            uint8_t baseNoteNumber, scaledNoteNumber;
          
//            // TODO: FOR MIDI TRACKS (when Elektron enable in the OT OS)
//            for (int trackType=INTERNAL_TRACKS; trackType<=MIDI_TRACKS; trackType++){
  
                int trackType=INTERNAL_TRACKS;            
    
                for (int trackNumber=0; trackNumber<8; trackNumber++){
                  
                    if(transposeTrackEnabled[trackType][trackNumber]){
                        
                        baseNoteNumber = rawNoteNumber + offset[trackType][trackNumber];
                        scaledNoteNumber = baseNoteNumber % 24; 
                         
//                        debugPage.debugParam1.setName("BAS");
//                        debugPage.debugParam1.setValue(baseNoteNumber);    
//                        debugPage.debugParam2.setName("SCL");
//                        debugPage.debugParam2.setValue(scaledNoteNumber);    
//                        debugPage.debugParam3.setName("CHN");
//                        debugPage.debugParam3.setValue(midiChannels[MIDI_TRACKS][0]);                
//                        debugPage.debugParam4.setName("CC");
//                        debugPage.debugParam4.setValue(transposeCCs[MIDI_TRACKS]);    
                       
            
                        // FOR INTERNAL TRACKS
                       if ((baseNoteNumber == 24) || (baseNoteNumber == 72) || (baseNoteNumber == 120)){
                           // Hack to allow transpose up by one full octave.  Novation Remote must be set to oct range -4, 0 or +4 to work properly.
                           MidiUart.sendCC(midiChannels[trackType][trackNumber], transposeCCs[trackType], 127);    
                       } else if (scaledNoteNumber == 12){
                           MidiUart.sendCC(midiChannels[trackType][trackNumber], transposeCCs[trackType], 64);    
                       }  else if (scaledNoteNumber == 0){
                           MidiUart.sendCC(midiChannels[trackType][trackNumber], transposeCCs[trackType], 0);    
                       }  else {
                           MidiUart.sendCC(midiChannels[trackType][trackNumber], transposeCCs[trackType], (scaledNoteNumber * 5) + 4);    
                       }             
            
//                       // TODO: FOR MIDI TRACKS
//                       if ((baseNoteNumber == 24) || (baseNoteNumber == 72) || (baseNoteNumber == 120)){
//                           // Hack to allow transpose up by one full octave.  Novation Remote must be set to oct range -4, 0 or +4 to work properly.
//                           MidiUart.sendCC(midiChannels[MIDI_TRACKS][0], transposeCCs[MIDI_TRACKS], 76);    
//                       } else {
//                           MidiUart.sendCC(midiChannels[MIDI_TRACKS][0], transposeCCs[MIDI_TRACKS], (scaledNoteNumber + 52));    
//                       }                    
                    }        
                    
                }
//            }         
          
                 

        }           

};
