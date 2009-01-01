#ifndef ENCODER_H__
#define ENCODER_H__

#include <avr/pgmspace.h>

#include <inttypes.h>

class SR165Class {
  inline void rst();
  inline void clk();
 public:
  SR165Class();
  uint8_t read();
  uint16_t read16();
  uint8_t read_norst();
};

#define NUM_ENCODERS 5

typedef struct encoder_s {
  int8_t normal;
  int8_t shift;
  int8_t button;
  int8_t button_shift;
} encoder_t;

class EncodersClass {
  uint16_t sr_old;

  static const int MACRO = 4;

 public:
  encoder_t encoders[NUM_ENCODERS];

  EncodersClass();

  void poll(uint16_t sr);
  void clearEncoders();

  inline int8_t getNormal(uint8_t i) { return encoders[i].normal; }
  inline int8_t getShift(uint8_t i) { return encoders[i].shift; }
  inline int8_t getButton(uint8_t i) { return encoders[i].button; }
  inline int8_t getButtonShift(uint8_t i) { return encoders[i].button_shift; }

  int8_t limitValue(int8_t value, int8_t min, int8_t max) {
    if (value > max)
      return max;
    if (value < min)
      return min;
    return value;
  }
};

#define B_BIT_CURRENT        0
#define B_BIT_OLD            1
#define B_BIT_PRESSED_ONCE   2
#define B_BIT_DOUBLE_CLICK   3
#define B_BIT_CLICK          4
#define B_BIT_LONG_CLICK     5

#define DOUBLE_CLICK_TIME 30
#define LONG_CLICK_TIME   100

#define B_STATUS(i, bit)        (IS_BIT_SET8(GUI.Buttons.buttons[i].status, bit))
#define B_SET_STATUS(i, bit)    (SET_BIT8(GUI.Buttons.buttons[i].status, bit))
#define B_CLEAR_STATUS(i, bit)  (CLEAR_BIT8(GUI.Buttons.buttons[i].status, bit))
#define B_STORE_STATUS(i, bit, j) { if (j) B_SET_STATUS(i, bit);	\
      else B_CLEAR_STATUS(i, bit); }

#define B_CURRENT(i)               (B_STATUS(i, B_BIT_CURRENT))
#define SET_B_CURRENT(i)           (B_SET_STATUS(i, B_BIT_CURRENT))
#define CLEAR_B_CURRENT(i)         (B_CLEAR_STATUS(i, B_BIT_CURRENT))
#define STORE_B_CURRENT(i, j)      (B_STORE_STATUS(i, B_BIT_CURRENT, j))

#define B_OLD(i)                   (B_STATUS(i, B_BIT_OLD))
#define SET_B_OLD(i)               (B_SET_STATUS(i, B_BIT_OLD))
#define CLEAR_B_OLD(i)             (B_CLEAR_STATUS(i, B_BIT_OLD))
#define STORE_B_OLD(i, j)          (B_STORE_STATUS(i, B_BIT_OLD, j))

#define B_PRESSED_ONCE(i)          (B_STATUS(i, B_BIT_PRESSED_ONCE))
#define SET_B_PRESSED_ONCE(i)      (B_SET_STATUS(i, B_BIT_PRESSED_ONCE))
#define CLEAR_B_PRESSED_ONCE(i)    (B_CLEAR_STATUS(i, B_BIT_PRESSED_ONCE))

#define B_CLICK(i)                 (B_STATUS(i, B_BIT_CLICK))
#define SET_B_CLICK(i)             (B_SET_STATUS(i, B_BIT_CLICK))
#define CLEAR_B_CLICK(i)           (B_CLEAR_STATUS(i, B_BIT_CLICK))

#define B_DOUBLE_CLICK(i)          (B_STATUS(i, B_BIT_DOUBLE_CLICK))
#define SET_B_DOUBLE_CLICK(i)      (B_SET_STATUS(i, B_BIT_DOUBLE_CLICK))
#define CLEAR_B_DOUBLE_CLICK(i)    (B_CLEAR_STATUS(i, B_BIT_DOUBLE_CLICK))

#define B_LONG_CLICK(i)          (B_STATUS(i, B_BIT_LONG_CLICK))
#define SET_B_LONG_CLICK(i)      (B_SET_STATUS(i, B_BIT_LONG_CLICK))
#define CLEAR_B_LONG_CLICK(i)    (B_CLEAR_STATUS(i, B_BIT_LONG_CLICK))

#define B_PRESS_TIME(i)            (buttons[(i)].press_time)

#define B_LAST_PRESS_TIME(i)       (buttons[(i)].last_press_time)


#define BUTTON_DOWN(button)           (!(B_CURRENT(button)))
#define BUTTON_UP(button)             (B_CURRENT(button))
#define OLD_BUTTON_DOWN(button)       (!(B_OLD(button)))
#define OLD_BUTTON_UP(button)         (B_OLD(button))
#define BUTTON_PRESSED(button)        (OLD_BUTTON_UP(button) && BUTTON_DOWN(button))
#define BUTTON_DOUBLE_CLICKED(button) (B_DOUBLE_CLICK(button))
#define BUTTON_LONG_CLICKED(button)   (B_LONG_CLICK(button))
#define BUTTON_CLICKED(button)        (B_CLICK(button))
#define BUTTON_RELEASED(button)       (OLD_BUTTON_DOWN(button) && BUTTON_UP(button))
#define BUTTON_PRESS_TIME(button)     (clock_diff(B_PRESS_TIME(button), slowclock))

typedef struct button_s {
  uint8_t  status;
  uint16_t press_time;
  uint16_t last_press_time;
} button_t;

#define NUM_BUTTONS 6

class ButtonsClass {
 public:
  button_t buttons[NUM_BUTTONS];

  ButtonsClass();
  void clear();
  void poll(uint8_t sr);

  static const int MAIN  = 4;
  static const int SHIFT = 5;
};

typedef struct line_s {
  char data[16];
  bool changed;
} line_t;

class GuiClass {
 protected:
  line_t lines[2];
  uint8_t curLine;
  
 public:
  GuiClass();
  void poll();
  void clear();
  void update();
  void (*handleButtons)();

  void put_value(uint8_t idx, uint8_t value);
  void put_value16(uint8_t idx, uint16_t value);
  void put_valuex(uint8_t idx, uint8_t value);
  void put_string(uint8_t idx, char *str);
  void put_p_string(uint8_t idx, PGM_P str);
  void setLine(uint8_t line) { curLine = line; }

  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
  
  SR165Class SR165;
  EncodersClass Encoders;
  ButtonsClass Buttons;
};

extern GuiClass GUI;

#endif /* ENCODER_H__ */

