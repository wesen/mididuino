/*
 * MidiCtrl - Juno encoder implementation
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

void JunoEncoderHandle(Encoder *enc) {
  JunoEncoder *junoEnc = (JunoEncoder *)enc;
  Juno.sendParameter(junoEnc->channel, junoEnc->param, junoEnc->getValue());
}

JunoEncoder::JunoEncoder(uint8_t channel, uint8_t param, char *_name, uint8_t init) : RangeEncoder(127, 0, _name, init) {
  initJunoEncoder(channel, param, _name, init);
  handler = JunoEncoderHandle;

}
