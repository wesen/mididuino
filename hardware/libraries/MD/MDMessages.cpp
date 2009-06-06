#include "Elektron.hh"
#include "MDMessages.hh"
#include "helpers.h"
#include "MDParams.hh"

extern uint8_t machinedrum_sysex_hdr[5];

uint16_t sysex16Bit(uint8_t b1, uint8_t b2) {
  return (b1 << 7) | b2; 
}

uint32_t to32Bit(uint8_t *b) {
  return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
}

void from32Bit(uint32_t num, uint8_t *b) {
  b[0] = (num >> 24) & 0xFF;
  b[1] = (num >> 16) & 0xFF;
  b[2] = (num >> 8) & 0xFF;
  b[3] = (num >> 0) & 0xFF;
}

bool MachineDrumGlobal::fromSysex(uint8_t *data, uint16_t len) {
  if (len != 0xBE)  {
    // wrong length 
    return false;
  }

  uint16_t cksum = 0;
  for (int i = 9 - 6; i < 0xC0 - 6; i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != sysex16Bit(data[0xc0 - 6], data[0xc1 - 6])) {
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
  return false;
}

uint16_t MachineDrumKit::toSysex(uint8_t *data, uint16_t len) {
  return 0;
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
  if (cksum != sysex16Bit(data[0xAC6 - 6], data[0xAC7 - 6])) {
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

bool MachineDrumSong::fromSysex(uint8_t *sysex, uint16_t len) {
  return false;
}

uint16_t MachineDrumSong::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}
