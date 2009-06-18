#include "WProgram.h"
#include "GUI.h"

#ifdef MIDIDUINO_USE_GUI

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

void GuiClass::updatePage() {
  if (page != NULL) {
    page->update();
  }
}

bool GuiClass::handleGui() {
  if (page != NULL)
    return page->handleGui();
  else
    return false;
}

void GuiClass::update() {
  bool redisplay = false;
  
  uint8_t tmp = SREG;
  cli();
  if (newPage != NULL) {
    page = newPage;
    newPage = NULL;
    redisplay = true;
    setLine(GUI.LINE1);
    clearLine();
    setLine(GUI.LINE2 );
    clearLine();
  }
  SREG = tmp;
  if (page != NULL) {
    page->display(redisplay);
    page->handle();
  }

  //  uint8_t tmp = SREG;
  //  cli();
  for (uint8_t i = 0; i < 2; i++) {
    if (lines[i].flashActive) {
      uint16_t clock = read_slowclock();
      uint16_t diff = clock_diff(lines[i].flashTimer, clock);
      if (diff > lines[i].duration) {
	lines[i].changed = true;
	lines[i].flashActive = false;
      }
      if (lines[i].flashChanged) {
	LCD.goLine(i);
	LCD.puts(lines[i].flash);
	lines[i].flashChanged = false;
      }
    }

    if (lines[i].changed && !lines[i].flashActive) {
      LCD.goLine(i);
      LCD.puts(lines[i].data);
      lines[i].changed = false;
    }
  }
  //  SREG = tmp;
}

char hex2c(uint8_t hex) {
  if (hex < 10) {
    return hex + '0';
  } else {
    return hex - 10 + 'a';
  }
}

void GuiClass::put_value(uint8_t idx, uint8_t value) {
  put_value_at(idx << 2, value);
}

void GuiClass::put_value(uint8_t idx, int value) {
  put_value_at(idx << 2, value);
}

void GuiClass::put_value16(uint8_t idx, uint16_t value) {
  put_value16_at(idx << 2, value);
}

void GuiClass::put_valuex(uint8_t idx, uint8_t value) {
  put_valuex_at(idx << 1, value);
}

void GuiClass::put_value_at(uint8_t idx, uint8_t value) {
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx] = value / 100 + '0';
  data[idx+1] = (value % 100) / 10 + '0';
  data[idx+2] = (value % 10) + '0';
  data[idx+3] = ' ';
}

void GuiClass::put_value_at(uint8_t idx, int value) {
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx] = (value % 1000) / 100 + '0';
  data[idx+1] = (value % 100) / 10 + '0';
  data[idx+2] = (value % 10) + '0';
  data[idx+3] = ' ';
}

void GuiClass::put_value16_at(uint8_t idx, uint16_t value) {
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx]   = hex2c(value >> 12 & 0xF);
  data[idx+1] = hex2c(value >> 8 & 0xF);
  data[idx+2] = hex2c(value >> 4 & 0xF);
  data[idx+3] = hex2c(value >> 0 & 0xF);
}

void GuiClass::put_valuex_at(uint8_t idx, uint8_t value) {
  char *data = lines[curLine].data;
  lines[curLine].changed = true;
  data[idx]   = hex2c(value >> 4 & 0xF);
  data[idx+1] = hex2c(value >> 0 & 0xF);
}


void GuiClass::put_string(uint8_t idx, char *str) {
  put_string_at(idx << 2, str);
}

void GuiClass::put_p_string(uint8_t idx, PGM_P str) {
  put_p_string_at(idx << 2, str);
}

void GuiClass::put_string_at(uint8_t idx, char *str) {
  char *data = lines[curLine].data;
  m_strncpy(data + idx, str, sizeof(lines[0].data) - idx);
  lines[curLine].changed = true;
}

void GuiClass::put_p_string_at(uint8_t idx, PGM_P str) {
  char *data = lines[curLine].data;
  m_strncpy_p(data + idx, str, sizeof(lines[0].data) - idx);
  lines[curLine].changed = true;
}


void GuiClass::put_string_at_fill(uint8_t idx, char *str) {
  char *data = lines[curLine].data;
  m_strncpy_fill(data + idx, str, sizeof(lines[0].data) - idx);
  lines[curLine].changed = true;
}

void GuiClass::put_p_string_at_fill(uint8_t idx, PGM_P str) {
  char *data = lines[curLine].data;
  m_strncpy_p_fill(data + idx, str, sizeof(lines[0].data) - idx);
  lines[curLine].changed = true;
}


void GuiClass::put_string_fill(char *str) {
  put_string_at_fill(0, str);
}

void GuiClass::put_p_string_fill(PGM_P str) {
  put_p_string_at_fill(0, str);
}

void GuiClass::put_string(char *str) {
  put_string_at(0, str);
}

void GuiClass::put_p_string(PGM_P str) {
  put_p_string_at(0, str);
}

