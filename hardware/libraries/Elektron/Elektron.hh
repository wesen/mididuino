#ifndef ELEKTRON_H__
#define ELEKTRON_H__

#include "WProgram.h"

#include <inttypes.h>

typedef struct machine_name_s {
  char name[11];
  uint8_t id;
} machine_name_t;

typedef struct model_param_name_s {
  char name[4];
  uint8_t id;
} model_param_name_t;

typedef struct model_to_param_names_s {
  uint8_t model;
  model_param_name_t *names;
} model_to_param_names_t;

class ElektronHelper {
public:
  static uint16_t MDSysexToData(uint8_t *sysex, uint8_t *data, uint16_t len);
  static uint16_t MDDataToSysex(uint8_t *data, uint8_t *sysex, uint16_t len);
  static uint16_t MNMDataToSysex(uint8_t *data, uint8_t *sysex, uint16_t len, uint16_t maxLen);
  static uint16_t MNMSysexToData(uint8_t *sysex, uint8_t *data, uint16_t len, uint16_t maxLen);
  static uint16_t to16Bit(uint8_t b1, uint8_t b2);
  static uint32_t to32Bit(uint8_t *b);
  static void from16Bit(uint16_t num, uint8_t *b);
  static void from32Bit(uint32_t num, uint8_t *b);
};

class DataEncoder {
public:
  uint8_t *data;
  uint8_t *ptr;
  uint16_t maxLen;

  virtual void init(uint8_t *_data, uint16_t _maxLen) {
    data = _data;
    maxLen = _maxLen;
    ptr = data;
  }
  
  virtual bool pack(uint8_t inb) = 0;
  virtual uint16_t finish() = 0;
#ifdef HOST_MIDIDUINO
  virtual ~DataEncoder() { };
#endif
};

class MNMDataToSysexEncoder : public DataEncoder {
  uint16_t retLen;
  uint16_t cnt7;

  uint8_t lastByte;
  uint8_t lastCnt;
  bool isFirstByte;
  
public:
  MNMDataToSysexEncoder(uint8_t *_sysex = NULL, uint16_t _sysexLen = 0) {
    init(_sysex, _sysexLen);
  }

  virtual void init(uint8_t *_sysex, uint16_t _sysexLen);
  bool encode7Bit(uint8_t inb);
  virtual bool pack(uint8_t inb);
  bool packLastByte();
  virtual uint16_t finish();
};

class MNMSysexToDataEncoder : public DataEncoder {
public:
  uint16_t retLen;
  uint8_t cnt7;
  uint8_t repeat;
  uint16_t cnt;
  uint8_t bits;
  uint8_t tmpData[7];
  
  MNMSysexToDataEncoder(uint8_t *_data = NULL, uint16_t _maxLen = 0) {
    init(_data, _maxLen);
  }

  virtual void init(uint8_t *_data, uint16_t _maxLen);
  virtual bool pack(uint8_t inb);
  bool unpack8Bit();
  virtual uint16_t finish();
};


#endif /* ELEKTRON_H__ */
