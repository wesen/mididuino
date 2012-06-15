/*
 * HexFile.h
 *
 *  Created on: Jun 14, 2012
 *      Author: manuel
 */

#ifndef HEXFILE_H__
#define HEXFILE_H__

#include <string.h>

class HexFile {
protected:
  char fileName[256];
  unsigned char deviceID;
  bool firmwareChecksumSent;
  uint8_t hdr_data[4];

public:
  HexFile(const unsigned char deviceId, const char *fileName) : deviceID(deviceId), firmwareChecksumSent(false) {
    strncpy(this->fileName, fileName, sizeof(this->fileName));
    hdr_data[0] = 0xF0;
    hdr_data[1] = MIDIDUINO_SYSEX_VENDOR_1;
    hdr_data[2] = MIDIDUINO_SYSEX_VENDOR_2;
    hdr_data[3] = deviceID;
    max_address = 0;
    cur_address = 0;
  }

  static const int MAX_KB_SIZE = 256;
  unsigned char ram[MAX_KB_SIZE * 1024];
  unsigned int max_address;
  unsigned int cur_address;

  static bool isHexFile(const char *fileName);
  bool load();
  bool loadSysexFile();
  bool loadHexFile();

  void reset() {
    firmwareChecksumSent = false;
    cur_address = 0;
  }

  int getMaxAddress() { return max_address; }

  bool hasNextSysexPart();
  int getNextSysexPart(unsigned char *outbuf, unsigned int maxSize);
  int convertHexToSysexPart(unsigned char *outbuf, unsigned int maxSize);
  int createHexChecksumSysexPart(unsigned char *outbuf, unsigned int maxSize);
};

#endif
