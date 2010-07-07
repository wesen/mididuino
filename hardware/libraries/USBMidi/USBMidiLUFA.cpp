#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include <midi-common.h>
#include <MidiUart.h>
#include "USBMidi.h"

void USBMidi::sendBuf() {
  Endpoint_SelectEndpoint(inEP);

  if (Endpoint_IsINReady()) {
		if (midi_buf_len > 0) {
			Endpoint_Write_Stream_LE(midi_buf, midi_buf_len);
			// XXX ret code checking
			midi_buf_len = 0;
			
			/* Send the data in the endpoint to the host */
			Endpoint_ClearIN();
		}
	}
}

void USBMidi::poll() {
  /* Device must be connected and configured for the task to run */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  Endpoint_SelectEndpoint(inEP);

  if (Endpoint_IsINReady()) {
    /* Parse UART data */
    while (uart->isAvailable()) {
      uint8_t c = uart->getByte();
      handleByte(c);
    }
    sendBuf();
  }
  
  Endpoint_SelectEndpoint(outEP);

  if (Endpoint_IsOUTReceived()) {
    uint16_t cnt;
    usb_midi_message_t msg;

    while ((cnt =  Endpoint_BytesInEndpoint()) >= 4) {
      Endpoint_Read_Stream_LE((void *)&msg, 4);
      handleUsbMessage(&msg);
    }

    /* Clear the endpoint buffer */
    Endpoint_ClearOUT();
  }
}
