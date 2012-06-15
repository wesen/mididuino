/*
 * MidiFirmwareSender.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: manuel
 */

#include "MidiSend.h"
#include "MidiFirmwareSender.h"

static uint16_t make_word(uint8_t *data, uint8_t cnt) {
  int8_t idx = 0;
  int8_t ptr = idx + cnt - 1;
  uint32_t ret = 0;
  for (; ptr >= idx; ptr--) {
    ret <<= 7;
    ret |= data[ptr];
  }
  return ret;
}

void MidiFirmwareSender::sendSysexBootloadCommand() {
  unsigned char bootmsg[] = { 0xF0, MIDIDUINO_SYSEX_VENDOR_1, MIDIDUINO_SYSEX_VENDOR_2, deviceId, CMD_START_BOOTLOADER, 0xF7 };
  canSendSysex = false;
  waitingForBootloader = true;
  logPrintf(LOG_INFO, "Start bootloader\n");
  MidiUart.sendRaw(bootmsg, sizeof(bootmsg));
}


void MidiFirmwareSender::sendSysexRebootCommand() {
  static unsigned char buf[] =
  {0xf0, MIDIDUINO_SYSEX_VENDOR_1, MIDIDUINO_SYSEX_VENDOR_2, deviceId, CMD_MAIN_PROGRAM, 0xf7 };
  canSendSysex = false;
  logPrintf(LOG_INFO, "Reboot device\n");
  MidiUart.sendRaw(buf, sizeof(buf));
}

bool MidiFirmwareSender::upload(HexFile *file, bool startBootloader) {
  this->file = file;
  this->transferIsComplete = false;
  if (startBootloader) {
    sendSysexBootloadCommand();
  } else {
    sendNextSysexPart();
  }
}

bool MidiFirmwareSender::sendNextSysexPart() {
  unsigned char part_buf[512];
  unsigned int len = file->getNextSysexPart(part_buf, sizeof(part_buf));

  if (len <= 0) {
    // no more data
    return false;
  }

  if (statusMessage) {
    if (part_buf[4] == CMD_BOOT_DATA_BLOCK) {
      uint16_t address = make_word(part_buf + 6, 4);
      float percent = (float)address/(float)file->getMaxAddress() * 100.0;
      logPrintf(LOG_PROGRESS, "%2.2f %%, sending %d/%d\n", percent, address, file->getMaxAddress());
    }
  }

  if (verbose >= 2) {
    if (part_buf[4] == CMD_BOOT_DATA_BLOCK) {
      uint16_t address = make_word(part_buf + 6, 4);
      logPrintf(LOG_INFO, "address: %x\n", address);
      if (verbose >= 3) {
        logPrintf(LOG_INFO, "code: \n");
        unsigned char code[512];
        unsigned int code_len = 0;
        unsigned int cnt;
        uint8_t bits;

        for (cnt = 0; cnt < (len - 12); cnt++) {
          uint8_t byte = part_buf[10 + cnt];

          if ((cnt % 8) == 0) {
            bits = byte;
          } else {
            code[code_len++] = byte | ((bits & 1) << 7);
            bits >>= 1;
          }
        }

        logHexdump(LOG_INFO, code, code_len);
      }
    }

    if (verbose >= 4) {
      logPrintf(LOG_INFO, "sysex: \n");
      logHexdump(LOG_INFO, part_buf, len);
    }
  }
  canSendSysex = 0;
  MidiUart.sendRaw(part_buf, len);

  return 0;
}

/* concurrency issues XXX */
void MidiFirmwareSender::handleByte(uint8_t b) {
  if (sysex->len == 3) {
    // XXX check for timeout

    // cmd byte
    switch (b) {
    case CMD_DATA_BLOCK_ACK:
      if (verbose >= 2) {
        logPrintf(LOG_INFO, "ACK received\n");
      }

      // wait for bootloader
      if (waitingForBootloader) {
        /* XXX move to helpers.h */
#ifdef WINDOWS
        Sleep(1500);
#else
        logPrintf(LOG_INFO, "Wait for bootloader\n");
        usleep(1500000);
#endif
        waitingForBootloader = false;
      }

      if (file->hasNextSysexPart()) {
        logPrintf(LOG_INFO, "Send next sysex part\n");
        sendNextSysexPart();
      } else {
        if (verbose >= 1) {
          logPrintf(LOG_INFO, "booting to main program\n");
        }
        if (statusMessage) {
          logPrintf(LOG_STATUS, "booting to main program\n");
        }

        sendSysexRebootCommand();

#ifdef WINDOWS
        Sleep(5);
#else
        usleep(5000);
#endif
        transferIsComplete = true;
      }

      break;

    default:
      logPrintf(LOG_ERROR, "unknown cmd %d received\n", b);
      break;
}
  }
}

