namespace MDArpeggiator {

bool loadedKit = false;

bool triggerRecording = false;
bool recording = false;
int recordStart = 0;
int recordLength = 32;
bool endRecording = false;

typedef enum {
  ARP_STYLE_UP = 0,
  ARP_STYLE_DOWN,
  ARP_STYLE_UPDOWN,
  ARP_STYLE_DOWNUP,
  ARP_STYLE_UP_AND_DOWN,
  ARP_STYLE_DOWN_AND_UP,
  ARP_STYLE_CONVERGE,
  ARP_STYLE_DIVERGE,
  ARP_STYLE_CON_AND_DIVERGE,
  ARP_STYLE_PINKY_UP,
  ARP_STYLE_PINKY_UPDOWN,
  ARP_STYLE_THUMB_UP,
  ARP_STYLE_THUMB_UPDOWN,
  ARP_STYLE_RANDOM,
  ARP_STYLE_RANDOM_ONCE,
  ARP_STYLE_ORDER,
  ARP_STYLE_CNT
} arp_style_t;

char arp_names[ARP_STYLE_CNT][8] PROGMEM = {
  "UP     ",
  "DOWN   ",
  "UPDOWN ",
  "DOWNUP ",
  "UP&DOWN",
  "DOWN&UP",
  "CONVERG",
  "DIVERGE",
  "CONDIV ",
  "PINK_UP",
  "PINK_UD",
  "THMB_UP",
  "THMB_UD",
  "RANDOM ",
  "RANDOM1",
  "ORDER  "
};

typedef enum {
  RETRIG_OFF = 0,
  RETRIG_NOTE,
  RETRIG_BEAT,
  RETRIG_CNT
} arp_retrig_type_t;

char retrig_names[RETRIG_CNT][5] PROGMEM = {
  "OFF ",
  "NOTE",
  "BEAT"
};

#define NUM_NOTES 8
#define MAX_ARP_LEN 64

class ArpeggiatorClass {
public:
  uint8_t notes[NUM_NOTES];
  uint8_t velocities[NUM_NOTES];

  uint8_t orderedNotes[NUM_NOTES];
  uint8_t orderedVelocities[NUM_NOTES];
  uint8_t numNotes;

  uint8_t arpNotes[MAX_ARP_LEN];
  uint8_t arpVelocities[MAX_ARP_LEN];
  
  uint8_t arpSpeed;
  uint8_t arpLen;
  uint8_t arpStep;
  uint8_t arpCount;
  uint8_t arpTrack;
  uint8_t arpTimes;
  uint8_t arpOctaves;
  uint8_t arpOctaveCount;
  uint8_t retrigSpeed;

  arp_style_t arpStyle;
  arp_retrig_type_t arpRetrig;

  uint16_t speedCounter;

  ArpeggiatorClass() {
    numNotes = 0;
    arpLen = 0;
    arpStep = 0;
    arpCount = 0;
    arpTimes = 0;
    arpStyle = ARP_STYLE_UP;
    arpRetrig = RETRIG_OFF;
    arpSpeed = 0;
    speedCounter = 0;
    arpTrack = 0;
    retrigSpeed = 1;
    arpOctaves = 0;
    arpOctaveCount = 0;

    for (int i = 0; i < NUM_NOTES; i++) {
      orderedNotes[i] = 128;
    }
  }
  
  void retrigger() {
    arpStep = 0;
    arpCount = 0;
    speedCounter = 0;
    arpOctaveCount = 0;
    if (arpStyle == ARP_STYLE_RANDOM_ONCE) {
      calculateArp();
    }
  }

  void bubbleSortUp() {
    bool completed = true;
    do {
      completed = true;
      for (int i = 0; i < numNotes-1; i++) {
        if (orderedNotes[i] > orderedNotes[i+1]) {
          completed = false;
          uint8_t tmp = orderedNotes[i];
          orderedNotes[i] = orderedNotes[i+1];
          orderedNotes[i+1] = tmp;
          tmp = orderedVelocities[i];
          orderedVelocities[i] = orderedVelocities[i+1];
          orderedVelocities[i+1] = tmp;
        }
      }
    } while (!completed);
  }

