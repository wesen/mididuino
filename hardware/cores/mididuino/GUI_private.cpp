#include "WProgram.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define SR165_OUT    PC5
#define SR165_SHLOAD PC6
#define SR165_CLK    PD2

#define SR165_DATA_PORT PORTC
#define SR165_DATA_DDR  DDRC
#define SR165_DATA_PIN  PINC

#define SR165_CLK_PORT  PORTD
#define SR165_CLK_DDR   DDRD

#define SR165_DELAY() { asm("nop"); asm("nop"); asm("nop"); }

inline void SR165Class::clk() {
  CLEAR_BIT8(SR165_CLK_PORT, SR165_CLK);
  SR165_DELAY();
  SET_BIT8(SR165_CLK_PORT, SR165_CLK);
  SR165_DELAY();
}

inline void SR165Class::rst() {
  CLEAR_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
}

SR165Class::SR165Class() {
  SR165_DATA_DDR |= _BV(SR165_SHLOAD);
  SR165_CLK_DDR |= _BV(SR165_CLK);
  CLEAR_BIT8(SR165_DATA_DDR, SR165_OUT);
  CLEAR_BIT8(SR165_DATA_PORT, SR165_OUT);
  SET_BIT8(SR165_CLK_PORT, SR165_CLK);
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
}

uint8_t SR165Class::read() {
  rst();
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_DATA_PIN, SR165_OUT);
    clk();
  }

  return res;
}

uint8_t SR165Class::read_norst() {
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_DATA_PIN, SR165_OUT);
    clk();
  }

  return res;
}


uint16_t SR165Class::read16() {
  rst();

  uint16_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 16; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_DATA_PIN, SR165_OUT);
    clk();
  }

  return res;
}

/**********************************************/

#define ENCODER_NORMAL(i) (encoders[(i)].normal)
#define ENCODER_BUTTON(i) (encoders[(i)].button)

EncodersClass::EncodersClass() {
  clearEncoders();
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    sr_old2s[i] = 0;
  }
  sr_old = 0;
}

void EncodersClass::clearEncoders() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    ENCODER_NORMAL(i) = ENCODER_BUTTON(i) = 0;
  }
}

#define ENCODER_FIX_INIT 0
#define ENCODER_FIX_MINUS_1 1
#define ENCODER_FIX_MINUS_0 2
#define ENCODER_FIX_PLUS_3  3
#define ENCODER_FIX_PLUS_1  4

