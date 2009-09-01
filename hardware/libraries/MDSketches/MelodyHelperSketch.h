#ifndef MELODY_HELPER_SKETCH_H__
#define MELODY_HELPER_SKETCH_H__

#include <MD.h>
#include <GUI.h>

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
  bool muted;

  MelodyHelperSketch() {
    muted = true;
  }
  
  void setup() {
    Midi.addOnControlChangeCallback(this, (void(MidiCallback::*)(uint8_t*))&MelodyHelperSketch::onCCCallback);
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

  void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("MEL "), 5);
    m_strncpy_p(n2, PSTR("ODY "), 5);
  }

  virtual void show() {
  }

  virtual void mute(bool pressed) {
    if (pressed) {
      muted = !muted;
      if (muted) {
	GUI.flash_strings_fill("MELODY", "MUTED");
      } else {
	GUI.flash_strings_fill("MELODY", "UNMUTED");
      }
    }
  }

  virtual void doExtra(bool pressed) {
  }

  virtual Page *getPage(uint8_t i) {
    return NULL;
  }
};

#endif /* MELODY_HELPER_SKETCH_H__ */
