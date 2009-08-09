#include "WProgram.h"
#include "MD.h"
#include "helpers.h"

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

#ifdef MIDIDUINO_USE_GUI

void MDEncoderHandle(Encoder *enc) {
  MDEncoder *mdEnc = (MDEncoder *)enc;
  MD.setTrackParam(mdEnc->track, mdEnc->param, mdEnc->getValue());
}

void MDFXEncoderHandle(Encoder *enc) {
  MDFXEncoder *mdEnc = (MDFXEncoder *)enc;
  MD.sendFXParam(mdEnc->param, mdEnc->getValue(), mdEnc->effect);
}

MDEncoder::MDEncoder(uint8_t _track, uint8_t _param, char *_name, uint8_t init) :
  CCEncoder(0, 0, _name, init) {
  initMDEncoder(_track, _param);
  handler = CCEncoderHandle;
  //  handler = MDEncoderHandle;
}

uint8_t MDEncoder::getCC() {
  uint8_t b = track & 3;
  uint8_t cc = param;
  if (param == 32) {
    cc = b + 12;
  } else if (param == 33) {
    cc = b + 8;
  } else if (b < 2) {
    cc += 16 + b * 24;
  } else {
    cc += 24 + b * 24;
  }
  return cc;
}

uint8_t MDEncoder::getChannel() {
  if (MD.loadedGlobal == false || MD.global.baseChannel == 127)
    return 127;
  
  uint8_t channel = track >> 2;
  return MD.global.baseChannel + channel;
}

void MDEncoder::initCCEncoder(uint8_t _channel, uint8_t _cc) {
  MD.parseCC(_channel, _cc, &track, &param);
  if (MD.loadedKit && (track != 255)) {
    PGM_P name = NULL;
    name = model_param_name(MD.kit.machines[track].model, param);
    if (name != NULL) {
      char myName[4];
      m_strncpy_p(myName, name, 4);
      setName(myName);
      GUI.redisplay();
    } else {
      setName("XXX");
      GUI.redisplay();
    }
  }
}

void MDEncoder::loadFromKit() {
  setValue(MD.kit.machines[track].params[param]);
}

MDFXEncoder::MDFXEncoder(uint8_t _param, uint8_t _effect, char *_name, uint8_t init) :
  RangeEncoder(127, 0, _name, init) {
  initMDFXEncoder(_effect, _param);
  handler = MDFXEncoderHandle;
}

void MDFXEncoder::loadFromKit() {
  switch (effect) {
  case MD_FX_ECHO:
    setValue(MD.kit.delay[param]);
    break;

  case MD_FX_REV:
    setValue(MD.kit.reverb[param]);
    break;

  case MD_FX_EQ:
    setValue(MD.kit.eq[param]);
    break;

  case MD_FX_DYN:
    setValue(MD.kit.dynamics[param]);
    break;
  }
}

void MDTrackFlashEncoder::displayAt(int i) {
  uint8_t track = getValue();
  RangeEncoder::displayAt(i);
  GUI.setLine(GUI.LINE2);
  GUI.flash_put_value(0, track);
  GUI.flash_p_string_at_fill(4, MD.getMachineName(MD.kit.machines[track].model));
}

void MDMelodicTrackFlashEncoder::displayAt(int i) {
    uint8_t track = getValue();
    RangeEncoder::displayAt(i);
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, track);
    if (MD.isMelodicTrack(track)) {
      GUI.flash_p_string_at_fill(4, MD.getMachineName(MD.kit.machines[track].model));
    } else {
      GUI.flash_p_string_at_fill(4, PSTR("XXX"));
    }
}

#endif

uint8_t standardDrumMapping[16] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

MDClass::MDClass()  {
  currentGlobal = -1;
  currentKit = -1;
  currentPattern = -1;
  global.baseChannel = 0;
  for (int i = 0; i < 16; i++) {
    global.drumMapping[i] = standardDrumMapping[i];
  }
  loadedKit = loadedGlobal = false;
}

