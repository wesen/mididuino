#include "WProgram.h"
#include "MD.h"

typedef struct machine_name_s {
  char name[7];
  uint8_t id;
} machine_name_t;

machine_name_t machine_names[] PROGMEM = {
  { "GND---", 0},
  { "GND-SN", 1}, { "GND-NS", 2},
  { "GND-IM", 3}, { "TRX-BD", 16},
  { "TRX-SD", 17}, { "TRX-XT", 18},
  { "TRX-CP", 19}, { "TRX-RS", 20},
  { "TRX-RS", 21}, { "TRX-CH", 22},
  { "TRX-OH", 23}, { "TRX-CY", 24},
  { "TRX-MA", 25}, { "TRX-CL", 26},
  { "TRX-XC", 27}, { "TRX-B2", 28},
  { "EFM-BD", 32}, { "EFM-SD", 33},
  { "EFM-XT", 34}, { "EFM-CP", 35},
  { "EFM-RS", 36}, { "EFM-CB", 37},
  { "EFM-HH", 38}, { "EFM-CY", 39},
  { "E12-BD", 48}, { "E12-SD", 49},
  { "E12-HT", 50}, { "E12-LT", 51},
  { "E12-CP", 52}, { "E12-RS", 53},
  { "E12-CB", 54}, { "E12-CH", 55},
  { "E12-OH", 56}, { "E12-RC", 57},
  { "E12-CC", 58}, { "E12-BR", 59},
  { "E12-TA", 60}, { "E12-TR", 61},
  { "E12-SH", 62}, { "E12-BC", 63},
  { "P-I-BD", 64}, { "P-I-SD", 65},
  { "P-I-MT", 66}, { "P-I-ML", 67},
  { "P-I-MA", 68}, { "P-I-RS", 69},
  { "P-I-RC", 70}, { "P-I-CC", 71},
  { "P-I-HH", 72}, { "INP-GA", 80},
  { "INP-GB", 81}, { "INP-FA", 82},
  { "INP-FB", 83}, { "INP-EA", 84},
  { "INP-EB", 85}, { "MID-01", 96},
  { "MID-02", 97}, { "MID-03", 98},
  { "MID-04", 99}, { "MID-05", 100},
  { "MID-06", 101}, { "MID-07", 102},
  { "MID-08", 103}, { "MID-09", 104},
  { "MID-10", 105}, { "MID-11", 106},
  { "MID-12", 107}, { "MID-13", 108},
  { "MID-14", 109}, { "MID-15", 110},
  { "MID-16", 111}, { "CTR-AL", 112},
  { "CTR-8P", 113},
			
  { "CTR-RE", 120},
  { "CTR-GB", 121},
  { "CTR-EQ", 122},
  { "CTR-DX", 123},
			
  { "ROM-01", 128},
  { "ROM-02", 129}, { "ROM-03", 130},
  { "ROM-04", 131}, { "ROM-05", 132},
  { "ROM-06", 133}, { "ROM-07", 134},
  { "ROM-08", 135}, { "ROM-09", 136},
  { "ROM-10", 137}, { "ROM-11", 138},
  { "ROM-12", 139}, { "ROM-13", 140},
  { "ROM-14", 141}, { "ROM-15", 142},
  { "ROM-16", 143}, { "ROM-17", 144},
  { "ROM-18", 145}, { "ROM-19", 146},
  { "ROM-20", 147}, { "ROM-21", 148},
  { "ROM-22", 149}, { "ROM-23", 150},
  { "ROM-24", 151}, { "ROM-25", 152},
  { "ROM-26", 153}, { "ROM-27", 154},
  { "ROM-28", 155}, { "ROM-29", 156},
  { "ROM-30", 157}, { "ROM-31", 158},
  { "ROM-32", 159}, { "RAM-R1", 160},
  { "RAM-R2", 161}, { "RAM-P1", 162},
  { "RAM-P2", 163},
			
  { "RAM-R3", 165},
  { "RAM-R4", 166},
  { "RAM-P3", 167},
  { "RAM-P4", 168},

  { "ROM-33", 176},
  { "ROM-34", 177},
  { "ROM-35", 178},
  { "ROM-36", 179},
  { "ROM-37", 180},
  { "ROM-38", 181},
  { "ROM-39", 182},
  { "ROM-40", 183},
  { "ROM-41", 184},
  { "ROM-42", 185},
  { "ROM-43", 186},
  { "ROM-44", 187},
  { "ROM-45", 188},
  { "ROM-46", 189},
  { "ROM-47", 190},
  { "ROM-48", 191}
};

