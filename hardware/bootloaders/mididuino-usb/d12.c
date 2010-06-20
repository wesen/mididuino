/**# Communication with PDIUSBD12
 **
 **/

#include "app.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdlib.h>

#include "d12.h"
#include "usb.h"
#include "midi_link_desc.h"

#define NOINLINE __attribute__ ((noinline))
#define SMALLNOP { asm("nop"); asm("nop");  }
#define NOP _delay_us(0.6)

/**
 ** Configured USB device address, set during enumeration
 **/
static uint8_t d12_dev_address = 0;

/**
 ** D12 chip configuration flags. These flags are set mostly during enumeration
 ** according to messages received from the server. Flags are also used to signal a
 ** suspend state.
 **/

static uint8_t d12_flags = 0;

uint8_t ep_buf[2];
uint8_t ep_status = 0;

/** Device has been configured during enumeration.
 **/
#define D12_FLAG_DEV_CONFIGURED 0x01
/**
 ** Device address received, need to set correct address.
 **/
#define D12_FLAG_TX_ADDRESS     0x02
/**
 ** Read data to be sent on endpoint 0 from flash memory and not from working memory
 **/
#define D12_FLAG_FLASH          0x04
/**
 ** Need to enable endpoints (configuration successfull.
 **/
#define D12_FLAG_ENABLE_EP      0x08
/**
 ** USB bus has gone into suspend, suspend flash.
 ** XXX need to set INT0 interrupt to wake up.
 **/
#define D12_FLAG_SUSPEND        0x10


/** Send buffer for ep0, because messages can be split between different interrupt
 ** states.
 **/
static uint8_t *tx_ptr;
static uint16_t tx_bytes;

/**
 ** Check the suspend line from D12.
 **/
static inline uint8_t d12_is_suspend(void) {
  return IS_PIN_SET(SUSPEND_PORT, SUSPEND_PIN);
}

/**
 ** Check the interruptline from d12 (low active).
 **/
static inline uint8_t d12_is_irq(void) {
  return !IS_PIN_SET(IRQ_PORT, IRQ_PIN);
}

/**
 ** External access to configured flag.
 **/
uint8_t d12_device_is_configured(void) {
  return (d12_flags & D12_FLAG_DEV_CONFIGURED) && !(d12_flags & D12_FLAG_ENABLE_EP);
}

/**## Communication with D12
 **
 ** Write and read the data lines to d12, and bitbang RD, WR and A0 accordingly.
 **/

/** Set the IO pins to output and write a byte to the data lines **/

void inline d12_write_to_output(uint8_t data) {
  D12_WRITE(data);
}

/**
 ** Switch the IO pins to input. **/
static inline void d12_set_to_input(void) {
  D12_SET_INPUT();
}

/** Initialize the pins needed for the communication with D12.
 **/
void d12_pins_init(void) {
  INIT_PIN_OUTPUT(A0_PORT, A0_PIN);
  INIT_PIN_OUTPUT(RD_PORT, RD_PIN);
  INIT_PIN_OUTPUT(WR_PORT, WR_PIN);
  INIT_PIN_OUTPUT(SUSPEND_PORT, SUSPEND_PIN);

  INIT_PIN_INPUT(IRQ_PORT, IRQ_PIN);
  CLEAR_PIN(IRQ_PORT, IRQ_PIN);
}

static void d12_write_data(uint8_t data);

/** Write a command byte to d12. **/
void d12_write_cmd_byte(uint8_t cmd) {
  SET_A0();
  CLEAR_WR();
  d12_write_to_output(cmd);
  SMALLNOP;
  SET_WR();
  NOP;
}

/** Write a command and its data to D12. **/
void d12_write_cmd(uint8_t cmd, uint8_t *data, uint8_t cnt) {
  d12_write_cmd_byte(cmd);
  uint8_t i;
  for (i = 0; i < cnt; i++) {
    d12_write_data(data[i]);
  }
}

NOINLINE void d12_write_cmd_1_byte(uint8_t cmd, uint8_t byte) {
  d12_write_cmd_byte(cmd);
  d12_write_data(byte);
}

NOINLINE void d12_write_cmd_1(uint8_t cmd) {
  d12_write_cmd_1_byte(cmd, 1);
}

/** Write a command and its data (from flash) to D12. **/
void d12_write_cmd_flash(uint8_t cmd, uint8_t *data, uint8_t cnt) {
  d12_write_cmd_byte(cmd);
  uint8_t i;
  for (i = 0; i < cnt; i++) {
    d12_write_data(pgm_read_byte(data + i));
  }
}

