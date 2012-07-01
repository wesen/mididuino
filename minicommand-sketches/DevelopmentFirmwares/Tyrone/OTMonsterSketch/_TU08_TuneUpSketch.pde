#include <inttypes.h>
#include <string.h>
#include <Scales.h>
#include <MidiTools.h>

// TT - I dunno what all this stuff is for, so have commented it out :-)
//
//inline void* operator new(unsigned int size) { return malloc(size); }
//inline void* operator new(unsigned int size, void* ptr){ return realloc(ptr,size);}
//inline void operator delete(void* ptr) { free(ptr); }
//
//inline void* operator new[](unsigned int size ) { return malloc(size);}
//inline void* operator new[](unsigned int size, void* ptr){ return realloc(ptr,size);}
//inline void operator delete[](void* ptr) { free(ptr); } 
//


// *****************************
// BO - VARIABLES
// *****************************

#define MAX_NUM_NOTES 32
uint8_t activeNotes[MAX_NUM_NOTES][4];

#define NUM_ALLOWED_NOTES 128
bool allowedNotes[NUM_ALLOWED_NOTES];

// *****************************
// GUI - CONSTANTS
// *****************************

#define NUM_CHORDS 19

scale_t octaveScale = {
  1,
  { 0 }
};

scale_t terzScale = {
  2,
  { 0, 3 }
};

scale_t quarteScale = {
  2,
  { 0, 5 }
};

scale_t quinteScale = {
  2,
  { 0, 7 }
};

scale_t sexteScale = {
  2,
  { 0, 9 }
};

struct Chord {
  const char* m_name;
  scale_t m_scale;
} chords[NUM_CHORDS] = {
  { "Oct", octaveScale }, // #1
  { "Trz", terzScale }, // #2
  { "Qua", quarteScale }, // #3
  { "Qui", quinteScale }, // #4
  { "Sex", sexteScale }, // #5
  { "Maj", majorArp }, // #6
  { "Min", minorArp }, // #7
  { "MM7", majorMaj7Arp }, // #8
  { "Mm7", majorMin7Arp }, // #9
  { "mm7", minorMin7Arp }, // #10
  { "mM7", minorMaj7Arp }, // #11
  { "M79", majorMaj7Arp9 }, // #12
  { "M78", majorMaj7ArpMin9 }, // #13
  { "Mm9", majorMin7Arp9 }, // #14
  { "Mm8", majorMin7ArpMin9 }, // #15
  { "m79", minorMin7Arp9 }, // #16
  { "m78", minorMin7ArpMin9 }, // #17
  { "mM9", minorMaj7Arp9 }, // #18
  { "mM8", minorMaj7ArpMin9 } // #19  
};

const char* chordNames[NUM_CHORDS];

void setupChords() {
  for (int i = 0; i < NUM_CHORDS; ++i) {
    chordNames[i] = chords[i].m_name;
  }
  for (int i = 0; i < MAX_NUM_NOTES; ++i) {
    activeNotes[i][0] = 0;
    activeNotes[i][1] = 0;
    activeNotes[i][2] = 0;
    activeNotes[i][3] = 0;
  }
}

#define NUM_TONARTEN 21

struct Tonart {
  const char* m_name;
  uint8_t m_scale12[12];
  scale_t m_scale;
} tonarten[NUM_TONARTEN] = {
  { "Ion", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, ionianScale }, // #1
  { "Dor", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, dorianScale }, // #2 
  { "Phr", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, phrygianScale }, // #3
  { "Lyd", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, lydianScale }, // #4
  { "Mix", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, mixolydianScale }, // #5
  { "Aeo", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, aeolianScale }, // #6
  { "Loc", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, locrianScale }, // #7
  { "Har", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, harmonicMinorScale }, // #8
  { "Mel", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, melodicMinorScale }, // #9
  { "LyD", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, lydianDominantScale }, // #10
  { "wTo", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, wholeToneScale }, // #11
  { "WHS", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, wholeHalfStepScale }, // #12
  { "HWS", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, halfWholeStepScale }, // #13
  { "MPn", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, majorPentatonicScale }, // #14
  { "mPn", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, minorPentatonicScale }, // #15
  { "sPn", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, suspendedPentatonicScale }, // #16
  { "InS", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, inSenScale }, // #17
  { "Blu", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, bluesScale }, // #18
  { "MBe", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, majorBebopScale }, // #19
  { "DBe", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, dominantBebopScale }, // #20
  { "mBe", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, minorBebopScale }, // #21
};