void EncodersClass::poll(uint16_t sr) {
  uint16_t sr_tmp = sr;
  
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    uint8_t sr2 = sr & 3;
    uint8_t sro2 = sr_old & 3;

    if (sr2 != sro2) {
      /*
	LCD.line1();
      LCD.putnumber(sr2);
      LCD.puts(" ");
      LCD.putnumber(sro2);
      LCD.puts(" ");
      LCD.putnumber(sr_old2s[i]);
      */
      
      volatile int8_t *val = &(ENCODER_NORMAL(i));
      
      if (BUTTON_DOWN(i)) {
	  val = &(ENCODER_BUTTON(i));
      }

	
#if 1
      if ((sr2 == 0 && sro2 == 1 && sr_old2s[i] != ENCODER_FIX_MINUS_1) ||
	  (sr2 == 3 && sro2 == 0 && sr_old2s[i] != ENCODER_FIX_MINUS_0)) {
	if (*val > -64)
	  (*val)--;
	if (sro2 == 1)
	  sr_old2s[i] = ENCODER_FIX_MINUS_1;
	if (sro2 == 0)
	  sr_old2s[i] = ENCODER_FIX_MINUS_0;
      } else if ((sr2 == 0 && sro2 == 3 && sr_old2s[i] != ENCODER_FIX_PLUS_3) ||
		 (sr2 == 3 && sro2 == 1 && sr_old2s[i] != ENCODER_FIX_PLUS_1)) {
	if (*val < 64)
	  (*val)++;
	if (sro2 == 1)
	  sr_old2s[i] = ENCODER_FIX_PLUS_1;
	if (sro2 == 3)
	  sr_old2s[i] = ENCODER_FIX_PLUS_3;
      }
#endif
    }
#if 0
      if( i== 0) {
	uint8_t sr0 = IS_BIT_SET(sr, 0);
	uint8_t sr1 = IS_BIT_SET(sr, 1);
	uint8_t sro0 = IS_BIT_SET(sr_old, 0);
	uint8_t sro1 = IS_BIT_SET(sr_old, 1);

	if (sr2 != sro2) {
	  static uint8_t val = 100;
	  
	  LCD.line2();
	  if (sr2 == 3) {
	    if (sro2 == 0) {
	      LCD.puts("LEFT ");
	      val--;
	    } else if (sro2 == 1) {
	      LCD.puts("RIGHT");
	      val++;
	    }
	  } else if ((sr2 == 1)  && (sro2 == 3)) {
	      LCD.puts("LEFT ");
	      val--;
	  } else if (sr2 == 0) {
	    if (sro2 == 3) {
	      LCD.puts("RIGHT");
	      val++;
	    }
	  }

	  LCD.line1();
	  //	  LCD.putnumberx(IS_BIT_SET8(sr, 0));
	  //	  LCD.putnumberx(IS_BIT_SET8(sr, 1));
	  //	  LCD.putnumberx(IS_BIT_SET8(sr_old, 0));
	  //	  LCD.putnumberx(IS_BIT_SET8(sr_old, 1));
	  LCD.putnumberx(sr2);
	  LCD.putnumberx( sro2);

	  LCD.puts(" ");
	  LCD.putnumber(val);
	}
      }
#endif
    sr >>= 2;
    sr_old >>= 2;
  }

  sr_old = sr_tmp;
}

/**********************************************/

ButtonsClass::ButtonsClass() {
  clear();
}

void ButtonsClass::clear() {
  for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
    CLEAR_B_DOUBLE_CLICK(i);
    CLEAR_B_CLICK(i);
    CLEAR_B_LONG_CLICK(i);
    STORE_B_OLD(i, B_CURRENT(i));
  }
}

void ButtonsClass::poll(uint8_t but) {
  uint8_t but_tmp = but;

  for (uint8_t i = 0; i < GUI_NUM_BUTTONS; i++) {
    STORE_B_CURRENT(i, IS_BIT_SET8(but_tmp, 0));

    if (BUTTON_PRESSED(i)) {
      B_PRESS_TIME(i) =  slowclock;
      
      if (B_PRESSED_ONCE(i)) {
	uint16_t diff = clock_diff(B_LAST_PRESS_TIME(i), B_PRESS_TIME(i));
	if (diff < DOUBLE_CLICK_TIME) {
	  SET_B_DOUBLE_CLICK(i);
	  CLEAR_B_PRESSED_ONCE(i);
	}
      } else {
	B_LAST_PRESS_TIME(i) = B_PRESS_TIME(i);
	SET_B_PRESSED_ONCE(i);
      }
    }
    
    if (BUTTON_DOWN(i) && B_PRESSED_ONCE(i)) {
      uint16_t diff = clock_diff(B_LAST_PRESS_TIME(i), slowclock);
      if (diff > LONG_CLICK_TIME) {
	SET_B_LONG_CLICK(i);
	CLEAR_B_PRESSED_ONCE(i);
      }
    }

    if (BUTTON_UP(i) && B_PRESSED_ONCE(i)) {
      uint16_t diff = clock_diff(B_LAST_PRESS_TIME(i), slowclock);
      if (diff > LONG_CLICK_TIME) {
	CLEAR_B_PRESSED_ONCE(i);
      } else if (diff > DOUBLE_CLICK_TIME) {
	CLEAR_B_PRESSED_ONCE(i);
	SET_B_CLICK(i);
      }
    }

    but_tmp >>= 1;
  }
}

SR165Class SR165;
EncodersClass Encoders;
ButtonsClass Buttons;

