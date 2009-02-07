#include <GUI.h>
#include <MD.h>
#include <Sequencer.h>

class EuclidPage {
  public:
  EuclidDrumTrack track;
  EncoderPage page;
  RangeEncoder pulseEncoder;
  RangeEncoder lengthEncoder;
  RangeEncoder offsetEncoder;
  RangeEncoder pitchEncoder;
  
  EuclidPage() : track(0, 8), pulseEncoder(0, 32), lengthEncoder(1, 32), 
            offsetEncoder(0, 32), pitchEncoder(0, 15) {
    page.encoders[0] = &pulseEncoder;
    page.encoders[1] = &lengthEncoder;
    page.encoders[2] = &offsetEncoder;
    page.encoders[3] = &pitchEncoder;
    lengthEncoder.setValue(8);
  }
};

EuclidPage euclids[4];
EuclidPage *currentEuclid = &euclids[0];

void switchPage(uint8_t i) {
  currentEuclid = &euclids[i];
  GUI.setPage(&currentEuclid->page);
}

void handleGui() {
  if (BUTTON_PRESSED(4))  
    MidiClock.pause();

  for (uint8_t i = 0; i < 4; i++) {
    if (BUTTON_DOWN(Buttons.SHIFT) && BUTTON_PRESSED(i)) {
      switchPage(i);
    }
  }
}


uint8_t cnt = 0;

void on16Callback() {
   for (uint8_t i = 0; i < 4; i++) {
    if (euclids[i].track.isHit(MidiClock.div16th_counter)) {
     MD.triggerTrack(euclids[i].pitchEncoder.getValue(), 100);
    }
  }
}

/* check encoder and real interrupt load */
void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
  switchPage(0);
}

void loop() {
  GUI.updatePage();
  
  if (currentEuclid->pulseEncoder.hasChanged() || 
      currentEuclid->lengthEncoder.hasChanged() || 
      currentEuclid->offsetEncoder.hasChanged()) {
    currentEuclid->track.setEuclid(currentEuclid->pulseEncoder.getValue(), 
                                   currentEuclid->lengthEncoder.getValue(), 
                                   currentEuclid->offsetEncoder.getValue());
  }
  GUI.setLine(GUI.LINE1);
  GUI.put_value16(2, currentEuclid->track.pattern);

  GUI.update();
}
