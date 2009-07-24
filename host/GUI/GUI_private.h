#ifndef GUI_PRIVATE_H__
#define GUI_PRIVATE_H__

#include "WProgram.h"

typedef struct encoder_s {
  int8_t normal;
  int8_t button;
} encoder_t;

class EncodersClass {
 public:
  encoder_t encoders[GUI_NUM_ENCODERS];

  EncodersClass() { }

  void poll(uint16_t sr) { }
  void clearEncoders() { }

  inline int8_t getNormal(uint8_t i) { return encoders[i].normal; }
  inline int8_t getButton(uint8_t i) { return encoders[i].button; }

  int8_t limitValue(int8_t value, int8_t min, int8_t max) {
    if (value > max)
      return max;
    if (value < min)
      return min;
    return value;
  }
};

extern EncodersClass Encoders;

typedef struct button_s {
  uint8_t  status;
  uint16_t press_time;
  uint16_t last_press_time;
} button_t;



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

  static const uint16_t ENCODER1_MASK = _BV(ENCODER1);
  static const uint16_t ENCODER2_MASK = _BV(ENCODER2);
  static const uint16_t ENCODER3_MASK = _BV(ENCODER3);
  static const uint16_t ENCODER4_MASK = _BV(ENCODER4);
  static const uint16_t BUTTON1_MASK = _BV(BUTTON1);
  static const uint16_t BUTTON2_MASK = _BV(BUTTON2);
  static const uint16_t BUTTON3_MASK = _BV(BUTTON3);
  static const uint16_t BUTTON4_MASK = _BV(BUTTON4);
  
  ButtonsClass() { }
  void clear() { }
  void poll(uint8_t sr) { }
};

extern ButtonsClass Buttons;

#endif /* GUI_PRIVATE_H__ */
