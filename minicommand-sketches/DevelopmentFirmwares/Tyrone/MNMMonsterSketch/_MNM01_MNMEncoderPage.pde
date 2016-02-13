#include <MNM.h>
#include "Platform.h"
#include "GUI.h"
#include "CCHandler.h"
static const int ENCODER_TOTAL_STEPS = 64;

class MNMRecordingEncoder : public MNMEncoder {

public:
  MNMEncoder *realEnc;
  int value[ENCODER_TOTAL_STEPS];
  bool recording;
  bool recordChanged;
  bool playing;
  int currentPos;

  /** Create a recording encoder wrapper for the actual MNMEncoder _realEnc. **/
  MNMRecordingEncoder(MNMEncoder *_realEnc = NULL) {
    initMNMRecordingEncoder(_realEnc);
  }

  void initMNMRecordingEncoder(MNMEncoder *_realEnc) {
    realEnc = _realEnc;
    recording = false;
    playing = true;
    clearRecording();
    currentPos = 0;
  }

  void startRecording();
  void stopRecording();
  void clearRecording();
  void playback();

  virtual char *getName() {
    return realEnc->getName();
  }

  virtual void setName(char *_name) {
    realEnc->setName(_name);
  }
  virtual int update(encoder_t *enc);
  virtual void checkHandle() {
    realEnc->checkHandle();
  }
  virtual bool hasChanged() {
    return realEnc->hasChanged();
  }

  virtual int getValue() {
    return realEnc->getValue();
  }
  virtual int getOldValue() {
    return realEnc->getOldValue();
  }
  virtual void setValue(int _value, bool handle = false) {
    realEnc->setValue(_value, handle);
    redisplay = realEnc->redisplay;
  }

  virtual void displayAt(int i) {
    realEnc->displayAt(i);
  }

};

void MNMRecordingEncoder::startRecording() {
  recordChanged = false;
  recording = true;
}

void MNMRecordingEncoder::stopRecording() {
  recordChanged = false;
  recording = false;
}

void MNMRecordingEncoder::clearRecording() {
  for (int i = 0; i < ENCODER_TOTAL_STEPS; i++) {
    value[i] = -1;
  }  
  playing = false;
}

int MNMRecordingEncoder::update(encoder_t *enc) {

  cur = realEnc->update(enc);
  redisplay = realEnc->redisplay;

  if (recording) {
    if (!recordChanged) {
      if (enc->normal != 0 || enc->button != 0) {
        recordChanged = true;
      }
    }
    if (recordChanged) {
      int pos = currentPos;
      value[pos] = cur;
      playing = true;
    }
  }
  return cur;
}  

void MNMRecordingEncoder::playback() {
  
  currentPos = (MidiClock.div16th_counter) % ENCODER_TOTAL_STEPS;

  if (!playing){
    return;
  }

  if (value[currentPos] != -1) {
    if (!(recording && recordChanged)) {
      realEnc->setValue(value[currentPos], true);
      redisplay = realEnc->redisplay;
    }
    // check if real encoder has change value XXX
  }
}




class MNMEncoderPage : public EncoderPage, public ClockCallback {
 public:
  MNMEncoder realEncoders[4];
  MNMRecordingEncoder recEncoders[4];
  bool muted;

  /** Button to press to autolearn last 4. **/
  uint8_t learnButton;
  /** Button to press to start/stop recording. **/
  uint8_t recordButton;
  /** Button to press to clear recordings. **/
  uint8_t clearButton;

  void startRecording();
  void stopRecording();
  void clearRecording();
  void clearRecording(uint8_t i);  
  void on16Callback();    
  
  void clearEncoder(uint8_t i);
  void learnEncoder(uint8_t i);  
  virtual void setup();
  void autoLearnLast4();

  virtual bool handleEvent(gui_event_t *event);
};

void MNMEncoderPage::on16Callback() {
  if (muted){
    return;
  }
  for (int i = 0; i < 4; i++) {
    recEncoders[i].playback();
  }
}

void MNMEncoderPage::startRecording() {
  for (int i = 0; i < 4; i++) {
    recEncoders[i].startRecording();
  }
}

void MNMEncoderPage::stopRecording() {
  for (int i = 0; i < 4; i++) {
    recEncoders[i].stopRecording();
  }
}

void MNMEncoderPage::clearRecording() {
  for (int i = 0; i < 4; i++) {
    recEncoders[i].clearRecording();
  }
}

void MNMEncoderPage::clearRecording(uint8_t i) {
  recEncoders[i].clearRecording();
}

void MNMEncoderPage::setup() {
  learnButton = Buttons.BUTTON2;
  recordButton = Buttons.BUTTON3;
  clearButton = Buttons.BUTTON4;
  
  muted = false;
  for (uint8_t i = 0; i < 4; i++) {
    realEncoders[i].initMNMEncoder(0, 0, "___", 0);
    recEncoders[i].initMNMRecordingEncoder(&realEncoders[i]);
    encoders[i] = &recEncoders[i];
    ccHandler.addEncoder(&realEncoders[i]);
  }
  
  MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&MNMEncoderPage::on16Callback);
  EncoderPage::setup();
}

