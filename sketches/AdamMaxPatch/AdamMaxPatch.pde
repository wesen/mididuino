typedef enum {
  REMIX_FREQ_NEVER = 0,
  REMIX_FREQ_4_BARS,
  REMIX_FREQ_2_BARS,
  REMIX_FREQ_CNT
} remixFrequency_t;

remixFrequency_t remixFrequency = REMIX_FREQ_NEVER;

char remixFrequency_names[REMIX_FREQ_CNT][7] PROGMEM = {
  "NEVER ",
  "4 BARS",
  "2 BARS"
};

bool isRecording() {
  switch (remixFrequency) {
    case REMIX_FREQ_NEVER:
      return true;
      
    case REMIX_FREQ_4_BARS:
      return ((MidiClock.div16th_counter % 64) < 48);
      
    case REMIX_FREQ_2_BARS:
      return ((MidiClock.div16th_counter % 32) < 16);
      
    default:
      return false;
  }
}

bool isRemix() {
  return !isRecording();
}

#define SEQUENCE_LENGTH 16
uint8_t remixPatterns[][16] = {
  { 0, 1, 2, 3, 
    0, 1, 2, 3,
    0, 1, 2, 3,
    0, 1, 2, 3 },

  { 0, 1, 0, 1,
    4, 5, 4, 5,
    8, 9, 8, 9,
    12, 13, 12, 13 },

  { 15, 14, 13, 12,
    11, 10, 9, 8,
    7, 6, 5, 4,
    3, 2, 1, }
};

#define REMIX_PATTERN_CNT (sizeof(remixPatterns) / 16)

uint8_t currentRemixPattern = 0;

class RecordClass {
  public:
  uint8_t pitches[SEQUENCE_LENGTH];
  uint8_t velocities[SEQUENCE_LENGTH];
  uint8_t lastNote;
};

#define NUM_CHANNELS 16
RecordClass records[NUM_CHANNELS]; /* one record structure for each channel */

RangeEncoder remixFrequencyEncoder(0, REMIX_FREQ_CNT - 1, "FRQ");
RangeEncoder remixEncoder(0, REMIX_PATTERN_CNT - 1, "RMX");

class RemixPage : public EncoderPage {
  public:
  RemixPage() : EncoderPage(&remixFrequencyEncoder, NULL, NULL, &remixEncoder) {
  }
  
  virtual void display(bool redisplay = false) {
    EncoderPage::display(redisplay);
    
    if (remixFrequencyEncoder.hasChanged() || redisplay) {
      GUI.setLine(GUI.LINE2);
      remixFrequency = (remixFrequency_t)remixFrequencyEncoder.getValue();
      GUI.put_p_string_at(0, remixFrequency_names[remixFrequency]);
    }
    
    if (remixEncoder.hasChanged()) {
      currentRemixPattern = remixEncoder.getValue();
    }
  }
};

RemixPage remixPage;

void sendNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  if (pitch != records[channel].lastNote) {
    MidiUart.sendNoteOff(channel, records[channel].lastNote, 0);
    records[channel].lastNote = 128;
  }
  MidiUart.sendNoteOn(channel, pitch, velocity);
  records[channel].lastNote = pitch;        
}

void on16Callback() {
  uint8_t idx = MidiClock.div16th_counter % 16;

  if (isRecording()) {
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
      if (records[channel].lastNote != 128) {
        MidiUart.sendNoteOff(channel, records[channel].lastNote, 0);
        records[channel].lastNote = 128;
      }
      records[channel].pitches[idx] = 128;
    }
  } else {
    uint8_t curStep = remixPatterns[currentRemixPattern][idx];
    
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
      uint8_t pitch = records[channel].pitches[curStep];
      uint8_t velocity = records[channel].velocities[curStep];
      if (pitch != records[channel].lastNote) {
        MidiUart.sendNoteOff(channel, records[channel].lastNote, 0);
        records[channel].lastNote = 128;
      }

      if (pitch != 128) {
        sendNoteOn(channel, pitch, velocity);
      }
    }
  }
}

void onNoteOnCallback(uint8_t *msg) {
  setLed2();
  
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  if (isRecording()) {
    sendNoteOn(channel, msg[1], msg[2]);
    
    if (msg[2] == 0)
      goto end;
    uint8_t idx = MidiClock.div16th_counter % 16;
    records[channel].pitches[idx] = msg[1];
    records[channel].velocities[idx] = msg[2];
  }

end:  
  clearLed2();
}

void onNoteOffCallback(uint8_t *msg) {
  msg[2] = 0;
  onNoteOnCallback(msg);
}

void setup() {
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
      records[channel].pitches[i] = 128;
      records[channel].velocities[i] = 128;
    }
    records[channel].lastNote = 128;
  }

  GUI.flash_p_strings_fill(PSTR("ADAM MAX PATCH"), PSTR(""));
 
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  Midi.setOnNoteOffCallback(onNoteOffCallback);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
  
  GUI.setPage(&remixPage);
}

void loop() {
  GUI.updatePage();


  GUI.update();
}

void handleGui() {
}