void MDClass::parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param) {
  if ((channel >= global.baseChannel) && (channel < (global.baseChannel + 4))) {
    channel -= global.baseChannel;
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
    } else if (cc >= 16) {
      *param = cc - 16;
    } else if (cc >= 12) {
      *track = (cc - 12);
      *param = 32; // MUTE
    } else if (cc >= 8) {
      *track = (cc - 8);
      *param = 33; // LEV
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
  if (global.drumMapping[track] != -1 && global.baseChannel != 127) {
    MidiUart.sendNoteOn(global.baseChannel, global.drumMapping[track], velocity);
  }
}

void MDClass::setTrackParam(uint8_t track, uint8_t param, uint8_t value) {
  if (global.baseChannel == 127)
    return;

  uint8_t channel = track >> 2;
  uint8_t b = track & 3;
  uint8_t cc = 0;
  if (param == 32) { // MUTE
    cc = 12 + b;
  } else if (param == 33) { // LEV
    cc = 8 + b;
  } else {
    cc = param;
    if (b < 2) {
      cc += 16 + b * 24;
    } else {
      cc += 24 + b * 24;
    }
  }
  MidiUart.sendCC(channel + global.baseChannel, cc, value);
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
				       sizeof(rom_tuning), 4,   rom_tuning };
static const tuning_t tunings[] = {
  { EFM_RS_MODEL, 59, sizeof(efm_rs_tuning), 4, efm_rs_tuning },
  { EFM_HH_MODEL, 59, sizeof(efm_hh_tuning), 8, efm_hh_tuning },
  { EFM_CP_MODEL, 47, sizeof(efm_cp_tuning), 3, efm_cp_tuning },
  { EFM_SD_MODEL, 47, sizeof(efm_sd_tuning), 5, efm_sd_tuning },
  { EFM_XT_MODEL, 29, sizeof(efm_xt_tuning), 7, efm_xt_tuning },
  { EFM_BD_MODEL, 20, sizeof(efm_bd_tuning), 4, efm_bd_tuning },
  { TRX_CL_MODEL, 83, sizeof(trx_cl_tuning), 7, trx_cl_tuning },
  { TRX_SD_MODEL, 53, sizeof(trx_sd_tuning), 12, trx_sd_tuning },
  { TRX_XC_MODEL, 41, sizeof(trx_xc_tuning), 6, trx_xc_tuning },
  { TRX_XT_MODEL, 38, sizeof(trx_xt_tuning), 6, trx_xt_tuning },
  { TRX_BD_MODEL, 23, sizeof(trx_bd_tuning), 7, trx_bd_tuning },
  { ROM_MODEL,    45, sizeof(rom_tuning),    4, rom_tuning    },
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

uint8_t MDClass::trackGetCCPitch(uint8_t track, uint8_t cc, int8_t *offset) {
  tuning_t const *tuning = getModelTuning(kit.machines[track].model);
  
  if (tuning == NULL)
    return 128;

  uint8_t i;
  int8_t off = 0;
  for (i = 0; i < tuning->len; i++) {
    uint8_t ccStored = pgm_read_byte(&tuning->tuning[i]);
    off = ccStored - cc;
    if (ccStored >= cc) {
      if (offset != NULL) {
	*offset = off;
      }
      if (off <= tuning->offset)
	return i + tuning->base;
      else 
	return 128;
    }
  }
  off = ABS(pgm_read_byte(&tuning->tuning[tuning->len - 1]) - cc);
  if (offset != NULL)
    *offset = off;
  if (off <= tuning->offset)
    return i + tuning->base;
  else
    return 128;
}

uint8_t MDClass::trackGetPitch(uint8_t track, uint8_t pitch) {
  tuning_t const *tuning = getModelTuning(kit.machines[track].model);
  
  if (tuning == NULL)
    return 128;

  uint8_t base = tuning->base;
  uint8_t len = tuning->len;
  
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
  //  setTrackParam(track, 0, realPitch);
  //  delay(20);
  triggerTrack(track, velocity);
  //  delay(20);
  //  setTrackParam(track, 0, realPitch - 10);
  //  triggerTrack(track, velocity);
}

void MDClass::sliceTrack32(uint8_t track, uint8_t from, uint8_t to, bool correct) {
  uint8_t pfrom, pto;
  if (from > to) {
    pfrom = MIN(127, from * 4 + 1);
    pto = MIN(127, to * 4);
  } else {
    pfrom = MIN(127, from * 4);
    pto = MIN(127, to * 4);
    if (correct && pfrom >= 64)
      pfrom++;
  }
  setTrackParam(track, 4, pfrom);
  setTrackParam(track, 5, pto);
  triggerTrack(track, 127);
}

void MDClass::sliceTrack16(uint8_t track, uint8_t from, uint8_t to) {
  if (from > to) {
    setTrackParam(track, 4, MIN(127, from * 8 + 1));
    setTrackParam(track, 5, MIN(127, to * 8));
  } else {
    setTrackParam(track, 4, MIN(127, from * 8));
    setTrackParam(track, 5, MIN(127, to * 8));
  }
  triggerTrack(track, 100);
}

bool MDClass::isMelodicTrack(uint8_t track) {
  return (getModelTuning(kit.machines[track].model) != NULL);
}

void MDClass::setLFOParam(uint8_t track, uint8_t param, uint8_t value) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  MidiUart.putc(0x62);
  MidiUart.putc(track << 3 | param);
  MidiUart.putc(value);
  MidiUart.putc(0xF7);
}

void MDClass::setLFO(uint8_t track, MDLFO *lfo) {
  setLFOParam(track, 0, lfo->destinationTrack);
  setLFOParam(track, 1, lfo->destinationParam);
  setLFOParam(track, 2, lfo->shape1);
  setLFOParam(track, 3, lfo->shape2);
  setLFOParam(track, 4, lfo->type);
  setLFOParam(track, 5, lfo->speed);
  setLFOParam(track, 6, lfo->depth);
  setLFOParam(track, 7, lfo->mix);
}

void MDClass::saveCurrentKit(uint8_t pos) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  MidiUart.putc(0x59);
  MidiUart.putc(pos & 0x7F);
  MidiUart.putc(0xF7);
}

