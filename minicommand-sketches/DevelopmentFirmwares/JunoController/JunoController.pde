#include <Juno.h>

JunoEncoder encoders[4];
EncoderPage encoderPage(&encoders[0], &encoders[1], &encoders[2], &encoders[3]);

void setup() {
  encoders[0].initJunoEncoder(0, JUNO_LFO_DELAY, 0);
  encoders[0].setName("LFD");
  encoders[1].initJunoEncoder(0, JUNO_LFO_RATE, 0);
  encoders[1].setName("LFR");
  encoders[2].initJunoEncoder(0, JUNO_ENV_ATTACK_RATE, 0);
  encoders[2].setName("ATR");
  encoders[3].initJunoEncoder(0, JUNO_VCF_CUTOFF_LEVEL, 0);
  encoders[3].setName("CUT");

  encoderPage.setup();

  GUI.setPage(&encoderPage);
}
