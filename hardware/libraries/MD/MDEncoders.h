#ifndef MD_ENCODERS_H__
#define MD_ENCODERS_H__

#include <MD.h>
#include <GUI.h>

#ifdef MIDIDUINO_USE_GUI
class MDEncoder : public CCEncoder {
public:
  uint8_t track;
  uint8_t param;

  virtual uint8_t getCC();
  virtual uint8_t getChannel();
  virtual void initCCEncoder(uint8_t _channel, uint8_t _cc);
  void initMDEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0) {
    track = _track;
    param = _param;
    setName(_name);
    setValue(init);
  }

  MDEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0);
  void loadFromKit();
};

class MDFXEncoder : public RangeEncoder {
 public:
  uint8_t effect;
  uint8_t param;

  void initMDFXEncoder(uint8_t _param = 0, uint8_t _effect = MD_FX_ECHO, char *_name = NULL, uint8_t init = 0) {
    effect = _effect;
    param = _param;
    setName(_name);
    setValue(init);
  }
  MDFXEncoder(uint8_t _param = 0, uint8_t _effect = MD_FX_ECHO, char *_name = NULL, uint8_t init = 0);
  void loadFromKit();
};

class MDLFOEncoder : public RangeEncoder {
 public:
  uint8_t track;
  uint8_t param;

  void initMDLFOEncoder(uint8_t _param = MD_LFO_TRACK, uint8_t _track = 0,
			char *_name = NULL, uint8_t init = 0) {
    param = _param;
    track = _track;
    if (_name == NULL) {
      setLFOParamName();
    } else {
      setName(_name);
    }
    setValue(init);
  }

  MDLFOEncoder(uint8_t _param = MD_LFO_TRACK, uint8_t _track = 0,
	       char *_name = NULL, uint8_t init = 0);
  
  void loadFromKit();
  void setLFOParamName();
  void setParam(uint8_t _param);

  virtual void displayAt(int i);
};

class MDTrackFlashEncoder : public RangeEncoder {
 public:
 MDTrackFlashEncoder(char *_name = NULL, uint8_t init = 0) : RangeEncoder(0, 15, _name, init) {
  }

  virtual void displayAt(int i);
};

class MDMelodicTrackFlashEncoder : public MDTrackFlashEncoder {
 public:
 MDMelodicTrackFlashEncoder(char *_name = NULL, uint8_t init = 0) : MDTrackFlashEncoder(_name, init) {
  }

  virtual void displayAt(int i);
};

class MDKitSelectEncoder : public RangeEncoder {
 public:
  MDKitSelectEncoder(const char *_name = NULL, uint8_t init = 0);

  virtual void displayAt(int i);
  void loadFromMD();
};

class MDPatternSelectEncoder : public RangeEncoder {
 public:
  MDPatternSelectEncoder(const char *_name = NULL, uint8_t init = 0);

  virtual void displayAt(int i);
  void loadFromMD();
};

class MDParamSelectEncoder : public RangeEncoder {
 public:
 MDParamSelectEncoder(uint8_t _track = 0, const char *_name = NULL, uint8_t init = 0) :
  track(_track), RangeEncoder(0, 23, _name, init) {
  }

  uint8_t track;
  virtual void displayAt(int i);
};

class MDAssignMachineEncoder : public RangeEncoder {
 public:
  MDAssignMachineEncoder(uint8_t _track = 0, const char *_name = NULL, uint8_t init = 0);

  uint8_t track;
  virtual void displayAt(int i);
  void loadFromMD();
};

class MDTrigGroupEncoder : public RangeEncoder {
 public:
  MDTrigGroupEncoder(uint8_t _track = 0, const char *_name = NULL, uint8_t init = 0);

  uint8_t track;
  virtual void displayAt(int i);
  void loadFromMD();
};

class MDMuteGroupEncoder : public RangeEncoder {
 public:
  MDMuteGroupEncoder(uint8_t _track = 0, const char *_name = NULL, uint8_t init = 0);

  uint8_t track;
  virtual void displayAt(int i);
  void loadFromMD();
};

#endif

#endif /* MD_ENCODERS_H__ */
