#include "MNMMessages.hh"
#include "MNMParams.hh"
#include "helpers.h"
#include "Elektron.hh"

#ifdef HOST_MIDIDUINO
#include <stdio.h>
#endif

bool MNMGlobal::fromSysex(uint8_t *data, uint16_t len) {
  if (len < 264) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong len, %d should be %d bytes\n", len, 272);
#endif
    return false;
  }

  uint8_t *udata = data + 3;

  origPosition = udata[0];
  autotrackChannel = udata[1];
  baseChannel = udata[2];
  channelSpan = udata[3];
  multitrigChannel = udata[4];
  multimapChannel = udata[5];
  clockIn = IS_BIT_SET(udata[6], 0);
  clockOut = IS_BIT_SET(udata[6], 5);
  ctrlIn = IS_BIT_SET(udata[6], 4);
  ctrlOut = IS_BIT_SET(udata[6], 6);
  transportIn = udata[7];
  sequencerOut = udata[8];
  arpOut = udata[9];
  keyboardOut = udata[0xa];
  transportOut = udata[0xb];
  midiClockOut = udata[0xc];
  pgmChangeOut = udata[0xd];
  for (int i = 0; i < 6; i++) {
    midiMachineChannels[i] = udata[0x13 + i];
    ccDestinations[i][0] = udata[0x19 + i];
    ccDestinations[i][1] = udata[0x19 + 6 + i];
    ccDestinations[i][2] = udata[0x19 + 12 + i];
    ccDestinations[i][3] = udata[0x19 + 18 + i];
    midiSeqLegato[i] = udata[0x31 + i];
    legato[i] = udata[0x37 + i];
  }
  for (int i = 0; i < 32; i++) {
    maps[i].range = udata[0x3d + i];
    maps[i].pattern = udata[0x5d + i];
    maps[i].offset = udata[0x7d + i];
    maps[i].length = udata[0x9d + i];
    maps[i].transpose = udata[0xbd + i];
    if (udata[0xdd + i] == 0) {
      maps[i].timing = 0;
    } else {
      maps[i].timing = 1 << (1 - udata[0xdd + i]);
    }
  }
  globalRouting = udata[0xfd];
  pgmChangeIn = udata[0xfe];
  baseFreq = ElektronHelper::to32Bit(udata + 0x105);
  //  printf("baseFreq: %ld\n", baseFreq);

  return true;
}

uint16_t MNMGlobal::toSysex(uint8_t *data, uint16_t len) {
  uint8_t udata[0x10a];
  
  data[0] = 0xF0;
  m_memcpy(data + 1, monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  data[6] = MNM_GLOBAL_MESSAGE_ID;
  data[7] = 0x02; // version
  data[8] = 0x01; // revision

  uint16_t cksum = 0;
  udata[0] = data[9] = origPosition;
  cksum += data[9];

  udata[1] = (autotrackChannel);
  udata[2] = (baseChannel);
  udata[3] = (channelSpan);
  udata[4] = (multitrigChannel);
  udata[5] = (multimapChannel);
  uint8_t sync = 0;
  if (clockIn)
    SET_BIT(sync, 0);
  if (clockOut)
    SET_BIT(sync, 5);
  if (ctrlIn)
    SET_BIT(sync, 4);
  if (ctrlOut)
    SET_BIT(sync, 6);
  udata[6] = (sync);
  udata[7] = (transportIn ? 1 : 0);
  udata[8] = (sequencerOut ? 1 : 0);
  udata[9] = (arpOut ? 1 : 0);
  udata[0xa] = (keyboardOut ? 1 : 0);
  udata[0xb] = (transportOut ? 1 : 0);
  udata[0xc] = (midiClockOut ? 1 : 0);
  udata[0xd] = (pgmChangeOut ? 1 : 0);
  udata[0xe] = (0); // note
  udata[0xf] = (0); // gate
  udata[0x10] = (0); // sense
  udata[0x11] = (0); // minVel
  udata[0x12] = (0); // maxVel
  for (int i = 0; i < 6; i++) {
    udata[0x13 + i] = (midiMachineChannels[i]);
    udata[0x19 + i] = (ccDestinations[i][0]);
    udata[0x19 + 6 + i] = (ccDestinations[i][1]);
    udata[0x19 + 12 + i] = (ccDestinations[i][2]);
    udata[0x19 + 18 + i] = (ccDestinations[i][3]);
    udata[0x31 + i] = (midiSeqLegato[i]);
    udata[0x37 + i] = (legato[i]);
  }
  for (int i = 0; i < 32; i++) {
    udata[0x3d + i] = (maps[i].range);
    udata[0x5d + i] = (maps[i].pattern);
    udata[0x7D + i] = (maps[i].offset);
    udata[0x9d + i] = (maps[i].length);
    udata[0xbd + i] = (maps[i].transpose);

    int j;
    for (j = 0; j < 6; j++) {
      if (IS_BIT_SET(maps[i].timing, j)) {
	udata[0xdd + i] = (j+1);
	break;
      }
    }
    if (j == 6) {
      udata[0xdd + i] = (0);
    }
  }
  udata[0xfd] = (globalRouting);
  udata[0xfe] = (pgmChangeIn);
  for (int i = 0; i < 6; i++) {
    udata[0xFF + i] = 0x00;
  }
  ElektronHelper::from32Bit(baseFreq, udata + 0x105);

  MNMDataToSysexEncoder encoder(data + 10, len - 10);
  for (int i = 1; i < 0x109; i++) {
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

  MNMDataToSysexEncoder encoder(data + 10, len - 10);
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
