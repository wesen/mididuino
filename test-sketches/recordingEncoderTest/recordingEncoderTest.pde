#include <MD.h>
#include <SDCard.h>

RangeEncoder trkEnc(1, 16, "TRK");
CCEncoder ccEnc(0, 0, "CC");
RecordingEncoder<64> recEnc(&ccEnc);
EncoderPage page(&trkEnc, &recEnc);

void onCCCallback(uint8_t *msg) {
  uint8_t track, param;
  MD.parseCC(msg[0] & 0xF, msg[1], &track, &param);
  PGM_P name = NULL;
  name = model_param_name(MD.kit.machines[track].model, param);
  if (name != NULL) {
    GUI.flash_p_strings_fill(MD.getMachineName(MD.kit.machines[track].model), name);
  }

}

void on32Callback() {
  recEnc.playback(MidiClock.div32th_counter);
}

void setup() {
  MDTask.setup();
  MDTask.interval = 3000;
  MDTask.autoLoadKit = true;
  GUI.setPage(&page);
  Midi.setOnControlChangeCallback(onCCCallback);
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.setOn32Callback(on32Callback);
  MidiClock.start();
  if (SDCard.init() != 0) {
   GUI.flash_strings_fill("NO SD CARD", "");
   }
}

void loop() {
  MDTask.check();
  GUI.updatePage();
  while (!EventRB.isEmpty()) {
    gui_event_t event;
    EventRB.getp(&event);

    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      saveTrack(trkEnc.getValue());
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      recEnc.startRecording();
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
      recEnc.stopRecording();
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      recEnc.clearRecording();
    }
  }
  GUI.update();
}

void saveTrack(uint8_t track) {
      PGM_P machineName = MD.getMachineName(MD.kit.machines[track].model);
  char initName[17] = "";
  m_strncpy_p(initName, machineName, 16);
  char *presetName = getNameModalGui("PRESET NAME:  OK", initName);

   SDCardFile file;
   char fileName[64] = "/MDPresets/settings/";
   m_strnappend(fileName, presetName, 64);
   if (!file.create(fileName)) {
   return;
   }
   if (file.write((unsigned char *)&MD.kit.machines[track], sizeof(MDMachine)) != sizeof(MDMachine))
   goto end;
   file.close();
   return;
   
   end:
   GUI.flash_strings_fill("ERROR WRITING", "PRESET");
   file.close();
}

void handleGui() {
  pollEventGUI();
}
