#include <inttypes.h>
#include "MDRecorder.h"

void _MDRecorder_onNoteOnCallback(uint8_t *msg);
void _MDRecorder_onCCCallback(uint8_t *msg);
void _MDRecorder_on16Callback();

MDRecorderClass::MDRecorderClass() {
  recording = false;
  playing = false;
  start16th = 0;
  recordLength = 0;
}

void MDRecorderClass::startRecord(uint8_t length) {
  eventList.freeAll();
  
  USE_LOCK();
  SET_LOCK();

  start16th = MidiClock.div16th_counter;
  rec16th_counter = 0;
  MidiUart.addOnNoteOnCallback(_MDRecorder_onNoteOnCallback);
  MidiUart.addOnControlChangeCallback(_MDRecorder_onCCCallback);
  MidiClock.addOn16Callback(_MDRecorder_on16Callback);

  CLEAR_LOCK();
}

void MDRecorderClass::playback() {
}

void MDRecorderClass::onNoteOnCallback(uint8_t *msg) {
  USE_LOCK();
  SET_LOCK();
  uint8_t pos = rec16th_counter;
  CLEAR_LOCK();
  
  ListElt<md_recorder_event_t> *elt = eventList.pool.alloc();
  if (elt != NULL) {
    elt->obj.channel = msg[0] & 0xF;
    elt->obj.pitch = msg[1];
    elt->obj.value = msg[2];
    elt->obj.step = pos;
    eventList.push(elt);
  }
}

void MDRecorderClass::onCCCallback(uint8_t *msg) {
  USE_LOCK();
  SET_LOCK();
  uint8_t pos = rec16th_counter;
  CLEAR_LOCK();
  
  ListElt<md_recorder_event_t> *elt = eventList.pool.alloc();
  if (elt != NULL) {
    elt->obj.channel = (msg[0] & 0xF) | 0x80;
    elt->obj.pitch = msg[1];
    elt->obj.value = msg[2];
    elt->obj.step = pos;
    eventList.push(elt);
  }
}

void MDRecorderClass::on16Callback() {
  if (++rec16th_counter > recordLength) {
    USE_LOCK();
    SET_LOCK();
    recording = false;
    MidiUart.removeOnNoteOnCallback(_MDRecorder_onNoteOnCallback);
    MidiUart.removeOnControlChangeCallback(_MDRecorder_onCCCallback);
    MidiClock.removeOn16Callback(_MDRecorder_on16Callback);
    CLEAR_LOCK();
  }
}

MDRecorderClass MDRecorder;

void _MDRecorder_onNoteOnCallback(uint8_t *msg) {
  MDRecorder.onNoteOnCallback(msg);
}

void _MDRecorder_onCCCallback(uint8_t *msg) {
  MDRecorder.onCCCallback(msg);
}

void _MDRecorder_on16Callback() {
  MDRecorder.on16Callback();
}
