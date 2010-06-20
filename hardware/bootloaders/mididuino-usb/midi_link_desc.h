#ifndef MIDI_LINK_DESC_H__
#define MIDI_LINK_DESC_H__

#include "usb.h"
#include "usb_midi.h"

#define USB_ENABLE_HID 1

#define USB_VERSION 0x0200 // XXX ??

/* XXXX replace */
#define USB_ID_VENDOR     0x1ACC
#define USB_ID_PRODUCT    0x0009
#define USB_ID_RELEASE    0x1000

#define USB_APP_CONFIG_NUM 1

#define MIDI_EP_IN 2
#define MIDI_EP_OUT 2
#define D12_MIDI_EP_IN 5
#define D12_MIDI_EP_OUT 4

#define USB_STR_MANUFACTURER 1
#define USB_STR_PRODUCT 1

/* application specific configuration setup */
typedef struct {
  /* standard things */
  usb_configuration_descriptor_t config;

  usb_interface_descriptor_t     ac_interface;
  usb_audiocontrol_descriptor_t  ac_specific_interface;

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
} PACKED usb_app_config_t;

extern const PROGMEM usb_app_config_t usb_configuration_descriptor;

#endif /* MIDI_LINK_DESC_H__ */
