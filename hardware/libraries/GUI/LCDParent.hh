#ifndef LCDPARENT_H__
#define LCDPARENT_H__

#ifndef HOST_MIDIDUINO
extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
}
#include <avr/pgmspace.h>
#include <inttypes.h>
#else
#include "WProgram.h"
#endif



class LCDParentClass {
protected:
  virtual void enable() { }
  virtual void putnibble(uint8_t nibble) { }
  virtual void putcommand(uint8_t command) { }
  virtual void putdata(uint8_t data) { }

  public:
#ifdef HOST_MIDIDUINO
  virtual ~LCDParentClass() { }
#endif
  
  void line1();
  void line1(char *s);
  void line1_fill(char *s);
  void line1_p(PGM_P s);
  void line1_p_fill(PGM_P s);
  void line2();
  void line2(char *s);
  void line2_fill(char *s);
  void line2_p(PGM_P s);
  void line2_p_fill(PGM_P s);

  inline void goLine(uint8_t line) {
    switch (line) {
    case 0:
      line1();
      break;
    case 1:
      line2();
      break;
    }
  }
  
  void clearLine();

  void puts(char *s);
  void puts_fill(char *s, uint8_t i);
  void puts_fill(char *s);
  void puts_p(PGM_P s);
  void puts_p_fill(PGM_P s, uint8_t i);
  void puts_p_fill(PGM_P s);
  
  void put(char *data, uint8_t cnt);

  void putnumber(uint8_t num);
  void putnumber16(uint16_t num);
  void putcx(uint8_t i);
  void putnumberx(uint8_t num);
  void putnumberx16(uint16_t num);
  void putnumber32(uint32_t num);

  void blinkCursor(bool blink);
  void moveCursor(uint8_t row, uint8_t column);
};

#endif /* LCDPARENT_H__ */
