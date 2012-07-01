// Note Repeat Enums, variables
typedef enum  { 
  NOTE_REPEAT_FILL_1 = 0,
  NOTE_REPEAT_FILL_2,
  NOTE_REPEAT_FILL_3,
  NOTE_REPEAT_FILL_4,
  NOTE_REPEAT_FILL_5,
  NOTE_REPEAT_8TH,
  NOTE_REPEAT_16TH,
  NOTE_REPEAT_1_6TH,
  NOTE_REPEAT_2_6TH,  
  NOTE_REPEAT_3_6TH,  
  NOTE_REPEAT_4_6TH,    
  NOTE_REPEAT_5_6TH,
  NOTE_REPEAT_2_16TH,
  NOTE_REPEAT_3_16TH,
  NOTE_REPEAT_4_16TH,
  NOTE_REPEAT_5_16TH,  
  NOTE_REPEAT_1_3RD,
  NOTE_REPEAT_2_3RD,
  NOTE_REPEAT_COUNT
} note_repeat_speed_type_t;

//class NoteRepeat : public ClockCallback{
class NoteRepeat{  
public:
    static const char *repeatSpeedNames[NOTE_REPEAT_COUNT];  
    note_repeat_speed_type_t speed;
    int velocity, probability;
    bool muted, overrideSpeed;
    
    NoteRepeat(){
        speed = (note_repeat_speed_type_t)NOTE_REPEAT_FILL_1;
        overrideSpeed = false;
        velocity = 127;
        probability = 100;
        muted = false;
    }    
};
NoteRepeat noteRepeat;
const char *NoteRepeat::repeatSpeedNames[NOTE_REPEAT_COUNT] = {
      "FILL 1 ", 
      "FILL 2 ", 
      "FILL 3 ", 
      "FILL 4 ",
      "FILL 5 ",      
      "1/8TH  ",      
      "1/16TH ",
      "1/6TH  ",
      "2/6THS  ",
      "3/6THS  ",
      "4/6THS  ",      
      "5/6THS  ",      
      "2/16THS ",
      "3/16THS ",
      "4/16THS ",
      "5/16THS ",
      "1/3RD  ",
      "2/3RD  "
};

// Define the NoteRepeat page as a class as it contains customised logic from a standard EncoderPage
class NoteRepeatPage : public EncoderPage {
  public:
    RangeEncoder noteRepeatSpeedEncoder, noteRepeatSpeedEncoder2, noteRepeatVelocityEncoder, noteRepeatProbabilityEncoder;
    NoteRepeatPage() : EncoderPage() {
      noteRepeatSpeedEncoder.initRangeEncoder(NOTE_REPEAT_FILL_1, NOTE_REPEAT_16TH, "NT", 0);
      noteRepeatSpeedEncoder2.initRangeEncoder(NOTE_REPEAT_1_6TH, NOTE_REPEAT_2_3RD, "RPT", 0);
      noteRepeatVelocityEncoder.initRangeEncoder(0, 127, "VEL", 64);
      noteRepeatProbabilityEncoder.initRangeEncoder(0, 100, " % ", 70);
      encoders[0] = &noteRepeatSpeedEncoder;
      encoders[1] = &noteRepeatSpeedEncoder2;
      encoders[2] = &noteRepeatVelocityEncoder; 
      encoders[3] = &noteRepeatProbabilityEncoder;    
    }
    
    virtual void loop() {     
      
          if (noteRepeatSpeedEncoder.hasChanged()) {
              noteRepeat.speed = (note_repeat_speed_type_t)noteRepeatSpeedEncoder.getValue();
          }
          if (noteRepeatSpeedEncoder2.hasChanged()) {
              noteRepeat.speed = (note_repeat_speed_type_t)noteRepeatSpeedEncoder2.getValue();
          }
          if (noteRepeatVelocityEncoder.hasChanged()) {
              noteRepeat.velocity = noteRepeatVelocityEncoder.getValue();
          }
          if (noteRepeatProbabilityEncoder.hasChanged()) {
              noteRepeat.probability = noteRepeatProbabilityEncoder.getValue();
          }   
          displayNoteRepeatSpeed();     
    }
    
    virtual bool handleEvent (gui_event_t *event){
      
          if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
                  noteRepeat.overrideSpeed = true;
          } else if (EVENT_RELEASED(event, Buttons.ENCODER1)){
                  noteRepeat.overrideSpeed = false; 
          } else {                  
                return false;
          }                    
    }
    
    void displayNoteRepeatSpeed() {
        GUI.setLine(GUI.LINE2);
        GUI.put_string_at(0, NoteRepeat::repeatSpeedNames[noteRepeat.speed]);
    }        
  
};
NoteRepeatPage noteRepeatPage;
