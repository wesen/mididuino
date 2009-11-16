#include <MD.h>

RangeEncoder paramEncoder(0, 23, "PRM");
RangeEncoder shape1Encoder(0, 5, "SH1");
RangeEncoder shape2Encoder(0, 5, "SH2");
RangeEncoder typeEncoder(0, 2, "TYP");
RangeEncoder speedEncoder(0, 127, "SPD");
RangeEncoder depthEncoder(0, 127, "DPT");
RangeEncoder mixEncoder(0, 127, "MIX");

EncoderPage page1(&paramEncoder, &shape1Encoder, &shape2Encoder, &typeEncoder);
EncoderPage page2(&speedEncoder, &depthEncoder, &mixEncoder);

void setup() {
  GUI.setPage(&page1);
}

void loop() {
  GUI.updatePage();
  if (paramEncoder.hasChanged())
    MD.setLFOParam(0, 1, paramEncoder.getValue());
  if (shape1Encoder.hasChanged())
    MD.setLFOParam(0, 2, shape1Encoder.getValue());
  if (shape2Encoder.hasChanged())
    MD.setLFOParam(0, 3, shape2Encoder.getValue());
  if (typeEncoder.hasChanged())
    MD.setLFOParam(0, 4, typeEncoder.getValue());

  if (speedEncoder.hasChanged())
    MD.setLFOParam(0, 5, speedEncoder.getValue());
  if (depthEncoder.hasChanged())
    MD.setLFOParam(0, 6, depthEncoder.getValue());
  if (mixEncoder.hasChanged())
    MD.setLFOParam(0, 7, mixEncoder.getValue());
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.setPage(&page1);
  }
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&page2);
  }
}