void GuiClass::setPage(Page *_page) {
  newPage = _page;
  // XXX needed?
  /*
  uint8_t tmp = SREG;
  cli();
  Encoders.clearEncoders();
  SREG = tmp;
  */
}

void GuiClass::clearLine() {
  for (uint8_t i = 0; i < sizeof(lines[0].data); i++)
    lines[curLine].data[i] = ' ';
  lines[curLine].changed = true;
}

void GuiClass::flash(uint16_t duration) {
  lines[curLine].flashChanged = lines[curLine].flashActive = true;
  lines[curLine].duration = duration;
  lines[curLine].flashTimer = read_slowclock();
}

void GuiClass::clearFlashLine() {
  for (uint8_t i = 0; i < sizeof(lines[0].data); i++)
    lines[curLine].flash[i] = ' ';
}

void GuiClass::clearFlash(uint16_t duration) {
  for (uint8_t i = 0; i < sizeof(lines[0].data); i++)
    lines[curLine].flash[i] = ' ';
  flash(duration);
}

void GuiClass::flash_put_value(uint8_t idx, uint8_t value, uint16_t duration) {
  flash_put_value_at(idx << 2, value, duration);
}

void GuiClass::flash_put_value16(uint8_t idx, uint16_t value, uint16_t duration) {
  flash_put_value16_at(idx << 2, value, duration);
}

void GuiClass::flash_put_valuex(uint8_t idx, uint8_t value, uint16_t duration) {
  flash_put_valuex_at(idx << 1, value, duration);
}

void GuiClass::flash_put_value_at(uint8_t idx, uint8_t value, uint16_t duration) {
  char *data = lines[curLine].flash;
  data[idx] = value / 100 + '0';
  data[idx+1] = (value % 100) / 10 + '0';
  data[idx+2] = (value % 10) + '0';
  data[idx+3] = ' ';
  flash(duration);
}

void GuiClass::flash_put_value16_at(uint8_t idx, uint16_t value, uint16_t duration) {
  char *data = lines[curLine].flash;
  data[idx]   = hex2c(value >> 12 & 0xF);
  data[idx+1] = hex2c(value >> 8 & 0xF);
  data[idx+2] = hex2c(value >> 4 & 0xF);
  data[idx+3] = hex2c(value >> 0 & 0xF);
  flash(duration);
}

void GuiClass::flash_put_valuex_at(uint8_t idx, uint8_t value, uint16_t duration) {
  char *data = lines[curLine].flash;
  data[idx]   = hex2c(value >> 4 & 0xF);
  data[idx+1] = hex2c(value >> 0 & 0xF);
  flash(duration);
}

void GuiClass::flash_string_at(uint8_t idx, char *str, uint16_t duration) {
  char *data = lines[curLine].flash;
  m_strncpy(data + idx, str, sizeof(lines[0].flash) - idx);
  flash(duration);
}

void GuiClass::flash_string_at_fill(uint8_t idx, char *str, uint16_t duration) {
  char *data = lines[curLine].flash;
  m_strncpy_fill(data + idx, str, sizeof(lines[0].flash) - idx);
  flash(duration);
}

void GuiClass::flash_p_string_at(uint8_t idx, PGM_P str, uint16_t duration) {
  char *data = lines[curLine].flash;
  m_strncpy_p(data + idx, str, sizeof(lines[0].flash) - idx);
  flash(duration);
}

void GuiClass::flash_p_string_at_fill(uint8_t idx, PGM_P str, uint16_t duration) {
  char *data = lines[curLine].flash;
  m_strncpy_p_fill(data + idx, str, sizeof(lines[0].flash) - idx);
  flash(duration);
}

void GuiClass::flash_string(char *str, uint16_t duration) {
  flash_string_at(0, str, duration);
}

void GuiClass::flash_p_string(PGM_P str, uint16_t duration) {
  flash_p_string_at(0, str, duration);
}

void GuiClass::flash_string_fill(char *str, uint16_t duration) {
  flash_string_at_fill(0, str, duration);
}

void GuiClass::flash_p_string_fill(PGM_P str, uint16_t duration) {
  flash_p_string_at_fill(0, str, duration);
}

void GuiClass::flash_string_clear(char *str, uint16_t duration) {
  setLine(LINE1);
  flash_string_fill(str, duration);
  setLine(LINE2);
  clearFlash(duration);
}


void GuiClass::flash_p_string_clear(char *str, uint16_t duration) {
  setLine(LINE1);
  flash_p_string_fill(str, duration);
  setLine(LINE2);
  clearFlash(duration);
}

void GuiClass::flash_strings_fill(char *str1, char *str2, uint16_t duration) {
  setLine(LINE1);
  flash_string_fill(str1, duration);
  setLine(LINE2);
  flash_string_fill(str2, duration);
}

void GuiClass::flash_p_strings_fill(PGM_P str1, PGM_P str2, uint16_t duration) {
  setLine(LINE1);
  flash_p_string_fill(str1, duration);
  setLine(LINE2);
  flash_p_string_fill(str2, duration);
}

GuiClass GUI;

#endif