/** Write a data byte to D12. **/
NOINLINE static void d12_write_data(uint8_t data) {
  CLEAR_A0();
  CLEAR_WR();
  d12_write_to_output(data);
  SMALLNOP;
  SET_WR();
  NOP;
}

/** Read a data byte from D12. **/
NOINLINE static uint8_t d12_read_data(void) {
  d12_set_to_input();
  CLEAR_A0();
  CLEAR_RD();
  SMALLNOP;
  uint8_t data = D12_READ();
  SET_RD();
  NOP;
  return data;
}

/** Write a command byte to D12 and read answer data. **/
void d12_read_cmd(uint8_t cmd, uint8_t *buf, uint8_t cnt) {
  d12_write_cmd_byte(cmd);

  uint8_t i;
  for (i = 0; i < cnt; i++) {
    buf[i] = d12_read_data();
  }
}

NOINLINE uint8_t d12_read_cmd_1(uint8_t cmd) {
  d12_write_cmd_byte(cmd);
  uint8_t ret = d12_read_data();
  return ret;
}

uint8_t d12_write_ep_buf(uint8_t ep, uint8_t cnt) {
  /* select endpoint */
  uint8_t status = 0;
  uint8_t cnt2 = 0;
  do {
    status = d12_read_cmd_1(D12_CMD_SELECT_EP + ep);
    if (status & 1)
      _delay_us(2);
    cnt2++;
  } while ((status & 1) && (cnt2 < 128));
  if (status & 1)
    return 0;
  
  d12_write_cmd_byte(D12_CMD_WRITE_BUFFER);
  d12_write_data(0x00);
  d12_write_data(cnt);
  return 1;
}

/** Write a data packet to an endpoint of D12, validate the buffer afterwards.
 ** Returns 1 if successful, 0 if endpoint full.
 **/
uint8_t d12_write_ep_pkt(uint8_t ep, uint8_t *buf, uint8_t cnt) {
  if (!d12_write_ep_buf(ep, cnt))
    return 0;
  uint8_t i;
  for (i = 0; i < cnt; i++) {
    d12_write_data(buf[i]);
  }
  d12_write_cmd_byte(D12_CMD_VALIDATE_BUFFER);
  return 1;
}

NOINLINE void d12_write_ep0_pkt_ep_buf(uint8_t cnt) {
  d12_write_ep_pkt(D12_EP0_IN, ep_buf, cnt);
}

NOINLINE void d12_write_ep_pkt_0(void) {
  d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
}

/** Write a data packet form flash to D12 endpoint.
 ** Returns 1 if successful, 0 if endpoint full.
 **/
uint8_t d12_write_ep_pkt_flash(uint8_t ep, uint8_t *buf, uint8_t cnt) {
  if (!d12_write_ep_buf(ep, cnt)) {
    return 0;
  }
  uint8_t i;
  for (i = 0; i < cnt; i++) {
    d12_write_data(pgm_read_byte(buf + i));
  }
  d12_write_cmd_byte(D12_CMD_VALIDATE_BUFFER);
  return 1;
}


/** Read an endpoint of D12 and clear the buffer afterwards.
 ** Returns the received length.
 **/
uint8_t d12_read_ep_pkt(uint8_t ep, uint8_t *buf, uint8_t cnt) {
  /* select endpoint */
  uint8_t status = d12_read_cmd_1(D12_CMD_SELECT_EP + ep);
  if (status & 0x01) {
    /* data available */
    uint8_t len[2];
    d12_read_cmd(D12_CMD_READ_BUFFER, len, 2);
    if (len[1]) {
      if (len[1] > cnt)
	len[1] = cnt;
      uint8_t i;
      for (i = 0; i < len[1]; i++) {
	buf[i] = d12_read_data();
      }
      d12_write_cmd_byte(D12_CMD_CLEAR_BUFFER);
    }
    return len[1];
  } else {
    return 0;
  }
}

/**
 ** Write the ep0 buffer to endpoint 0.
 **/
void d12_write_ep0_buf(void) {
  uint16_t to_send = 0;

  if (tx_bytes == 0) {
    // send empty packet ??
    return;
  }
  
  to_send = tx_bytes;
  if (to_send > D12_EP0_SIZE)
    to_send = D12_EP0_SIZE;

  if (d12_flags & D12_FLAG_FLASH) 
    d12_write_ep_pkt_flash(D12_EP0_IN, tx_ptr, to_send);
  else
    d12_write_ep_pkt(D12_EP0_IN, tx_ptr, to_send);

  tx_ptr += to_send;
  tx_bytes -= to_send;

}

