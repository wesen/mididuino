#include <avr/io.h>
#include <avr/interrupt.h>
#include "MidiDuino.h"
#include "GUI.h"
#include "LCD.h"

#define SR165_OUT    PB3
#define SR165_SHLOAD PB2
#define SR165_CLK    PB1

#define SR165_DATA_PORT PORTB
#define SR165_DDR_PORT  DDRB
#define SR165_PIN_PORT  PINB

#define SR165_DELAY() { asm("nop"); asm("nop"); asm("nop"); }

inline void SR165Class::clk() {
  CLEAR_BIT8(SR165_DATA_PORT, SR165_CLK);
  SR165_DELAY();
  SET_BIT8(SR165_DATA_PORT, SR165_CLK);
  SR165_DELAY();
}

inline void SR165Class::rst() {
  CLEAR_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
}

SR165Class::SR165Class() {
  SR165_DDR_PORT |= _BV(SR165_SHLOAD) | _BV(SR165_CLK);
  CLEAR_BIT8(SR165_DDR_PORT, SR165_OUT);
  CLEAR_BIT8(SR165_DATA_PORT, SR165_OUT);
  SET_BIT8(SR165_DATA_PORT, SR165_CLK);
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
}

uint8_t SR165Class::read() {
  rst();
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
    clk();
  }

  return res;
}

uint8_t SR165Class::read_norst() {
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
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
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
    clk();
  }

  return res;
}

/**********************************************/

#define ENCODER_NORMAL(i) (encoders[(i)].normal)
#define ENCODER_SHIFT(i)  (encoders[(i)].shift)
#define ENCODER_BUTTON(i) (encoders[(i)].button)
#define ENCODER_BUTTON_SHIFT(i) (encoders[(i)].button_shift)

EncodersClass::EncodersClass() {
  clearEncoders();
  sr_old = 0;
}

void EncodersClass::clearEncoders() {
  for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
    ENCODER_NORMAL(i) = ENCODER_BUTTON(i) = ENCODER_SHIFT(i) =
      ENCODER_BUTTON_SHIFT(i) = 0;
  }
}

void EncodersClass::poll(uint16_t sr) {
  uint16_t sr_tmp = sr;
  
  for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
    if (IS_BIT_SET8(sr, 0) == !IS_BIT_SET8(sr_old, 0)) {
      volatile int8_t *val = &(ENCODER_NORMAL(i));
      
      if (BUTTON_DOWN(i)) {
	if (BUTTON_DOWN(GUI.Buttons.SHIFT))
	  val = &(ENCODER_BUTTON_SHIFT(i));
	else
	  val = &(ENCODER_BUTTON(i));
      } else if (BUTTON_DOWN(GUI.Buttons.SHIFT)) {
	val = &(ENCODER_SHIFT(i));
      }

      if (IS_BIT_SET8(sr, 1) == IS_BIT_SET8(sr, 0)) {
	if (*val < 64)
	  (*val)++;
      } else {
	if (*val > -64)
	  (*val)--;
      }
    }
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
  for (int i = 0; i < NUM_BUTTONS; i++) {
    CLEAR_B_DOUBLE_CLICK(i);
    CLEAR_B_CLICK(i);
    CLEAR_B_LONG_CLICK(i);
    STORE_B_OLD(i, B_CURRENT(i));
  }
}

void ButtonsClass::poll(uint8_t but) {
  uint8_t but_tmp = but;

  but_tmp >>= 2;
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
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

/************************************************/
GuiClass::GuiClass() : SR165(), Encoders(), Buttons() {
  curLine = LINE1;
  for (uint8_t i = 0; i < 16; i++) {
    lines[0].data[i] = ' ';
    lines[1].data[i] = ' ';
  }
  lines[0].changed = false;
  lines[1].changed = false;
  handleButtons = NULL;
}

void GuiClass::clear() {
}

void GuiClass::poll() {
  uint8_t tmp = SREG;
  cli();
  uint16_t sr = SR165.read16();
  SREG = tmp;
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.clearEncoders();
  Encoders.poll(sr);

  if (handleButtons != NULL)
    handleButtons();
  //  update();
}

void GuiClass::update() {
  for (uint8_t i = 0; i < 2; i++) {
    if (lines[i].changed) {
      LCD.goLine(i);
      LCD.puts(lines[i].data);
      lines[i].changed = false;
    }
  }
}

char hex2c(uint8_t hex) {
  if (hex < 10) {
    return hex + '0';
  } else {
    return hex - 10 + 'a';
  }
}

void GuiClass::put_value(uint8_t idx, uint8_t value) {
  idx <<= 2;
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx] = value / 100 + '0';
  data[idx+1] = (value % 100) / 10 + '0';
  data[idx+2] = (value % 10) + '0';
  data[idx+3] = ' ';
}

void GuiClass::put_value16(uint8_t idx, uint16_t value) {
  idx <<= 2;
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx]   = hex2c(value >> 12 & 0xF);
  data[idx+1] = hex2c(value >> 8 & 0xF);
  data[idx+2] = hex2c(value >> 4 & 0xF);
  data[idx+3] = hex2c(value >> 0 & 0xF);
}

void GuiClass::put_valuex(uint8_t idx, uint8_t value) {
  idx <<= 1;
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx]   = hex2c(value >> 4 & 0xF);
  data[idx+1] = hex2c(value >> 0 & 0xF);
}

void GuiClass::put_string(uint8_t idx, char *str) {
  /* XXX */
}

void GuiClass::put_p_string(uint8_t idx, PGM_P str) {
  /* XXX */
}

GuiClass GUI;
