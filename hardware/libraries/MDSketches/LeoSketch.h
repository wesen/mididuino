/* Copyright (c) 2009 - http://ruinwesen.com/ */

#ifndef LEO_SKETCH_H__
#define LEO_SKETCH_H__

#include <MD.h>
#include <Scales.h>
#include <MDLFOPage.h>
#include <AutoEncoderPage.h>
#include <MDPitchEuclid.h>
#include <MDPitchEuclidSketch.h>

/**
 * \addtogroup MD Elektron MachineDrum
 *
 * @{
 * 
 * \addtogroup md_sketches MachineDrum Sketches
 * 
 * @{
 **/

/**
 * \addtogroup md_sketches_leo MachineDrum Leo Sketch
 *
 * @{
 **/

class LeoTriggerClass {
public:
  bool isTriggerOn;
  const static scale_t *scales[];

  const scale_t *currentScale;
  //  uint8_t currentScale;
  uint8_t numOctaves;
  uint8_t basePitch;
  uint8_t scaleSpread;
  uint8_t trackStart;
  
  LeoTriggerClass() {
    isTriggerOn = true;
    currentScale = 0;
    numOctaves = 1;
    scaleSpread = 5;
    trackStart = 1;
    basePitch = MIDI_NOTE_C2;
  }
  
  void triggerTrack(uint8_t track) {
    uint8_t pitch = randomScalePitch(currentScale, numOctaves);
    uint8_t value = MIN(127, pitch * scaleSpread + basePitch);

    MD.setTrackParam(track, 0, value);
    
    if (isTriggerOn) {
      MD.triggerTrack(track, 100);
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      triggerTrack(trackStart);
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      triggerTrack(trackStart + 1);
      return true;
    }
    return false;
  }
};

const scale_t *LeoTriggerClass::scales[] = {
  &ionianScale,
  &aeolianScale,

  &harmonicMinorScale,
  &melodicMinorScale,
  &lydianDominantScale,

  &wholeToneScale,
  &wholeHalfStepScale,
  &halfWholeStepScale,

  &bluesScale,
  &majorPentatonicScale,
  &minorPentatonicScale,
  &suspendedPentatonicScale,
  &inSenScale,

  &majorBebopScale,
  &dominantBebopScale,
  &minorBebopScale,

  &majorArp,
  &minorArp,
  &majorMaj7Arp,
  &majorMin7Arp,
  &minorMin7Arp,
};

LeoTriggerClass leoTrigger;

class ScaleSelectEncoder: public VarRangeEncoder {
public:
  ScaleSelectEncoder(uint8_t *_var, int _max = 127, int _min = 0, const char *_name = NULL, int init = 0) :
    VarRangeEncoder(_var, _max, _min, _name, init) {
  }
  
  void displayAt(int i) {
    GUI.put_string_at(i * 4, LeoTriggerClass::scales[getValue()]->shortName);
  }
};

class LeoScalePage : public EncoderPage {
public:
  //  ScaleSelectEncoder scaleSelectEncoder;
  ScaleEncoder scaleSelectEncoder;
  NotePitchEncoder basePitchEncoder;
  //  VarRangeEncoder basePitchEncoder;
  VarRangeEncoder spreadEncoder;
  VarRangeEncoder octaveEncoder;

  LeoScalePage() :
    //  scaleSelectEncoder(&leoTrigger.currentScale, 0, countof(LeoTriggerClass::scales) - 1, "SCL"),
    scaleSelectEncoder("SCL", LeoTriggerClass::scales, countof(LeoTriggerClass::scales) - 1),
    //    basePitchEncoder(&leoTrigger.basePitch, 0, 127, "BAS"),
    basePitchEncoder("BAS"),
    spreadEncoder(&leoTrigger.scaleSpread, 1, 10, "SPR"),
    octaveEncoder(&leoTrigger.numOctaves, 0, 5, "OCT") {
    setEncoders(&scaleSelectEncoder, &basePitchEncoder, &spreadEncoder, &octaveEncoder);
    basePitchEncoder.setValue(leoTrigger.basePitch);
  }