const char* tonartenNames[NUM_TONARTEN];

class TuneUpSketch : public Sketch, public MidiCallback  {

    public:
    
    // *****************************
    // GUI VARIABLES
    // *****************************
    
    BoolEncoder transActivateEncoder;
    BoolEncoder chordActivateEncoder;
    EnumEncoder chordEncoder;
    NotePitchEncoder noteEncoder;
    EnumEncoder tonartEncoder;
    
    RangeEncoder upOctavesEncoder, downOctavesEncoder, numberNotesEncoder;
    EncoderPage encoderPage1, encoderPage2;
    
    SwitchPage switchPage;
    
    void setupTonarten() {
      for (int i = 0; i < NUM_TONARTEN; ++i) {
        setupScaleMap(tonarten[i].m_scale12, tonarten[i].m_scale);
        tonartenNames[i] = tonarten[i].m_name;
      }
    }
    
    void setupScaleMap(uint8_t* scaleMap, scale_t scale) {
      int cur = 0;
      int next = 1;
      for (int i = 0; i < 12; ++i) {
        if (next < scale.size) {
          uint8_t nextPitch = scale.pitches[next];
          if (nextPitch == i) {
            cur++;
            next++;
          }
        }
        
        if (cur < scale.size) {
          scaleMap[i] = scale.pitches[cur];
        } else {
          scaleMap[i] = 0;
        }
      }
    }
    
    // *****************************
    // MIDI-CALLBACK
    // *****************************
    
    void onControlChange(uint8_t *msg) {
      if (msg[1] == 1) {
        uint8_t value = msg[2];
        value = value * chordEncoder.cnt / 128;
        chordEncoder.setValue(value);
      }
    }
    
    void onNoteOn(uint8_t *msg) {
      if (chordActivateEncoder.getValue() == 1) {
        endChord();
        playChord(msg);
      } else {
        uint8_t newPitch = pitchNote(msg[1], "ON ");
        MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
      }
    }
    
    void onNoteOff(uint8_t *msg) {
      if (chordActivateEncoder.getValue() == 1) {
        endChord();
      } else {
        uint8_t newPitch = pitchNote(msg[1], "OFF");
        MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
      }
    }
    
    uint8_t pitchNote(uint8_t note, const char* typ) {
      uint8_t newPitch = note;
      if (transActivateEncoder.getValue() == 1) {
        newPitch = scalePitch(note, getRoot(), getScale());
        debugNotePitch(typ, note, newPitch);
      }
      return newPitch;
    }
    
    void debugNotePitch(const char* typ, uint8_t from, uint8_t to) {
      if (typ != NULL) { 
        GUI.setLine(GUI.LINE2);
        char name1[5];
        getNotePitch(from, name1);
        char name2[5];
        getNotePitch(to, name2);
        GUI.flash_printf_fill("%s %s -> %s", typ, name1, name2);
      }
    }
    
    uint8_t getRndNoteAndClear(uint8_t& curNumNotes) {
      uint8_t numNote = random(curNumNotes);
      for (int i = 0; i < NUM_ALLOWED_NOTES; ++i) {
        if (allowedNotes[i]) {
          if (numNote == 0) {
            allowedNotes[i] = 0;
            --curNumNotes;
            return i;
          } else {
            --numNote;
          }
        }
      }
      GUI.setLine(GUI.LINE2);
      GUI.flash_printf_fill("no note found %b", curNumNotes);
      return 128;
    }
    
