#ifndef MD_H__
#define MD_H__

#include <inttypes.h>

#include "Elektron.hh"
#include "MDSysex.hh"

extern uint8_t machinedrum_sysex_hdr[5];

typedef struct tuning_s {
  uint8_t model;
  uint8_t base;
  uint8_t len;
  const uint8_t *tuning;
} tuning_t;

class MDClass {
 public:
  MDClass() {
    currentGlobal = 0;
    currentKit = 0;
    baseChannel = 0;
    for (uint8_t i = 0; i < 16; i++) {
      trackModels[i] = 0;
    }
  }
  uint8_t currentGlobal;
  uint8_t currentKit;
  uint8_t baseChannel;
  uint8_t trackModels[16];

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
  void sendNoteOn(uint8_t track, uint8_t pitch,
			    uint8_t velocity);

  void sliceTrack32(uint8_t track, uint8_t from, uint8_t to);
  void sliceTrack16(uint8_t track, uint8_t from, uint8_t to);

  const tuning_t* getModelTuning(uint8_t model);
  uint8_t noteToTrack(uint8_t pitch);
};

extern MDClass MD;

#define MD_ASSIGN_MACHINE_ID                             0x5b
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS                 0
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS         1
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS_ROUTING 2

#define MD_CURRENT_GLOBAL_SLOT_REQUEST       0x01
#define MD_CURRENT_KIT_REQUEST               0x02
#define MD_CURRENT_PATTERN_REQUEST           0x04
#define MD_CURRENT_SONG_REQUEST              0x08
#define MD_CURRENT_SEQUENCER_MODE            0x10
#define MD_CURRENT_LOCK_MODE                 0x20

#define MD_GLOBAL_MESSAGE_ID                 0x50
#define MD_GLOBAL_REQUEST_ID                 0x51
#define MD_KIT_MESSAGE_ID                    0x52
#define MD_KIT_REQUEST_ID                    0x53
#define MD_LOAD_KIT_ID                       0x58
#define MD_LOAD_PATTERN_ID                   0x57
#define MD_LOAD_SONG_ID                      0x6c
#define MD_PATTERN_MESSAGE_ID                0x67
#define MD_PATTERN_REQUEST_ID                0x68
#define MD_RESET_MIDI_NOTE_MAP_ID            0x64
#define MD_SAVE_KIT_ID                       0x59
#define MD_SAVE_SONG_ID                      0x6d
#define MD_SET_ACTIVE_GLOBAL_ID              0x56
#define MD_SET_CURRENT_KIT_NAME_ID           0x55
#define MD_SET_DYNAMIX_PARAM_ID              0x60
#define MD_SET_EQ_PARAM_ID                   0x5f
#define MD_SET_GATE_BOX_PARAM_ID             0x5e
#define MD_SET_LFO_PARAM_ID                  0x62
#define MD_SET_MIDI_NOTE_TO_TRACK_MAPPING_ID 0x5a
#define MD_SET_MUTE_GROUP_ID                 0x66
#define MD_SET_RECEIVE_DUMP_POSITION_ID      0x6b
#define MD_SET_RHYTHM_ECHO_PARAM_ID          0x5d
#define MD_SET_STATUS_ID                     0x71
#define MD_SET_TEMPO_ID                      0x61
#define MD_SET_TRACK_ROUTING_ID              0x5c
#define MD_SET_TRIG_GROUP_ID                 0x65
#define MD_SONG_MESSAGE_ID                   0x69
#define MD_SONG_REQUEST_ID                   0x6a
#define MD_STATUS_REQUEST_ID                 0x70
#define MD_STATUS_RESPONSE_ID                0x72

#define EFM_RS_MODEL 36
#define EFM_HH_MODEL 38
#define EFM_CP_MODEL 35
#define EFM_SD_MODEL 33
#define EFM_XT_MODEL 34
#define EFM_BD_MODEL 32
#define TRX_CL_MODEL 26
#define TRX_SD_MODEL 17
#define TRX_XC_MODEL 27
#define TRX_XT_MODEL 18
#define TRX_BD_MODEL 16
#define ROM_MODEL    128

#define MD_ECHO_TIME 0
#define MD_ECHO_MOD  1
#define MD_ECHO_MFRQ 2
#define MD_ECHO_FB   3
#define MD_ECHO_FLTF 4
#define MD_ECHO_FLTW 5
#define MD_ECHO_MONO 6
#define MD_ECHO_LEV  7

#define MD_REV_DVOL  0
#define MD_REV_PRED  1
#define MD_REV_DEC   2
#define MD_REV_DAMP  3
#define MD_REV_HP    4
#define MD_REV_LP    5
#define MD_REV_GATE  6
#define MD_REV_LEV   7

#define MD_EQ_LF     0
#define MD_EQ_LG     1
#define MD_EQ_HF     2
#define MD_EQ_HG     3
#define MD_EQ_PF     4
#define MD_EQ_PG     5
#define MD_EQ_PQ     6
#define MD_EQ_GAIN   7

#define MD_DYN_ATCK  0
#define MD_DYN_REL   1
#define MD_DYN_TRHD  2
#define MD_DYN_RTIO  3
#define MD_DYN_KNEE  4
#define MD_DYN_HP    5
#define MD_DYN_OUTG  6
#define MD_DYN_MIX   7

#endif /* MD_H__ */
