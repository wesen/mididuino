#include "Elektron.hh"
#include "MDMessages.hh"
#include "helpers.h"
#include "MDParams.hh"

extern uint8_t machinedrum_sysex_hdr[5];

uint16_t to16Bit(uint8_t b1, uint8_t b2) {
  return (b1 << 7) | b2; 
}

uint32_t to32Bit(uint8_t *b) {
  return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
}

void from16Bit(uint16_t num, uint8_t *b) {
  b[0] = (num >> 8) & 0xFF;
  b[1] = num & 0xFF;
}

void from32Bit(uint32_t num, uint8_t *b) {
  b[0] = (num >> 24) & 0xFF;
  b[1] = (num >> 16) & 0xFF;
  b[2] = (num >> 8) & 0xFF;
  b[3] = (num >> 0) & 0xFF;
}

bool MachineDrumGlobal::fromSysex(uint8_t *data, uint16_t len) {
  if (len != 0xC5 - 7)  {
    // wrong length 
    return false;
  }

  uint16_t cksum = 0;
  for (int i = 9 - 6; i < 0xC0 - 6; i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != to16Bit(data[0xc0 - 6], data[0xc1 - 6])) {
    // wrong checksum
    return false;
  }

  origPosition = data[9 - 6];
  for (int i = 0; i < 16; i++) {
    drumRouting[i] = data[0xA - 6 + i];
  }
  
  sysex_to_data_elektron(data + 0x1A - 6, keyMap, 147);
  baseChannel = data[0xAD - 6];
  tempo = (data[0xAF - 6] << 7) | data[0xB0 - 6];
  extendedMode = (data[0xB1 - 6] == 1);
  clockIn = ((data[0xB2 - 6] & 1) != 0);
  clockOut = ((data[0xB2 - 6] & 32) != 0);
  transportIn = ((data[0xB2 - 6] & 16) != 0);
  transportOut = ((data[0xB2 - 6] & 64) != 0);
  localOn = (data[0xb3 - 6] == 1);

  drumLeft = data[0xB4 - 6];
  drumRight = data[0xB5 - 6];
  gateLeft = data[0xB6 - 6];
  gateRight = data[0xB7 - 6];
  senseLeft = data[0xB8 - 6];
  senseRight = data[0xB9 - 6];
  minLevelLeft = data[0xBA - 6];
  minLevelRight = data[0xBB - 6];
  maxLevelLeft = data[0xBC - 6];
  maxLevelRight = data[0xBD - 6];
  
  programChange = data[0xBE - 6];
  trigMode = data[0xBF - 6];

  return true;
}

uint16_t MachineDrumGlobal::toSysex(uint8_t *data, uint16_t len) {
  if (len < 0xC5)
    return 0;
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_GLOBAL_MESSAGE_ID;
  data[7] = 0x05; // version
  data[8] = 0x01;
  data[9] = origPosition;
  for (int i = 0; i < 16; i++){
    data[0xA + i] = drumRouting[i];
  }
  data_to_sysex_elektron(keyMap, data + 0x1A, 128);
  data[0xAd] = baseChannel;
  data[0xAe] = 0;
  data[0xAF] = (uint8_t)((tempo >> 7) & 0x7F);
  data[0xB0] = (uint8_t)(tempo & 0x7F);
  data[0xB1] = (uint8_t) (extendedMode ? 1 : 0);
  data[0xb2] = 0;
  if (clockIn)
    data[0xb2] |= 1;
  if (clockOut)
    data[0xb2] |= 32;
  if (transportIn)
    data[0xb2] |= 16;
  if (transportOut)
    data[0xb2] |= 64;
  data[0xb3] = (uint8_t) (localOn ? 1 : 0);

  data[0xb4] = drumLeft;
  data[0xb6] = gateLeft;
  data[0xb8] = senseLeft;
  data[0xBa] = minLevelLeft;
  data[0xBC] = maxLevelLeft;

  data[0xb5] = drumRight;
  data[0xb7] = gateRight;
  data[0xb9] = senseRight;
  data[0xBB] = minLevelLeft;
  data[0xBD] = maxLevelLeft;

  data[0xBE] = programChange;
  data[0xBF] = trigMode;
		
  uint16_t checksum = 0;
  for (int i = 9; i < 0xC0; i++)
    checksum += data[i];
  data[0xC0] = (uint8_t)((checksum >> 7) & 0x7F);
  data[0xC1] = (uint8_t)(checksum & 0x7F);
  uint16_t length = 0xC5 - 7 - 3;
  data[0xC2] = (uint8_t)((length >> 7) &0x7F);
  data[0xC3] = (uint8_t)(length & 0x7F);
  data[0xC4] = 0xF7;

  return 0xC5;
}