    uint8_t setupAllowedNotes(struct Chord* c, uint8_t pitch, uint8_t up, uint8_t down, uint8_t numNote = 0) {
      // RESET ALLOWED NOTES
      for (int i = 0; i < NUM_ALLOWED_NOTES; ++i) {
        allowedNotes[i] = 0;
      }
      
      uint8_t notes = 0;
      uint8_t offset = c->m_scale.pitches[numNote];
      
      for (int8_t octave = -down; octave <= up; ++octave) {
        int8_t shiftedPitch = pitch + octave * 12;
        for (int i = 0; i < c->m_scale.size; ++i) {
          uint8_t note = shiftedPitch + c->m_scale.pitches[i] - offset;
          if (note >= 0 && note  < NUM_ALLOWED_NOTES && note != pitch) {
            allowedNotes[note] = 1;
            ++notes;
          }
        }
      }
      
      return notes;
    }
    
    bool contains(scale_t* scale, uint8_t pitch) {
      for (int i = 0; i < scale->size; ++i) {
        if (scale->pitches[i] == pitch) {
          return true;
        }
      }
      return false;
    }
    
    bool allowedNotesMatchScale() {
      scale_t* curScale = &tonarten[tonartEncoder.getValue()].m_scale;
      for (int i = 0; i < NUM_ALLOWED_NOTES; ++i) {
        if (allowedNotes[i]) {
          uint8_t pitch = (i - (noteEncoder.getValue() % 12)) % 12;
          if (!contains(curScale, pitch)) {
            return false;
          }  
        }
      }
      return true;
    }
    
