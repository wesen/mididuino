#include "MNM.h"

class MNMDataToSysexEncoder {
  uint8_t *sysex;
  uint8_t *ptr;
  uint16_t sysexLen;

  uint16_t retLen;
  uint16_t cnt7;

  uint8_t lastByte;
  uint8_t lastCnt;
  bool isFirstByte;
  
public:
  MNMDataToSysexEncoder(uint8_t *_sysex, uint16_t _sysexLen) {
    init(_sysex, _sysexLen);
  }

  void init(uint8_t *_sysex, uint16_t _sysexLen) {
    ptr = sysex = sysex;
    sysexLen = _sysexLen;
    cnt7 = 0;
    retLen = 0;
    lastByte = 0;
    lastCnt = 0;
    isFirstByte = true;
  }

  bool encode7Bit(uint8_t inb) {
    uint8_t msb = inb >> 7;
    uint8_t c = inb & 0x7F;
    ptr[0] |= msb << (6 - cnt7);
    ptr[1 + cnt7] = c;
    if (++cnt7 == 7) {
      retLen += 8;
      ptr += 8;
      if (ptr >= (sysex + sysexLen - 8)) {
	return false;
      }
      ptr[0] = 0;
      cnt7 = 0;
    }
    return true;
  }

  bool pack(uint8_t inb) {
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

  bool packLastByte() {
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

  uint16_t finish() {
    if (!packLastByte())
      return 0;
    else
      return retLen;
  }

  
};

uint16_t data_to_sysex_mnm(uint8_t *data, uint8_t *sysex, uint16_t len, uint16_t sysexLen) {
  MNMDataToSysexEncoder encoder(sysex, sysexLen);
  for (uint16_t i = 0; i < len; i++) {
    if (!encoder.pack(data[i]))
      return 0;
  }
  return encoder.finish();
}

uint16_t sysex_to_data_mnm(uint8_t *sysex, uint8_t *data, uint16_t len) {
  uint8_t tmpData[7];
  
  uint16_t cnt;
  uint16_t cnt2 = 0;
  uint16_t cnt3 = 0;
  uint16_t bits = 0;
  uint8_t repeat = 0;
  
  for (cnt = 0; cnt < len; cnt++) {
    if ((cnt % 8) == 0) {
      bits = sysex[cnt];
    } else {
      bits <<= 1;
      tmpData[cnt2++] = sysex[cnt] | (bits & 0x80);
    }
    if (cnt2 == 7) {
      for (uint8_t i = 0; i < 7; i++) {
	if (repeat == 0) {
	  if (tmpData[i] & 0x80) {
	    repeat = tmpData[i] & 0x7F;
	  } else {
	    data[cnt3++] = tmpData[i];
	  }
	} else {
	  for (uint8_t j = 0; j < repeat; j++) {
	    data[cnt3++] = tmpData[i];
	  }
	  repeat = 0;
	}
      }
      cnt2 = 0;
    }
  }
  return cnt3;
}

bool MNMGlobal::fromSysex(uint8_t *sysex, uint16_t len) {
  return false;
}

uint16_t MNMGlobal::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}

bool MNMKit::fromSysex(uint8_t *sysex, uint16_t len) {
  return false;
}

uint16_t MNMKit::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}

bool MNMSong::fromSysex(uint8_t *sysex, uint16_t len) {
  return false;
}

uint16_t MNMSong::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}
