#ifndef ELEKTRON_H__
#define ELEKTRON_H__

#include "WProgram.h"

#include <inttypes.h>

typedef struct mnm_machine_name_s {
  char name[11];
  uint8_t id;
} mnm_machine_name_t;

typedef struct md_machine_name_s {
  char name[7];
  uint8_t id;
} md_machine_name_t;

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
  static uint16_t to16Bit7(uint8_t b1, uint8_t b2);
  static uint16_t to16Bit(uint8_t b1, uint8_t b2);
  static uint16_t to16Bit(uint8_t *b);
  static uint32_t to32Bit(uint8_t *b);
  static void from16Bit(uint16_t num, uint8_t *b);
  static void from32Bit(uint32_t num, uint8_t *b);
  static uint64_t to64Bit(uint8_t *b);
  static void from64Bit(uint64_t num, uint8_t *b);
	static bool checkSysexChecksum(uint8_t *data, uint16_t len);
	static bool calculateSysexChecksum(uint8_t *data, uint16_t len);
};

#include "MNMDataEncoder.hh"
#include "MDDataEncoder.hh"

#endif /* ELEKTRON_H__ */
