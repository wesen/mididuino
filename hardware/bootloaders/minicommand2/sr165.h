#ifndef SR165_H__
#define SR165_H__

  
#define SR165_OUT    PD5
#define SR165_SHLOAD PD6
#define SR165_CLK    PD7

#define SR165_DATA_PORT PORTD
#define SR165_DDR_PORT  DDRD
#define SR165_PIN_PORT  PIND

void sr165_init(void);
uint8_t sr165_read(void);
uint8_t sr165_read_norst(void);
uint16_t sr165_read16(void);

#endif /* SR165_H__ */
