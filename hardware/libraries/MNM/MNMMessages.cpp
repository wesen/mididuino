#include "MNMMessages.hh"
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
    ccDestinations[i][0] = udata[0x1a + i];
    ccDestinations[i][1] = udata[0x1a + 6 + i];
    ccDestinations[i][2] = udata[0x1a + 12 + i];
    ccDestinations[i][3] = udata[0x1a + 18 + i];
    midiSeqLegato[i] = udata[0x32 + i];
    legato[i] = udata[0x38 + i];
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
  baseFreq = ElektronHelper::to32Bit(udata + 0x104);

  return true;
}

uint16_t MNMGlobal::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
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
