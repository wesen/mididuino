#include "MNMMessages.hh"
#include "MNMParams.hh"
#include "helpers.h"
#include "Elektron.hh"

#ifdef HOST_MIDIDUINO
#include <stdio.h>
#endif

bool MNMGlobal::fromSysex(uint8_t *data, uint16_t len) {
	if (!ElektronHelper::checkSysexChecksum(data, len)) {
		return false;
	}

	origPosition = data[3];
	MNMSysexDecoder decoder(DATA_ENCODER_INIT(data + 4, len - 4));

	decoder.get(&autotrackChannel, 5);
	/*
		autotrackChannel = udata[1];
		baseChannel = udata[2];
		channelSpan = udata[3];
		multitrigChannel = udata[4];
		multimapChannel = udata[5];
	*/

	uint8_t byte = 0;
	decoder.get8(&byte);
	
  clockIn = IS_BIT_SET(byte, 0);
  clockOut = IS_BIT_SET(byte, 5);
  ctrlIn = IS_BIT_SET(byte, 4);
  ctrlOut = IS_BIT_SET(byte, 6);

	decoder.getb(&transportIn);
	decoder.getb(&sequencerOut);
	decoder.getb(&arpOut);
	decoder.getb(&keyboardOut);
	decoder.getb(&transportOut);
	decoder.getb(&midiClockOut);
	decoder.getb(&pgmChangeOut);
	
	decoder.get(&note, 5); // note, gate, sense, minvel, maxvel (not used)
	
	decoder.get(midiMachineChannels, 6);
	decoder.get((uint8_t *)ccDestinations, 6 * 4);
	decoder.get(midiSeqLegato, 6);
	decoder.get(legato, 6);

	decoder.get(mapRange, 32 * 6);
	decoder.get8(&globalRouting);
	decoder.getb(&pgmChangeIn);
	decoder.get(unused, 5);
	decoder.get32(&baseFreq);

  return true;
}

uint16_t MNMGlobal::toSysex(uint8_t *data, uint16_t len) {
	MNMDataToSysexEncoder encoder(DATA_ENCODER_INIT(data, len));
	return toSysex(encoder);
}

uint16_t MNMGlobal::toSysex(MNMDataToSysexEncoder &encoder) {
	encoder.stop7Bit();
	encoder.pack8(0xF0);
	encoder.pack(monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
	encoder.pack8(MNM_GLOBAL_MESSAGE_ID);
	encoder.pack8(0x02); // version
	encoder.pack8(0x01); // revision

	encoder.startChecksum();
	encoder.pack8(origPosition);
	encoder.start7Bit();

	encoder.pack(&autotrackChannel, 5);

	uint8_t byte = 0;
	if (clockIn)
		SET_BIT(byte, 0);
	if (clockOut)
		SET_BIT(byte, 5);
	if (ctrlIn)
		SET_BIT(byte, 4);
	if (ctrlOut)
		SET_BIT(byte, 6);
	encoder.pack8(byte);

	encoder.packb(transportIn);
	encoder.packb(sequencerOut);
	encoder.packb(arpOut);
	encoder.packb(keyboardOut);
	encoder.packb(transportOut);
	encoder.packb(midiClockOut);
	encoder.packb(pgmChangeOut);

	encoder.pack(&note, 5);

	encoder.pack(midiMachineChannels, 6);
	encoder.pack((uint8_t *)ccDestinations, 6 * 4);
	encoder.pack(midiSeqLegato, 6);
	encoder.pack(legato, 6);

	encoder.pack(mapRange, 32 * 6);
	encoder.pack8(globalRouting);
	encoder.packb(pgmChangeIn);
	encoder.pack(unused, 5);
	encoder.pack32(baseFreq);

	uint16_t enclen = encoder.finish();
	encoder.finishChecksum();

	return enclen + 5;
}

bool MNMKit::fromSysex(uint8_t *data, uint16_t len) {
  if (len < 698) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong len, %d should be %d bytes\n", len, 698 + 8);
#endif
    return false;
  }

  uint8_t *udata = data + 3;
  origPosition = udata[0];
  m_memcpy(name, (char *)(udata + 1), 16);

  for (int i = 0; i < 6; i++) {
    machines[i].level = udata[0xC + i];
    for (int j = 0; j < 72; j++) {
      machines[i].params[j] = udata[0x12 + i * 72 + j];
    }
    machines[i].model = udata[0x1C2 + i];
    machines[i].type = udata[0x1C8 + i];
  }

  patchBusIn = ElektronHelper::to16Bit(udata + 0x1ce);
  for (int i = 0; i < 6; i++) {
    machines[i].modifier.mirrorLR = IS_BIT_SET(udata[0x1d1], i);
    machines[i].modifier.mirrorUD = IS_BIT_SET(udata[0x1d2], i);
    machines[i].modifier.LPKeyTrack = IS_BIT_SET(udata[0x2ab], i);
    machines[i].modifier.HPKeyTrack = IS_BIT_SET(udata[0x2ac], i);
  }

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      machines[i].modifier.destPage[j][0] = udata[0x1d3 + i * 12 + j * 2];
      machines[i].modifier.destPage[j][1] = udata[0x1d3 + i * 12 + j * 2 + 1];
      machines[i].modifier.destParam[j][0] = udata[0x21b + i * 12 + j * 2];
      machines[i].modifier.destParam[j][1] = udata[0x21b + i * 12 + j * 2 + 1];
      machines[i].modifier.range[j][0] = udata[0x263 + i * 12 + j * 2];
      machines[i].modifier.range[j][1] = udata[0x263 + i * 12 + j * 2 + 1];
    }
  }

  for (int i = 0; i < 6; i++) {
    machines[i].trig.track = udata[0x2ae + i];
    machines[i].trig.portamento = IS_BIT_SET(udata[0x2ad], i);
    machines[i].trig.legatoAmp = IS_BIT_SET(udata[0x2b4], i);
    machines[i].trig.legatoFilter = IS_BIT_SET(udata[0x2b5], i);
    machines[i].trig.legatoLFO = IS_BIT_SET(udata[0x2b6], i);
  }

  commonMultimode = udata[0x2b7];
  if (udata[0x2b8] == 0) {
    commonTiming = 0;
  } else {
    commonTiming = 1 << (1 - udata[0x2b8]);
  }
  splitKey = udata[0x2b9];
  splitRange = udata[0x2ba];

  return true;
}

