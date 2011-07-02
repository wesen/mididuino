/*
 * MidiCtrl - Hardware implementation of GUI backend for the minicommand
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef GUI_PRIVATE_H__
#define GUI_PRIVATE_H__

#include <avr/pgmspace.h>
#include <inttypes.h>

/**
 * \addtogroup GUI
 *
 * @{
 *
 * \addtogroup gui_backend GUI backend for minicommand2
 *
 * @{
 *
 * \file
 * Hardware backend
 **/

/**
 * Class to interface with the SR165 shift register on the IO Board.
 **/
class SR165Class {
  /** Pulse the RST line. **/
  inline void rst();
  /** Pulse the clock line. **/
  inline void clk();
 public:
  SR165Class();
  /** Read out an 8 bit value (after RST) **/
  uint8_t read();
  /** Read out a 16 bit value (after RST) **/
  uint16_t read16();
  /** Read the next 8 bits without RST. **/
  uint8_t read_norst();
};

/** Defines the number of encoders on the minicommand. **/
#define GUI_NUM_ENCODERS 4
/** Defines the number of buttons on the minicommand (4 + 4 Encoders) **/
#define GUI_NUM_BUTTONS  8

/**
 * Structure to represent a hardware encoder to pass information over
 * to the platform independent GUI
 **/
typedef struct encoder_s {
  int8_t normal;
  int8_t button;
} encoder_t;

/**
 * Class to poll the hardware encoders and fill the `encoder_t` structures.
 **/
class EncodersClass {
  /** Previous value of shift register. **/
  uint16_t sr_old;
  uint8_t sr_old2s[GUI_NUM_ENCODERS];

 public:
  /** Structures to pass encoder information over to the GUI encoders. **/
  encoder_t encoders[GUI_NUM_ENCODERS];

  EncodersClass();

  /** Analyze the new shift register input to detect encoder movement. **/
  void poll(uint16_t sr);
  void clearEncoders();

  /** Return the normal value of an encoder structure. **/
  inline int8_t getNormal(uint8_t i) { return encoders[i].normal; }
  /** Return the button state of an encoder structure. **/
  inline int8_t getButton(uint8_t i) { return encoders[i].button; }

  int8_t limitValue(int8_t value, int8_t min, int8_t max) {
    if (value > max)
      return max;
    if (value < min)
      return min;
    return value;
  }
};

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

/**
 * Structure to represent a hardware button to pass information over
 * to the platform independent GUI
 **/
typedef struct button_s {
  uint8_t  status;
  uint16_t press_time;
  uint16_t last_press_time;
} button_t;

/**
 * Class to poll the hardware buttons and fill the `button_t` structures.
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

  static const uint8_t NO_BUTTON = 255;

  static const uint16_t ENCODER1_MASK = _BV(ENCODER1);
  static const uint16_t ENCODER2_MASK = _BV(ENCODER2);
  static const uint16_t ENCODER3_MASK = _BV(ENCODER3);
  static const uint16_t ENCODER4_MASK = _BV(ENCODER4);
  static const uint16_t BUTTON1_MASK = _BV(BUTTON1);
  static const uint16_t BUTTON2_MASK = _BV(BUTTON2);
  static const uint16_t BUTTON3_MASK = _BV(BUTTON3);
  static const uint16_t BUTTON4_MASK = _BV(BUTTON4);
  
  ButtonsClass();
  void clear();
  void poll(uint8_t sr);
};

extern SR165Class SR165;
extern EncodersClass Encoders;
extern ButtonsClass Buttons;

#endif /* GUI_PRIVATE_H__ */

