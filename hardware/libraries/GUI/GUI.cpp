#include "WProgram.h"

#include "GUI.h"

Encoder::Encoder(char *_name) {
  old = 0;
  cur = 0;
  setName(_name);
}

void Encoder::clear() {
  old = 0;
  cur = 0;
}

void Encoder::handle(uint8_t val) {
}

void Encoder::update(encoder_t *enc) {
  cur = cur + enc->normal + 5 * enc->button;
}

void RangeEncoder::update(encoder_t *enc) {
  int8_t inc = enc->normal + 5 * enc->button;
  cur = u_limit_value(cur, inc, min, max);
}

void CCEncoder::handle(uint8_t val) {
  MidiUart.sendCC(cc, val);
}

void TempoEncoder::handle(uint8_t val) {
  MidiClock.setTempo(val);
}

/********************/

void EncoderPage::update() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->update(Encoders.encoders + i);
  }
}

void EncoderPage::handle() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->checkHandle();
  }
}

void EncoderPage::clear() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL)
      encoders[i]->clear();
  }
}

void EncoderPage::display(bool redisplay) {
  for (uint8_t i = 0; i < 4; i++) {
    if (encoders[i] != NULL)
      if (encoders[i]->hasChanged() || redisplay)
	GUI.put_value(i, encoders[i]->getValue());
  }
}

/************************************************/
GuiClass::GuiClass() {
  curLine = LINE1;
  for (uint8_t i = 0; i < 16; i++) {
    lines[0].data[i] = ' ';
    lines[1].data[i] = ' ';
  }
  lines[0].changed = false;
  lines[1].changed = false;
  handleButtons = NULL;
  page = NULL;
}

void GuiClass::update() {
  if (page != NULL) {
    cli();
    page->update();
    sei();
  }
  display();
  if (page != NULL) {
    cli();
    page->handle();
    sei();
  }
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
  idx <<= 2;
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  for (uint8_t i = 0; i < 4 && str[i] != 0; i++) {
    data[idx + i] = str[i];
  }
}

void GuiClass::put_p_string(uint8_t idx, PGM_P str) {
  idx <<= 2;
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  for (uint8_t i = 0; i < 4 && str[i] != 0; i++) {
    data[idx + i] = pgm_read_byte(str + i);
  }
}

void GuiClass::display(bool redisplay) {
  if (page != NULL) {
    setLine(LINE2);
    page->display(redisplay);
  }
}

void GuiClass::displayNames() {
  setLine(LINE1);
  if (page != NULL) {
    for (uint8_t i = 0; i < 4; i++) {
      if (page->encoders[i] != NULL)
	put_string(i, page->encoders[i]->getName());
      else
	put_string(i, (char *)" ");
    }
  } else {
    for (uint8_t i = 0; i < 4; i++) {
      put_string(i, (char *)"    ");
    }
  }
}

GuiClass GUI;
