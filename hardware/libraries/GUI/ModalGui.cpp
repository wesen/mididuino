#include "WProgram.h"

#include "GUI.h"
#include "ModalGui.hh"

void __mainInnerLoop();

class ModalGuiPage : public Page {
public:
  char line1[16];
  char line2[16];
  bool hasPressedKey;
  int pressedKey;
  int numButtons;
  
  ModalGuiPage() {
    hasPressedKey = false;
    pressedKey = 0;
    numButtons = 2;
    line1[0] = '\0';
    line2[0] = '\0';
  }

  int getModalKey(int _numButtons) {
    hasPressedKey = false;
    numButtons = _numButtons;

    Page *previousPage = GUI.page;
    
    GUI.setPage(this);
    
    while (! hasPressedKey) {
      __mainInnerLoop();
    }
    GUI.setPage(previousPage);

    return pressedKey;
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill(line1);
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(line2);
    }
  }

  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      hasPressedKey = true;
      pressedKey = 0;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      hasPressedKey = true;
      pressedKey = 1;
    }
    return true;
  }
};

ModalGuiPage modalGuiPage;

int showModalGui(char *line1, char *line2, int numOptions) {
  m_strncpy(modalGuiPage.line1, line1, 16);
  m_strncpy(modalGuiPage.line2, line2, 16);
  return modalGuiPage.getModalKey(numOptions);
}

int showModalGui_p(PGM_P line1, PGM_P line2, int numOptions) {
  m_strncpy_p(modalGuiPage.line1, line1, 16);
  m_strncpy_p(modalGuiPage.line2, line2, 16);
  return modalGuiPage.getModalKey(numOptions);
}

class NameModalGuiPage : public Page {
public:
  char line1[16];
  char line2[16];
  char name[17];
  bool hasPressedKey;
  int pressedKey;
  
  NameModalGuiPage() {
    hasPressedKey = false;
    pressedKey = 0;
    line1[0] = '\0';
    line2[0] = '\0';
  }

  char *getName() {
    //    RangeEncoder 
    hasPressedKey = false;

    Page *previousPage = GUI.page;
    
    GUI.setPage(this);

    LCD.blinkCursor(true);
    LCD.moveCursor(1, 0);
    while (hasPressedKey == false) {
      __mainInnerLoop();
    }
    LCD.blinkCursor(false);
    GUI.setPage(previousPage);
    

    return name;
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill(line1);
    }
  }

  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      hasPressedKey = true;
      pressedKey = 0;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      hasPressedKey = true;
      pressedKey = 1;
    }
    return true;
  }
};

