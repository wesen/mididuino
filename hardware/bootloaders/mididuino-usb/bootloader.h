#ifndef BOOTLOADER_H__
#define BOOTLOADER_H__

#define SYSEX_VENDOR_1 0x00
#define SYSEX_VENDOR_2 0x13
#define SYSEX_VENDOR_3 0x38 // mididuino

#define CMD_BOOT_DATA_BLOCK    0x01
#define CMD_DATA_BLOCK_ACK     0x02
#define CMD_FIRMWARE_CHECKSUM  0x03
#define CMD_MAIN_PROGRAM       0x04
#define CMD_START_BOOTLOADER   0x05
#define CMD_DATA_BLOCK_NAK     0x10

#define FIRMWARE_LENGTH_ADDR ((uint16_t *)0x00)
#define FIRMWARE_CHECKSUM_ADDR ((uint16_t *)0x02)
#define START_MAIN_APP_ADDR ((uint16_t *)0x04)

void midi_sysex_send_ack(void);
void midi_sysex_send_nak(void);
extern uint8_t ack_msg[6];
extern uint8_t nak_msg[6];

extern uint8_t sysex_cnt;

void handle_sysex(void);
void start_sysex(void);
void handle_sysex_byte(uint8_t c);
uint8_t jump_to_main_program(void);

#endif /* BOOTLOADER_H__ */
