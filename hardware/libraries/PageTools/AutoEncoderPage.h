/* Copyight (c) 2010 - http://ruinwesen.com/ */

#ifndef AUTOENCODERPAGE_H__
#define AUTOENCODERPAGE_H__

#include "Platform.h"
#include "GUI.h"
#include "CCHandler.h"

/**
 * \addtogroup GUI
 *
 * @{
 *
 * \addtogroup PageTools
 *
 * @{
 *
 * \addtogroup AutoEncoderPage
 *
 * @{
 *
 * \file
 * Recording Encoder Page implementation
 **/
 

// possibly clever specializition on CCEncoder to avoid duplicaiton of code
// XXX
// read up in template books

// template specialization of handleEvent

/**
 * Creates a page feature 4 encoders that can be configured using a
 * template class parameter. These 4 encoders are overlayed with
 * recording encoder to provide recording functionality. The page also
 * provides autolearning functionality to MIDI learn 4 encoders on the
 * fly.
 *
 * The page functionality is mapped in the following way:
 *
 * - BUTTON2 (lower left): autoLearnLast4
 * - BUTTON3 (lower right): hold to record knob movements
 * - BUTTON4 (upper right) + encoder: clear recording
 *
 **/
template <typename EncoderType> 
class AutoEncoderPage : public EncoderPage, public ClockCallback {
 public:
  EncoderType realEncoders[4];
  const static int RECORDING_LENGTH = 64; // recording length in 32th
  RecordingEncoder<RECORDING_LENGTH> recEncoders[4];

  bool muted;

  /** Button to press to autolearn last 4. **/
  uint8_t learnButton;
  /** Button to press to start/stop recording. **/
  uint8_t recordButton;
  /** Button to press to clear recordings. **/
  uint8_t clearButton;

  void on32Callback(uint32_t counter);
  void startRecording();
  void stopRecording();
  void clearRecording();
  void clearRecording(uint8_t i);
  virtual void setup();

  void autoLearnLast4();

  virtual bool handleEvent(gui_event_t *event);
};


template <typename EncoderType>
void AutoEncoderPage<EncoderType>::setup() {
  learnButton = Buttons.BUTTON2;
  recordButton = Buttons.BUTTON3;
  clearButton = Buttons.BUTTON4;

  muted = false;
  for (uint8_t i = 0; i < 4; i++) {
    realEncoders[i].setName("___");
    recEncoders[i].initRecordingEncoder(&realEncoders[i]);
    encoders[i] = &recEncoders[i];
    ccHandler.addEncoder(&realEncoders[i]);
  }
  MidiClock.addOn32Callback(this, (midi_clock_callback_ptr_t)&AutoEncoderPage<EncoderType>::on32Callback);
  EncoderPage::setup();
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::on32Callback(uint32_t counter) {
  if (muted)
    return;

  /*
   * The following code just copies the handling of recording encoders in order to save CPU time.
   * This callback is called from the clock callback and thus needs to be quick.
   */
  uint8_t pos = counter & 0xFF;
  uint8_t currentPos = pos % RECORDING_LENGTH;
  for (uint8_t i = 0; i < 4; i++) {
    RecordingEncoder<RECORDING_LENGTH> *encoder = recEncoders + i;
    encoder->currentPos = currentPos;
    if (encoder->value[currentPos] != -1) {
      if (!(encoder->recording && encoder->recordChanged)) {
        encoder->realEnc->setValue(encoder->value[currentPos], true);
        encoder->redisplay = encoder->realEnc->redisplay;
      }
    }
  }
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::startRecording() {
  for (uint8_t i = 0; i < 4; i++) {
    recEncoders[i].startRecording();
  }
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::stopRecording() {
  for (uint8_t i = 0; i < 4; i++) {
    recEncoders[i].stopRecording();
  }
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::clearRecording() {
  for (uint8_t i = 0; i < 4; i++) {
    recEncoders[i].clearRecording();
  }
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::clearRecording(uint8_t i) {
  recEncoders[i].clearRecording();
}

template <typename EncoderType>
void AutoEncoderPage<EncoderType>::autoLearnLast4() {
  /* maps from received CC indexes to encoder indexes */
  int8_t ccAssigned[4] = { -1, -1, -1, -1 };
  /* maps from encoder indexes to last received CCs */
  int8_t encoderAssigned[4] = { -1, -1, -1, -1 };
  incoming_cc_t ccs[4];

  uint8_t count = ccHandler.incomingCCs.size();
  for (uint8_t i = 0; i < count; i++) {
    ccHandler.incomingCCs.getCopy(i, &ccs[i]);
    incoming_cc_t *cc = &ccs[i];
    for (uint8_t j = 0; j < 4; j++) {
      if ((realEncoders[j].getCC() == cc->cc) &&
          (realEncoders[j].getChannel() == cc->channel)) {
        ccAssigned[i] = j;
        encoderAssigned[j] = i;
        break;
      }
    }
  }

  for (uint8_t i = 0; i < count; i++) {
    incoming_cc_t *cc = &ccs[i];
    int8_t idx = ccAssigned[i];
    if(idx != -1) {
      /* this check is probably redundant XXX */
      if ((realEncoders[idx].getChannel() != cc->channel) ||
          (realEncoders[idx].getCC() != cc->cc)) {
        realEncoders[idx].initCCEncoder(cc->channel, cc->cc);
        realEncoders[idx].setValue(cc->value);
        clearRecording(idx);
      }
    } else {
      for (uint8_t j = 0; j < 4; j++) {
        if (encoderAssigned[j] == -1) {
          idx = ccAssigned[i] = j;
          encoderAssigned[j] = i;
          realEncoders[idx].initCCEncoder(cc->channel, cc->cc);
          realEncoders[idx].setValue(cc->value);
          clearRecording(idx);
          break;
        }
      }
    }
  }
}

template <typename EncoderType>
bool AutoEncoderPage<EncoderType>::handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, learnButton)) {
    autoLearnLast4();
    return true;
  }
  if (EVENT_PRESSED(event, recordButton)) {
    startRecording();
    return true;
  }
  if (EVENT_RELEASED(event, recordButton)) {
    stopRecording();
    return true;
  }
  if (EVENT_PRESSED(event, clearButton) || EVENT_RELEASED(event, clearButton)) {
    return true;
  }
  if (BUTTON_DOWN(clearButton)) {
    for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
        GUI.setLine(GUI.LINE1);
        GUI.flash_string_fill("CLEAR");
        GUI.setLine(GUI.LINE2);
        GUI.flash_put_value(0, i);
        clearRecording(i);
      }
    }
  }
  return false;
}

/** @} @} @} **/

#endif /* AUTOENCODERPAGE_H__ */