    void playChord(uint8_t *msg) {
      uint8_t num = numberNotesEncoder.getValue();
      struct Chord* c = &chords[chordEncoder.getValue()];
      uint8_t pitch = msg[1];
      uint8_t up = upOctavesEncoder.getValue();
      uint8_t down = downOctavesEncoder.getValue();
      
      if (transActivateEncoder.getValue()) {
        pitch = pitchNote(pitch, NULL);
      }
      
      int8_t octave = pitch / 12;
      up = MAX(MIN(octave + up, 9) - octave, 0);
      down = MIN(down, octave);
      
      uint8_t maxNotes = ((up + down + 1) * c->m_scale.size) - 1;
      uint8_t numberNotes = MIN(numberNotesEncoder.getValue(), maxNotes);
    
      uint8_t curNumNotes = 0;
      if (transActivateEncoder.getValue()) {
        bool found = false;
        for (int i = 0; i < c->m_scale.size; ++i) {
          curNumNotes = setupAllowedNotes(c, pitch, up, down, i);
          if (allowedNotesMatchScale()) {
            found = true;
            break;
          }
        }
        if (!found) {
          GUI.setLine(GUI.LINE2);
          GUI.flash_printf_fill("no chord found");
          return;
        }
      } else {
        curNumNotes = setupAllowedNotes(c, pitch, up, down);
      }
      
      activeNotes[0][0] = msg[0];
      activeNotes[0][1] = pitch;
      activeNotes[0][2] = msg[2];
      activeNotes[0][3] = 1;
    
      for (int i = 1; i < numberNotes + 1; ++i) {
        uint8_t newPitch = getRndNoteAndClear(curNumNotes);
        if (newPitch >= 0 && newPitch <= 128) {
          activeNotes[i][0] = msg[0];
          activeNotes[i][1] = newPitch;
          activeNotes[i][2] = msg[2];
          activeNotes[i][3] = 1;
        }
      }
      
      for (int i = 0; i < MAX_NUM_NOTES; ++i) {
        if (activeNotes[i][3]) {
          uint8_t channel = activeNotes[i][0];
          uint8_t pitch = activeNotes[i][1];
          uint8_t velocity = activeNotes[i][2];
          MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(channel), pitch, velocity);
        }
      }
    
    }
    
    void endChord() {
      for (int i = 0; i < MAX_NUM_NOTES; ++i) {
        if (activeNotes[i][3]) {
          uint8_t channel = activeNotes[i][0];
          uint8_t pitch = activeNotes[i][1];
          uint8_t velocity = activeNotes[i][2];
          MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(channel), pitch, velocity);
        }
        activeNotes[i][0] = 0;
        activeNotes[i][1] = 0;
        activeNotes[i][2] = 0;
        activeNotes[i][3] = 0;
      }
    }
    
    
    // *****************************
    // GUI - HELPER
    // *****************************
    
    uint8_t* getScale() {
      uint8_t value = tonartEncoder.getValue();
      return tonarten[value].m_scale12;
    }
    
    uint8_t getRoot() {
      return noteEncoder.getValue();
    }
    
    // *****************************
    // FRAMEWORK
    // *****************************
    
    void setup() {
      
      // upOctavesEncoder.setName("Up");
      // upOctavesEncoder.setValue(2);
      // RangeEncoder upOctavesEncoder(4);
      upOctavesEncoder.initRangeEncoder(4, 0, "Up", 2);
      
      //RangeEncoder downOctavesEncoder(4);
      //downOctavesEncoder.setName("Dwn");
      //downOctavesEncoder.setValue(1);
      downOctavesEncoder.initRangeEncoder(4, 0, "Dwn", 1);
      
      //RangeEncoder numberNotesEncoder(MAX_NUM_NOTES - 1);
      //numberNotesEncoder.setName("Num");
      //numberNotesEncoder.setValue(3);
      numberNotesEncoder.initRangeEncoder(MAX_NUM_NOTES - 1, 0, "Num", 3);
      
      setupTonarten();
      setupChords();
    
      transActivateEncoder.setName("KOn");
      transActivateEncoder.setValue(1);
      
      chordActivateEncoder.setName("COn");
      chordActivateEncoder.setValue(1);
      
      noteEncoder.setName("Pit"); 
      
      tonartEncoder.initEnumEncoder(tonartenNames, NUM_TONARTEN);
      tonartEncoder.setName("Key");
      
      chordEncoder.initEnumEncoder(chordNames, NUM_CHORDS);
      chordEncoder.setName("Chr");
      
      encoderPage1.setup();
      encoderPage1.setShortName("Str");
      //EncoderPage encoderPage1(&transActivateEncoder, &chordActivateEncoder, &tonartEncoder, &noteEncoder);
      encoderPage1.setEncoders(&transActivateEncoder, &chordActivateEncoder, &tonartEncoder, &noteEncoder);
      encoderPage2.setup();
      encoderPage2.setShortName("Chr");
      //EncoderPage encoderPage2(&chordEncoder, &numberNotesEncoder, &upOctavesEncoder, &downOctavesEncoder);
      encoderPage2.setEncoders(&chordEncoder, &numberNotesEncoder, &upOctavesEncoder, &downOctavesEncoder);
    
      Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&TuneUpSketch::onNoteOn);
      Midi.addOnNoteOffCallback(this, (midi_callback_ptr_t)&TuneUpSketch::onNoteOff);
      Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&TuneUpSketch::onControlChange);
      switchPage.initPages(&encoderPage1, &encoderPage2);
      
      
      
      GUI.setPage(&encoderPage1);
    }
    
    void getName(char *n1, char *n2) {
        m_strncpy_p(n1, PSTR("TUN "), 5);
        m_strncpy_p(n2, PSTR("UP  "), 5);
    }   
    
    virtual void show() {
        if (currentPage() == &switchPage){
            popPage(&switchPage);
        }
        if (currentPage() == NULL){
            setPage(&encoderPage1);                        
        }
    }
    
    virtual void hide() {
        if (currentPage() == &switchPage){
            popPage(&switchPage);
        }
    }
    
    virtual void mute(bool pressed) {
        if (pressed) {
            muted = !muted;
            if (muted) {
                GUI.flash_strings_fill("TUNE UP SKETCH:", "MUTED");
            } else {
                GUI.flash_strings_fill("TUNE UP SKETCH:", "UNMUTED");
            }
        }
    }
    
    bool handleEvent(gui_event_t *event) {
      if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
        GUI.pushPage(&switchPage);
        return true;
      } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
        GUI.popPage(&switchPage);
        return true;
      }
      return false;
    }
    
    void loop() {
    }

};
