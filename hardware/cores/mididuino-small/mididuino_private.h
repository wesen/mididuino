#ifndef MIDIDUINO_PRIVATE_H__
#define MIDIDUINO_PRIVATE_H__

#include "helpers.h"

void init(void);

void setLed(void);

void clearLed(void);

#define FIRMWARE_LENGTH_ADDR ((uint16_t *)0x00)
#define FIRMWARE_CHECKSUM_ADDR ((uint16_t *)0x02)
#define START_MAIN_APP_ADDR ((uint16_t *)0x04)

void start_bootloader(void);

#define BOARD_ID 0x43

#endif /* MIDIDUIDNO_PRIVATE_H__ */
