RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder speedEncoder(1, 16, "SPD");
RangeEncoder octavesEncoder(0, 5, "OCT");
  
typedef enum {
  ARP_STYLE_UP = 0,
  ARP_STYLE_DOWN,
  ARP_STYLE_UPDOWN,
  ARP_STYLE_DOWNUP,
  ARP_STYLE_UP_AND_DOWN,
  ARP_STYLE_DOWN_AND_UP,
  ARP_STYLE_CONVERGE,
  ARP_STYLE_DIVERGE,
  ARP_STYLE_PINKY_UP,
  ARP_STYLE_PINKY_UPDOWN,
  ARP_STYLE_THUMB_UP,
  ARP_STYLE_THUMB_UPDOWN,
  ARP_STYLE_RANDOM,
  ARP_STYLE_RANDOM_ONCE,
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
  "PINK_UP",
  "PINK_UD",
  "THMB_UP",
  "THMB_UD",
  "RANDOM ",
  "RANDOM1"
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

uint8_t notes[NUM_NOTES];
uint8_t velocities[NUM_NOTES];

uint8_t orderedNotes[NUM_NOTES];
uint8_t orderedVelocities[NUM_NOTES];
uint8_t numNotes = 0;

#define MAX_ARP_LEN 64
uint8_t arpNotes[MAX_ARP_LEN];
uint8_t arpVelocities[MAX_ARP_LEN];
uint8_t arpLen = 0;

uint8_t arpStep = 0;

arp_style_t arpStyle = ARP_STYLE_UP;
arp_retrig_type_t arpRetrig = RETRIG_OFF;

uint16_t speedCounter = 0;

void on16Callback() {
  if (arpLen == 0)
    return;
  if (++speedCounter == speedEncoder.getValue()) {
    speedCounter = 0;
    MD.sendNoteOn(trackEncoder.getValue(), arpNotes[arpStep], arpVelocities[arpStep]);
    if (++arpStep == arpLen) {
      arpStep = 0;
    }
  } 
}

void calculateArp() {
    uint8_t tmp = SREG;
    cli();

  switch (arpStyle) {
    case ARP_STYLE_UP: {
    for (int i = 0; i < numNotes; i++) {
      arpNotes[i] = orderedNotes[i];
      arpVelocities[i] = orderedVelocities[i];
    }
    arpLen = numNotes;
    arpStep = 0;
    
    }
    break;
    
    default:
    break;
  }

    SREG = tmp;
}

void setupArp() {
  for (int i = 0; i < NUM_NOTES; i++) {
    orderedNotes[i] = 128;
  }
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
}

void removeNote(uint8_t pitch) {
  for (int i = 0; i < NUM_NOTES; i++) {
    if (orderedNotes[i] == pitch) {
      orderedNotes[i] = 128;
      reorderNotes();
      numNotes--;
      return;
    }
  }
}

void setupMidi() {
  setupArp();
  
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
  Midi2.setOnNoteOnCallback(onNoteOnCallbackKeyboard);
  Midi2.setOnNoteOffCallback(onNoteOffCallbackKeyboard);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void handleNoteOn(uint8_t *msg) {
  addNote(msg[1], msg[2]);
  calculateArp();
}

void handleNoteOff(uint8_t *msg) {
  removeNote(msg[1]);
  calculateArp();
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