void MNMEncoderPage::autoLearnLast4() {
  int8_t ccAssigned[4] = { -1, -1, -1, -1 };
  int8_t encoderAssigned[4] = { -1, -1, -1, -1 };
  incoming_cc_t ccs[4];
	
  // see if any encoders are already assigned to ccs in the cchandler buffer
  uint8_t count = ccHandler.incomingCCs.size();
  for (uint8_t i = 0; i < count; i++) {
    ccHandler.incomingCCs.getCopy(i, &ccs[i]);
    incoming_cc_t *cc = &ccs[i];
    for (uint8_t j = 0; j < 4; j++) {
      if ((realEncoders[j].getCC() == cc->cc) && (realEncoders[j].getChannel() == cc->channel)) {
        ccAssigned[i] = j;
        encoderAssigned[j] = i;
        break;
      }
    }
  }

  for (uint8_t i = 0; i < count; i++) {
    incoming_cc_t *cc = &ccs[i];
    if (ccAssigned[i] != -1) {
      if ((realEncoders[ccAssigned[i]].getChannel() != cc->channel) && (realEncoders[ccAssigned[i]].getCC() != cc->cc)) {
        realEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
        realEncoders[ccAssigned[i]].setValue(cc->value);
      }
    } else {
      for (uint8_t j = 0; j < 4; j++) {
        if (encoderAssigned[j] == -1) {
          ccAssigned[i] = j;
          encoderAssigned[j] = i;
          realEncoders[ccAssigned[i]].initCCEncoder(cc->channel, cc->cc);
          realEncoders[ccAssigned[i]].setValue(cc->value);
          break;
        }
      }
    }
  }
}

void MNMEncoderPage::clearEncoder(uint8_t i) {
	
        realEncoders[i].cc = 0;
      	realEncoders[i].channel = 0;
      	realEncoders[i].initMNMEncoder(0, 0, "___", 0);
      	ccHandler.incomingCCs.clear();
}

// assigns the last incoming cc in the cchandler buffer to the specified encoder
void MNMEncoderPage::learnEncoder(uint8_t i) {
	  
  incoming_cc_t cc;	
  uint8_t count = ccHandler.incomingCCs.size();	
  for (uint8_t j = 0; j < count; j++) {
  	  if (j == 0){
	    ccHandler.incomingCCs.getCopy(j, &cc);
	    realEncoders[i].initCCEncoder(cc.channel, cc.cc);
	    realEncoders[i].setValue(cc.value);	    
	  }
  }



}

bool MNMEncoderPage::handleEvent(gui_event_t *event) {

  // Button3 while Button1 is UP and Button3 is UP = start/stop Encoder Record Mode
  if (BUTTON_UP(learnButton) && BUTTON_UP(clearButton)) {
  	if (EVENT_PRESSED(event, recordButton)) {
  		GUI.flash_strings_fill("RECORD MODE ON", "");
    	startRecording();
    	return true;
  	}
	if (EVENT_RELEASED(event, recordButton)) {
  		GUI.flash_strings_fill("RECORD MODE OFF", "");	
    	stopRecording();
   		return true;
  	}
  }

  // Pressing Encoder while Button2 is UP and Button3 is DOWN and Button4 is DOWN = clear Encoder Recording
  if (BUTTON_DOWN(clearButton) && BUTTON_UP(learnButton) && BUTTON_DOWN(recordButton)) {
    for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
        GUI.setLine(GUI.LINE1);
        GUI.flash_string_fill("CLEAR RECORDING:");
        GUI.setLine(GUI.LINE2);
        GUI.flash_put_value(0, i + 1);
        clearRecording(i);
      }
    }
  }

  // Pressing Encoder while Button2 is UP and Button4 is DOWN = clear CC assigned to Encoder
  if (BUTTON_DOWN(clearButton) && BUTTON_UP(learnButton) && BUTTON_UP(recordButton)) {
    for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
		    GUI.setLine(GUI.LINE1);
		    GUI.flash_string_fill("CLEAR ENCODER CC");
		    GUI.setLine(GUI.LINE2);
		    GUI.flash_put_value(0, i + 1);      
        	clearEncoder(i);
      }
    }
  }

  // Pressing Encoder while Button2 is DOWN and Button4 is UP = assign last incoming CC to Encoder  
  if (BUTTON_UP(clearButton) && BUTTON_DOWN(learnButton) && BUTTON_UP(recordButton)) {
    for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
		    GUI.setLine(GUI.LINE1);
		    GUI.flash_string_fill("LEARN ENCODER CC");
		    GUI.setLine(GUI.LINE2);
		    GUI.flash_put_value(0, i + 1);      
        	learnEncoder(i);
        	return true;
      } 
    } 
    
  // Pressing Button 1 while Button2 is DOWN and Button4 is UP = assign last 4 incoming CCs to Encoders  
	if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
  		GUI.flash_strings_fill("AUTO LEARN", "LAST 4 CCs");	
  		autoLearnLast4();
   		return true;
  	}      
    
  }  
  
  
  return false;
}
