#ifndef MIDI_H__
#define MIDI_H__

#include "midi-common.h"

typedef enum {
  midi_wait_status = 0,
  midi_wait_byte_2,
  midi_wait_byte_1,
  midi_wait_sysex,
  midi_ignore_message
} midi_state_t;

typedef enum {
  sysex_wait_start = 0,
  sysex_wait_vendor_1,
  sysex_wait_vendor_2,
  sysex_wait_vendor_3,
  sysex_wait_cmd,
  sysex_wait_addr1,
  sysex_wait_addr2,
  sysex_wait_addr3,
  sysex_wait_addr4,
  sysex_wait_length,
  sysex_wait_data,
  sysex_wait_checksum,
  sysex_wait_len1,
  sysex_wait_len2,
  sysex_wait_len3,
  sysex_wait_checksum1,
  sysex_wait_checksum2,
  sysex_wait_patnum,
  sysex_wait_end
} sysex_state_t;

typedef struct sysex_in_stm_s {
  sysex_state_t state;
  uint8_t vendor[3];
  uint8_t cmd;
  uint32_t address;
  uint8_t length;
  uint8_t recvd;
  uint8_t cnt;
  uint8_t bits;
  uint8_t data[64];
  uint8_t checksum;
  void (*callback)(struct sysex_in_stm_s *);
} sysex_in_stm_t;

typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
  midi_callback_t callback;
} midi_parse_t;


typedef struct midi_in_stm_s {
  midi_state_t in_state;
  uint8_t      last_status;
  uint8_t      running_status;
  uint8_t      in_msg_len;
  uint8_t      msg[3];
  midi_callback_t callback;
  sysex_in_stm_t sysex_stm;
} midi_in_stm_t;

void midi_in_stm_init(midi_in_stm_t *stm);
void handle_midi_rx2(uint8_t byte);

void midi_start_sysex(midi_in_stm_t *stm);
void midi_abort_sysex(midi_in_stm_t *stm);
void midi_end_sysex(midi_in_stm_t *stm);
void midi_byte_sysex(midi_in_stm_t *stm, uint8_t byte);

void sysex_in_stm_init(sysex_in_stm_t *stm);
void handle_midi_rx_only_sysex(midi_in_stm_t *stm, uint8_t byte);

#endif 
