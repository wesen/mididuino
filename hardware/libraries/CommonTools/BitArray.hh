#ifndef BITARRAY_H__
#define BITARRAY_H__

#include <inttypes.h>
#include "helpers.h"

class BitArray {
  uint8_t *addr;
  uint8_t offset;
public:
  BitArray(uint8_t *_addr, uint8_t _offset) {
    addr = _addr;
    offset = _offset;
  };

  uint8_t getBit(uint16_t idx) {
    idx += offset;
    return IS_BIT_SET(addr[idx >> 3], idx & 7);
  }
  
  void setBit(uint16_t idx) {
    idx += offset;
    SET_BIT(addr[idx >> 3], idx & 7);
  }

  void setBit(uint16_t idx, uint8_t value) {
    idx += offset;
    if (value) {
      SET_BIT(addr[idx >> 3], idx & 7);
    } else {
      CLEAR_BIT(addr[idx >> 3], idx & 7);
    }
  }
  
  
  void clearBit(uint16_t idx) {
    idx += offset;
    CLEAR_BIT(addr[idx >> 3], idx & 7);
  }
  
  uint8_t toggleBit(uint16_t idx) {
    idx += offset;
    return TOGGLE_BIT(addr[idx >> 3], idx & 7);
  }
};

template <int N> class BitField {
public:
  uint8_t _bits[(N / 8) + 1];

  uint8_t isBitSet(uint16_t idx) {
    return IS_BIT_SET(_bits[idx >> 3], idx & 7);
  }

  void setBit(uint16_t idx) {
    SET_BIT(_bits[idx >> 3], idx & 7);
  }


  void setBit(uint16_t idx, uint8_t value) {
    if (value) {
      SET_BIT(_bits[idx >> 3], idx & 7);
    } else {
      CLEAR_BIT(_bits[idx >> 3], idx & 7);
    }
  }

  void clearBit(uint16_t idx) {
    CLEAR_BIT(_bits[idx >> 3], idx & 7);
  }

  void toggleBit(uint16_t idx) {
    TOGGLE_BIT(_bits[idx >> 3], idx & 7);
  }
};

#endif /* BITARRAY_H__ */