bool MachineDrumKit::fromSysex(uint8_t *data, uint16_t len) {
  if (len < 0x4d1 - 7)
    return false;
  
  uint16_t cksum = 0;
  for (int i = 9 - 6; i < 0x4CC - 6; i++) {
    cksum += data[i];
  }
		
  cksum &= 0x3FFF;
  if (cksum != to16Bit(data[0x4CC - 6], data[0x4CD - 6]))
    return false;
		
  origPosition = data[9 - 6];
  m_strncpy(name, (char *)data + 0xA - 6, 16);

  int idx = 0x1A - 6;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      machines[i].params[j] = data[idx++];
    }
  }
  for (int i = 0; i < 16; i++) {
    machines[i].level = data[idx++];
  }

  uint8_t data2[16 * 36];
  uint8_t *ptr = data2;
  sysex_to_data_elektron(data + 0x1aa - 6, data2, 74);
  for (int i = 0; i < 16; i++) {
    machines[i].model = to32Bit(ptr);
    ptr += 4;
    machines[i].track = i;
  }

  ptr = data2;
  sysex_to_data_elektron(data + 0x1f4 - 6, data2, 659);
  for (int i = 0; i < 16; i++) {
    machines[i].lfo.destinationTrack = ptr[0];
    machines[i].lfo.destinationParam = ptr[1];
    machines[i].lfo.shape1 = ptr[2];
    machines[i].lfo.shape2 = ptr[3];
    machines[i].lfo.type = ptr[4];
    ptr += 36;
  }

  for (int i = 0; i < 8; i++) {
    reverb[i] = data[0x487 + i - 6];
    delay[i] = data[0x48F + i - 6];
    eq[i] = data[0x497 + i - 6];
    dynamics[i] = data[0x49F + i - 6];
  }

  ptr = data2;
  sysex_to_data_elektron(data + 0x4a7 - 6, data2, 37);
  for (int i = 0; i < 16; i++) {
    machines[i].trigGroup = ptr[i * 2];
    machines[i].muteGroup = ptr[i * 2 + 1];
    ptr += 2;
  }

  return true;
}

uint16_t MachineDrumKit::toSysex(uint8_t *data, uint16_t len) {
  if (len < 0x4d1)
    return 0;
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_KIT_MESSAGE_ID;
  data[7] = 0x04; // version
  data[8] = 0x01;
		
  data[9] = origPosition;
  bool afterEnd = false;
  for (int i = 0; i < 16; i++) {
    if (afterEnd) {
      data[i + 0xA] = ' ';
    } else {
      data[i + 0xA] = name[i] & 0x7F;
    }
  }

  int idx = 0x1A;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      data[idx++] = machines[i].params[j];
    }
  }
  for (int i = 0; i < 16; i++) {
    data[idx++] = machines[i].level;
  }

  uint8_t data2[16 * 36];
  uint8_t *ptr = data2;
  for (int i = 0; i < 16; i++) {
    from32Bit(machines[i].model, ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0x1AA, 16 * 4);

  ptr = data2;
  for (int i = 0; i < 16; i++) {
    ptr[0] = machines[i].lfo.destinationTrack;
    ptr[1] = machines[i].lfo.destinationParam;
    ptr[2] = machines[i].lfo.shape1;
    ptr[3] = machines[i].lfo.shape2;
    ptr[4] = machines[i].lfo.type;
    ptr[5] = 1;
    ptr += 36;
  }
  data_to_sysex_elektron(data2, data + 0x1F4, 16 * 36);
		
  for (int i = 0; i < 8; i++) {
    data[0x487 + i] = reverb[i];
    data[0x48F + i] = delay[i];
    data[0x497 + i] = eq[i];
    data[0x49F + i] = dynamics[i];
  }

  ptr = data2;
  for (int i = 0; i < 16; i++) {
    ptr[i*2] = machines[i].trigGroup;
    ptr[i*2+1] = machines[i].muteGroup;
    ptr += 2;
  }
  data_to_sysex_elektron(data2, data + 0x4a7, 32);

  int checksum = 0;
  for (int i = 9; i <  0x4cc; i++)
    checksum += data[i];
  data[0x4cc] = (uint8_t)((checksum >> 7) & 0x7F);
  data[0x4cd] = (uint8_t)(checksum & 0x7F);
  int length = 0x4d1 - 10;
  data[0x4ce] = (uint8_t)((length >> 7) &0x7F);
  data[0x4cf] = (uint8_t)(length & 0x7F);
  data[0x4d0] = 0xF7;

  return 0x4d1;
}