/** Stall the control endpoint (in case a command can't be answered. **/
NOINLINE void d12_stall_ep0(void) {
  /* stall pid in response to next DATA stage TX */
  d12_write_cmd_1(D12_CMD_SET_EP_STATUS + D12_EP0_IN);
  /* stall pid in response to the next status stage */
  d12_write_cmd_1(D12_CMD_SET_EP_STATUS + D12_EP0_OUT);
}

/** Stall an endpoint. **/
NOINLINE void d12_stall_ep(uint8_t ep) {
  d12_read_cmd_1(D12_CMD_READ_LAST_TX_STATUS + ep);
  d12_write_cmd_1(D12_CMD_SET_EP_STATUS + ep);
}

/** Initialize the D12 stack. **/
void d12_init(void) {
  uint8_t buf[2];

  // no soft connect
  buf[0] =
      _BV(1) // no lazyclock
    | _BV(2) // clock running, even during suspend
    | _BV(3) // interrupt mode
    | _BV (4) // softconnect
    // mode 0
    ;
  //  buf[1] = 0x2; // clock division 48 / (2 + 1) = 16 Mhz
  buf[1] = 0x2;
  d12_write_cmd(D12_CMD_SET_MODE, buf, 2);

  //  d12_write_cmd_1_byte(D12_CMD_SET_EP_STATUS + D12_EP0_IN, 0);
  /* stall pid in response to the next status stage */
  //  d12_write_cmd_1_byte(D12_CMD_SET_EP_STATUS + D12_EP0_OUT, 0);
		  
}

/** Structure to hold the callbacks for different D12 interrupt status. **/
typedef struct {
  uint8_t mask;
  void (*function)(uint8_t);
  uint8_t param;
} usb_event_list_t;

/** Switch to atmel power saving mode.
 ** XXX need toenable interruptson INT0 here!
 **/
void set_power_save(void) {
  MCUCR = _BV(SE) | _BV(SM0) | _BV(SM1) | (MCUCR & 0x0F);
}

/**
 ** Handler for USB suspend state. Switch to poewr save mode.
 ** XXX interrupt handler for INT0
 **/
void d12_suspend_handler(void) {
  cli();
  if (!(d12_flags & D12_FLAG_SUSPEND))
    goto end;
  
  if (!d12_is_suspend())
    goto end;
  sei();
  _delay_ms(1);
  if (!d12_is_suspend())
    goto end;

  set_power_save();
  sleep_cpu();

  cli();

 end:
  sei();
  return;
}

/** Reset the D12 stack after a bus reset.
 **/
void d12_bus_reset(uint8_t i) {
  d12_flags = 0;
  tx_bytes = 0;
  tx_ptr = NULL;

  /* set address and enable */
  d12_write_cmd_1_byte(D12_CMD_SET_ADDRESS, 0x80);

  /* enable generic endpoints */
  d12_write_cmd_1(D12_CMD_SET_EP_ENABLE);
}

/**## USB enumeration handling.
 **
 ** The USB enumeration process is mostly the same as on the AT90USB1286, except
 ** that interrupts form the USB contorller have to be fetched explicitly and acted upon.
 ** This dispatching is done through an event list with callback function pointers.
 **/

/** Generic endpoint callback, just clear interrupt status, don't answer. **/
void d12_generic_ep_callback(uint8_t ep) {
  d12_read_cmd_1(D12_CMD_READ_LAST_TX_STATUS + ep);
}

#ifndef D12_EP2_IN_CALLBACK
#define D12_EP2_IN_CALLBACK d12_generic_ep_callback
#endif /* D12_EP2_IN_CALLBACK */
#ifndef D12_EP2_OUT_CALLBACK
#define D12_EP2_OUT_CALLBACK d12_generic_ep_callback
#endif /* D12_EP2_OUT_CALLBACK */

#ifndef D12_EP1_IN_CALLBACK
#define D12_EP1_IN_CALLBACK d12_generic_ep_callback
#endif /* D12_EP1_IN_CALLBACK */
#ifndef D12_EP1_OUT_CALLBACK
#define D12_EP1_OUT_CALLBACK d12_generic_ep_callback
#endif /* D12_EP1_OUT_CALLBACK */