uint16_t MNMKit::toSysex(uint8_t *data, uint16_t len) {
  uint8_t udata[0x2ba + 1];

  data[0] = 0xF0;
  m_memcpy(data + 1, monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  data[6] = MNM_KIT_MESSAGE_ID;
  data[7] = 0x02;
  data[8] = 0x01;

  uint16_t cksum = 0;
  udata[0] = data[9] = origPosition;
  cksum += data[9];

  m_memcpy(udata + 1, name, 16);
  for (int i = 0; i < 6; i++) {
    udata[0xC + i] = machines[i].level;
    for (int j = 0; j < 72; j++) {
      udata[0x12 + i * 72 + j] = machines[i].params[j];
    }
    udata[0x1c2 + i] = machines[i].model;
    udata[0x1c8 + i] = machines[i].type;
  }
  
  ElektronHelper::from16Bit(patchBusIn, udata + 0x1ce);
  udata[0x1d1] = udata[0x1d2] = udata[0x2ab] = udata[0x2ac] = 0;
  udata[0x1d0] = 1; // ???
  for (int i = 0; i < 6; i++) {
    if (machines[i].modifier.mirrorLR)
      SET_BIT(udata[0x1d1], i);
    if (machines[i].modifier.mirrorUD)
      SET_BIT(udata[0x1d2], i);
    if (machines[i].modifier.LPKeyTrack)
      SET_BIT(udata[0x2ab], i);
    if (machines[i].modifier.HPKeyTrack)
      SET_BIT(udata[0x2ac], i);
  }
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      udata[0x1d3 + i * 12 + j * 2] = machines[i].modifier.destPage[j][0];
      udata[0x1d3 + i * 12 + j * 2 + 1] = machines[i].modifier.destPage[j][1];
      udata[0x21b + i * 12 + j * 2] = machines[i].modifier.destParam[j][0];
      udata[0x21b + i * 12 + j * 2 + 1] = machines[i].modifier.destParam[j][1];
      udata[0x263 + i * 12 + j * 2] = machines[i].modifier.range[j][0];
      udata[0x263 + i * 12 + j * 2 + 1] = machines[i].modifier.range[j][1];
    }
  }
  udata[0x2b4] = udata[0x2b5] = udata[0x2b6] = 0xF0;
  udata[0x2ad] = 0;
  for (int i = 0; i < 6; i++) {
    udata[0x2ae + i] = machines[i].trig.track;
    if (machines[i].trig.portamento)
      SET_BIT(udata[0x2ad], i);
    if (machines[i].trig.legatoAmp)
      SET_BIT(udata[0x2b4], i);
    if (machines[i].trig.legatoFilter)
      SET_BIT(udata[0x2b5], i);
    if (machines[i].trig.legatoLFO)
      SET_BIT(udata[0x2b6], i);
  }

  udata[0x2b7] = commonMultimode;
  int j;
  for (j = 0; j < 6; j++) {
    if (IS_BIT_SET(commonTiming, j)) {
      udata[0x2b8] = (j+1);
      break;
    }
  }
  if (j == 6) {
    udata[0x2b8] = (0);
  }

  udata[0x2b9] = splitKey;
  udata[0x2ba] = splitRange;

  MNMDataToSysexEncoder encoder(DATA_ENCODER_INIT(data + 10, len - 10));
  for (int i = 1; i < 699; i++) {
    encoder.pack8(udata[i]);
  }
  uint16_t enclen = encoder.finish();
  for (uint16_t i = 0; i < enclen; i++) {
    cksum += data[10 + i];
 }
  data[10 + enclen] = (cksum >> 7) & 0x7F;
  data[10 + enclen + 1] = cksum & 0x7F;
  data[10 + enclen + 2] = ((enclen + 5) >> 7) & 0x7F;
  data[10 + enclen + 3] = (enclen + 5) & 0x7F;
  data[10 + enclen + 2 + 2] = 0xF7;
  
  return enclen + 10 + sizeof(monomachine_sysex_hdr);
}

bool MNMSong::fromSysex(uint8_t *data, uint16_t len) {
  if (len < 3) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong length of unpacked data, %d should be 264\n", len);
#endif
    return false;
  }

  //  uint8_t *udata = data + 3;

  return false;
}

uint16_t MNMSong::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}
