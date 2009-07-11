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
  uint8_t udata[0x109];
  
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
    encoder.pack(udata[i]);
  }
  uint16_t enclen = encoder.finish();
  for (int i = 0; i < enclen; i++) {
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

  return false;
}

uint16_t MNMKit::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}

bool MNMSong::fromSysex(uint8_t *data, uint16_t len) {
  uint8_t udata[0x108];

  uint16_t ulen = ElektronHelper::to16Bit(data[len-2], data[len-1]);
  if ((ulen + 3) != len) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong len, %d should be %d bytes\n", len, ulen + 3);
#endif
    return false;
  }

  uint16_t cksum = 0;
  for (int i = 9 - 6; i < (len - 4); i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != ElektronHelper::to16Bit(data[len - 4], data[len - 3])) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong checksum, %x should be %x\n", cksum,
	    ElektronHelper::to16Bit(data[len-4], data[len-3]));
#endif
    return false;
  }

  ulen = ElektronHelper::MNMSysexToData(data + 4, udata, len - 8, sizeof(udata));
  if (ulen != 264) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong length of unpacked data, %d should be 264\n", ulen);
#endif
    return false;
  }

  return false;
}

uint16_t MNMSong::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}