PGM_P getMachineName(uint8_t machine) {
  for (uint8_t i = 0; i < countof(machine_names); i++) {
    if (pgm_read_byte(&machine_names[i].id) == machine) {
      return machine_names[i].name;
    }
  }
  return NULL;
}

uint8_t machinedrum_sysex_hdr[5] = {
  0x00,
  0x20,
  0x3c,
  0x02,
  0x00
};

uint8_t track_pitches[] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

uint8_t MDClass::noteToTrack(uint8_t pitch) {
  uint8_t i;
  for (i = 0; i < sizeof(track_pitches); i++) {
    if (pitch == track_pitches[i])
      return i;
  }
  return 128;
}

void MDClass::parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param) {
  if ((channel >= baseChannel) && (channel < baseChannel + 4)) {
    channel -= baseChannel;
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

void MDClass::sendRequest(uint8_t type, uint8_t param) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  MidiUart.putc(type);
  MidiUart.putc(param);
  MidiUart.putc(0xF7);
}

void MDClass::triggerTrack(uint8_t track, uint8_t velocity) {
  MidiUart.sendNoteOn(baseChannel, track_pitches[track], velocity);
}

void MDClass::setTrackParam(uint8_t track, uint8_t param, uint8_t value) {
  uint8_t channel = track >> 2;
  uint8_t b = track & 3;
  uint8_t cc = param;
  if (b < 2) {
    cc += 16 + b * 24;
  } else {
    cc += 24 + b * 24;
  }
  MidiUart.sendCC(channel + baseChannel, cc, value);
}

//  0x5E, 0x5D, 0x5F, 0x60

void MDClass::sendFXParam(uint8_t param, uint8_t value, uint8_t type) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  uint8_t data[4] = {
    type, param, value, 0xF7
  };
  MidiUart.sendRaw(data, 4);
}
  
void MDClass::setEchoParam(uint8_t param, uint8_t value) {
  sendFXParam(param, value, 0x5D);
}

void MDClass::setReverbParam(uint8_t param, uint8_t value) {
  sendFXParam(param, value, 0x5E);
}

void MDClass::setEQParam(uint8_t param, uint8_t value) {
  sendFXParam(param, value, 0x5F);
}

void MDClass::setCompressorParam(uint8_t param, uint8_t value) {
  sendFXParam(param, value, 0x60);
}

/*** tunings ***/

/* XXX convert to PGM_P */
static const uint8_t efm_rs_tuning[] PROGMEM = {
   1,  3, 6, 9, 11, 14, 17, 19, 22, 25, 27, 30, 33, 35, 38, 41, 43,
  46, 49, 51, 54, 57, 59, 62, 65, 67, 70, 73, 75, 78, 81, 83, 86,
  89, 91, 94, 97, 99, 102, 105, 107, 110, 113, 115, 118, 121, 123,
  126
};
static const uint8_t efm_hh_tuning[] PROGMEM = {
  1, 5, 9, 14, 18, 22, 27, 31, 35, 39, 44, 48, 52, 56, 61, 65, 69,
  73, 78, 82, 86, 91, 95, 99, 103, 108, 112, 116, 120, 125
};
static const uint8_t efm_cp_tuning[] PROGMEM = {
  0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 29, 31, 33,
  35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 62, 64, 66,
  68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 95, 97, 99, 101,
  103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127};

