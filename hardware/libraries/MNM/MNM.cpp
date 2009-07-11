#include "MNM.h"

MNMClass::MNMClass() {
  global.baseChannel = 0;
  currentTrack = 0;
}

#ifndef HOST_MIDIDUINO
void MNMClass::sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity) {
  MidiUart.sendNoteOn(track + global.baseChannel, note, velocity);
}

void MNMClass::sendNoteOff(uint8_t track, uint8_t note) {
  MidiUart.sendNoteOff(track + global.baseChannel, note);
}

void MNMClass::sendMultiTrigNoteOn(uint8_t note, uint8_t velocity) {
  MidiUart.sendNoteOn(global.multitrigChannel, note, velocity);
}

void MNMClass::sendMultiTrigNoteOff(uint8_t note) {
  MidiUart.sendNoteOff(global.multitrigChannel, note);
}

void MNMClass::sendMultiMapNoteOn(uint8_t note, uint8_t velocity) {
  MidiUart.sendNoteOn(global.multimapChannel, note, velocity);
}

void MNMClass::sendMultiMapNoteOff(uint8_t note) {
  MidiUart.sendNoteOff(global.multimapChannel, note);
}

void MNMClass::sendAutoNoteOn(uint8_t note, uint8_t velocity) {
  MidiUart.sendNoteOn(global.autotrackChannel, note, velocity);
}

void MNMClass::sendAutoNoteOff(uint8_t note) {
  MidiUart.sendNoteOff(global.autotrackChannel, note);
}

void MNMClass::triggerTrack(uint8_t track, bool amp, bool lfo, bool filter) {
  MidiUart.sendNRPN(global.baseChannel,
		    (uint16_t)(0x7F << 7),
		    (uint8_t)((track << 3) | (amp ? 4 : 0) | (lfo ? 2 : 0) | (filter ? 1 : 0)));
}

void MNMClass::setMultiEnvParam(uint8_t param, uint8_t value) {
  MidiUart.sendNRPN(global.baseChannel, 0x40 + param, value);
}

void MNMClass::setMute(uint8_t track, bool mute) {
  MidiUart.sendCC(track + global.baseChannel, 3, mute ? 0 : 1);
}

void MNMClass::setAutoMute(bool mute) {
  MidiUart.sendCC(global.autotrackChannel, 3, mute ? 0 : 1);
}

void MNMClass::setMidiParam(uint8_t track, uint8_t param, uint8_t value) {
  MidiUart.sendNRPN(global.baseChannel, (track << 7) | (0x38 + param), value);
}

void MNMClass::setTrackPitch(uint8_t track, uint8_t pitch) {
  MidiUart.sendNRPN(global.baseChannel, (112 + track) << 7, pitch);
}

void MNMClass::setLevel(uint8_t track, uint8_t level) {
  MidiUart.sendCC(global.baseChannel + track, 7, level);
}

void MNMClass::setAutoParam(uint8_t param, uint8_t value) {
  if (param < 0x30) {
    MidiUart.sendCC(global.autotrackChannel, param + 0x30, value);
  } else {
    MidiUart.sendCC(global.autotrackChannel, param + 0x38, value);
  }
}

void MNMClass::setAutoLevel(uint8_t level) {
  MidiUart.sendCC(global.autotrackChannel, 7, level);
}

void MNMClass::setParam(uint8_t track, uint8_t param, uint8_t value) {
  if (param < 0x30) {
    MidiUart.sendCC(global.baseChannel + track, param + 0x30, value);
  } else {
    MidiUart.sendCC(global.baseChannel + track, param + 0x38, value);
  }
}

bool MNMClass::parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param) {
  if ((channel >= global.baseChannel) && (channel < (global.baseChannel + 6))) {
    *track = channel - global.baseChannel;
    if ((cc >= 0x30) && (cc <= 0x5f)) {
      *param = cc - 0x30;
      return true;
    } else if ((cc >= 0x38) && (cc <= 0x77)) {
      *param = cc - 0x38;
      return true;
    }
  }

  return false;
}

