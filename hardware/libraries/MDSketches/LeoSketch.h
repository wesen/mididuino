#ifndef LEO_SKETCH_H__
#define LEO_SKETCH_H__

#include <MD.h>
#include <Scales.h>
#include <MDLFOPage.h>

class LeoTriggerClass {
 public:
  bool isTriggerOn;
  const static scale_t *scales[];
  uint8_t currentScale;
  uint8_t numOctaves;
  uint8_t basePitch;
  uint8_t scaleSpread;
  
  LeoTriggerClass() {
    isTriggerOn = true;
    currentScale = 0;
    numOctaves = 1;
    scaleSpread = 5;
  }
  
  void triggerTrack(uint8_t track) {
    uint8_t pitch = basePitch + randomScalePitch(scales[currentScale], numOctaves);
    uint8_t value = MIN(127, pitch * scaleSpread);
    MD.setTrackParam(track, 0, value);
    
    if (isTriggerOn) {
      MD.triggerTrack(track, 100);
    }
  }
};

const scale_t *LeoTriggerClass::scales[] = {
  &ionianScale,
  &aeolianScale,
  &bluesScale,
  &majorPentatonicScale,
  &majorMaj7Arp,
  &majorMin7Arp,
  &minorMin7Arp
};

LeoTriggerClass leoTrigger;

class LeoScalePage : public EncoderPage {
 public:
  VarRangeEncoder scaleSelectEncoder;
  VarRangeEncoder basePitchEncoder;
  VarRangeEncoder spreadEncoder;
  VarRangeEncoder octaveEncoder;

 LeoScalePage() :
  scaleSelectEncoder(&leoTrigger.currentScale, 0, countof(LeoTriggerClass::scales) - 1, "SCL"),
    basePitchEncoder(&leoTrigger.basePitch, 0, 127, "BAS"),
    spreadEncoder(&leoTrigger.scaleSpread, 1, 10, "SPR"),
    octaveEncoder(&leoTrigger.numOctaves, 0, 5, "OCT") {
    setEncoders(&scaleSelectEncoder, &basePitchEncoder, &spreadEncoder, &octaveEncoder);
  }
};

class LeoTriggerPage : public EncoderPage {
  public:
  MDTrackFlashEncoder trackStartEncoder;
  BoolEncoder triggerOnOffEncoder;
  MDKitSelectEncoder kitSelectEncoder;
  MDPatternSelectEncoder patternSelectEncoder;
  
  LeoTriggerPage() : trackStartEncoder("STR"), triggerOnOffEncoder("TRG"),
    kitSelectEncoder("KIT"), patternSelectEncoder("PAT") {
    setEncoders(&trackStartEncoder, &triggerOnOffEncoder, &kitSelectEncoder, &patternSelectEncoder);
    for (uint8_t i = 0; i < 4; i++) {
      encoders[i]->pressmode = true;
    }
  }
  
  virtual void loop() {
    if (triggerOnOffEncoder.hasChanged()) {
      leoTrigger.isTriggerOn = triggerOnOffEncoder.getBoolValue();
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
	leoTrigger.triggerTrack(trackStartEncoder.getValue() + (i - Buttons.BUTTON1));
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
