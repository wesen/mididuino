#include "WProgram.h"
#include "MD.h"

uint8_t MD::baseChannel = 0;
uint8_t MD::trackModels[16] = { 0 };

uint8_t track_pitches[] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

uint8_t md_note_to_track(uint8_t pitch) {
  uint8_t i;
  for (i = 0; i < sizeof(track_pitches); i++) {
    if (pitch == track_pitches[i])
      return i;
  }
  return 128;
}

void md_parse_cc(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param) {
  if ((channel >= MD::baseChannel) && (channel < MD::baseChannel + 4)) {
    channel -= MD::baseChannel;
    *track = channel * 4;
    if (cc >= 96) {
      *track += 3;
      *param = cc - 96;
    } else if (cc >= 72) {
      *track += 2;
      *param = cc - 72;
    } else if (cc >= 40) {
      *track += 1;
      *param = cc - 40;
    } else {
      *param = cc - 16;
    }
  } else {
    *track = 255;
  }
}

void MD::triggerTrack(uint8_t track, uint8_t velocity) {
  MidiUart.sendNoteOn(MD::baseChannel, track_pitches[track], velocity);
}

void MD::setTrackParam(uint8_t track, uint8_t param, uint8_t value) {
  uint8_t channel = track >> 2;
  uint8_t b = track & 3;
  uint8_t cc = param;
  if (b < 2) {
    cc += 16 + b * 24;
  } else {
    cc += 24 + b * 24;
  }
  MidiUart.sendCC(channel + MD::baseChannel, cc, value);
}

void md_put_header(void) {
  uint8_t header[6] = {
    0xF0, 0x00, 0x20, 0x3C, 0x02, 0x00
  };
  MidiUart.sendRaw(header, 6);
}

//  0x5E, 0x5D, 0x5F, 0x60

void md_send_fx_param(uint8_t param, uint8_t value, uint8_t type) {
  md_put_header();
  uint8_t data[4] = {
    type, param, value, 0xF7
  };
  MidiUart.sendRaw(data, 4);
}
  
void MD::setEchoParam(uint8_t param, uint8_t value) {
  md_send_fx_param(param, value, 0x5D);
}

void MD::setReverbParam(uint8_t param, uint8_t value) {
  md_send_fx_param(param, value, 0x5E);
}

void MD::setEQParam(uint8_t param, uint8_t value) {
  md_send_fx_param(param, value, 0x5F);
}

void MD::setCompressorParam(uint8_t param, uint8_t value) {
  md_send_fx_param(param, value, 0x60);
}

/*** tunings ***/

static const uint8_t efm_rs_tuning[] = {
   1,  3, 6, 9, 11, 14, 17, 19, 22, 25, 27, 30, 33, 35, 38, 41, 43,
  46, 49, 51, 54, 57, 59, 62, 65, 67, 70, 73, 75, 78, 81, 83, 86,
  89, 91, 94, 97, 99, 102, 105, 107, 110, 113, 115, 118, 121, 123,
  126
};
static const uint8_t efm_hh_tuning[] = {
  1, 5, 9, 14, 18, 22, 27, 31, 35, 39, 44, 48, 52, 56, 61, 65, 69,
  73, 78, 82, 86, 91, 95, 99, 103, 108, 112, 116, 120, 125
};
static const uint8_t efm_cp_tuning[] = {
  0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 29, 31, 33,
  35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 62, 64, 66,
  68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 95, 97, 99, 101,
  103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127};
