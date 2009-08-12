#include <MD.h>
// #include <SDCard.h>
// #include <MidiClockPage.h>
#include <Merger.h>

char *melodyHelperNoteNames[] = {
  "C",
  "Db",
  "D",
  "Eb",
  "E",
  "F",
  "Gb",
  "G",
  "Ab",
  "A",
  "Bb",
  "B"
};

#define NOTE_OFFSET 8

class MelodyHelperSketch : 
public Sketch, public MidiCallback, public MDCallback {
public:
  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MelodyHelperSketch::onKitChanged);
    GUI.addTask(&MDTask);

    Midi.addOnControlChangeCallback(this, (void(MidiCallback::*)(uint8_t*))&MelodyHelperSketch::onCCCallback);
  }
  
    void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }


  virtual void onCCCallback(uint8_t *msg) {
    uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
    uint8_t track, param;
    MD.parseCC(channel, msg[1], &track, &param);

    if (param == 0) {
      int8_t offset;
      uint8_t pitch = MD.trackGetCCPitch(track, msg[2], &offset);

      GUI.setLine(GUI.LINE1);
      GUI.put_value_at(13, track);

      GUI.setLine(GUI.LINE2);
      GUI.put_p_string_at(0, MD.getMachineName(MD.kit.machines[track].model));

      if (pitch == 128) {
        GUI.put_p_string_at_fill(NOTE_OFFSET, PSTR("XXX"));
      } 
      else {
        uint8_t note = pitch % 12;
        uint8_t octave = pitch / 12;
        GUI.put_string_at_fill(NOTE_OFFSET, melodyHelperNoteNames[note]);
        GUI.lines[GUI.curLine].data[NOTE_OFFSET + 1] = octave + '0';
        for (uint8_t i = 0; i < offset; i += 2) {
          GUI.lines[GUI.curLine].data[NOTE_OFFSET + 2 + (i >> 1)] = '+';
        }
      }
    }
  }
};

MelodyHelperSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);

#if 0
  if (SDCard.init() != 0) {
    GUI.flash_strings_fill("SDCARD ERROR", "");
    GUI.display();
    delay(800);
    MidiClock.mode = MidiClock.EXTERNAL_MIDI;
    MidiClock.transmit = true;
    MidiClock.start();
  } 
  else {
    midiClockPage.setup();
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      GUI.pushPage(&midiClockPage);
    }
  }
  #endif
}

void loop() {
}
