#ifndef MDARPEGGIATORSKETCH_H__
#define MDARPEGGIATORSKETCH_H__

class ConfigPage_1 : public EncoderPage {
  public:
  MDMelodicTrackFlashEncoder trackEncoder;
  RangeEncoder speedEncoder;
  RangeEncoder octavesEncoder;
  RangeEncoder lenEncoder;
  
  ConfigPage_1() :
    trackEncoder("TRK"),
    speedEncoder(1, 16, "SPD"),
    octavesEncoder(0, 5, "OCT"),
    lenEncoder(0, 16, "LEN")
  {
    encoders[0] = &trackEncoder;
    encoders[1] = &speedEncoder;
    encoders[2] = &octavesEncoder;
    encoders[3] = &lenEncoder;
    trackEncoder.setValue(arpeggiator.arpTrack);
    speedEncoder.setValue(arpeggiator.arpSpeed);
  }  
  
  virtual void display() {
    EncoderPage::display();
    if (redisplay || lenEncoder.hasChanged()) {
      if (lenEncoder.getValue() == 0)
        GUI.put_p_string_at(12, PSTR("INF"));
    }
  }
  
  virtual void loop() {
    if (lenEncoder.hasChanged()) {
      arpeggiator.arpTimes = lenEncoder.getValue();
    }
    if (speedEncoder.hasChanged()) {
      arpeggiator.arpSpeed = speedEncoder.getValue();
    }
    if (octavesEncoder.hasChanged()) {
      arpeggiator.arpOctaves = octavesEncoder.getValue();
      arpeggiator.arpOctaveCount = 0;
    }
    if (trackEncoder.hasChanged()) {
      uint8_t track = trackEncoder.getValue();
      if (MD.isMelodicTrack(track)) {
        arpeggiator.arpTrack = track;
      }
    }
  }
};

class ConfigPage_2 : public EncoderPage {
  public:
  EnumEncoder styleEncoder;
  EnumEncoder retrigEncoder;
  RangeEncoder retrigSpeedEncoder;
  
  ConfigPage_2() :
    styleEncoder(arp_names, (int)ARP_STYLE_CNT, "STY"),
    retrigEncoder(retrig_names, (int)RETRIG_CNT, "TRG"),
    retrigSpeedEncoder(1, 32, "SPD")
  {
    encoders[0] = &styleEncoder;
    encoders[2] = &retrigEncoder;
    encoders[3] = &retrigSpeedEncoder;
    styleEncoder.setValue(0);
    arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
    retrigEncoder.setValue(0);
    arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
    arpeggiator.calculateArp();
  }
  
  virtual void loop() {
    bool changed = false;
    if (styleEncoder.hasChanged()) {
      arpeggiator.arpStyle = (arp_style_t)styleEncoder.getValue();
      changed = true;
    }
    if (retrigEncoder.hasChanged()) {
      arpeggiator.arpRetrig = (arp_retrig_type_t)retrigEncoder.getValue();
      changed = true;
    }
    if (changed) {
      arpeggiator.calculateArp();
    }
    if (retrigSpeedEncoder.hasChanged()) {
      arpeggiator.retrigSpeed = retrigSpeedEncoder.getValue();
    }
  }
};

class ArpeggiatorSketch : public Sketch, public MDCallback {
public:
  MDArpeggiatorClass arpeggiator;

  ConfigPage_1 configPage_1;
  ConfigPage_2 configPage_2;

  ArpeggiatorSketch() {
  }

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&ArpeggiatorSketch::onKitChanged);
    GUI.addTask(&MDTask);
    
    arpeggiator.setup();

    setPage(&configPage_1);
  }

  void destroy() {
    GUI.removeTask(&MDTask);
    MDTask.destroy();
  }

  bool handleEvent(gui_event_t *event) {
    if (currentPage() == &configPage_1) {
      if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
	setPage(&configPage_2);
	return true;
      }
    } else if (currentPage() == &configPage_2) {
      if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
	setPage(&configPage_1);
	return true;
      }
    }

    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      arpeggiator.startRecording();
    }
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MD.kit.name);
  }
};

#endif /* MDARPEGGIATORSKETCH_H__ */