  void loop() {
    if (scaleSelectEncoder.hasChanged()) {
      leoTrigger.currentScale = scaleSelectEncoder.getScale();
    }
    if (basePitchEncoder.hasChanged()) {
      leoTrigger.basePitch = basePitchEncoder.getValue();
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    return leoTrigger.handleEvent(event);
  }
};

// set to true when the kit encoder is moved but no kit loading
bool MDKitSelectEncoderChanged = false;

// special handler to avoid loading new kits if ENCODER1 is held down
void MDKitSelectEncoderHandleSpecial(Encoder *enc) {
  if (BUTTON_UP(Buttons.ENCODER1)) {
    MD.loadKit(enc->getValue());
    MDKitSelectEncoderChanged = false;
  } else {
    MDKitSelectEncoderChanged = true;
  }
}

// set to true when the pattern encoder is moved but no pattern loading
bool MDPatternSelectEncoderChanged = false;

// special handler to avoid loading new patterns if ENCODER1 is held down
void MDPatternSelectEncoderHandleSpecial(Encoder *enc) {
  if (BUTTON_UP(Buttons.ENCODER1)) {
    MD.loadPattern(enc->getValue());
    MDPatternSelectEncoderChanged = false;
  } else {
    MDPatternSelectEncoderChanged = true;
  }
}


class LeoTriggerPage : public EncoderPage {
public:
  MDTrackFlashEncoder trackStartEncoder;
  BoolEncoder triggerOnOffEncoder;
  MDKitSelectEncoder kitSelectEncoder;
  MDPatternSelectEncoder patternSelectEncoder;
  
  LeoTriggerPage() :
    trackStartEncoder("STR"), triggerOnOffEncoder("TRG", leoTrigger.isTriggerOn),
    kitSelectEncoder("KIT"), patternSelectEncoder("PAT") {

    trackStartEncoder.setValue(leoTrigger.trackStart);
    // set special handlers for kit and pattern select encoders
    kitSelectEncoder.handler = MDKitSelectEncoderHandleSpecial;
    patternSelectEncoder.handler = MDPatternSelectEncoderHandleSpecial;
    
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
  MDTempoEncoder tempoEncoder;

  MuteTrigPage() :
    trackEncoder("TRK"),
    trigEncoder(0, "TRG", 16),
    muteEncoder(0, "MUT", 16),
    tempoEncoder("TMP") {
    setEncoders(&trackEncoder, &trigEncoder, &muteEncoder, &tempoEncoder);
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

void LeoMDLFOEncoderHandle(Encoder *enc) {
  MDLFOEncoder *mdEnc = (MDLFOEncoder *)enc;
  if (BUTTON_DOWN(Buttons.BUTTON1)) {
    for (uint8_t i = 0; i < 16; i++) {
      MD.setLFOParam(i, mdEnc->param, mdEnc->getValue());
    }
  } else {
    MD.setLFOParam(mdEnc->track, mdEnc->param, mdEnc->getValue());
  }
}


class LeoMDLFOTrackSelectPage : public EncoderPage {
  /**
   * \addtogroup md_lfo_page
   *
   * @{
   **/
public:
  MDTrackFlashEncoder trackEncoder;
  MDLFOPage *lfoPage1, *lfoPage2;

  LeoMDLFOTrackSelectPage(MDLFOPage *_lfoPage1, MDLFOPage *_lfoPage2) : trackEncoder("TRK") {
    encoders[0] = &trackEncoder;
    lfoPage1 = _lfoPage1;
    lfoPage2 = _lfoPage2;
  }  

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      lfoPage1->setTrack(trackEncoder.getValue());
      lfoPage2->setTrack(trackEncoder.getValue());
    }
  }
  /* @} */
};

class LeoMDLFOPage : public MDLFOPage, MDCallback {
public:
  bool isInPage1;
	
  LeoMDLFOPage() : MDLFOPage() {
    isInPage1 = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)(&LeoMDLFOPage::onKitChanged));
    for (int i = 0; i < 4; i++) {
      lfoEncoders[i].handler = LeoMDLFOEncoderHandle;
    }
  }

  void onKitChanged() {
    loadFromKit();
  }
	
