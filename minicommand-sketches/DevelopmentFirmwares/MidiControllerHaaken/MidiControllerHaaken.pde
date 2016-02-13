#include <AutoMidiController.h>
#include <MidiClockPage.h>

AutoMidiControllerSketch sketch;

void CustomCCEncoderHandle(Encoder *enc) {
  CCEncoder *ccEnc = (CCEncoder *)enc;
  uint8_t channel = ccEnc->getChannel();
  uint8_t cc = ccEnc->getCC();
  uint8_t value = ccEnc->getValue();
  
  MidiUart.sendCC(channel, cc, value);
}

void setup() {
  sketch.setup();
  
  for (int page_idx = 0; page_idx < 4; page_idx++) {
    AutoEncoderPage<AutoNameCCEncoder> *page = &sketch.autoPages[page_idx];
    
    for (int encoder_idx = 0; encoder_idx < 4; encoder_idx++) {
      AutoNameCCEncoder *encoder = &page->realEncoders[encoder_idx];
      encoder->handler = CustomCCEncoderHandle;
    }
  }
  
  GUI.setSketch(&sketch);
  initClockPage();
}
