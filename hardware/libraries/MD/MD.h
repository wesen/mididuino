#ifndef MD_H__
#define MD_H__

#include <inttypes.h>

#include "Elektron.hh"
#include "MDSysex.hh"
#include "MDParams.hh"

extern uint8_t machinedrum_sysex_hdr[5];

typedef struct tuning_s {
  uint8_t model;
  uint8_t base;
  uint8_t len;
  uint8_t offset;
  const uint8_t *tuning;
} tuning_t;

typedef struct lfo_s {
  uint8_t params[8];
} lfo_t;

typedef struct md_machine_s {
  uint8_t model;
  uint8_t params[24];
  lfo_t lfo;
} md_machine_t;

typedef struct machine_name_s {
  char name[7];
  uint8_t id;
} machine_name_t;

#ifdef MIDIDUINO_USE_GUI
class MDEncoder : public RangeEncoder {
public:
  uint8_t track;
  uint8_t param;

  MDEncoder(uint8_t _track, uint8_t _param, char *_name = NULL, uint8_t init = 0);
  void handle(uint8_t val);
};
#endif

class MDClass {
 public:
  MDClass() {
    currentGlobal = -1;
    currentKit = -1;
    baseChannel = 0;
    for (uint8_t i = 0; i < 16; i++) {
      trackModels[i] = 0;
    }
  }
  int currentGlobal;
  int currentKit;
  uint8_t baseChannel;
  uint8_t trackModels[16];
  uint8_t trackParams[16][24];
  uint8_t trackLevels[16];
  char name[16];

  void parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param);
  void triggerTrack(uint8_t track, uint8_t velocity);
  void setTrackParam(uint8_t track, uint8_t param, uint8_t value);

  void sendFXParam(uint8_t param, uint8_t value, uint8_t type);
  void setEchoParam(uint8_t param, uint8_t value);
  void setReverbParam(uint8_t param, uint8_t value);
  void setEQParam(uint8_t param, uint8_t value);
  void setCompressorParam(uint8_t param, uint8_t value);

  void sendRequest(uint8_t type, uint8_t param);

  uint8_t trackGetPitch(uint8_t track, uint8_t pitch);
  uint8_t trackGetCCPitch(uint8_t track, uint8_t cc, int8_t *offset = NULL);
  
  void sendNoteOn(uint8_t track, uint8_t pitch,
			    uint8_t velocity);

  void sliceTrack32(uint8_t track, uint8_t from, uint8_t to, bool correct = true);
  void sliceTrack16(uint8_t track, uint8_t from, uint8_t to);

  const tuning_t* getModelTuning(uint8_t model);
  uint8_t noteToTrack(uint8_t pitch);
  bool isMelodicTrack(uint8_t track);

  void setLFOParam(uint8_t track, uint8_t param, uint8_t value);
  void setLFO(uint8_t track, lfo_t *lfo);
  
  void assignMachine(uint8_t track, uint8_t model);
  
  void setMachine(uint8_t track, md_machine_t *machine);

  void muteTrack(uint8_t track, bool mute = true);
  void unmuteTrack(uint8_t track) {
    muteTrack(track, false);
  }
};

PGM_P getMachineName(uint8_t machine);

extern MDClass MD;

#endif /* MD_H__ */