bool MachineDrumPattern::fromSysex(uint8_t *data, uint16_t len) {
  if (len != 0xACA - 6)  {
    // wrong length 
    return false;
  }

  uint16_t cksum = 0;
  for (int i = 9 - 6; i < 0xAC6 - 6; i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != to16Bit(data[0xAC6 - 6], data[0xAC7 - 6])) {
    // wrong checksum
    return false;
  }

  origPosition = data[9 - 6];
  uint8_t data2[204];
  sysex_to_data_elektron(data + 0xA - 6, data2, 74);
  uint8_t *ptr = data2;
  for (int i = 0; i < 16; i++) {
    trigPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
    
  sysex_to_data_elektron(data + 0x54 - 6, data2, 74);
  ptr = data2;
  for (int i = 0; i < 16; i++) {
    lockPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }

  sysex_to_data_elektron(data + 0x9e - 6, data2, 19);
  accentPattern = to32Bit(data2);
  slidePattern = to32Bit(data2 + 4);
  swingPattern = to32Bit(data2 + 8);
  swingAmount = to32Bit(data2 + 12);

  accentAmount = data[0xB1 - 6];
  patternLength = data[0xB2 - 6];
  doubleTempo = (data[0xb3 - 6] == 1);
  scale = data[0xb4 - 6];
  kit = data[0xb5 - 6];
  numLockedRows = data[0xb6 - 6];
  sysex_to_data_elektron(data + 0xB7 - 6, (uint8_t *)locks, 2341);
  sysex_to_data_elektron(data + 0x9DC - 6, data2, 234);

  accentEditAll = (to32Bit(data2) == 1);
  slideEditAll = (to32Bit(data2 + 4) == 1);
  swingEditAll = (to32Bit(data2 + 8) == 1);

  ptr = data2 + 12;
  for (int i = 0; i < 16; i++) {
    accentPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    slidePatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    swingPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      paramLocks[i][j] = -1;
    }
  }

  numRows = 0;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      if (IS_BIT_SET(lockPatterns[i], j)) {
	paramLocks[i][j] = numRows++;
      }
    }
  }
  

  return true;
  
}

