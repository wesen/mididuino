#ifndef MD_H__
#define MD_H__

#include <inttypes.h>

class MD {
 public:
  static uint8_t baseChannel;
  static uint8_t trackModels[16];

  static void triggerTrack(uint8_t track, uint8_t velocity);
  static void setTrackParam(uint8_t track, uint8_t param, uint8_t value);
  static void setEchoParam(uint8_t param, uint8_t value);
  static void setReverbParam(uint8_t param, uint8_t value);
  static void setEQParam(uint8_t param, uint8_t value);
  static void setCompressorParam(uint8_t param, uint8_t value);

  static uint8_t trackGetPitch(uint8_t track, uint8_t pitch);
  static void sendNoteOn(uint8_t track, uint8_t pitch,
			    uint8_t velocity);

  static void sliceTrack32(uint8_t track, uint8_t from, uint8_t to);
  static void sliceTrack16(uint8_t track, uint8_t from, uint8_t to);
};

#define MD_ASSIGN_MACHINE_ID 0x5b
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS 0
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS 1
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS_ROUTING 2
#define MD_CURRENT_GLOBAL_SLOT_REQUEST 1
#define MD_CURRENT_KIT_REQUEST 2
#define MD_CURRENT_LOCK_MODE 0x20
#define MD_CURRENT_PATTERN_REQUEST 4
#define MD_CURRENT_SEQUENCER_MODE 0x10
#define MD_CURRENT_SONG_REQUEST 8
#define MD_GLOBAL_MESSAGE_ID 0x50
#define MD_GLOBAL_REQUEST_ID 0x51
#define MD_KIT_MESSAGE_ID 0x52
#define MD_KIT_REQUEST_ID 0x53
#define MD_LOAD_KIT_ID 0x58
#define MD_LOAD_PATTERN_ID 0x57
#define MD_LOAD_SONG_ID 0x6c
#define MD_PATTERN_MESSAGE_ID 0x67
#define MD_PATTERN_REQUEST_ID 0x68
#define MD_RESET_MIDI_NOTE_MAP_ID 0x64
#define MD_SAVE_KIT_ID 0x59
#define MD_SAVE_SONG_ID 0x6d
#define MD_SET_ACTIVE_GLOBAL_ID 0x56
#define MD_SET_CURRENT_KIT_NAME_ID 0x55
#define MD_SET_DYNAMIX_PARAM_ID 0x60
#define MD_SET_EQ_PARAM_ID 0x5f
#define MD_SET_GATE_BOX_PARAM_ID 0x5e
#define MD_SET_LFO_PARAM_ID 0x62
#define MD_SET_MIDI_NOTE_TO_TRACK_MAPPING_ID 0x5a
#define MD_SET_MUTE_GROUP_ID 0x66
#define MD_SET_RECEIVE_DUMP_POSITION_ID 0x6b
#define MD_SET_RHYTHM_ECHO_PARAM_ID 0x5d
#define MD_SET_STATUS_ID 0x71
#define MD_SET_TEMPO_ID 0x61
#define MD_SET_TRACK_ROUTING_ID 0x5c
#define MD_SET_TRIG_GROUP_ID 0x65
#define MD_SONG_MESSAGE_ID 0x69
#define MD_SONG_REQUEST_ID 0x6a
#define MD_STATUS_REQUEST_ID 0x70
#define MD_STATUS_RESPONSE_ID 0x72

typedef struct tuning_s {
  uint8_t model;
  uint8_t base;
  uint8_t len;
  const uint8_t *tuning;
} tuning_t;

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


#endif /* MD_H__ */
