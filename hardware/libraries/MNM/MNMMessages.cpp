#include "MNMMessages.hh"
#include "helpers.h"
#include "Elektron.hh"

#ifdef HOST_MIDIDUINO
#include <stdio.h>
#endif

bool MNMGlobal::fromSysex(uint8_t *data, uint16_t len) {
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

  origPosition = data[3];
  autotrackChannel = udata[0];
  baseChannel = udata[1];
  channelSpan = udata[2];
  multitrigChannel = udata[3];
  multimapChannel = udata[4];
  clockIn = IS_BIT_SET(udata[5], 0);
  clockOut = IS_BIT_SET(udata[5], 5);
  ctrlIn = IS_BIT_SET(udata[5], 4);
  ctrlOut = IS_BIT_SET(udata[5], 6);
  transportIn = udata[6];
  sequencerOut = udata[7];
  arpOut = udata[8];
  keyboardOut = udata[9];
  transportOut = udata[0xa];
  midiClockOut = udata[0xb];
  pgmChangeOut = udata[0xc];
  for (int i = 0; i < 6; i++) {
    midiMachineChannels[i] = udata[0x12 + i];
    ccDestinations[i][0] = udata[0x19 + i];
    ccDestinations[i][1] = udata[0x19 + 6 + i];
    ccDestinations[i][2] = udata[0x19 + 12 + i];
    ccDestinations[i][3] = udata[0x19 + 18 + i];
    midiSeqLegato[i] = udata[0x31 + i];
    legato[i] = udata[0x37 + i];
  }
  for (int i = 0; i < 32; i++) {
    maps[i].range = udata[0x3c + i];
    maps[i].pattern = udata[0x5c + i];
    maps[i].offset = udata[0x7c + i];
    maps[i].length = udata[0x9c + i];
    maps[i].transpose = udata[0xbc + i];
    if (udata[0xdc + i] == 0) {
      maps[i].timing = 0;
    } else {
      maps[i].timing = 1 << (1 - udata[0xdc + i]);
    }
  }
  globalRouting = udata[0xfc];
  pgmChangeIn = udata[0xfd];
  baseFreq = ElektronHelper::to32Bit(udata + 0x103);

  return true;
}

uint16_t MNMGlobal::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}

bool MNMKit::fromSysex(uint8_t *data, uint16_t len) {
  uint8_t udata[0x2b9];

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
  if (ulen != 0x2ba) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong length of unpacked data, %d should be 698\n", ulen);
#endif
    return false;
  }

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