void MDClass::assignMachine(uint8_t track, uint8_t model) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  MidiUart.putc(0x5B);
  MidiUart.putc(track);
  if (model > 128) {
    MidiUart.putc(model - 128);
    MidiUart.putc(0x01);
  } else {
    MidiUart.putc(model);
    MidiUart.putc(0x00);
  }
  MidiUart.putc(0x00);
  MidiUart.putc(0xF7);
}

void MDClass::setMachine(uint8_t track, MDMachine *machine) {
  assignMachine(track, machine->model);
  for (uint8_t i = 0; i < 24; i++) {
    setTrackParam(track, i, machine->params[i]);
  }
  setLFO(track, &machine->lfo);
}

void MDClass::muteTrack(uint8_t track, bool mute) {
  if (global.baseChannel == 127)
    return;
      
  uint8_t channel = track >> 2;
  uint8_t b = track & 3;
  uint8_t cc = 16 + b;
  MidiUart.sendCC(channel + global.baseChannel, cc, mute ? 1 : 0);
}

void MDClass::setStatus(uint8_t id, uint8_t value) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));
  MidiUart.putc(0x71);
  MidiUart.putc(id & 0x7F);
  MidiUart.putc(value & 0x7F);
  MidiUart.putc(0xf7);
}

void MDClass::loadGlobal(uint8_t id) {
  setStatus(1, id);
}

void MDClass::loadKit(uint8_t kit) {
  setStatus(2, kit);
}

void MDClass::loadPattern(uint8_t pattern) {
  setStatus(4, pattern);
}

void MDClass::loadSong(uint8_t song) {
  setStatus(8, song);
}


void MDClass::setSequencerMode(uint8_t mode) {
  setStatus(16, mode);
}

void MDClass::setLockMode(uint8_t mode) {
  setStatus(32, mode);
}

void MDClass::getPatternName(uint8_t pattern, char str[5]) {
  uint8_t bank = pattern / 16;
  uint8_t num = pattern % 16 + 1;
  str[0] = 'A' + bank;
  str[1] = '0' + (num / 10);
  str[2] = '0' + (num % 10);
  str[3] = ' ';
  str[4] = 0;
}

void MDClass::requestKit(uint8_t kit) {
  sendRequest(MD_KIT_REQUEST_ID, kit);
}

void MDClass::requestPattern(uint8_t pattern) {
  sendRequest(MD_PATTERN_REQUEST_ID, pattern);
}

void MDClass::requestSong(uint8_t song) {
  sendRequest(MD_SONG_REQUEST_ID, song);
}

void MDClass::requestGlobal(uint8_t global) {
  sendRequest(MD_GLOBAL_REQUEST_ID, global);
}

MDClass MD;
