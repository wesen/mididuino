/*
 * MidiCtrl - Juno abstraction
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

 #ifndef JUNO_H__
 #define JUNO_H__

#include "PlatformConfig.h"

/**
 * \addtogroup Juno 106
 *
 * @{
 * \defgroup juno_juno Juno 106 class
 * \defgroup juno_callbacks Juno 106 callbacks
 */

#define ROLAND_MANUFACTURER_ID 0x41

#define JUNO_CC_MODULATION 0x01
#define JUNO_CC_PEDAL 0x40

#define JUNO_LFO_RATE 0x00
#define JUNO_LFO_DELAY 0x01
#define JUNO_DCO_LFO_MODULATION_LEVEL 0x02
#define JUNO_DCO_PWM_MODULATION_LEVEL 0x03
#define JUNO_NOISE_LEVEL 0x04
#define JUNO_VCF_CUTOFF_LEVEL 0x05
#define JUNO_VCF_RESONANCE_LEVEL 0x06
#define JUNO_VCF_ENV_LEVEL 007
#define JUNO_VCF_LFO_MODULATION_LEVEL 0x08
#define JUNO_VCF_KYBD_MODULATION_LEVEL 0x09
#define JUNO_VCA_LEVEL 0x0A
#define JUNO_ENV_ATTACK_RATE 0x0B
#define JUNO_ENV_DECAY_RATE 0x0C
#define JUNO_ENV_SUSTAIN_LEVEL 0x0D
#define JUNO_ENV_RELEASE_RATE 0x0E
#define JUNO_SUB_LEVEL 0x0F
#define JUNO_SWITCHES_1 0x10
#define JUNO_SWITCHES_2 0x11
#define JUNO_MAX_PARAMETERS 0x0F

#define JUNO_SWITCH_16_IDX _BV(0)
#define JUNO_SWITCH_8_IDX  _BV(1)
#define JUNO_SWITCH_4_IDX  _BV(2)
#define JUNO_SWITCH_PULSE_IDX _BV(3)
#define JUNO_SWITCH_TRI_IDX _BV(4)
#define JUNO_SWITCH_CHORUS_IDX _BV(5)
#define JUNO_SWITCH_CHORUS_LEVEL_IDX _BV(6)

#define JUNO_SWITCH2_DCO_PWM_IDX _BV(0)
#define JUNO_SWITCH2_DCO_PWM_LFO 0
#define JUNO_SWITCH2_DCO_PWM_MAN 1
#define JUNO_SWITCH2_VCA__IDX _BV(1)
#define JUNO_SWITCH2_VCA_ENV 0
#define JUNO_SWITCH2_VCA_GATE 1
#define JUNO_SWITCH2_VCF_POLARITY_IDX _BV(2)
#define JUNO_SWITCH2_VCF_POLARITY_PLUS 0
#define JUNO_SWITCH2_VCF_POLARITY_MINUS 1
#define JUNO_SWITCH2_HPF_IDX _BV(3)

#define JUNO_SYSEX_MSG_CONTROL_CHANGE 0x32
#define JUNO_SYSEX_MSG_BANK_CHANGE 0x30
#define JUNO_SYSEX_USER_MANUAL_MODE 0x31

/**
 * \addtogroup juno_callbacks
 *
 * @{
 * Juno Callback class, inherit form this class if you want to use callbacks on Juno events
 */
class JunoCallback {
};

typedef void(JunoCallback::*juno_callback_control_change_ptr_t)(uint8_t channel, uint8_t type, uint8_t param);
typedef void(JunoCallback::*juno_callback_bank_change_ptr_t)(uint8_t channel, uint8_t bank, JunoPatch *patch);
typedef void(JunoCallback::*juno_callback_manual_mode_ptr_t)(uint8_t channel, uint8_t bank, JunoPatch *patch);

/**
 * @}
 */

#include "JunoSysex.h"
#include "JunoEncoders.h"

/**
 * Stores the juno parameters for a channel
 */
class JunoPatch {
  public:
  JunoPatch() {
  }

  uint8_t parameters[JUNO_MAX_PARAMETERS];
  bool modulation;
  bool pedal;
  uint8_t bank;
};

/**
 * This class stores the parameter settings for a complete Juno kit
 */
class JunoKit {
public:
  JunoKit() {
  }

  char name[17];

  /**
   *  parameters per channel
   */
  JunoPatch patches[16];
  JunoPatch manual;
};

class JunoClass {
public:
  JunoKit kit;
  uint8_t currentBank;
  uint8_t currentChannel;
  bool manualMode;

  JunoClass() : kit() {
    currentBank = 0;
    manualMode = false;
    currentChannel = 0;
  }

  void sendParameter(uint8_t channel, uint8_t idx, uint8_t value);
  void sendParameter(uint8_t idx, uint8_t value) {
    sendParameter(currentChannel, idx, value);
  }
  void sendPatch(uint8_t channel);
  void sendPatch() {
    sendPatch(currentChannel);
  }
  void sendKit();

  void setModulation(uint8_t channel, bool modulation);
  void setModulation(bool modulation) {
    setModulation(currentChannel, modulation);
  }
  void setPedal(uint8_t channel, bool pedal);
  void setPedal(bool pedal) {
    setPedal(currentChannel, pedal);
  }

  /** helper methods / shortcuts **/
  void setLfoRate(uint8_t channel, uint8_t rate) {
    sendParameter(channel, JUNO_LFO_RATE, rate);
  }

  void setLfoDelay(uint8_t channel, uint8_t delay) {
    sendParameter(channel, JUNO_LFO_DELAY, delay);
  }

  void setDcoLfoModulationLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_DCO_LFO_MODULATION_LEVEL, level);
  }

  void setDcoPwmModulationLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_DCO_PWM_MODULATION_LEVEL, level);
  }

  void setNoiseLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_NOISE_LEVEL, level);
  }

  void setVcfCutoffLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_VCF_CUTOFF_LEVEL, level);
  }

  void setVcfResonanceLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_VCF_RESONANCE_LEVEL, level);
  }

  void setVcfLfoModulationLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_VCF_LFO_MODULATION_LEVEL, level);
  }

  void setVcfKeyboardModulationLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_VCF_KYBD_MODULATION_LEVEL, level);
  }

  void setVcaLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_VCA_LEVEL, level);
  }

  void setEnvAttackRate(uint8_t channel, uint8_t rate) {
    sendParameter(channel, JUNO_ENV_ATTACK_RATE, rate);
  }

  void setEnvDecayRate(uint8_t channel, uint8_t rate) {
    sendParameter(channel, JUNO_ENV_DECAY_RATE, rate);
  }

  void setEnvSustainLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_ENV_SUSTAIN_LEVEL, level);
  }

  void setEnvReleaseRate(uint8_t channel, uint8_t rate) {
    sendParameter(channel, JUNO_ENV_RELEASE_RATE, rate);
  }

  void setSubLevel(uint8_t channel, uint8_t level) {
    sendParameter(channel, JUNO_SUB_LEVEL, level);
  }

  void setSwitches1(uint8_t channel, uint8_t switches) {
    sendParameter(channel, JUNO_SWITCHES_1, switches);
  }

  void setSwitches2(uint8_t channel, uint8_t switches) {
    sendParameter(channel, JUNO_SWITCHES_2, switches);
  }

};

/**
 * The standard always present object representing the Juno to which the
 * minicommand is connected.
 **/
extern JunoClass Juno;

/* @} */
 
 #endif /* JUNO_H__ */
