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
  uint8_t trackStart;
  
  LeoTriggerClass() {
    isTriggerOn = true;
    currentScale = 0;
    numOctaves = 1;
    scaleSpread = 5;
  }
  
  void triggerTrack(uint8_t track) {
    uint8_t pitch = randomScalePitch(scales[currentScale], numOctaves);
    uint8_t value = MIN(127, pitch * scaleSpread + basePitch);

#if 0
    GUI.setLine(GUI.LINE1);
    GUI.flash_put_value(0, track);
    GUI.flash_put_value(1, pitch);
    GUI.flash_put_value(2, value);
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, currentScale);
    GUI.flash_put_value(1, numOctaves);
    GUI.flash_put_value(2, basePitch);
    GUI.flash_put_value(3, scaleSpread);
#endif

    MD.setTrackParam(track, 0, value);
    
    if (isTriggerOn) {
      MD.triggerTrack(track, 100);
    }
  }

  bool handleEvent(gui_event_t *event) {
    for (uint8_t i = Buttons.BUTTON1; i <= Buttons.BUTTON4; i++) {
      if (EVENT_PRESSED(event, i)) {
	triggerTrack(trackStart + (i - Buttons.BUTTON1));
	return true;
      }
    }
    return false;
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

  virtual bool handleEvent(gui_event_t *event) {
    return leoTrigger.handleEvent(event);
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
  }
  
  virtual void loop() {
    if (triggerOnOffEncoder.hasChanged()) {
      leoTrigger.isTriggerOn = triggerOnOffEncoder.getBoolValue();
    }
    if (trackStartEncoder.hasChanged()) {
      leoTrigger.trackStart = trackStartEncoder.getValue();
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
    return leoTrigger.handleEvent(event);
  }
};
 
class MuteTrigPage : public EncoderPage, MDCallback {
 public:
  MDTrackFlashEncoder trackEncoder;
  MDTrigGroupEncoder trigEncoder;
  MDMuteGroupEncoder muteEncoder;

 MuteTrigPage() : trackEncoder("TRK"), trigEncoder(0, "TRG", 16), muteEncoder(0, "MUT", 16) {
    setEncoders(&trackEncoder, &trigEncoder, &muteEncoder);
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)(&MuteTrigPage::onKitChanged));
  }

  void onKitChanged() {
    muteEncoder.loadFromMD();
    trigEncoder.loadFromMD();
  }

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      uint8_t track = trackEncoder.getValue();
      trigEncoder.track = track;
      muteEncoder.track = track;
      onKitChanged();
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    return leoTrigger.handleEvent(event);
  }
};

class LeoSketch : public Sketch, public MDCallback {
  LeoTriggerPage triggerPage;
  LeoScalePage scalePage;
  //  MDLFOPage lfoPage;
  MuteTrigPage muteTrigPage;
  ScrollSwitchPage switchPage;

 public:
  virtual void setup() {
    //    lfoPage.setName("LFOS");
    scalePage.setName("SCALE");
    triggerPage.setName("TRIGGER");
    muteTrigPage.setName("MUTE & TRIG");
    switchPage.addPage(&triggerPage);
    switchPage.addPage(&scalePage);
    switchPage.addPage(&muteTrigPage);
    //    switchPage.addPage(&lfoPage);

    //    lfoPage.encoders[3]->pressmode = true;
    scalePage.encoders[3]->pressmode = true;
    triggerPage.encoders[3]->pressmode = true;

    switchPage.parent = this;
    
    setPage(&triggerPage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.ENCODER4)) {
      if (!switchPage.setSelectedPage()) {
	popPage(&switchPage);
      }
      return true;
    }

    return false;
  }
};

#endif /* LEO_SKETCH_H__ */