static const uint8_t efm_sd_tuning[] = {
  1, 5, 9, 14, 18, 22, 27, 31, 35, 39, 44, 48, 52, 56, 61, 65, 69, 73, 78, 82, 
86, 91, 95, 99, 103, 108, 112, 116, 120, 125, 
};
static const uint8_t efm_xt_tuning[] = {
  1, 7, 12, 17, 23, 28, 33, 39, 44, 49, 55, 60, 65, 71, 76, 81, 87, 92, 97, 102, 
  108, 113, 118, 124, 
};
static const uint8_t efm_bd_tuning[] = {
  1, 3, 6, 9, 11, 14, 17, 19, 22, 25, 27, 30, 33, 35, 38, 41, 43, 46, 49, 51, 54, 
  57, 59, 62, 65, 67, 70, 73, 75, 78, 81, 83, 86, 89, 91, 94, 97, 99, 102, 105, 107, 
  110, 113, 115, 118, 121, 123, 126, 
};
static const uint8_t trx_cl_tuning[] = {
  5, 11, 17, 23, 29, 36, 42, 48, 54, 60, 66, 72, 78, 84, 91, 97, 103, 109, 115, 121, 
  127, 
};
static const uint8_t trx_sd_tuning[] = {
  3, 13, 24, 35, 45, 56, 67, 77, 88, 98, 109, 120, 
};
static const uint8_t trx_xc_tuning[] = {
  1, 6, 11, 17, 22, 27, 33, 38, 43, 49, 54, 60, 65, 70, 76, 81, 86, 92, 97, 102, 
  108, 113, 118, 124, 
};
static const uint8_t trx_xt_tuning[] = {
  2, 7, 12, 18, 23, 28, 34, 39, 44, 49, 55, 60, 65, 71, 76, 81, 87, 92, 97, 103, 
  108, 113, 118, 124,
};
static const uint8_t trx_bd_tuning[] = {
  1, 7, 12, 17, 23, 28, 33, 39, 44, 49, 55, 60, 66, 71, 76, 82, 87, 92, 98, 103, 
  108, 114, 119, 124, 
};
static const uint8_t rom_tuning[] = {
  0, 2, 5, 7, 9, 12, 14, 16, 19, 21, 23, 26, 28, 31, 34, 37, 40, 43, 46, 49, 52, 
  55, 58, 61, 64, 67, 70, 73, 76, 79, 82, 85, 88, 91, 94, 97, 100, 102, 105, 107, 
  109, (112), 114, 116, 119, 121, 123, 125, 
};
static const tuning_t rom_tuning_t = { ROM_MODEL,    45,
				       sizeof(rom_tuning),    rom_tuning };
static const tuning_t tunings[] = {
  { EFM_RS_MODEL, 59, sizeof(efm_rs_tuning), efm_rs_tuning },
  { EFM_HH_MODEL, 59, sizeof(efm_hh_tuning), efm_hh_tuning },
  { EFM_CP_MODEL, 47, sizeof(efm_cp_tuning), efm_cp_tuning },
  { EFM_SD_MODEL, 47, sizeof(efm_sd_tuning), efm_sd_tuning },
  { EFM_XT_MODEL, 29, sizeof(efm_xt_tuning), efm_xt_tuning },
  { EFM_BD_MODEL, 20, sizeof(efm_bd_tuning), efm_bd_tuning },
  { TRX_CL_MODEL, 83, sizeof(trx_cl_tuning), trx_cl_tuning },
  { TRX_SD_MODEL, 53, sizeof(trx_sd_tuning), trx_sd_tuning },
  { TRX_XC_MODEL, 41, sizeof(trx_xc_tuning), trx_xc_tuning },
  { TRX_XT_MODEL, 38, sizeof(trx_xt_tuning), trx_xt_tuning },
  { TRX_BD_MODEL, 23, sizeof(trx_bd_tuning), trx_bd_tuning },
  { ROM_MODEL,    45, sizeof(rom_tuning),    rom_tuning    },
};

tuning_t const *track_tunings[16];

const tuning_t *model_get_tuning(uint8_t model) {
  uint8_t i;
  if (((model >= 128) && (model <= 159))) {
    return &rom_tuning_t;
  }
  for (i = 0; i < countof(tunings); i++) {
    if (model == tunings[i].model) {
      return tunings + i;
    }
  }

  return NULL;
}

uint8_t MD::trackGetPitch(uint8_t track, uint8_t pitch) {
  tuning_t const *tuning = model_get_tuning(trackModels[track]);
  
  if (tuning == NULL)
    return 128;

  if ((pitch < tuning->base) || (pitch >= (tuning->base + tuning->len))) {
    return 128;
  }

  return tuning->tuning[pitch - tuning->base];
}

void MD::sendNoteOn(uint8_t track, uint8_t pitch, uint8_t velocity) {
  uint8_t realPitch = MD::trackGetPitch(track, pitch);
  if (realPitch == 128)
    return;
  MD::setTrackParam(track, 0, realPitch);
  MD::triggerTrack(track, velocity);
}

void MD::sliceTrack32(uint8_t track, uint8_t from, uint8_t to) {
  MD::setTrackParam(track, 4, MIN(127, from * 4));
  MD::setTrackParam(track, 5, MIN(127, to * 4));
  MD::triggerTrack(track, 100);
}

void MD::sliceTrack16(uint8_t track, uint8_t from, uint8_t to) {
  MD::setTrackParam(track, 4, MIN(127, from * 8));
  MD::setTrackParam(track, 5, MIN(127, to * 8));
  MD::triggerTrack(track, 100);
}
