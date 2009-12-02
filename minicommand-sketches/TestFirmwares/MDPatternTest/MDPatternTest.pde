#include <MD.h>

uint8_t outputSysex[8192];

class PatternSketch : 
public Sketch {
public:
  MDPattern pattern;
  RangeEncoder patEncoder;
  EncoderPage page;

  PatternSketch() {
    patEncoder.initRangeEncoder(0, 127, "PAT", 0);  
    page.setEncoders(&patEncoder);
    setPage(&page);
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      MD.requestPattern(patEncoder.getValue());
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      uint16_t len = pattern.toSysex(outputSysex, sizeof(outputSysex));
      MidiUart.sendRaw(outputSysex, len);
    }
  }
};

PatternSketch sketch;

void onPatternCallback() {
//  MidiUart.putc(0xF0);
//  MidiUart.sendRaw(MidiSysex.data + 0x1400, MidiSysex.recordLen - 0x1400);
//  MidiUart.putc(0xF7);
  if (!sketch.pattern.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
//    GUI.flash_strings_fill("ERROR PARSING", "PATTERN");
  } else {
    GUI.flash_strings_fill("PARSED PATTERN", "");
  }
}

void setup() {
  MDSysexListener.setup();
  MDSysexListener.setOnPatternMessageCallback(onPatternCallback);
  GUI.setSketch(&sketch);
}

void loop() {
}