/** Standard string descriptor. **/
static const PROGMEM usb_language_id_descriptor_t usb_langid_string = {
  sizeof(usb_language_id_descriptor_t),    /* bLength */
  USB_DESCRIPTOR_STRING,                   /* bDescriptorType */
  USB_LANGID_US_EN                         /* wLANGID */
};

#ifdef BOOTLOADER
#include "midi_link_str_boot.h"
#else
#include "midi_link_str.h"
#endif

/** USB String list to answer GET_STRING_DESCRIPTOR requests. The strings
 ** themselves are in midi_link_str.h
 **/
static const PROGMEM usb_string_list_t usb_string_list[] = {
  { 0, sizeof(usb_langid_string), (uint8_t PROGMEM *)&usb_langid_string },
#ifdef USB_STR_MANUFACTURER
  { 1, sizeof(usb_manufacturer_string), usb_manufacturer_string },
#endif
#ifdef USB_STR_PRODUCT
  { 2, sizeof(usb_product_string), usb_product_string },
#endif
#ifdef USB_STR_SERIAL
  { 3, sizeof(usb_serial_string), usb_serial_string },
#endif
#ifdef USB_STR_CONFIGURATION
  { 4, sizeof(usb_configuration_string), usb_configuration_string },
#endif
#ifdef USB_STR_INTERFACE
  { 5, sizeof(usb_interface_string), usb_interface_string },
#endif
};

/** Answer a GET_DESCRIPTOR request on ep0.
 **/
void d12_get_descriptor(usb_setup_request_t *setup_pkt) {
  tx_ptr = NULL;
  tx_bytes = 0;
  d12_flags &= ~D12_FLAG_FLASH;

  uint8_t value = setup_pkt->wValue >> 8;
  
  switch (value) {
  case USB_DESCRIPTOR_DEVICE:
    tx_ptr = (uint8_t *)&usb_device_descriptor;
    tx_bytes = sizeof(usb_device_descriptor);
    d12_flags |= D12_FLAG_FLASH;
    break;

  case USB_DESCRIPTOR_CONFIGURATION:
    tx_ptr = (uint8_t *)&usb_configuration_descriptor;
    tx_bytes = sizeof(usb_configuration_descriptor);
    d12_flags |= D12_FLAG_FLASH;
    break;
    
  case USB_DESCRIPTOR_STRING:
    {
      uint8_t i;
      for (i = 0; i < sizeof(usb_string_list) / sizeof(usb_string_list_t); i++) {
	const usb_string_list_t *list = usb_string_list + i;
	if (pgm_read_byte(&list->index) == (setup_pkt->wValue & 0xFF)) {
	  tx_ptr = GET_FLASH_PTR(&list->str);
	  tx_bytes = pgm_read_byte(&list->size);
	  d12_flags |= D12_FLAG_FLASH;
	}
      }
      break;
    }
  }

  if (tx_bytes) {
    if (tx_bytes > setup_pkt->wLength)
      tx_bytes = setup_pkt->wLength;
    d12_write_ep0_buf();
  } else {
    d12_stall_ep0();
  }
}

/** Callback for Endpoint 0 IN, write any data available if necessary,
 ** else act according to D12 flags.  This interrupt is called
 ** periodically apparently, but maybe we should move the code to
 ** themain loop.
 **/
void d12_ep0_in_callback(uint8_t ep) {
  uint8_t status = d12_read_cmd_1(D12_CMD_READ_LAST_TX_STATUS + D12_EP0_IN);
  /* discard status */
  status = 0;

  if (d12_flags & D12_FLAG_TX_ADDRESS) {
  } else if (d12_flags & D12_FLAG_ENABLE_EP) {
    /* enable generic endpoints */
    d12_write_cmd_1_byte(D12_CMD_SET_EP_ENABLE, d12_flags & D12_FLAG_DEV_CONFIGURED ? 1 : 0);
    d12_flags &= ~D12_FLAG_ENABLE_EP;
  } else {
    /* data tx may be in progress, send more */
    d12_write_ep0_buf();
  }
}

