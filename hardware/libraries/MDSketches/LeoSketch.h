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
  
 LeoTriggerPage() : trackStartEncoder("STR"), triggerOnOffEncoder("TRG"),
    kitSelectEncoder("KIT"), patternSelectEncoder("PAT") {

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
		// change kit only if encoder1 is not pressed
		// when encoder1 is released, then change kit if it was moved
		if (EVENT_RELEASED(event, Buttons.ENCODER1)) {
			if (MDKitSelectEncoderChanged) {
				MDKitSelectEncoderHandleSpecial(&kitSelectEncoder);
			}
			if (MDPatternSelectEncoderChanged) {
				MDPatternSelectEncoderHandleSpecial(&patternSelectEncoder);
			}
			return true;
		}
		
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

class LeoMDLFOPage : public MDLFOPage, MDCallback {
 public:
	bool isInPage1;
	
 LeoMDLFOPage() : MDLFOPage() {
		isInPage1 = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)(&LeoMDLFOPage::onKitChanged));
	}

	void onKitChanged() {
		loadFromKit();
	}
	
	virtual bool handleEvent(gui_event_t *event) {
		for (uint8_t i = Buttons.ENCODER1; i < Buttons.ENCODER4; i++) {
			if (EVENT_PRESSED(event, i)) {
				uint8_t enci = i - Buttons.ENCODER1;
				if (lfoEncoders[enci].param == MD_LFO_SHP1) {
					GUI.flash_string_fill("RANDOM SHP1");
					return true;
				}
				if (lfoEncoders[enci].param == MD_LFO_SHP2) {
					GUI.flash_string_fill("RANDOM SHP2");
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
  MuteTrigPage muteTrigPage;
  ScrollSwitchPage switchPage;
  AutoEncoderPage<MDEncoder> autoMDPage;

	//  MDPitchEuclidConfigPage1 euclidPage1;
	//  MDPitchEuclidConfigPage2 euclidPage2;
	//  MDPitchEuclid pitchEuclid;

 public:
 LeoSketch()
	// :
	//	euclidPage1(&pitchEuclid), euclidPage2(&pitchEuclid)

		{
	}
	
  virtual void setup() {
    scalePage.setName("SCALE");
    triggerPage.setName("TRIGGER");
    switchPage.addPage(&triggerPage);
    switchPage.addPage(&scalePage);

		// lfo page setup
		lfoPage1.setName("LFOS 1/2");
		lfoPage1.initLFOParams(0, 1, 5, 6);
		lfoPage2.setName("LFOS 2/2");
		lfoPage2.initLFOParams(2, 3, 4, 7);
		switchPage.addPage(&lfoPage1);
		switchPage.addPage(&lfoPage2);

		// euclid config
		//    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&LeoSketch::on16Callback);
		//		euclidPage1.setName("EUCLID 1/2");
		//		euclidPage2.setName("EUCLID 2/2");
		//		switchPage.addPage(&euclidPage1);
		//		switchPage.addPage(&euclidPage2);

		// auto page setup
		autoMDPage.setup();
		autoMDPage.setName("AUTO LEARN");
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
		if ((currentPage() == &scalePage) ||
				(currentPage() == &triggerPage)) {
			if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
				pushPage(&switchPage);
				return true;
			}
		} else if (currentPage() == &switchPage) {
			if (EVENT_RELEASED(event, Buttons.ENCODER4) || EVENT_RELEASED(event, Buttons.BUTTON4)) {
				if (!switchPage.setSelectedPage()) {
					popPage(&switchPage);
				}
				return true;
			}
		} else {
			if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
				MidiUart.sendNoteOn(0, 0);
				pushPage(&switchPage);
				return true;
			} 
		}			

    return false;
  }

	void on16Callback(uint32_t counter) {
		//		pitchEuclid.on16Callback(counter);
	}
	
};

/* @} @} @} */

#endif /* LEO_SKETCH_H__ */
