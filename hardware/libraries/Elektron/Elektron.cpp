#include "Elektron.hh"

uint16_t ElektronHelper::MDDataToSysex(uint8_t *data, uint8_t *sysex, uint16_t len) {
  uint16_t retlen = 0;
  uint16_t cnt;
  uint16_t cnt7 = 0;

  sysex[0] = 0;
  for (cnt = 0; cnt < len; cnt++) {
    uint8_t c = data[cnt] & 0x7F;
    uint8_t msb = data[cnt] >> 7;
    sysex[0] |= msb << (6 - cnt7);
    sysex[1 + cnt7] = c;

    if (cnt7++ == 6) {
      sysex += 8;
      retlen += 8;
      sysex[0] = 0;
      cnt7 = 0;
    }
  }
  return retlen + cnt7 + (cnt7 != 0 ? 1 : 0);
}

uint16_t ElektronHelper::MDSysexToData(uint8_t *sysex, uint8_t *data, uint16_t len) {
  uint16_t cnt;
  uint16_t cnt2 = 0;
  uint16_t bits = 0;
  for (cnt = 0; cnt < len; cnt++) {
    if ((cnt % 8) == 0) {
      bits = sysex[cnt];
    } else {
      bits <<= 1;
      data[cnt2++] = sysex[cnt] | (bits & 0x80);
    }
  }
  return cnt2;
}

uint16_t ElektronHelper::MNMDataToSysex(uint8_t *data, uint8_t *sysex,
					uint16_t len, uint16_t sysexLen) {
  MNMDataToSysexEncoder encoder(sysex, sysexLen);
  for (uint16_t i = 0; i < len; i++) {
    if (!encoder.pack(data[i]))
      return 0;
  }
  return encoder.finish();
}

uint16_t ElektronHelper::MNMSysexToData(uint8_t *sysex, uint8_t *data,
					uint16_t len, uint16_t maxLen) {
  MNMSysexToDataEncoder encoder(data, maxLen);
  for (uint16_t i = 0; i < len; i++) {
    if (!encoder.pack(sysex[i]))
      return 0;
  }
  return encoder.finish();
}

uint16_t ElektronHelper::to16Bit(uint8_t b1, uint8_t b2) {
  return (b1 << 7) | b2; 
}

uint32_t ElektronHelper::to32Bit(uint8_t *b) {
  return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
}

void ElektronHelper::from16Bit(uint16_t num, uint8_t *b) {
  b[0] = (num >> 8) & 0xFF;
  b[1] = num & 0xFF;
}

void ElektronHelper::from32Bit(uint32_t num, uint8_t *b) {
  b[0] = (num >> 24) & 0xFF;
  b[1] = (num >> 16) & 0xFF;
  b[2] = (num >> 8) & 0xFF;
  b[3] = (num >> 0) & 0xFF;
}

/* Encoders */

void MNMDataToSysexEncoder::init(uint8_t *_sysex, uint16_t _sysexLen) {
  DataEncoder::init(_sysex, _sysexLen);
  cnt7 = 0;
  retLen = 0;
  lastByte = 0;
  lastCnt = 0;
  isFirstByte = true;
}

bool MNMDataToSysexEncoder::encode7Bit(uint8_t inb) {
  uint8_t msb = inb >> 7;
  uint8_t c = inb & 0x7F;
  ptr[0] |= msb << (6 - cnt7);
  ptr[1 + cnt7] = c;
  if (++cnt7 == 7) {
    retLen += 8;
    ptr += 8;
    if (ptr >= (data + maxLen - 8)) {
      return false;
    }
    ptr[0] = 0;
    cnt7 = 0;
  }
  return true;
}

bool MNMDataToSysexEncoder::pack(uint8_t inb) {
  if (isFirstByte) {
    lastByte = inb;
    lastCnt = 1;
    return true;
  } else {
    if (inb == lastByte) {
      lastCnt++;
      if (lastCnt == 127) {
	if (!packLastByte())
	  return false;
      }
    } else {
      if (!packLastByte())
	return false;
      lastByte = inb;
      lastCnt = 1;
    }
  }
  return true;
}

bool MNMDataToSysexEncoder::packLastByte() {
  if (lastCnt > 0) {
    if ((lastCnt == 1) && !(lastByte & 0x80)) {
      if (!encode7Bit(lastByte))
	return false;
      lastCnt = 0;
    } else {
      if (!encode7Bit(0x80 | lastCnt))
	return false;
      if (!encode7Bit(lastByte))
	return false;
      lastCnt = 0;
    }
  }
  return true;
}

uint16_t MNMDataToSysexEncoder::finish() {
  if (!packLastByte())
    return 0;
  else
    return retLen + cnt7;
}

void MNMSysexToDataEncoder::init(uint8_t *_data, uint16_t _maxLen) {
  DataEncoder::init(_data, _maxLen);
  cnt7 = 0;
  repeat = 0;
  cnt = 0;
  retLen = 0;
}

bool MNMSysexToDataEncoder::pack(uint8_t inb) {
  if ((cnt % 8) == 0) {
    bits = inb;
  } else {
    bits <<= 1;
    tmpData[cnt7++] = inb | (bits & 0x80);
  }
  cnt++;

  if (cnt7 == 7) {
    return unpack8Bit();
  }
  return true;
}

bool MNMSysexToDataEncoder::unpack8Bit() {
  for (uint8_t i = 0; i < cnt7; i++) {
    if (repeat == 0) {
      if (tmpData[i] & 0x80) {
	repeat = tmpData[i] & 0x7F;
      } else {
	if (retLen > maxLen) {
	  return false;
	}
	*(ptr++) = tmpData[i];
	retLen++;
      }
    } else {
      for (uint8_t j = 0; j < repeat; j++) {
	if (retLen > maxLen) {
	  return false;
	}
	*(ptr++) = tmpData[i];
	retLen++;
      }
      repeat = 0;
    }
  }
  cnt7 = 0;

  return true;
}

uint16_t MNMSysexToDataEncoder::finish() {
  if (!unpack8Bit()) {
    return 0;
  }
  return retLen;
}