/** Handle a DEVICE setup packet. **/
void d12_handle_dev_setup_pkt(usb_setup_request_t *setup_pkt) {
  switch (setup_pkt->bRequest) {
  case USB_REQ_GET_STATUS:
    ep_buf[0] = USB_STATUS_SELF_POWERED;
    ep_buf[1] = 0x00;
    d12_write_ep0_pkt_ep_buf(2);
    break;

  case USB_REQ_SET_ADDRESS:
    d12_dev_address = setup_pkt->wValue | 0x80;
    d12_write_cmd_1_byte(D12_CMD_SET_ADDRESS, d12_dev_address);
    //    d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
    d12_write_ep_pkt_0();
    break;

  case USB_REQ_GET_DESCRIPTOR:
    d12_get_descriptor(setup_pkt);
    break;

  case USB_REQ_GET_CONFIGURATION:
    if (d12_flags & D12_FLAG_DEV_CONFIGURED)
      ep_buf[0] = 1;
    else
      ep_buf[0] = 0;
    d12_write_ep0_pkt_ep_buf(1);
    break;

  case USB_REQ_SET_CONFIGURATION:
    if (setup_pkt->wValue & 0xFF) {
      d12_flags |= D12_FLAG_DEV_CONFIGURED | D12_FLAG_ENABLE_EP;
    }
    else {
      d12_flags &= ~D12_FLAG_DEV_CONFIGURED | D12_FLAG_ENABLE_EP;
    }
    //    d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
    d12_write_ep_pkt_0();
    break;

  case USB_REQ_SET_DESCRIPTOR:
  case USB_REQ_CLEAR_FEATURE:
  case USB_REQ_SET_FEATURE:
  default:
    d12_stall_ep0();
  }
}

/** Handle an interface setup packet. **/
void d12_handle_interface_setup_pkt(usb_setup_request_t *setup_pkt) {
  switch (setup_pkt->bRequest) {
  case USB_REQ_GET_STATUS:
    ep_buf[0] = 0x00;
    ep_buf[1] = 0x00;
    d12_write_ep0_pkt_ep_buf(2);
    break;

  case USB_REQ_SET_INTERFACE:
    /* support only first interface */
    if ((setup_pkt->wIndex == 0) && (setup_pkt->wValue == 0))
      d12_write_ep_pkt_0();
    //      d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
    else
      d12_stall_ep0();
    break;

  case USB_REQ_GET_DESCRIPTOR:
    d12_get_descriptor(setup_pkt);
    break;

  case USB_REQ_GET_INTERFACE:
    if (setup_pkt->wIndex == 0) {
      ep_buf[0] = 0;
      d12_write_ep0_pkt_ep_buf(1);
    } else {
      d12_stall_ep0();
    }
    break;

  case USB_REQ_CLEAR_FEATURE:
  case USB_REQ_SET_FEATURE:
  default:
    d12_stall_ep0();
    break;
  }
}

/** Handle an endpoint setup packet. **/
void d12_handle_ep_setup_pkt(usb_setup_request_t *setup_pkt) {
  switch (setup_pkt->bRequest) {
  case USB_REQ_CLEAR_FEATURE:
  case USB_REQ_SET_FEATURE:
    #if 0
    /* supports only STALL feature */
    if (setup_pkt->wValue == USB_FEATURE_EP_HALT) {
      uint8_t ep = setup_pkt->wIndex & 0x7F;
      if (ep > 2) {
	d12_stall_ep0();
      } else {	
	uint8_t sep = D12_CMD_SET_EP_STATUS + ep * 2;
	if (setup_pkt->wIndex & USB_DIR_IN)
	  ep++;
	d12_write_cmd_1_byte(sep, setup_pkt->bRequest == USB_REQ_CLEAR_FEATURE ? 0 : 1);
	d12_write_ep_pkt_0();
	//	d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
      }
    } else {
      d12_stall_ep0();
    }
#endif
    d12_stall_ep0();
    break;

  case USB_REQ_GET_STATUS:
    {
      /* return 1 if endpoint is stalled */
      uint8_t ep = setup_pkt->wIndex & 0x7F;
      if (ep > 2) {
	d12_stall_ep0();
      } else {
	// XXX ?? buggy because pkt_ep_buf(2) 
	uint8_t rep = D12_CMD_READ_EP_STATUS + ep * 2;
	if (setup_pkt->wIndex & USB_DIR_IN)
	  ep++;
	uint8_t status = d12_read_cmd_1(rep);
	ep_buf[1] = 0;
	if (status & 0x80) {
	  ep_buf[0] = 1; /* endpoint is stalled */
	} else {
	  ep_buf[0] = 0;
	}
	d12_write_ep0_pkt_ep_buf(2);
      }
    }
    break;

  default:
    d12_stall_ep0();
    break;
  }
}

