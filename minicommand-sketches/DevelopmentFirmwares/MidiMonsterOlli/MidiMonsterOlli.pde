#include <MidiClockPage.h>
#include <MidiEuclidSketch.h>
#include <AutoMidiController.h>

/***************************************************************************
 *
 * Evolver Encoder and Evolver magic sketch
 *
 ***************************************************************************/

void EvolverEncoderHandle(Encoder *enc);

/* inherit from CCEncoder to put in AutoEncoderPage */
class EvolverEncoder : public CCEncoder {
  public:
  uint8_t param;
  
  virtual uint8_t getParam() {
    return param;
  }
  
  void initEvolverController(const char *_name, uint8_t _param, uint8_t _max) {
    setName(_name);
    max = _max;
    param = _param;
  }
  
  EvolverEncoder(uint8_t _param = 0, const char *_name = NULL, uint8_t _max = 100, int init = 0) :
  CCEncoder(0, 0, _name, init) {
    initEvolverController(_name, _param, _max);
    handler = EvolverEncoderHandle;
  }
};

void EvolverEncoderHandle(Encoder *enc) {
  EvolverEncoder *eEnc = (EvolverEncoder *)enc;
  uint8_t sysexData[] = {
    0xf0, 0x01, 0x20, 0x01, 0x01,
    0x00, 0x00, 0x00, /* param number, lower nibble, higher nibble */
    0xf7
  };
  sysexData[5] = eEnc->getParam();
  uint8_t value = eEnc->getValue();
  sysexData[6] = value & 0x0F;
  sysexData[7] = (value >> 4) & 0x0F;
	
  MidiUart.puts(sysexData, countof(sysexData));
}

class EvolverControllerSketch : public Sketch {
  public:
  AutoEncoderPage<EvolverEncoder> autoPage;
  
  EvolverControllerSketch() {
  }
  
  void setup() {
    autoPage.setup();
    autoPage.setShortName("EVO");
    autoPage.realEncoders[0].initEvolverController("FRQ", 16, 164);
    autoPage.realEncoders[1].initEvolverController("ENV", 17, 198);
    autoPage.realEncoders[2].initEvolverController("ATK", 18, 110);
    autoPage.realEncoders[3].initEvolverController("DEC", 19, 110);
    autoPage.redisplay = true;
    
    setPage(&autoPage);
  }
  
  void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("MGC "), 5);
    m_strncpy_p(n2, PSTR("EVO "), 5);
  }

  virtual void show() {
    if (currentPage() == NULL)
      setPage(&autoPage);
  }

  virtual void mute(bool pressed) {
  }

  virtual void doExtra(bool pressed) {
  }

  virtual Page *getPage(uint8_t i) {
    if (i < 4) {
      return &autoPage;
    } else {
      return NULL;
    }
  }
};

/***************************************************************************
 *
 * Actual sketch
 *
 ***************************************************************************/

PitchEuclidSketch sketch;
AutoMidiControllerSketch sketch2;
EvolverControllerSketch sketch3;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2, &sketch3);

void setup() {
  sketch.setupMonster(true);
  sketch2.setupMonster(true);
  sketch3.setupMonster(true);
  
  GUI.setSketch(&_defaultSketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);
  
  initClockPage();
}

bool handleEvent(gui_event_t *event) {
  return sketchSwitchPage.handleGlobalEvent(event);
}

void loop() {
}
