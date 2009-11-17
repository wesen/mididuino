#include <MD.h>

MDFXEncoder flfEncoder(MD_ECHO_FLTF, MD_FX_ECHO, "FLF");
MDFXEncoder flwEncoder(MD_ECHO_FLTW, MD_FX_ECHO, "FLW");
MDFXEncoder fbEncoder(MD_ECHO_FB, MD_FX_ECHO, "FB");
MDFXEncoder levEncoder(MD_ECHO_LEV, MD_FX_ECHO, "LEV");
EncoderPage page(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

void setup() {
  GUI.setPage(&page);
}