void MNMClass::setStatus(uint8_t id, uint8_t value) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  MidiUart.putc(0x71);
  MidiUart.putc(id & 0x7F);
  MidiUart.putc(value & 0x7F);
  MidiUart.putc(0xF7);
}

void MNMClass::loadGlobal(uint8_t id) {
  setStatus(1, id);
}

void MNMClass::loadKit(uint8_t id) {
  setStatus(2, id);
}

void MNMClass::loadPattern(uint8_t id) {
  setStatus(4, id);
}

void MNMClass::loadSong(uint8_t id) {
  setStatus(8, id);
}

void MNMClass::setSequencerMode(bool songMode) {
  setStatus(0x10, songMode ? 1 : 0);
}

void MNMClass::setAudioMode(bool polyMode) {
  setStatus(0x20, polyMode ? 1 : 0);
}

void MNMClass::setSequencerModeMode(bool midiMode) {
  setStatus(0x21, midiMode ? 1 : 0);
}

void MNMClass::setAudioTrack(uint8_t track) {
  setStatus(0x22, track);
}

void MNMClass::setMidiTrack(uint8_t track) {
  setStatus(0x23, track);
}

void MNMClass::setCurrentKitName(char *name) {
}

void MNMClass::saveCurrentKit(uint8_t id) {
}

void MNMClass::sendRequest(uint8_t type, uint8_t param) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  MidiUart.putc(type);
  MidiUart.putc(param);
  MidiUart.putc(0xF7);
}

void MNMClass::requestKit(uint8_t _kit) {
  sendRequest(MNM_KIT_REQUEST_ID, _kit);
}

void MNMClass::requestPattern(uint8_t _pattern) {
  sendRequest(MNM_PATTERN_REQUEST_ID, _pattern);
}

void MNMClass::requestSong(uint8_t _song) {
  sendRequest(MNM_SONG_REQUEST_ID, _song);
}

void MNMClass::requestGlobal(uint8_t _global) {
  sendRequest(MNM_GLOBAL_REQUEST_ID, _global);
}

void MNMClass::assignMachine(uint8_t track, uint8_t model, bool initAll, bool initSynth) {
  MidiUart.putc(0xF0);
  MidiUart.sendRaw(monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  MidiUart.putc(MNM_LOAD_GLOBAL_ID);
  MidiUart.putc(track);
  MidiUart.putc(model);
  if (initAll) {
    MidiUart.putc(0x01);
  } else if (initSynth) {
    MidiUart.putc(0x02);
  } else {
    MidiUart.putc(0x00);
  }
  MidiUart.putc(0xF7);
}

void MNMClass::setMachine(uint8_t track, MNMMachine *machine) {
  assignMachine(track, machine->model);
  for (int i = 0; i < 72; i++) {
    setParam(track, i, machine->params[i]);
  }
  setLevel(track, machine->level);
}

MNMEncoder::MNMEncoder(uint8_t _track, uint8_t _param, char *_name, uint8_t init) :
  CCEncoder(0, 0, _name, init) {
  initMNMEncoder(_track, _param);
  handler = CCEncoderHandle;
}

uint8_t MNMEncoder::getCC() {
  if (param < 0x30) {
    return 0x30 + param;
  } else {
    return 0x38 + param;
  }
}

uint8_t MNMEncoder::getChannel() {
  return MNM.global.baseChannel + track;
}

void MNMEncoder::initCCEncoder(uint8_t _channel, uint8_t _cc) {
  if (MNM.parseCC(_channel, _cc, &track, &param)) {
    if (MNM.loadedKit) {
      PGM_P name= NULL;
      name = MNM.getModelParamName(MNM.kit.machines[track].model, param);
      if (name != NULL) {
	char myName[4];
	m_strncpy_p(myName, name, 4);
	setName(myName);
	GUI.redisplay();
      }
    }
  }
}

void MNMEncoder::loadFromKit() {
  setValue(MNM.kit.machines[track].params[param]);
}

#endif /* HOST_MIDIDUINO */

MNMClass MNM;
