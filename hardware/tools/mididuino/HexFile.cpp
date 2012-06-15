/*
 * HexFile.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: manuel
 */

#include "Platform.h"
#include "MidiSend.h"
#include "HexFile.h"

#include <string.h>
#include <stdio.h>

bool HexFile::isHexFile(const char *fileName) {
  int len = strlen(fileName);
  if (len > 4) {
    if (!strncmp(fileName + (len - 4), ".hex", 4)) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

bool HexFile::load() {
  if (isHexFile(fileName)) {
    return loadHexFile();
  } else {
    return loadSysexFile();
  }
}

bool HexFile::loadSysexFile() {
  FILE *fin = fopen(fileName, "r");
  if (fin == NULL) {
    logPrintf(LOG_ERROR, "Could not open file %s\n", fileName);
    return false;
  }

  int res = fread(ram, 1, sizeof(ram), fin);
  cur_address = 0;
  max_address = res;

  fclose(fin);

  return true;
}

bool HexFile::loadHexFile() {
  FILE *fin = fopen(fileName, "r");
  if (fin == NULL) {
    logPrintf(LOG_ERROR, "Could not open file %s\n", fileName);
    return false;
  }

  char buf[128];
  unsigned long i;

  for (i = 0; i < sizeof(ram); i++) {
    ram[i] = 0xFF;
  }

  max_address = 0;
  cur_address = 0;
  firmwareChecksumSent = 0;

  while (fgets(buf, sizeof(buf), fin)) {
    unsigned int len = strlen(buf);
    if (len < 10) {
      logPrintf(LOG_ERROR, "wrong ihex length, should be at least 10: %s\n", buf);
      exit(1);
    }

    unsigned long address;

    char size_str[3];
    size_str[2] = 0;
    size_str[0] = buf[1];
    size_str[1] = buf[2];
    unsigned int size;
    sscanf(size_str, "%x", &size);

    char address_str[5];
    address_str[4] = 0;
    address_str[0] = buf[3];
    address_str[1] = buf[4];
    address_str[2] = buf[5];
    address_str[3] = buf[6];
    sscanf(address_str, "%lx", &address);

    char type_str[3];
    type_str[2] = 0;
    type_str[0] = buf[7];
    type_str[1] = buf[8];
    unsigned int type;
    sscanf(type_str, "%x", &type);

    static unsigned int i = 0;

    if (type == 0x00) {
      unsigned int cnt = 0;
      for (i = 0; i < size * 2; i+=2) {
        char data_str[3];
        data_str[2] = 0;
        data_str[0] = buf[9 + i];
        data_str[1] = buf[10 + i];

        unsigned int byte;
        sscanf(data_str, "%x", &byte);
        ram[address + cnt++] = byte & 0xFF;
      }
      //      printf("address: %x, size: %x, type: %x\n", address, size, type);
      //      hexdump](flashram + address, size);
      max_address = address + cnt;
    }
  }

  uint16_t checksum = 0;
  for (i = 0; i < max_address; i++) {
    checksum += ram[i];
  }
  debugPrintf(1, "firmware length: %x, checksum: %x\n", max_address, checksum & 0x3FFF);
  if (fin != NULL) {
    fclose(fin);
    fin = NULL;
  }

  return true;
}

bool HexFile::hasNextSysexPart() {
  if (isHexFile(fileName)) {
    return firmwareChecksumSent == 0;
  } else {
    return cur_address < max_address;
  }
}

int HexFile::getNextSysexPart(unsigned char *outbuf, unsigned int maxSize) {
  uint8_t hdr_data[4];
  hdr_data[0] = 0xF0;
  hdr_data[1] = MIDIDUINO_SYSEX_VENDOR_1;
  hdr_data[2] = MIDIDUINO_SYSEX_VENDOR_2;
  hdr_data[3] = deviceID;

  if (isHexFile(fileName)) {
    return convertHexToSysexPart(outbuf, maxSize);
  } else {
    unsigned int i = 0;
    for (i = 0; i < maxSize && cur_address < max_address; i++) {
      int c = ram[cur_address++];
      outbuf[i] = c;
      if (c == 0xf7) {
        i++;
        break;
      }
    }
    return i;
  }
}

int HexFile::convertHexToSysexPart(unsigned char *outbuf, unsigned int maxSize) {
  unsigned int idx = 0;
  unsigned char len = 64;
  if (firmwareChecksumSent == 1) {
    return 0;
  }

  /* finally, send the checksum */
  if (cur_address >= max_address) {
    return createHexChecksumSysexPart(outbuf, maxSize);
  }

  memcpy(outbuf + idx, hdr_data, 4);
  outbuf[idx + 3] = deviceID;
  idx += 4;
  unsigned char cmd =  CMD_BOOT_DATA_BLOCK;
  unsigned char checksum = cmd;
  outbuf[idx++] = cmd;
  checksum ^= len;
  outbuf[idx++] = len;

  unsigned int address = cur_address;
  unsigned char address_bytes[4];
  address_bytes[0] = address & 0x7F;
  address >>= 7;
  address_bytes[1] = address & 0x7F;
  address >>= 7;
  address_bytes[2] = address & 0x7F;
  address >>= 7;
  address_bytes[3] = address & 0x7F;
  memcpy(outbuf + idx, address_bytes, 4);
  idx += 4;
  checksum ^= address_bytes[0];
  checksum ^= address_bytes[1];
  checksum ^= address_bytes[2];
  checksum ^= address_bytes[3];

  unsigned char *buf = ram + cur_address;

  unsigned char i;
  unsigned char bits = 0;
  unsigned char tmpbuf[8];
  for (i = 0; i < len; i++) {
    unsigned char c = buf[i] & 0x7F;
    unsigned char msb = buf[i] >> 7;
    bits |= msb << (i % 7);
    tmpbuf[i % 7] = c;

    if ((i % 7) == 6) {
      checksum ^= bits;
      outbuf[idx++] = bits;
      bits = 0;
      memcpy(outbuf + idx, tmpbuf, 7);
      idx += 7;
    }
    checksum ^= c;
  }
  if ((i % 7) > 0) {
    checksum ^= bits;
    outbuf[idx++] = bits;
    bits = 0;
    memcpy(outbuf + idx, tmpbuf, i % 7);
    idx += i % 7;
  }
  checksum &= 0x7f;
  outbuf[idx++] = checksum;
  outbuf[idx++] = '\xf7';
  cur_address += len;

  return idx;
}

int HexFile::createHexChecksumSysexPart(unsigned char *outbuf, unsigned int maxSize) {
  unsigned int idx = 0;

  unsigned int i;
  unsigned short firmware_len = 0;
  unsigned short firmware_checksum = 0;
  for (i = 0; i < max_address; i++) {
    firmware_checksum += ram[i];
  }
  firmware_len = max_address;

  memcpy(outbuf + idx, hdr_data, 4);
  outbuf[idx + 3] = deviceID;
  idx += 4;
  unsigned char cmd = CMD_FIRMWARE_CHECKSUM;
  outbuf[idx++] = cmd;
  unsigned char b = firmware_len & 0x7F;
  outbuf[idx++] = b;
  b = (firmware_len >> 7) & 0x7F;
  outbuf[idx++] = b;
  b = (firmware_len >> 14) & 0x7F;
  outbuf[idx++] = b;
  b = firmware_checksum & 0x7F;
  outbuf[idx++] = b;
  b = (firmware_checksum >> 7) & 0x7f;
  outbuf[idx++] = b;
  outbuf[idx++] = '\xf7';

  logPrintf(LOG_INFO, "len: %x (%x %x), checksum: %x (%x %x) \n",
            firmware_len, (firmware_len & 0x7F), (firmware_len >> 7) & 0x7F,
            firmware_checksum, (firmware_checksum & 0x7F), (firmware_checksum >> 7) & 0x7F);

  firmwareChecksumSent = 1;
  return idx;
}


