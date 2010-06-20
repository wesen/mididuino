/**# General USB structures (not atmel specific).
 **
 ** Contains structure definitions for general usb descriptors, and
 ** masks and type definition macros for USB parsing.
 **/

#ifndef USB_H__
#define USB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define PACKED __attribute__ ((packed))

/** type masks **/
#define USB_REQ_REPLY                    0x80 /* reply from device */
#define USB_REQ_TYPE_MASK                0x60
#define USB_REQ_STANDARD                 0x00
#define USB_REQ_VENDOR                   0x40 /* vendor specific request */
#define USB_REQ_CLASS                    0x20 /* class specific request */

#define USB_REQ_TARGET_MASK              0x0F
#define USB_REQ_OTHER                    0x03
#define USB_REQ_EP                       0x02
#define USB_REQ_IF                       0x01
#define USB_REQ_DEV                      0x00

#define USB_REQ_IS_REPLY(req)   ((req).bmRequestType == USB_REQ_REPLY)
#define USB_REQ_IS_REPLY(req)   ((req).bmRequestType == USB_REQ_REPLY)


/** generic request codes **/
#define USB_REQ_GET_STATUS               0x00
#define USB_REQ_CLEAR_FEATURE            0x01
#define USB_REQ_SET_FEATURE              0x03
#define USB_REQ_SET_ADDRESS              0x05
#define USB_REQ_GET_DESCRIPTOR           0x06
#define USB_REQ_SET_DESCRIPTOR           0x07
#define USB_REQ_GET_CONFIGURATION        0x08
#define USB_REQ_SET_CONFIGURATION        0x09
#define USB_REQ_GET_INTERFACE            0x0a
#define USB_REQ_SET_INTERFACE            0x0b
#define USB_REQ_SYNCH_FRAME              0x0c

/** hid request codes **/
#define USB_HID_GET_REPORT               0x01
#define USB_HID_GET_IDLE                 0x02
#define USB_HID_GET_PROTOCOL             0x03
#define USB_HID_SET_REPORT               0x09
#define USB_HID_SET_IDLE                 0x0a
#define USB_HID_SET_PROTOCOL             0x0b

/** descriptor codes **/
#define USB_DESCRIPTOR_DEVICE            0x01
#define USB_DESCRIPTOR_CONFIGURATION     0x02
#define USB_DESCRIPTOR_STRING            0x03
#define USB_DESCRIPTOR_INTERFACE         0x04
#define USB_DESCRIPTOR_ENDPOINT          0x05

/* usb endpoint types */
#define USB_EP_TYPE_CONTROL     0
#define USB_EP_TYPE_ISOCHRONOUS 1
#define USB_EP_TYPE_BULK        2
#define USB_EP_TYPE_INTERRUPT   3

/** classes **/
#define USB_CLASS_UNDEFINED         0x00
#define USB_SUBCLASS_UNDEFINED      0x00

/** HID **/
#define USB_CLASS_HID               0x03
#define USB_SUBCLASS_BOOT_PROTOCOL  0x01

/** AUDIO **/
#define USB_CLASS_AUDIO             0x01
#define USB_SUBCLASS_AUDIO_CONTROL  0x01
#define USB_SUBCLASS_MIDI_STREAMING 0x03

/** protocols **/
#define USB_PROTOCOL_UNDEFINED      0x00
#define USB_PROTOCOL_KEYBOARD       0x01
#define USB_PROTOCOL_MOUSE          0x02


/** class specific descriptor **/
#define USB_DESCRIPTOR_HID	               0x21
#define USB_DESCRIPTOR_REPORT                  0x22
#define USB_DESCRIPTOR_CLASS_SPECIFIC          0x24
#define USB_DESCRIPTOR_ENDPOINT_CLASS_SPECIFIC 0x25

#define USB_DESCRIPTOR_SUBTYPE_HEADER          0x01

#define USB_DESCRIPTOR_SUBTYPE_MIDI_GENERAL    0x01

#define USB_DESCRIPTOR_SUBTYPE_MIDI_HEADER     0x01
#define USB_DESCRIPTOR_SUBTYPE_MIDI_IN_JACK    0x02
#define USB_DESCRIPTOR_SUBTYPE_MIDI_OUT_JACK   0x03
#define USB_DESCRIPTOR_SUBTYPE_ELEMENT         0x04

/** usb direction flags **/
#define USB_DIR_IN  0x80
#define USB_DIR_OUT 0x00

/** usb status flags **/
#define USB_STATUS_SELF_POWERED   0x01
#define USB_STATUS_REMOTE_WAKEUP  0x02

/** string descriptor indexes **/
#define USB_LANG_INDEX         0x00
#define USB_MANUFACTURER_INDEX 0x01
#define USB_PRODUCT_INDEX      0x02
#define USB_SERIAL_INDEX       0x03

/** USB features **/
#define USB_FEATURE_EP_HALT           0x00
#define USB_FEATURE_DEV_REMOTE_WAKEUP 0x01

/**## Usb packets
 **/

typedef struct {
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} PACKED usb_setup_request_t;

/**## Usb decsriptor structures.
 **/

/** USB device descriptor **/
typedef struct {
  uint8_t bLength;                    /* sizeof(usb_device_descriptor_t) */
  uint8_t bDescriptorType;            /* USB_DESCRIPTOR_DEVICE */
  uint16_t bcdUSB;                     /* usb version */
  uint8_t bDeviceClass;               /* class code */
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} PACKED usb_device_descriptor_t;

/** USB configuration descriptor **/
typedef struct {
  uint8_t bLength;                    /* sizeof(usb_configuration_descriptor_t) */
  uint8_t bDescriptorType;            /* USB_DESCRIPTOR_CONFIGURATION */
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t MaxPower;
} PACKED usb_configuration_descriptor_t;

/** USB interface descriptor **/
typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;          /* USB_SCRIPTOR_INTERFACE */
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} PACKED usb_interface_descriptor_t;

typedef struct {
  uint8_t  index;
  uint8_t  size;
  uint8_t  *str;
} usb_string_list_t;

typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;            /* USB_DESCRIPTOR_STRING */
  uint16_t wLangId;                    /* USB_LANGID */
} PACKED usb_language_id_descriptor_t;

/** USB endpoint descriptor. **/
typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmaxpacketsize0;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
  uint8_t  bRefresh;
  uint8_t  bSynAddress;
} PACKED usb_endpoint_descriptor_t;

/**## HID class specific structures
 **/
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;            // USB_DESCRIPTOR_HID
  uint16_t wHIDClassSpecComp;
  uint8_t  bCountry;
  uint8_t  bNumDescriptors;
  uint8_t  b1stDescType;
  uint16_t w1stDescLength;
} PACKED usb_hid_descriptor_t;


#define USB_LANGID_US_EN  0x0409

extern const PROGMEM usb_device_descriptor_t usb_device_descriptor;

/* user config descriptor */
#include "usb_desc.h"

#ifdef __cplusplus
}
#endif


#endif /* USB_H__ */
