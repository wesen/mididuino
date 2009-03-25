#include "app.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "common.h"
#include "lcd.h"
#include "bootloader.h"
#include "usb_midi.h"

#ifdef __AVR_ATmega168__
#define ROM_SIZE 8192L
#endif

#ifdef __AVR_ATmega8__
#define ROM_SIZE 4096L
#endif

#ifdef __AVR_ATmega32__
#define ROM_SIZE 16384L
#endif

#define _B2048  1

#ifdef _B128
  #define APP_PAGES ((2*ROM_SIZE / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
  #define APP_END (APP_PAGES * SPM_PAGESIZE)
#endif
#ifdef _B256
  #define APP_PAGES ((2*ROM_SIZE / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
  #define APP_END (APP_PAGES * SPM_PAGESIZE)
#endif
#ifdef _B512
  #define APP_PAGES ((2*ROM_SIZE / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
  #define APP_END (APP_PAGES * SPM_PAGESIZE)
#endif
#ifdef _B1024
  #define APP_PAGES ((2*ROM_SIZE / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
  #define APP_END (APP_PAGES * SPM_PAGESIZE)
#endif  
#ifdef _B2048

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) 
  #error "_B2048 not suppoted on this device"
#endif

  #define APP_PAGES ((2*ROM_SIZE / SPM_PAGESIZE)- (2*2048 / SPM_PAGESIZE )) 
  #define APP_END (APP_PAGES * SPM_PAGESIZE)
#endif

void (*jump_to_app)(void) = 0x0000;
static uint16_t sysex_address = 0;
static uint16_t recvd = 0;

void write_block_data(void);

uint8_t check_firmware_checksum(void) {
  uint16_t len = eeprom_read_word(FIRMWARE_LENGTH_ADDR);
  uint16_t firm_checksum = eeprom_read_word(FIRMWARE_CHECKSUM_ADDR);
  uint16_t i;
  uint16_t checksum = 0;

  for (i = 0; i < len; i++) {
    uint8_t byte = pgm_read_byte(i);
    checksum += byte;
  }

  if ((checksum & 0x3FFF) == firm_checksum)
    return 1;
  else {
#if 0
    lcd_line1();
    lcd_puts("CK ");
    lcd_putnumberx16(len);
    lcd_puts(" ");
    lcd_putnumberx16(firm_checksum);
    lcd_line2();
    lcd_putnumberx16(checksum & 0x3FFF);
#endif
    return 0;
  }
}

uint8_t ack_msg[6] = {
  0xf0, SYSEX_VENDOR_1, SYSEX_VENDOR_2, SYSEX_VENDOR_3, CMD_DATA_BLOCK_ACK, 0xf7
};

uint8_t nak_msg[6] = {
  0xf0, SYSEX_VENDOR_1, SYSEX_VENDOR_2, SYSEX_VENDOR_3, CMD_DATA_BLOCK_NAK, 0xf7
};

uint8_t jump_to_main_program(void) {
  if (recvd != 0) {
    write_block_data();
  }
  
  _delay_ms(100);

  if (check_firmware_checksum()) {
    eeprom_write_word(START_MAIN_APP_ADDR, 1);
    jump_to_app();
    return 1;
  } else {
    //    lcd_line2();
    //    lcd_puts("WRONG CHECK");
    return 0;
  }
}

uint8_t block_cnt = 0;
uint8_t sysex_data[SPM_PAGESIZE];

uint8_t data[250];
uint8_t sysex_cnt;
uint8_t in_sysex= 0;

uint16_t make_word(uint8_t idx, uint8_t cnt) {
  uint8_t ptr = idx + cnt - 1;
  uint32_t ret = 0;
  for (; ptr >= idx; ptr--) {
    ret <<= 7;
    ret |= data[ptr];
  }
  return ret;
}

extern uint8_t butstat;

uint8_t write_checksum(void) {
  if (recvd != 0) {
    write_block_data();
  }
  
  uint16_t len = make_word(4, 3);
  uint16_t checksum = data[7] | (data[8] << 7);
  eeprom_write_word(FIRMWARE_LENGTH_ADDR, len);
  eeprom_write_word(FIRMWARE_CHECKSUM_ADDR, checksum);
  eeprom_write_word(START_MAIN_APP_ADDR, 1);
  return 1;
}

void write_block_data(void) {
  uint16_t i;
  uint8_t sreg = SREG;
  lcd_line2();
  lcd_put((uint8_t *)"BLK ", 4);
  lcd_putnumberx(sysex_address >> 8);

  
  cli();
  boot_page_erase(sysex_address);
  boot_spm_busy_wait();
  uint16_t address = sysex_address;
  for (i = 0; i < SPM_PAGESIZE; i+=2) {
    uint16_t tmp = sysex_data[i] | (sysex_data[i + 1] << 8);
    boot_page_fill(address, tmp);
    address += 2;
  }
  boot_page_write(sysex_address);
  boot_spm_busy_wait();
  boot_rww_enable();
  SREG = sreg;
  recvd = 0;
}

uint8_t write_block(void) {
  uint8_t checksum = 0;
  uint8_t i;

  for (i = 3; i < sysex_cnt - 1; i++) {
    checksum ^= data[i];
  }

  uint16_t address = make_word(5, 4);
  recvd = address & 0x7f;
  if (recvd == 0) {
    sysex_address = address;
  }
  
  uint8_t length = data[4];
  //  uint8_t length = 128;

#if 0
  lcd_line2();
  lcd_putnumberx16(address);
  lcd_putnumberx(recvd);
  //  _delay_ms(1000);
#endif
  
  if (sysex_address >= APP_END) {
    return 0;
  }

  uint8_t cnt = 0;
  uint8_t bits = 0;
  i = 0;
  for (cnt = 0; cnt < (sysex_cnt - 9) && recvd < SPM_PAGESIZE; cnt++) {
    uint8_t byte = data[9 + cnt];

    if ((cnt % 8) == 0) {
      bits = byte;
    } else {
      sysex_data[recvd++] = byte | ((bits & 1) << 7);
      i++;
      bits >>= 1;
    }
    if (i >= length)
      break;
  }

  uint8_t check = data[sysex_cnt - 1];
  checksum &= 0x7f;

  if ((checksum != check) || (i != 64)) {
    return 0;
  }

  if (recvd == SPM_PAGESIZE) {
    write_block_data();
  }
  
  return 1;
}

uint8_t pkt_cnt = 0;
void handle_sysex(void) {
  uint8_t res = 0;
  if (sysex_cnt < 4) {
    goto end;
  } else {
    if (data[0] != SYSEX_VENDOR_1 ||
	data[1] != SYSEX_VENDOR_2 ||
	data[2] != SYSEX_VENDOR_3) {
      goto end;
    }

    if (data[3] == CMD_START_BOOTLOADER) {
      sysex_address = 0;
      res = 1;
      recvd = 0;
    } else if (data[3] == CMD_MAIN_PROGRAM) {
      res = jump_to_main_program();
    } else if (data[3] == CMD_BOOT_DATA_BLOCK) {
      res = write_block();
    } else if (data[3] == CMD_FIRMWARE_CHECKSUM && sysex_cnt == 9) {
      res = write_checksum();
    }

    if (res) {
      midi_sysex_send_ack();
    } else {
      midi_sysex_send_nak();
    }
  }

 end:
  in_sysex = 0;
  sysex_cnt = 0;
}

void handle_sysex_byte(uint8_t c) {
  if (c > 0x7F)
    return;
  data[sysex_cnt++] = c;
  if (sysex_cnt > 100) {
    in_sysex = 0;
  }
}