/** Dispatch an incoming setup packet, ignore vendor and class requests (not needed for MIDI).
 **/
void d12_handle_setup_pkt(usb_setup_request_t *setup_pkt) {
  switch (setup_pkt->bmRequestType & USB_REQ_TYPE_MASK) {
  case USB_REQ_STANDARD:
    switch (setup_pkt->bmRequestType & USB_REQ_TARGET_MASK) {
    case USB_REQ_DEV:
      d12_handle_dev_setup_pkt(setup_pkt);
      break;

    case USB_REQ_IF:
      d12_handle_interface_setup_pkt(setup_pkt);
      break;

    case USB_REQ_EP:
      d12_handle_ep_setup_pkt(setup_pkt);
      break;

    default:
      goto stall;
    }
    break;

  case USB_REQ_VENDOR:
  case USB_REQ_CLASS:
    d12_write_ep_pkt_0();
    //    d12_write_ep_pkt(D12_EP0_IN, NULL, 0);
    break;

  default:
    goto stall;
  }
  return;

 stall:
  d12_stall_ep0();
}

/** Receive a packet on endpoint 0 (only act on setup packets).
 **/
void d12_ep0_out_callback(uint8_t ep) {
  uint8_t status = d12_read_cmd_1(D12_CMD_READ_LAST_TX_STATUS + D12_EP0_OUT);

  if (status & 0x20) {
    usb_setup_request_t setup_pkt;
    /* setup packet */
    d12_read_ep_pkt(D12_EP0_OUT, (uint8_t *)&setup_pkt, sizeof(setup_pkt));
    static const uint8_t cmds[5] = { D12_CMD_ACK_SETUP,
			D12_CMD_CLEAR_BUFFER,
			D12_CMD_SELECT_EP + D12_EP0_IN,
			D12_CMD_ACK_SETUP,
			D12_CMD_CLEAR_BUFFER };
    uint8_t i;
    for (i = 0; i < 5; i++) {
      d12_write_cmd_byte(cmds[i]);
    }

#if 0
    /* ack setup pkt */
    d12_write_cmd(D12_CMD_ACK_SETUP, NULL, 0);
    /* flush buffer after ack */
    d12_write_cmd(D12_CMD_CLEAR_BUFFER, NULL, 0);
    d12_write_cmd(D12_CMD_SELECT_EP + D12_EP0_IN, NULL, 0);
    d12_write_cmd(D12_CMD_ACK_SETUP, NULL, 0);
    d12_write_cmd(D12_CMD_CLEAR_BUFFER, NULL, 0);
#endif

    d12_handle_setup_pkt(&setup_pkt);
  }
}

/** Interrupt state callback list. **/
static const PROGMEM usb_event_list_t event_list[] = {
  //  { 0x80, d12_suspend_callback, 0 },
  { 0x40, d12_bus_reset, 0 },
  { 0x20, D12_EP2_IN_CALLBACK, D12_EP2_IN },
  { 0x10, D12_EP2_OUT_CALLBACK, D12_EP2_OUT },
  { 0x08, D12_EP1_IN_CALLBACK, D12_EP1_IN },
  { 0x04, D12_EP1_OUT_CALLBACK, D12_EP1_OUT },
  { 0x02, d12_ep0_in_callback, D12_EP0_IN },
  { 0x01, d12_ep0_out_callback, D12_EP0_OUT },
  //  { 0x02, d12_generic_ep_callback, D12_EP0_IN },
  //  { 0x01, d12_generic_ep_callback, D12_EP0_OUT },
};

/** Main routine for d12, poll interrupts and check suspend state.
 **/
void d12_main(void) {
  while (d12_is_irq()) {
    uint8_t irq[2];
    d12_read_cmd(D12_CMD_READ_INT_REG, irq, 2);
    
    uint8_t i;
    for (i = 0; i < sizeof(event_list) / sizeof(usb_event_list_t); i++) {
      const usb_event_list_t *event = event_list + i;
      if (irq[0] & GET_FLASH(&(event->mask))) {
	void (*function)(uint8_t);
	function = GET_FLASH_PTR(&(event->function));
	function(GET_FLASH(&(event->param)));
      }
    }
  }
  // d12_suspend_handler();
}

void d12_suspend_callback(uint8_t bla) {
#if 0
  if (d12_is_suspend()) {
    d12_flags |= D12_FLAG_SUSPEND;
  } else {
    d12_flags &= ~D12_FLAG_SUSPEND;
  }
#endif
}