uint16_t MachineDrumPattern::toSysex(uint8_t *data, uint16_t len) {
  if (len < 0xacb)
    return 0;
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_PATTERN_MESSAGE_ID;
  data[7] = 0x03; // version
  data[8] = 0x01;
  data[9] = origPosition;

  uint8_t data2[204];
  uint8_t *ptr = data2;
  for (int i = 0; i < 16; i++) {
    from32Bit(trigPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0xA, 64);
    
  ptr = data2;
  for (int i = 0; i < 16; i++) {
    from32Bit(lockPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0x54, 64);

  from32Bit(accentPattern, data2);
  from32Bit(slidePattern, data2 + 4);
  from32Bit(swingPattern, data2 + 8);
  from32Bit(swingAmount, data2 + 12);
  data_to_sysex_elektron(data2, data + 0x9e, 16);

  data[0xB1] = accentAmount;
  data[0xB2] = patternLength;
  data[0xb3] = doubleTempo ? 1 : 0;
  data[0xb4] = scale;
  data[0xb5] = kit;
  data[0xb6] = numLockedRows;

  data_to_sysex_elektron((uint8_t*)locks, data + 0xB7, 64 * 32);

  from32Bit(accentEditAll ? 1 : 0, data2);
  from32Bit(slideEditAll ? 1 : 0, data2 + 4);
  from32Bit(swingEditAll ? 1 : 0, data2 + 8);

  ptr = data2 + 12;
  for (int i = 0; i < 16; i++) {
    from32Bit(accentPatterns[i], ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    from32Bit(slidePatterns[i], ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    from32Bit(swingPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0x9DC, 204);


  uint16_t checksum = 0;
  for (int i = 9; i < 0xac6; i++)
    checksum += data[i];
  data[0xac6] = (uint8_t)((checksum >> 7) & 0x7F);
  data[0xac7] = (uint8_t)(checksum & 0x7F);
  uint16_t length = 0xacb - 7 - 3;
  data[0xac8] = (uint8_t)((length >> 7) &0x7F);
  data[0xac9] = (uint8_t)(length & 0x7F);
  data[0xaca] = 0xF7;

  return 0xacb;
}

bool MachineDrumSong::fromSysex(uint8_t *data, uint16_t len) {
  if (len < 0x1a - 7)
    return false;

  numRows = (len - (0x1A - 7)) / 12;
  uint16_t end = 0x1A + numRows * 12;
  
  uint16_t cksum = 0;
  for (uint16_t i = 9 - 6; i < end - 6; i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != to16Bit(data[end - 6], data[end + 1 - 6])) {
    // wrong checksum
    return false;
  }

  origPosition = data[9 - 6];
  m_strncpy(name, (char *)data + 0xA - 6, 16);

  uint8_t data2[10];
  uint8_t *ptr = data + 0x1A - 6;
  for (int i = 0; i < numRows; i++) {
    sysex_to_data_elektron(ptr, data2, 12);
    rows[i].pattern = data2[0];
    rows[i].kit = data2[1];
    rows[i].loopTimes = data2[2];
    rows[i].jump = data2[3];
    rows[i].mutes = to16Bit(data2[4], data2[5]);
    rows[i].tempo = to16Bit(data2[6], data2[7]);
    rows[i].startPosition = data2[8];
    rows[i].endPosition = data2[9];
    ptr += 12;
  }
  
  return true;
}

uint16_t MachineDrumSong::toSysex(uint8_t *data, uint16_t len) {
  if (len < (uint16_t)(0x1F + numRows * 12 ))
    return 0;
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_SONG_MESSAGE_ID;
  data[7] = 0x02; // version
  data[8] = 0x02;
  data[9] = origPosition;

  bool afterEnd = false;
  for (int i = 0; i < 16; i++) {
    if (afterEnd) {
      data[i + 0xA] = ' ';
    } else {
      data[i + 0xA] = name[i] & 0x7F;
    }
  }

  uint8_t *ptr = data + 0x1A;
  uint8_t data2[10];
  for (int i = 0; i < numRows; i++) {
    data2[0] = rows[i].pattern;
    data2[1] = rows[i].kit;
    data2[2] = rows[i].loopTimes;
    data2[3] = rows[i].jump;
    from16Bit(rows[i].mutes, data2 + 4);
    from16Bit(rows[i].tempo, data2 + 6);
    data2[8] = rows[i].startPosition;
    data2[9] = rows[i].endPosition;
    data_to_sysex_elektron(data2, ptr, 10);
    ptr += 12;
  }

  int end = 0x1A + 12 * numRows;
  int checksum = 0;
  for (int i = 9; i <  end; i++)
    checksum += data[i];
  data[end] = (uint8_t)((checksum >> 7) & 0x7F);
  data[end + 1] = (uint8_t)(checksum & 0x7F);
  int length = 0x1F + 12 * numRows - 10;
  data[end + 2] = (uint8_t)((length >> 7) &0x7F);
  data[end + 3] = (uint8_t)(length & 0x7F);
  data[end + 4] = 0xF7;

  return length + 10;
}
