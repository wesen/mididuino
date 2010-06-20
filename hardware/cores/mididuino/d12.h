#ifndef D12_H__
#define D12_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mididuino_private.h"
  
#define DATA_PORT A
#define DATA_PORT_PORT PORTA
#define DATA_PORT_PIN  PINA
#define DATA_PORT_DDR  DDRA

#define CTRL_PORT D
#define CTRL_PORT_PORT PORTD
#define CTRL_PORT_PIN  PIND
#define CTRL_PORT_DDR  DDRD

#define A0_PIN 5
#define WR_PIN 7
#define RD_PIN 6
#define SUSPEND_PIN 4
#define IRQ_PIN 3

#define CLEAR_A0() CLEAR_BIT(CTRL_PORT_PORT, A0_PIN)
#define SET_A0()   SET_BIT(CTRL_PORT_PORT,   A0_PIN)
#define CLEAR_WR() CLEAR_BIT(CTRL_PORT_PORT, WR_PIN)
#define SET_WR()   SET_BIT(CTRL_PORT_PORT,   WR_PIN)
#define CLEAR_RD() CLEAR_BIT(CTRL_PORT_PORT, RD_PIN)
#define SET_RD()   SET_BIT(CTRL_PORT_PORT,   RD_PIN)

#define D12_CMD_SET_ADDRESS         0xD0
#define D12_CMD_SET_EP_ENABLE       0xD8
#define D12_CMD_SET_MODE            0xF3
#define D12_CMD_SET_DMA             0xFB
#define D12_CMD_READ_INT_REG        0xF4
#define D12_CMD_SELECT_EP           0x00
#define D12_CMD_READ_EP_STATUS      0x80
#define D12_CMD_READ_LAST_TX_STATUS 0x40
#define D12_CMD_READ_BUFFER         0xF0
#define D12_CMD_WRITE_BUFFER        0xF0
#define D12_CMD_CLEAR_BUFFER        0xF2
#define D12_CMD_VALIDATE_BUFFER     0xFA
#define D12_CMD_SET_EP_STATUS       0x40
#define D12_CMD_ACK_SETUP           0xF1
#define D12_CMD_SEND_RESUME         0xF6
#define D12_CMD_READ_FRAME_NUM      0xF5

#define D12_EP0_OUT 0
#define D12_EP0_IN  1
#define D12_EP1_OUT 2
#define D12_EP1_IN  3
#define D12_EP2_OUT 4
#define D12_EP2_IN  5

#define D12_EP0_SIZE 16
#define D12_EP1_SIZE 16
#define D12_EP2_SIZE 64

void d12_write_cmd(uint8_t cmd, uint8_t *data, uint8_t cnt);
void d12_write_cmd_flash(uint8_t cmd, uint8_t *data, uint8_t cnt);
void d12_read_cmd(uint8_t cmd, uint8_t *buf, uint8_t cnt);
uint8_t d12_write_ep_pkt(uint8_t ep, uint8_t *buf, uint8_t cnt);
uint8_t d12_write_ep_pkt_flash(uint8_t ep, uint8_t *buf, uint8_t cnt);
uint8_t d12_read_ep_pkt(uint8_t ep, uint8_t *buf, uint8_t cnt);
void d12_stall_ep(uint8_t ep);
void d12_pins_init(void);
void d12_init(void);
uint8_t d12_device_is_configured(void);
void d12_main(void);
void d12_set_lock(void);
void d12_unset_lock(void);
void d12_interrupt_handler(void);
void d12_bus_reset(uint8_t i);

#ifdef __cplusplus
}
#endif

#endif /* D12_H__ */