  void bubbleSortDown() {
    bool completed = true;
    do {
      completed = true;
      for (int i = 0; i < numNotes-1; i++) {
        if (orderedNotes[i] < orderedNotes[i+1]) {
          completed = false;
          uint8_t tmp = orderedNotes[i];
          orderedNotes[i] = orderedNotes[i+1];
          orderedNotes[i+1] = tmp;
          tmp = orderedVelocities[i];
          orderedVelocities[i] = orderedVelocities[i+1];
          orderedVelocities[i+1] = tmp;
        }
      }
    } while (!completed);
  }

  void calculateArp() {
    uint8_t tmp = SREG;
    cli();

    arpStep = 0;
    switch (arpStyle) {
    case ARP_STYLE_UP:
      bubbleSortUp();
      m_memcpy(arpNotes, orderedNotes, numNotes);
      m_memcpy(arpVelocities, orderedVelocities, numNotes);
      arpLen = numNotes;
      break;
    
    case ARP_STYLE_DOWN:
      bubbleSortDown();
      m_memcpy(arpNotes, orderedNotes, numNotes);
      m_memcpy(arpVelocities, orderedVelocities, numNotes);
      arpLen = numNotes;
      break;
    
    case ARP_STYLE_ORDER: {
      m_memcpy(arpNotes, orderedNotes, numNotes);
      m_memcpy(arpVelocities, orderedVelocities, numNotes);
      arpLen = numNotes;
    }
      break;
    
    case ARP_STYLE_UPDOWN:
      if (numNotes > 1) {
	bubbleSortUp();
	m_memcpy(arpNotes, orderedNotes, numNotes);
	m_memcpy(arpVelocities, orderedVelocities, numNotes);
	for (int i = 0; i < numNotes - 2; i++) {
	  arpNotes[numNotes + i] = orderedNotes[numNotes - 2 - i];
	  arpVelocities[numNotes + i] = arpVelocities[numNotes - 2 - i];
	}
	arpLen = numNotes + numNotes - 2;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
  
    case ARP_STYLE_DOWNUP:
      if (numNotes > 1) {
	bubbleSortDown();
	m_memcpy(arpNotes, orderedNotes, numNotes);
	m_memcpy(arpVelocities, orderedVelocities, numNotes);
	for (int i = 0; i < numNotes - 2; i++) {
	  arpNotes[numNotes + i] = orderedNotes[numNotes - 2 - i];
	  arpVelocities[numNotes + i] = arpVelocities[numNotes - 2 - i];
	}
	arpLen = numNotes + numNotes - 2;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_UP_AND_DOWN:
      if (numNotes > 1) {
	bubbleSortUp();
	m_memcpy(arpNotes, orderedNotes, numNotes);
	m_memcpy(arpVelocities, orderedVelocities, numNotes);
	for (int i = 0; i < numNotes; i++) {
	  arpNotes[numNotes + i] = orderedNotes[numNotes - 1 - i];
	  arpVelocities[numNotes + i] = arpVelocities[numNotes - 1 - i];
	}
	arpLen = numNotes + numNotes;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_DOWN_AND_UP:
      if (numNotes > 1) {
	bubbleSortDown();
	m_memcpy(arpNotes, orderedNotes, numNotes);
	m_memcpy(arpVelocities, orderedVelocities, numNotes);
	for (int i = 0; i < numNotes; i++) {
	  arpNotes[numNotes + i] = orderedNotes[numNotes - 1 - i];
	  arpVelocities[numNotes + i] = arpVelocities[numNotes - 1 - i];
	}
	arpLen = numNotes + numNotes;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_CONVERGE:
      bubbleSortUp();
      if (numNotes > 1) {
        arpLen = 0;
        for (int i = 0; i < (numNotes >> 1); i++) {
          arpNotes[arpLen] = orderedNotes[i];
          arpVelocities[arpLen] = orderedVelocities[i];
	  arpLen++;
          arpNotes[arpLen] = orderedNotes[numNotes-i-1];
          arpVelocities[arpLen] = orderedVelocities[numNotes-i-1];
	  arpLen++;
        }
        if (numNotes & 1) {
          arpNotes[arpLen] = orderedNotes[(numNotes >> 1)];
          arpVelocities[arpLen] = orderedVelocities[(numNotes >> 1)];
          arpLen++;
        }
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_DIVERGE:
      bubbleSortUp();
      if (numNotes > 1) {
        arpLen = 0;
        if (numNotes & 1) {
          arpNotes[arpLen] = orderedNotes[numNotes >> 1];
          arpVelocities[arpLen] = orderedVelocities[numNotes >> 1];
          arpLen++;
        }
        for (int i = (numNotes >> 1) - 1; i >= 0; i--) {
          arpNotes[arpLen] = orderedNotes[i];
          arpVelocities[arpLen] = orderedVelocities[i];
          arpLen++;
          arpNotes[arpLen] = orderedNotes[numNotes-i-1];
          arpVelocities[arpLen] = orderedVelocities[numNotes-i-1];
          arpLen++;
        }
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
      
    case ARP_STYLE_CON_AND_DIVERGE:
      bubbleSortUp();
      if (numNotes > 1) {
        arpLen = 0;
        for (int i = 0; i < (numNotes >> 1); i++) {
          arpNotes[arpLen] = orderedNotes[i];
          arpVelocities[arpLen] = orderedVelocities[i];
	  arpLen++;
          arpNotes[arpLen] = orderedNotes[numNotes-i-1];
          arpVelocities[arpLen] = orderedVelocities[numNotes-i-1];
	  arpLen++;
        }
        if (numNotes & 1) {
          arpNotes[arpLen] = orderedNotes[(numNotes >> 1)];
          arpVelocities[arpLen] = orderedVelocities[(numNotes >> 1)];
          arpLen++;
        }
        for (int i = (numNotes >> 1) - 1; i >= 0; i--) {
          arpNotes[arpLen] = orderedNotes[i];
          arpVelocities[arpLen] = orderedVelocities[i];
          arpLen++;
          arpNotes[arpLen] = orderedNotes[numNotes-i-1];
          arpVelocities[arpLen] = orderedVelocities[numNotes-i-1];
          arpLen++;
        }
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_PINKY_UP:
      bubbleSortUp();
      if (numNotes > 1) {
        for (int i = 0; i < numNotes-1; i++) {
          arpNotes[i*2] = orderedNotes[numNotes-1];
          arpNotes[i*2+1] = orderedNotes[i];
        }
        arpLen = numNotes * 2;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_PINKY_UPDOWN:
      bubbleSortUp();
      if (numNotes > 1) {
        for (int i = 0; i < numNotes-1; i++) {
          arpNotes[i*2] = orderedNotes[numNotes-1];
          arpNotes[i*2+1] = orderedNotes[i];
        }
        for (int i = 0; i < numNotes-1; i++) {
          arpNotes[i*2 + 2 * numNotes] = orderedNotes[numNotes-1];
          arpNotes[i*2+1 + 2 * numNotes] = orderedNotes[numNotes - i - 2];
        }
        arpLen = numNotes * 4;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_THUMB_UP:
      bubbleSortUp();
      if (numNotes > 1) {
        for (int i = 1; i < numNotes; i++) {
          arpNotes[i*2] = orderedNotes[0];
          arpNotes[i*2+1] = orderedNotes[i];
        }
        arpLen = numNotes * 2;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_THUMB_UPDOWN:
      bubbleSortUp();
      if (numNotes > 1) {
        for (int i = 1; i < numNotes; i++) {
          arpNotes[i*2] = orderedNotes[0];
          arpNotes[i*2+1] = orderedNotes[i];
        }
        for (int i = 0; i < numNotes-1; i++) {
          arpNotes[i*2 + 2 * numNotes] = orderedNotes[0];
          arpNotes[i*2+1 + 2 * numNotes] = orderedNotes[numNotes - i - 1];
        }
        arpLen = numNotes * 4;
      } else {
	arpNotes[0] = orderedNotes[0];
	arpVelocities[0] = orderedVelocities[0];
	arpLen = 1;
      }
      break;
   
    case ARP_STYLE_RANDOM:
      arpLen = numNotes;
      break;
   
    case ARP_STYLE_RANDOM_ONCE:
      m_memcpy(arpNotes, orderedNotes, numNotes);
      m_memcpy(arpVelocities, orderedVelocities, numNotes);
      for (int i = 0; i < numNotes; i++) {
        uint8_t rand = random(numNotes);
        uint8_t tmp;
        tmp = arpNotes[i];
        arpNotes[i] = arpNotes[rand];
        arpNotes[rand] = tmp;
        tmp = arpVelocities[i];
        arpVelocities[i] = arpVelocities[rand];
        arpVelocities[i] = tmp;
      }
      arpLen = numNotes;
      break;
   
    default:
      break;
    }

    SREG = tmp;
  }

  void reorderNotes() {
    uint8_t write = 0;
    for (int i = 0; i < NUM_NOTES; i++) {
      if (orderedNotes[i] != 128) {
	orderedNotes[write] = orderedNotes[i];
	orderedVelocities[write] = orderedVelocities[i];
	if (i != write)
	  orderedNotes[i] = 128;
	write++;
      }
    }
  }

  void addNote(uint8_t pitch, uint8_t velocity) {
    // replace
    bool replaced = false;
    for (int i = 0; i < NUM_NOTES; i++) {
      if (orderedNotes[i] == pitch) {
	orderedVelocities[i] = velocity;
	replaced = true;
      }
    }
    if (replaced)
      return;

    uint8_t replaceNote = 128;
    if (numNotes == NUM_NOTES) {
      orderedNotes[0] = 128;
      numNotes--;
      reorderNotes();
    }

    orderedNotes[numNotes] = pitch;
    orderedVelocities[numNotes] = velocity;
    numNotes++;
    
    calculateArp();
    if (arpRetrig == RETRIG_NOTE || numNotes == 1) {
      retrigger();
    }      
  }

  void removeNote(uint8_t pitch) {
    for (int i = 0; i < NUM_NOTES; i++) {
      if (orderedNotes[i] == pitch) {
	orderedNotes[i] = 128;
	reorderNotes();
	numNotes--;
	break;
      }
    }
    calculateArp();
  }

  uint8_t recordPitches[64];

  void recordNote(int pos, uint8_t track, uint8_t note, uint8_t velocity) {
      uint8_t realPitch = MD.trackGetPitch(track, note);
        if (realPitch == 128)
          return;  
    MD.triggerTrack(track, velocity);
    recordPitches[pos] = note;
  }
  
  void recordNoteSecond(int pos, uint8_t track) {
    pos -= recordLength;
    if (recordPitches[pos + 1] != 128) {
      uint8_t realPitch = MD.trackGetPitch(track, recordPitches[pos + 1]);
        if (realPitch == 128)
          return;  
        MD.setTrackParam(track, 0, realPitch - 5);
        delay(3);
        MD.setTrackParam(track, 0, realPitch);
    }
  }
  
  void playNext(bool recording = false) {
    int pos = MidiClock.div16th_counter - recordStart;
    if (pos == 0 && recording)
      retrigger();
      
    if (recording && (pos >= (recordLength - 1))) {
      recordNoteSecond(pos, arpTrack);
    }
    
    if (arpLen == 0 || (arpTimes != 0 && arpCount >= arpTimes))
      return;
      
    if (arpRetrig == RETRIG_BEAT && (MidiClock.div16th_counter % retrigSpeed) == 0)
      retrigger();
    if (++speedCounter >= arpSpeed) {
      speedCounter = 0;
      if (arpStyle == ARP_STYLE_RANDOM) {
	uint8_t i = random(numNotes);
        if (recording && (pos < recordLength)) {
          recordNote(pos, arpTrack, orderedNotes[i] + random(arpOctaves) * 12, orderedVelocities[i]);
        } else if (!recording) {
    	  MD.sendNoteOn(arpTrack, orderedNotes[i] + random(arpOctaves) * 12, orderedVelocities[i]);
        }
      } else {
        if (recording && (pos < recordLength)) {
          recordNote(pos, arpTrack, arpNotes[arpStep] + 12 * arpOctaveCount, arpVelocities[arpStep]);
        } else if (!recording) {
  	  MD.sendNoteOn(arpTrack, arpNotes[arpStep] + 12 * arpOctaveCount, arpVelocities[arpStep]);
        }
	if (++arpStep == arpLen) {
          if (arpOctaveCount < arpOctaves) {
            arpStep = 0;
            arpOctaveCount++;
          } else {
  	    arpStep = 0;
            arpOctaveCount = 0;
            arpCount++;
          }
	}
      }
    } 
  }

};

ArpeggiatorClass arpeggiator;


void handleGuiSysex() {
}

void on16Callback() {
  if (triggerRecording && (MidiClock.div16th_counter % 16) == 0) {
    triggerRecording = false;
    recording = true;
    recordStart = MidiClock.div16th_counter;
    for (int i = 0; i < 64; i++) {
      arpeggiator.recordPitches[i] = 128;
    }
  }
  
  if (recording || endRecording) {
    int pos = MidiClock.div16th_counter - recordStart;
    if (pos >= (recordLength * 3)) {
      endRecording = false;
    } else if (pos >= (recordLength * 2)) {
      recording = false;
      endRecording = true;
      return;
    }
  }
  
  if (!triggerRecording) {
    arpeggiator.playNext(recording);
  }
}

void handleNoteOn(uint8_t *msg) {
  arpeggiator.addNote(msg[1], msg[2]);
}

void handleNoteOff(uint8_t *msg) {
  arpeggiator.removeNote(msg[1]);
}

void onNoteOnCallbackMD(uint8_t *msg) {
}

void onNoteOffCallbackKeyboard(uint8_t *msg) {
  handleNoteOff(msg);
}

void onNoteOnCallbackKeyboard(uint8_t *msg) {
  if (msg[2] != 0) {
    handleNoteOn(msg);
  } else {
    handleNoteOff(msg);
  }
}
void setupMidi() {
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
  Midi2.setOnNoteOffCallback(onNoteOffCallbackKeyboard);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder speedEncoder(1, 16, "SPD");
RangeEncoder octavesEncoder(0, 5, "OCT");
RangeEncoder lenEncoder(0, 16, "LEN");
  
class ConfigPage_1 : public EncoderPage {
  public:
  ConfigPage_1() {
    encoders[0] = &trackEncoder;
    encoders[1] = &speedEncoder;
    encoders[2] = &octavesEncoder;
    encoders[3] = &lenEncoder;
    trackEncoder.setValue(arpeggiator.arpTrack);
    speedEncoder.setValue(arpeggiator.arpSpeed);
  }  
  
  virtual void display(bool redisplay = false) {
    EncoderPage::display(redisplay);
    if (redisplay || lenEncoder.hasChanged()) {
      if (lenEncoder.getValue() == 0)
        GUI.put_p_string_at(12, PSTR("INF"));
    }
    if (lenEncoder.hasChanged()) {
      arpeggiator.arpTimes = lenEncoder.getValue();
    }
    if (speedEncoder.hasChanged()) {
      arpeggiator.arpSpeed = speedEncoder.getValue();
    }
    if (octavesEncoder.hasChanged()) {
      arpeggiator.arpOctaves = octavesEncoder.getValue();
      arpeggiator.arpOctaveCount = 0;
    }
    if (trackEncoder.hasChanged()) {
      uint8_t track = trackEncoder.getValue();
      GUI.setLine(GUI.LINE2);
      GUI.clearFlash();
      GUI.flash_put_value(0, track);
      if (MD.isMelodicTrack(track)) {
        arpeggiator.arpTrack = track;
        GUI.flash_p_string_at_fill(8, MD.getMachineName(MD.kit.machines[track].model));
      } else {
        GUI.flash_p_string_at_fill(8, PSTR("XXX"));
      }
    }
  }
};

RangeEncoder styleEncoder(0, ARP_STYLE_CNT - 1, "STY");
RangeEncoder retrigEncoder(0, RETRIG_CNT - 1, "TRG");
RangeEncoder retrigSpeedEncoder(1, 32, "SPD");

class ConfigPage_2 : public EncoderPage {
  public:
  
  ConfigPage_2() {
    encoders[0] = &styleEncoder;
    encoders[2] = &retrigEncoder;
    encoders[3] = &retrigSpeedEncoder;
    styleEncoder.setValue(0);
    arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
    retrigEncoder.setValue(0);
    arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
    arpeggiator.calculateArp();
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_fill(PSTR("STYLE   TRG  SPD "));
      displayStyle();
      displayRetrig();
    }
  }
  
  void displayStyle() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(0, arp_names[styleEncoder.getValue()]);
  }
  
  void displayRetrig() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(8, retrig_names[retrigEncoder.getValue()]);
    if (arpeggiator.arpRetrig == RETRIG_BEAT) {
      GUI.put_value_at(13, retrigSpeedEncoder.getValue());
    }
  }
  
  virtual void handle() {
    bool changed = false;
    if (styleEncoder.hasChanged()) {
      arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
      displayStyle();
      changed = true;
    }
    styleEncoder.checkHandle();
    if (retrigEncoder.hasChanged()) {
      arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
      displayRetrig();
      changed = true;
    }
    retrigEncoder.checkHandle();
    if (changed) {
      arpeggiator.calculateArp();
    }
    if (retrigSpeedEncoder.hasChanged()) {
      arpeggiator.retrigSpeed = retrigSpeedEncoder.getValue();
      displayRetrig();
    }
    retrigSpeedEncoder.checkHandle();
  }
};

ConfigPage_1 configPage_1;
ConfigPage_2 configPage_2;

void loopPages() {
}

void setupPages() {
  GUI.setPage(&configPage_1);
}

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT", MD.kit.name);
  GUI.setLine(GUI.LINE1);
  loadedKit = true;
}

void handleGuiPages() {
  if (GUI.page == &configPage_1) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadedKit = false;
      MDSysexListener.getCurrentKit(onCurrentKitCallback);
    } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      GUI.setPage(&configPage_2);
    }
  } else if (GUI.page == &configPage_2) {
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      GUI.setPage(&configPage_1);
    }
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    triggerRecording = true;
    recording = false;
    endRecording = false;
  }
}


class MDArpeggiatorSketch : public SubSketch {
void setup() {
  setupMidi();
  setupPages();
}

void loop() {
  GUI.updatePage();
  
  loopPages();
  
  if (MidiUart.rxRb.overflow > 0) {
    setLed();
  }
  GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}

void destroy() {
  Midi.setOnNoteOnCallback(NULL);
  Midi2.setOnNoteOnCallback(NULL);
  Midi2.setOnNoteOffCallback(NULL);
  MidiClock.setOn16Callback(NULL);
  MidiClock.stop();
}

PGM_P getName() {
  return PSTR("ARPEGGIATOR");
}
};

MDArpeggiatorSketch sketch;
}
