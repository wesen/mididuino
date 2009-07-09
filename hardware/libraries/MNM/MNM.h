#ifndef MNM_H__
#define MNM_H__

#include <inttypes.h>

#include "WProgram.h"

extern uint8_t monomachine_sysex_hdr[5];

class MNMEncoder : public CCEncoder {
 public:
  uint8_t track;
  uint8_t param;

  virtual uint8_t getCC();
  virtual uint8_t getChannel();
  virtual void initCCEncoder(uint8_t _channel, uint8_t _cc);
  void initMNMEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0) {
    track = _track;
    param = _param;
    setName(_name);
    setValue(init);
  }

  MDEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0);
  void loadFromKit();
};

class MNMClass {
 public:
  MNMClass() {
    baseChannel = 0;
  }

  uint8_t baseChannel;
  uint8_t currentTrack;
  
  int currentGlobal;
  bool loadedGlobal;
  MNMGlobal global;
  
  int currentKit;
  bool loadedKit;
  MNMKit kit;

  int currentPattern;

  void sendNoteOn(uint8_t note, uint8_t velocity) {
    sendNoteOn(currentTrack, note, velocity);
  }
  void sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity);
  void sendNoteOff(uint8_t note) {
    sendNoteOff(currentTrack, note);
  }
  void sendNoteOff(uint8_t track, uint8_t note);

  void setParam(uint8_t param, uint8_t value) {
    setParam(currentTrack, param, value);
  }
  void setParam(uint8_t track, uint8_t param, uint8_t value);
  void parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param);

  void loadGlobal(uint8_t id);
  void loadKit(uint8_t id);
  void loadPattern(uint8_t id);
  void loadSong(uint8_t id);

  void setCurrentKitName(char *name);
  void saveCurrentKit(uint8_t id);

  void sendRequest(uint8_t type, uint8_t param);
  void requestKit(uint8_t kit);
  void requestPattern(uint8_t pattern);
  void requestSong(uint8_t song);
  void requestGlobal(uint8_t global);

  void assignMachine(uint8_t model) {
    assignMachine(currentTrack, model);
  }
  void assignMachine(uint8_t track, uint8_t model);
  void setMachine(MNMMachine *machine) {
    setMachine(currentTrack, machine);
  }
  void setMachine(uint8_t track, MNMMachine *machine);

  void muteTrack() {
    muteTrack(currentTrack);
  }
  void muteTrack(uint8_t track);
  void unmuteTrack() {
    unmuteTrack(currentTrack);
  }
  void unmuteTrack(uint8_t track);

  PGM_P getMachineName(uint8_t machine);
  PGM_P getModelParamName(uint8_t model, uint8_t param);
  void getPatternName(uint8_t pattern, char str[5]);
  
};

extern MNMClass MNM;

#include "MNMMessages.hh"
#include "MNMPattern.hh"
#include "MNMParams.hh"
#include "MNMSysex.hh"
#include "MNMTask.hh"

#endif /* MNM_H__ */
