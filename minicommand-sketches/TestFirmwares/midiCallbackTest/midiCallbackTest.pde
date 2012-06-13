class MyCallback: public MidiCallback {
  public:
  void onMidiCallback(uint8_t *msg) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill("NOTE");
  }
};



MyCallback cb;

void setup() {

    ADD_CALLBACK(Midi.addOnNoteOnCallback, cb, onMidiCallback);
  //  Midi.addOnNoteOnCallback(&cb, (midi_callback_ptr_t)&MyCallback::onMidiCallback);
}
