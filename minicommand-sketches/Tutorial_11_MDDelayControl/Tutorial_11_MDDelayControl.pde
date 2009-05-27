#include <MD.h>

RangeEncoder flfEncoder(0, 127, "FLF");
RangeEncoder flwEncoder(0, 127, "FLW");
RangeEncoder fbEncoder(0, 127, "FB");
RangeEncoder levEncoder(0, 127, "LEV");
EncoderPage page(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

void setup() {
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();
  if (flfEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTF, flfEncoder.getValue());
  if (flwEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTW, flwEncoder.getValue());
  if (fbEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FB, fbEncoder.getValue());
  if (levEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_LEV, levEncoder.getValue());
  GUI.update();
}

void handleGui() {
}
