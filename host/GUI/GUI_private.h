/*
 * MidiCtrl - Host implementation of hardware interfacing stubs.
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef GUI_PRIVATE_H__
#define GUI_PRIVATE_H__

#include "PlatformConfig.h"

typedef struct encoder_s {
  int8_t normal;
  int8_t button;
} encoder_t;

/**
 * Mock encoder class for unit testing.
 **/
class EncodersClass {
 public:
  encoder_t encoders[GUI_NUM_ENCODERS];

  EncodersClass();
  void clearEncoders();

  /** Simulate an encoder turn. **/
  void turnEncoder(uint8_t encoder, int i, bool button = false);

  inline int8_t getNormal(uint8_t i) { return encoders[i].normal; }
  inline int8_t getButton(uint8_t i) { return encoders[i].button; }
};

extern EncodersClass Encoders;

/** States for a button press (not used at the moment. **/
#define B_BIT_CURRENT        0
#define B_BIT_OLD            1
#define B_BIT_PRESSED_ONCE   2
#define B_BIT_DOUBLE_CLICK   3
#define B_BIT_CLICK          4
#define B_BIT_LONG_CLICK     5

// XXX adjust these to correct length of irq time
#define DOUBLE_CLICK_TIME 200
#define LONG_CLICK_TIME   800

/** Macros to access the status of a button. **/
#define B_STATUS(i, bit)        (IS_BIT_SET8(Buttons.buttons[i].status, bit))
#define B_SET_STATUS(i, bit)    (SET_BIT8(Buttons.buttons[i].status, bit))
#define B_CLEAR_STATUS(i, bit)  (CLEAR_BIT8(Buttons.buttons[i].status, bit))
#define B_STORE_STATUS(i, bit, j) { if (j) B_SET_STATUS(i, bit);	\
    else B_CLEAR_STATUS(i, bit); }

/** Macros to access the current state of a button. **/
#define B_CURRENT(i)               (B_STATUS(i, B_BIT_CURRENT))
#define SET_B_CURRENT(i)           (B_SET_STATUS(i, B_BIT_CURRENT))
#define CLEAR_B_CURRENT(i)         (B_CLEAR_STATUS(i, B_BIT_CURRENT))
#define STORE_B_CURRENT(i, j)      (B_STORE_STATUS(i, B_BIT_CURRENT, j))

/** Macros to access the old state of a button. **/
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

/**
 * Mock button class for unit testing.
 **/
class ButtonsClass {
 public:
  button_t buttons[GUI_NUM_BUTTONS];

  static const uint8_t BUTTON1 = 4;
  static const uint8_t BUTTON2 = 5;
  static const uint8_t BUTTON3 = 6;
  static const uint8_t BUTTON4 = 7;
  static const uint8_t ENCODER1 = 0;
  static const uint8_t ENCODER2 = 1;
  static const uint8_t ENCODER3 = 2;
  static const uint8_t ENCODER4 = 3;

  static const uint8_t NO_BUTTON = 127;

  static const uint16_t ENCODER1_MASK = _BV(ENCODER1);
  static const uint16_t ENCODER2_MASK = _BV(ENCODER2);
  static const uint16_t ENCODER3_MASK = _BV(ENCODER3);
  static const uint16_t ENCODER4_MASK = _BV(ENCODER4);
  static const uint16_t BUTTON1_MASK = _BV(BUTTON1);
  static const uint16_t BUTTON2_MASK = _BV(BUTTON2);
  static const uint16_t BUTTON3_MASK = _BV(BUTTON3);
  static const uint16_t BUTTON4_MASK = _BV(BUTTON4);
  
  ButtonsClass();

  void clearButtons();
  void pressButton(uint8_t button);
  void releaseButton(uint8_t button);
  void printButtons();

  void tick();
};

extern ButtonsClass Buttons;

/**
 * Make a single tick of the GUI (update buttons status etc...)
 **/
extern void GUI_tick();

#endif /* GUI_PRIVATE_H__ */
