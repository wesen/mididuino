#ifndef RECORDING_ENCODER_H__
#define RECORDING_ENCODER_H__

template <int N>
class RecordingEncoder : public Encoder {
public:
  Encoder *realEnc;
  int value[N];
  bool recording;
  bool recordChanged;
  bool playing;
  int currentPos;

  RecordingEncoder(Encoder *_realEnc) {
    realEnc = _realEnc;
    recording = false;
    playing = true;
    clearRecording();
    currentPos = 0;
  }

  void startRecording();
  void stopRecording();
  void clearRecording();
  void playback(int pos);

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
  virtual void setValue(int value, bool handle = false) {
    realEnc->setValue(value, handle);
    redisplay = realEnc->redisplay;
  }
  virtual void displayAt(int i) {
    realEnc->displayAt(i);
  }
};

/* RecordingEncoder */
template <int N>
void RecordingEncoder<N>::startRecording() {
  recordChanged = false;
  recording = true;
}

template <int N>
void RecordingEncoder<N>::stopRecording() {
  recordChanged = false;
  recording = false;
}

template <int N>
void RecordingEncoder<N>::clearRecording() {
  for (int i = 0; i < N; i++) {
    value[i] = -1;
  }  
}

template <int N>
int RecordingEncoder<N>::update(encoder_t *enc) {
  uint8_t tmp = SREG;
  //    cli();
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
    }
  }
  SREG = tmp;
  return cur;
}  

template <int N>
void RecordingEncoder<N>::playback(int pos) {
  if (!playing)
    return;

  currentPos = (pos % N);
  if (value[currentPos] != -1) {
    if (!(recording && recordChanged))
      setValue(value[currentPos], true);
  }
}

#endif /* RECORDING_ENCODER_H__ */
