#ifndef LEO_SKETCH_H__
#define LEO_SKETCH_H__

#include <MD.h>
#include <Scales.h>
#include <MDLFOPage.h>

scale_t *currentScale = &minorMin7Arp;

class LeoTriggerPage : public EncoderPage {
  public:
  MDTrackFlashEncoder trackStartEncoder;
  BoolEncoder triggerOnOffEncoder;
  MDKitSelectEncoder kitSelectEncoder;
  MDPatternSelectEncoder patternSelectEncoder;
  
  LeoTriggerPage() : trackStartEncoder("STR"), triggerOnOffEncoder("TRG"),
    kitSelectEncoder("KIT"), patternSelectEncoder("PAT") {
    encoders[0] = &trackStartEncoder;
    encoders[1] = &triggerOnOffEncoder;
    encoders[2] = &kitSelectEncoder;
    encoders[3] = &patternSelectEncoder;
    for (uint8_t i = 0; i < 4; i++) {
      encoders[i]->pressmode = true;
    }
  }
  
  virtual void loop() {
    if (kitSelectEncoder.hasChanged()) {
    }
    if (patternSelectEncoder.hasChanged()) {
      MD.loadPattern(patternSelectEncoder.getValue());
    }
  }
  
  virtual void display() {
    EncoderPage::display();
    if (patternSelectEncoder.hasChanged()) {
      uint8_t pattern = patternSelectEncoder.getValue();
      char name[5];
      MD.getPatternName(pattern, name);
      GUI.put_string_at(12, name);
    }
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    for (uint8_t i = Buttons.BUTTON1; i <= Buttons.BUTTON4; i++) {
      if (EVENT_PRESSED(event, i)) {
        static const uint8_t ccValues[8] = {
          10, 20,  30, 50, 70, 90, 100, 110
        };
        uint8_t track = trackStartEncoder.getValue() + (i - Buttons.BUTTON1);
        uint8_t pitch = randomScalePitch(currentScale, 1) ;
        uint8_t realPitch = MD.trackGetPitch(track, pitch);
        MD.setTrackParam(track, 0, ccValues[random(8)]);
        
        if (triggerOnOffEncoder.getValue() == 1) {
          MD.triggerTrack(track, 100);
        }
      }
    }
  }
};

class LeoSketch : public Sketch, public MDCallback {
  LeoTriggerPage triggerPage;
  MDLFOPage lfoPage;
  EncoderSwitchPage switchPage;

 public:
  virtual void setup() {
    lfoPage.setName("LFOS");
    triggerPage.setName("TRIGGER");
    switchPage.initPages(&triggerPage, &lfoPage);
    switchPage.parent = this;
    
    setPage(&lfoPage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.ENCODER1)) {
      popPage(&switchPage);
      return true;
    }

    return false;
  }
};


#endif /* LEO_SKETCH_H__ */
