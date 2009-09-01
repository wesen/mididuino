#ifndef MNMWesenLivePatchSkech_H__
#define MNMWesenLivePatchSkech_H__

#include <MNM.h>
#include <AutoEncoderPage.h>
#include <MNMMagicPage.h>

class MNMWesenLivePatchSketch : 
public Sketch, public MNMCallback, public ClockCallback {
public:
  AutoEncoderPage<MNMEncoder> autoMNMPages[2];
  SwitchPage switchPage;
  MagicSwitchPage magicSwitchPages[2];

  uint8_t ramP1Track;

  void setupPages() {
    for (int i = 0; i < 2; i++) {
      autoMNMPages[i].setup();
      autoMNMPages[i].setShortName("P ");
      autoMNMPages[i].shortName[1] = '0' + i;
      magicSwitchPages[i].setup();
      magicSwitchPages[i].setShortName("M ");
      magicSwitchPages[i].shortName[1] = '0' + i;
    }

    switchPage.initPages(&autoMNMPages[0], &autoMNMPages[1], &magicSwitchPages[0], &magicSwitchPages[1]);
    switchPage.parent = this;
  }

  virtual void setup() {
    setupPages();

    MNMTask.addOnKitChangeCallback(this, (mnm_callback_ptr_t)&MNMWesenLivePatchSketch::onKitChanged);
    MidiClock.addOn32Callback(this, (midi_clock_callback_ptr_t)&MNMWesenLivePatchSketch::on32Callback);
    
    ccHandler.setup();
    //    ccHandler.setCallback(onLearnCallback);

    setPage(&autoMNMPages[0]);
  }

  virtual void destroy() {
    MidiClock.stop();
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
    } 
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
        MNM.revertToCurrentKit(true);
        GUI.flash_strings_fill("REVERT TO", MNM.kit.name);
        clearAllRecording();
      } 
      else if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
        MNM.revertToCurrentTrack(true);
        GUI.flash_strings_fill("REVERT TO ", "");
        GUI.setLine(GUI.LINE1);
        GUI.flash_put_value_at(10, MNM.currentTrack + 1);
        GUI.setLine(GUI.LINE2);
        GUI.flash_p_string_fill(MNM.getMachineName(MNM.kit.machines[MNM.currentTrack].model));
      }
      return true;
    } 

    return true;
  }

  void onKitChanged() {
    clearAllRecording();

    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MNM.kit.name);
  }  

  void on32Callback(uint32_t counter) {
    for (int i = 0; i < 2; i++) {
      autoMNMPages[i].on32Callback(counter);
      for (int j = 0; j < 4; j++) {
        magicSwitchPages[i].magicPages[j].on32Callback(counter);
      }
    }
  }
  //  GUI.flash_put_value(0, MidiClock.div32th_counter);

  void clearAllRecording() {
    for (int i = 0; i < 2; i++) {
      autoMNMPages[i].clearRecording();
      for (int j = 0; j < 4; j++) {
        magicSwitchPages[i].magicPages[j].clearRecording();
      }
    }
  }
};

#endif /* MNMWesenLivePatchSkech_H__ */
