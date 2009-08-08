void on16Callback() {
  if (MidiClock.div16th_counter % 4 == 0) {
    MidiUart.sendNoteOn(0, 100);
  }
  GUI.setLine(GUI.LINE1);
  GUI.put_value(3, (uint8_t)(MidiClock.div16th_counter % 16));
}

void on16Callback2() {
  if (MidiClock.div16th_counter % 4 == 2) {
    MidiUart.sendNoteOn(0, 102);
  }
}

void on96Callback() {
}

void setup() {
//  MidiClock.mode = MidiClock.EXTERNAL_UART2;
   MidiClock.mode = MidiClock.EXTERNAL_MIDI;
  MidiClock.start();
//  MidiClock.transmit = true;
  //MidiClock.setOn96Callback(on96Callback);
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.addOn16Callback(on16Callback2);
}
  
void loop() {
} 
