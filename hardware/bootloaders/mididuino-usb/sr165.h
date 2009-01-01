#ifndef SR165_H__
#define SR165_H__

  
#define SR165_OUT    PC5
#define SR165_SHLOAD PC6
#define SR165_CLK    PD2

#define SR165_DATA_PORT PORTC
#define SR165_DATA_DDR  DDRC
#define SR165_DATA_PIN  PINC

#define SR165_CLK_PORT PORTD
#define SR165_CLK_DDR  DDRD

void sr165_init(void);
uint8_t sr165_read(void);
uint8_t sr165_read_norst(void);
uint16_t sr165_read16(void);

#endif /* SR165_H__ */
