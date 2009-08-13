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
  GUI.put_value(i, getValue()+1);
  redisplay = false;
  GUI.setLine(GUI.LINE2);
  GUI.flash_put_value(0, track + 1);
  GUI.flash_p_string_at_fill(4, MD.getMachineName(MD.kit.machines[track].model));
}

void MDMelodicTrackFlashEncoder::displayAt(int i) {
    uint8_t track = getValue();
    GUI.put_value(i, getValue()+1);
    redisplay = false;
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, track + 1);
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

bool MDClass::checkParamSettings() {
  if (loadedGlobal) {
    return (MD.global.baseChannel >= 0) && (MD.global.baseChannel <= 12);
  } else {
    return false;
  }
}

bool MDClass::checkTriggerSettings() {
  return false;
}

bool MDClass::checkClockSettings() {
  return false;
}

MDClass MD;
