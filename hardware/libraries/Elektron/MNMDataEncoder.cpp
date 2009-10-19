#include "Elektron.hh"
#include "MNMDataEncoder.hh"

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
  ptr[cnt7 + 1] = c;
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
  //  printf("patck: %x\n", inb);
  if (isFirstByte) {
    lastByte = inb;
    lastCnt = 1;
    isFirstByte = false;
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
    return retLen + ((cnt7 > 0) ? (cnt7 + 1) : 0);
}

void MNMSysexToDataEncoder::init(uint8_t *_data, uint16_t _maxLen) {
  DataEncoder::init(_data, _maxLen);
  cnt7 = 0;
  repeat = 0;
  cnt = 0;
  retLen = 0;
}

bool MNMSysexToDataEncoder::pack(uint8_t inb) {
  //  printf("pack: %x\n", inb);
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
    //    printf("tmpdata[%d]: %x\n", i, tmpData[i]);
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
  //  printf("cnt7: %d\n", cnt7);
  if (!unpack8Bit()) {
    return 0;
  }
  return retLen;
}
