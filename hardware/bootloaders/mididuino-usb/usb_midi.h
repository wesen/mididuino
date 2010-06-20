/**# USB midi specific data structures and definitions.
 **
 ** Includes descriptors for a simple MIDI link USB device (2 jacks
 ** and end connectors). The midi receiving state machine is
 ** implemented in `usb_midi_handle_rx_byte' which should be called
 ** each time a character is received on the midi interface. MIDI
 ** messages from USB are handled by the endpoint callback.
 **/

#ifndef USB_MIDI_H__
#define USB_MIDI_H__

#include "common.h"
#include "usb.h"

/**## MIDI definitions and MIDI parsing macros
 **/

/**
 ** Parse midi message macros
 **/
#define MIDI_IS_STATUS_BYTE(b)       ((b) & 0x80)
#define MIDI_IS_VOICE_STATUS_BYTE(b) ((b) <= 0xEF)
#define MIDI_VOICE_TYPE_NIBBLE(b)    ((b) & 0xF0)
#define MIDI_VOICE_CHANNEL(b)        ((b) & 0x0F)

#define MIDI_IS_SYSCOMMON_STATUS_BYTE(b) (((b) >= 0xF0) & ((b) < 0xF8))
#define MIDI_IS_REALTIME_STATUS_BYTE(b)  ((b) >= 0xF8)

/**
 ** Voice category messages
 **/
#define MIDI_NOTE_OFF         0x80 /* 2 bytes data */
#define MIDI_NOTE_ON          0x90 /* 2 bytes data */
#define MIDI_AFTER_TOUCH      0xA0 /* 2 bytes data */
#define MIDI_CONTROL_CHANGE   0xB0 /* 2 bytes data */
#define MIDI_PROGRAM_CHANGE   0xC0 /* 1 byte data */
#define MIDI_CHANNEL_PRESSURE 0xD0 /* 1 byte data */
#define MIDI_PITCH_WHEEL      0xE0 /* 2 bytes data */

/**
 ** System common category messages
 **/
#define MIDI_SYSEX_START       0xF0
#define MIDI_SYSEX_END         0xF7
#define MIDI_MTC_QUARTER_FRAME 0xF1 /* 1 byte data */
#define MIDI_SONG_POSITION_PTR 0xF2 /* 2 bytes data */
#define MIDI_SONG_SELECT       0xF3 /* 1 byte data */
#define MIDI_TUNE_REQUEST      0xF6 /* no data */

/**
 ** Realtime category messages, can be sent anytime
 **/
#define MIDI_CLOCK             0xF8 /* no data */
#define MIDI_TICK              0xF9 /* no data */
#define MIDI_START             0xFA /* no data */
#define MIDI_CONTINUE          0xFB /* no data */
#define MIDI_STOP              0xFC /* no data */
#define MIDI_ACTIVE_SENSE      0xFE /* no data */
#define MIDI_RESET             0xFF /* no data */

/**## USB midi structures, macros and definitions
 **/

/**
 ** Size of the endpoint for MIDI messages
 **/

#define MIDI_EP_SIZE 64
#define MIDI_BUF_SIZE MIDI_EP_SIZE

/**
 ** USB MIDI message structure definition
 **
 **/
typedef struct {
  unsigned int cin:4;
  unsigned int cable:4;
  uint8_t      msg[3];
} PACKED usb_midi_message_t;

typedef enum {
  midi_wait_status = 0,
  midi_wait_byte_2,
  midi_wait_byte_1,
  midi_wait_sysex,
  midi_ignore_message
} midi_state_t;

typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
  uint8_t      cin;
} midi_to_usb_t;


/**
 ** USB MIDI message types
 **/
#define CIN_MISC             0x00
#define CIN_CABLE            0x01
#define CIN_SYSCOMMON_2BYTES 0x02
#define CIN_SYSCOMMON_3BYTES 0x03
#define CIN_SYSEX_START      0x04
#define CIN_SYSCOMMON_1BYTE  0x05
#define CIN_SYSEX_END_2BYTES 0x06
#define CIN_SYSEX_END_3BYTES 0x07
#define CIN_NOTEOFF          0x08
#define CIN_NOTEON           0x09
#define CIN_POLYKEYPRESS     0x0A
#define CIN_CONTROL_CHANGE   0x0B
#define CIN_PROGRAM_CHANGE   0x0C
#define CIN_CHANNEL_PRESSURE 0x0D
#define CIN_PITCHBEND_CHANGE 0x0E
#define CIN_SINGLE_BYTE      0x0F

/**
 ** USB MIDI jack types
 **/
#define USB_MIDI_JACK_EMBEDDED 0x01
#define USB_MIDI_JACK_EXTERNAL 0x02

/**
 ** Class-specific AudioControl interface descriptor
 **/
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
  uint8_t  bInCollection;
  uint8_t  baInterfaceNr;
} PACKED usb_audiocontrol_descriptor_t;

/**
 ** Class-specific midiStreaming interface descriptor
 **/
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
} PACKED usb_midistreaming_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bJackType;
  uint8_t  bJackID;
  uint8_t  iJack;
} PACKED usb_midistreaming_injack_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bJackType;
  uint8_t  bJackID;
  uint8_t  bNrInputPins;
  uint8_t  BaSourceID;
  uint8_t  BaSourcePin;
  uint8_t  iJack;
} PACKED usb_midistreaming_outjack_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bNumEmbMIDIJack;
  uint8_t  BaAssocJackID;
} PACKED usb_midistreaming_ep_descriptor_t;

/**
 ** Application specific configuration setup
 **/
typedef struct {
  /* standard things */
  usb_configuration_descriptor_t         config;

  usb_interface_descriptor_t             ac_interface;
  usb_audiocontrol_descriptor_t          ac_specific_interface;

  usb_interface_descriptor_t             ms_interface;
  usb_midistreaming_descriptor_t         ms_specific_interface;
  usb_midistreaming_injack_descriptor_t  ms_injack_emb_descriptor;
  usb_midistreaming_injack_descriptor_t  ms_injack_ext_descriptor;
  usb_midistreaming_outjack_descriptor_t ms_outjack_emb_descriptor;
  usb_midistreaming_outjack_descriptor_t ms_outjack_ext_descriptor;

  /* device specific endpoints */
  usb_endpoint_descriptor_t         out_ep;
  usb_midistreaming_ep_descriptor_t ms_out_ep;
  usb_endpoint_descriptor_t         int_ep;
  usb_midistreaming_ep_descriptor_t ms_int_ep;
} PACKED usb_midi_config_t;

/**
 ** USB midi functions
 **/
void usb_midi_init(void);
void midi_uart_putc(uint8_t byte);
uint8_t usb_midi_buf_full(void);
void usb_midi_send_message(usb_midi_message_t *msg);
void usb_midi_process_data_out(uint8_t  ep);
void usb_midi_handle_rx_byte(uint8_t  byte);
void usb_midi_send_buf(void);
void usb_midi_send_sysex(uint8_t *buf, uint16_t len);
uint8_t usb_midi_handle_out_msg(uint8_t ep, usb_midi_message_t *out_msg);

#endif /* USB_MIDI_H__ */