static const uint8_t efm_sd_tuning[] PROGMEM = {
  1, 5, 9, 14, 18, 22, 27, 31, 35, 39, 44, 48, 52, 56, 61, 65, 69, 73, 78, 82, 
86, 91, 95, 99, 103, 108, 112, 116, 120, 125, 
};

static const uint8_t efm_xt_tuning[] PROGMEM = {
  1, 7, 12, 17, 23, 28, 33, 39, 44, 49, 55, 60, 65, 71, 76, 81, 87, 92, 97, 102, 
  108, 113, 118, 124, 
};

static const uint8_t efm_bd_tuning[] PROGMEM = {
  1, 3, 6, 9, 11, 14, 17, 19, 22, 25, 27, 30, 33, 35, 38, 41, 43, 46, 49, 51, 54, 
  57, 59, 62, 65, 67, 70, 73, 75, 78, 81, 83, 86, 89, 91, 94, 97, 99, 102, 105, 107, 
  110, 113, 115, 118, 121, 123, 126, 
};
static const uint8_t trx_cl_tuning[] PROGMEM = {
  5, 11, 17, 23, 29, 36, 42, 48, 54, 60, 66, 72, 78, 84, 91, 97, 103, 109, 115, 121, 
  127, 
};
static const uint8_t trx_sd_tuning[] PROGMEM = {
  3, 13, 24, 35, 45, 56, 67, 77, 88, 98, 109, 120, 
};
static const uint8_t trx_xc_tuning[] PROGMEM = {
  1, 6, 11, 17, 22, 27, 33, 38, 43, 49, 54, 60, 65, 70, 76, 81, 86, 92, 97, 102, 
  108, 113, 118, 124, 
};
static const uint8_t trx_xt_tuning[] PROGMEM = {
  2, 7, 12, 18, 23, 28, 34, 39, 44, 49, 55, 60, 65, 71, 76, 81, 87, 92, 97, 103, 
  108, 113, 118, 124,
};
static const uint8_t trx_bd_tuning[] PROGMEM = {
  1, 7, 12, 17, 23, 28, 33, 39, 44, 49, 55, 60, 66, 71, 76, 82, 87, 92, 98, 103, 
  108, 114, 119, 124, 
};
static const uint8_t rom_tuning[] PROGMEM = {
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

const tuning_t PROGMEM *MDClass::getModelTuning(uint8_t model) {
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

uint8_t MDClass::trackGetPitch(uint8_t track, uint8_t pitch) {
  tuning_t const *tuning = getModelTuning(trackModels[track]);
  
  if (tuning == NULL)
    return 128;

  uint8_t base = pgm_read_byte(&tuning->base);
  uint8_t len = pgm_read_byte(&tuning->len);
  
  if ((pitch < base) || (pitch >= (base + len))) {
    return 128;
  }

  return pgm_read_byte(&tuning->tuning[pitch - base]);
}

void MDClass::sendNoteOn(uint8_t track, uint8_t pitch, uint8_t velocity) {
  uint8_t realPitch = trackGetPitch(track, pitch);
  if (realPitch == 128)
    return;
  setTrackParam(track, 0, realPitch);
  triggerTrack(track, velocity);
}

void MDClass::sliceTrack32(uint8_t track, uint8_t from, uint8_t to) {
  setTrackParam(track, 4, MIN(127, from * 4));
  setTrackParam(track, 5, MIN(127, to * 4));
  triggerTrack(track, 100);
}

void MDClass::sliceTrack16(uint8_t track, uint8_t from, uint8_t to) {
  setTrackParam(track, 4, MIN(127, from * 8));
  setTrackParam(track, 5, MIN(127, to * 8));
  triggerTrack(track, 100);
}

bool MDClass::isMelodicTrack(uint8_t track) {
  return (getModelTuning(trackModels[track]) != NULL);
}

MDClass MD;