  virtual bool handleEvent(gui_event_t *event) {
    for (uint8_t i = Buttons.ENCODER1; i < Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
	uint8_t enci = i - Buttons.ENCODER1;
        // randomize all separately if button3 is pressed
        // randomize same if track is set to ALL XXX
	if (lfoEncoders[enci].max < 127) {
	  lfoEncoders[enci].setValue(random(0, lfoEncoders[enci].max), true);
	  return true;
	}
      }
    }

    return false;
  }
};

class LeoSketch : public Sketch, public MDCallback, public ClockCallback {
  LeoTriggerPage triggerPage;
  LeoScalePage scalePage;

  LeoMDLFOPage lfoPage1;
  LeoMDLFOPage lfoPage2;
  LeoMDLFOTrackSelectPage lfoSelectPage;

  MuteTrigPage muteTrigPage;
  ScrollSwitchPage switchPage;
  AutoEncoderPage<MDEncoder> autoMDPage;

  MDPitchEuclidConfigPage1 euclidPage1;
  MDPitchEuclidConfigPage2 euclidPage2;
  MDPitchEuclid pitchEuclid;

public:
  LeoSketch() :
    lfoSelectPage(&lfoPage1, &lfoPage2),
    euclidPage1(&pitchEuclid),
    euclidPage2(&pitchEuclid, LeoTriggerClass::scales, countof(LeoTriggerClass::scales) - 1)
  {
  }
	
  virtual void setup() {
    /* clock setup */
    MidiClock.stop();
    MidiClock.mode = MidiClock.EXTERNAL_MIDI;
    MidiClock.useImmediateClock = true;
    MidiClock.start();
    
    triggerPage.setName("TRIGGER");
    switchPage.addPage(&triggerPage);

    scalePage.setName("SCALE");
    switchPage.addPage(&scalePage);

    // lfo page setup
    lfoPage1.setName("LFOS 1/2");
    lfoPage1.initLFOParams(0, 1, 5, 6);
    lfoPage2.setName("LFOS 2/2");
    lfoPage2.initLFOParams(2, 3, 4, 7);
    switchPage.addPage(&lfoPage1);
    switchPage.addPage(&lfoPage2);

    // euclid config
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&LeoSketch::on16Callback);
    euclidPage1.setName("EUCLID 1/2");
    euclidPage2.setName("EUCLID 2/2");
    switchPage.addPage(&euclidPage1);
    switchPage.addPage(&euclidPage2);

    // auto page setup
    autoMDPage.setup();
    autoMDPage.setName("AUTO LEARN");
    autoMDPage.clearButton = Buttons.BUTTON1;
    ccHandler.setup();

    switchPage.addPage(&autoMDPage);

    muteTrigPage.setName("MUTE & TRIG");
    switchPage.addPage(&muteTrigPage);

    scalePage.encoders[3]->pressmode = true;
    triggerPage.encoders[3]->pressmode = true;

    switchPage.parent = this;
    
    setPage(&triggerPage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    EncoderPage *_currentPage = (EncoderPage *)currentPage();
    // handle modal pages first
    if (_currentPage == &switchPage) {
      if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
	if (!switchPage.setSelectedPage()) {
	  popPage(&switchPage);
	}
	return true;
      }
      
      return false;
    }

    // return from lfoSelectPage
    if (_currentPage == &lfoSelectPage) {
      if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
	popPage(&lfoSelectPage);
        return true;
      }
      
      return false;
    }

    // page select on button 4 always
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      pushPage(&switchPage);
      return true;
    }

    if ((_currentPage == &lfoPage1) ||
        (_currentPage == &lfoPage2)) {
      if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
        pushPage(&lfoSelectPage);
        return true;
      }
    }

    // hold on button2 except for autoMDPage
    if (_currentPage != &autoMDPage) {
      if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
        _currentPage->lockEncoders();
        return true;
      } else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
        _currentPage->unlockEncoders();
        return true;
      }
    }
    
    return false;
  }

  void on16Callback(uint32_t counter) {
    pitchEuclid.on16Callback(counter);
  }
	
};

/* @} @} @} */

#endif /* LEO_SKETCH_H__ */
