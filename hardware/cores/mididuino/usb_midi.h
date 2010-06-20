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

#ifdef __cplusplus
extern "C" {
#endif

#include "usb.h"
#include "midi-common.hh"

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
uint8_t usb_midi_buf_full(void);
void usb_midi_send_message(usb_midi_message_t *msg);
void usb_midi_process_data_out(uint8_t  ep);
void usb_midi_handle_rx_byte(uint8_t  byte);
void usb_midi_send_buf(void);
void usb_midi_send_sysex(uint8_t *buf, uint16_t len);
uint8_t usb_midi_handle_out_msg(uint8_t ep, usb_midi_message_t *out_msg);

#ifdef __cplusplus
}
#endif


#endif /* USB_MIDI_H__ */
