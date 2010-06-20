#include <avr/pgmspace.h>

#include "d12.h"
#include "usb.h"
#include "midi_link_desc.h"
#include "usb_midi.h"

/* fixed descriptors in flash */
const PROGMEM usb_device_descriptor_t usb_device_descriptor = {
  sizeof(usb_device_descriptor_t), /* bLength */
  USB_DESCRIPTOR_DEVICE,           /* bDescriptorType */
  USB_VERSION,                     /* bcdUSB */
  /* specified in interface description */
  USB_CLASS_UNDEFINED,             /* bDeviceClass */
  USB_SUBCLASS_UNDEFINED,          /* bDeviceSubclass */
  USB_PROTOCOL_UNDEFINED,          /* bDeviceProtocol */
  D12_EP0_SIZE,       /* bMaxPacketSize (8 bytes) */
  USB_ID_VENDOR,                   /* idVendor (from config.h) */
  USB_ID_PRODUCT,                  /* idProduct (from config.h) */
  USB_ID_RELEASE,                  /* bcdDevice (from config.h) */
#ifdef USB_STR_MANUFACTURER
  1,                               /* iManufacturer String Index */
#else
  0,
#endif
#ifdef USB_STR_PRODUCT
  2,                               /* iProduct String Index */
#else
  0,
#endif
#ifdef USB_STR_SERIAL
  3,                               /* iSerialNumber String Index */
#else
  0,
#endif
  1                                /* bNumberConfigurations */
};

/* the complete configuration */
const PROGMEM usb_app_config_t usb_configuration_descriptor = {
  {                                         /* configuration descriptor */
    sizeof(usb_configuration_descriptor_t), /* bLength */
    USB_DESCRIPTOR_CONFIGURATION,           /* bDescriptorType */
    sizeof(usb_app_config_t),               /* wTotalLength */
    2,                                      /* bNumInterfaces */
    1,                                      /* bConfigurationValue */
    0,                                      /* iConfiguration String Index */
    USB_STATUS_SELF_POWERED,                /* bmAttributes */ 
    0                                       /* bMaxPower 0mA */
  },

  /* audio control interface */
  {
    /* interface descriptor */
    sizeof(usb_interface_descriptor_t),     /* bLength */
    USB_DESCRIPTOR_INTERFACE,               /* bDescriptorType */
    0,                                      /* bInterface Number */
    0,                                      /* bAlternateSetting */
    0,                                      /* bNumeps */
    USB_CLASS_AUDIO,                        /* bInterfaceClass */
    USB_SUBCLASS_AUDIO_CONTROL,             /* bInterfaceSubClass */
    USB_PROTOCOL_UNDEFINED,                 /* bInterfaceProtocol */
    0,                                      /* iInterface String Index */
  },
  {
    /* class specific interface descriptor */
    sizeof(usb_audiocontrol_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_HEADER,
    0x100, /* revision of class spec - 1.0 */
    sizeof(usb_audiocontrol_descriptor_t),
    1, /* number of streaming interfaces */
    1  /* midistreaming interface 1 belongs to this AC interface */
  },

  /* midi streaming interface */
  {
    /* interface descriptor */
    sizeof(usb_interface_descriptor_t),     /* bLength */
    USB_DESCRIPTOR_INTERFACE,               /* bDescriptorType */
    1,                                      /* bInterface Number */
    0,                                      /* bAlternateSetting */
    2,                                      /* bNumeps */
    USB_CLASS_AUDIO,                        /* bInterfaceClass */
    USB_SUBCLASS_MIDI_STREAMING,            /* bInterfaceSubClass */
    USB_PROTOCOL_UNDEFINED,                 /* bInterfaceProtocol */
    0,                                      /* iInterface String Index */
  },
  {
    /* class specific interface descriptor */
    sizeof(usb_midistreaming_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_HEADER,
    0x100, /* revision of class spec - 1.0 */
    sizeof(usb_midistreaming_descriptor_t) +
    sizeof(usb_midistreaming_injack_descriptor_t) * 2 + 
    sizeof(usb_midistreaming_outjack_descriptor_t) * 2 +
    ( sizeof(usb_endpoint_descriptor_t) + 
      sizeof(usb_midistreaming_ep_descriptor_t) ) * 2
  },
  /* in jack embedded and external */
  {
    sizeof(usb_midistreaming_injack_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_IN_JACK,
    USB_MIDI_JACK_EMBEDDED,
    1, /* jackId */
    0
  },
  {
    sizeof(usb_midistreaming_injack_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_IN_JACK,
    USB_MIDI_JACK_EXTERNAL,
    2, /* jackId */
    0
  },

  /* out jack embedded and external */
  {
    sizeof(usb_midistreaming_outjack_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_OUT_JACK,
    USB_MIDI_JACK_EMBEDDED,
    3, /* jackId */
    1, /* number of input pins */
    2, /* ID of the entity to which this pin is connected */
    1, /* output pin number of the entity to which this input pin is connected */
    0
  },
  {
    sizeof(usb_midistreaming_outjack_descriptor_t),
    USB_DESCRIPTOR_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_OUT_JACK,
    USB_MIDI_JACK_EXTERNAL,
    4, /* jackId */
    1, /* number of input pins */
    1, /* ID of the entity to which this pin is connected */
    1, /* output pin number of the entity to which this input pin is connected */
    0
  },


  /* standard bulk out descriptor */
  {
    sizeof(usb_endpoint_descriptor_t),     /* bLength */
    USB_DESCRIPTOR_ENDPOINT,               /* bDescriptorType */
    MIDI_EP_OUT + USB_DIR_OUT,                       /* bep Address EP1 OUT */
    USB_EP_TYPE_BULK,                      /* bmAttributes */
    64,                       /* wMaxPacketSize */
    0,                                      /* bInterval */
    0,
    0
  },
  /* midi adapter class specific out endpoint */
  {
    sizeof(usb_midistreaming_ep_descriptor_t),
    USB_DESCRIPTOR_ENDPOINT_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_GENERAL,
    1, /* number of embedded MIDI in jacks */
    1 /* ID of the embedded midi in jack */
  },

  /* standard bulk in descriptor */
  {
    sizeof(usb_endpoint_descriptor_t),     /* bLength */
    USB_DESCRIPTOR_ENDPOINT,               /* bDescriptorType */
    MIDI_EP_IN + USB_DIR_IN,                        /* bep Address EP1 OUT */
    USB_EP_TYPE_BULK,                      /* bmAttributes */
    64,                       /* wMaxPacketSize */
    0,                                      /* bInterval */
    0,
    0
  },
  /* midi adapter class specific out endpoint */
  {
    sizeof(usb_midistreaming_ep_descriptor_t),
    USB_DESCRIPTOR_ENDPOINT_CLASS_SPECIFIC,
    USB_DESCRIPTOR_SUBTYPE_MIDI_GENERAL,
    1, /* number of embedded MIDI out jacks */
    3 /* ID of the embedded midi out jack */
  }
};
