/*
 * MidiCtrl - Octatrack params
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */


#include "Platform.h"
#include "helpers.h"
#include "OT.h"

const model_param_name_t ot_playback_names[] PROGMEM =
{
 { "PL1", 0 },
 { "PL2", 1 },
 { "PL3", 2 },
 { "PL4", 3 },
 { "PL5", 4 },
 { "PL6", 5 },
 { "", 127 }
};

const model_param_name_t ot_amp_names[] PROGMEM =
{
 { "ATK", 0 },
 { "HLD", 1 },
 { "REL", 2 },
 { "VOL", 3 },
 { "BAL", 4 },
 { "", 127 }
};

const model_param_name_t ot_lfo_names[] PROGMEM =
{
 {"LP1", 0 },
 {"LP2", 1 },
 {"LP3", 2 },
 {"LD1", 3 },
 {"LD2", 4 },
 {"LD3", 5 },
 { "", 127 }
};

const model_param_name_t ot_fx1_names[] PROGMEM =
{
 {"FX1", 0},
 {"FX2", 1},
 {"FX3", 2},
 {"FX4", 3},
 {"FX5", 4},
 {"FX6", 5},
 { "", 127 }
};

const model_param_name_t ot_fx2_names[] PROGMEM =
{
 {"FX1", 0},
 {"FX2", 1},
 {"FX3", 2},
 {"FX4", 3},
 {"FX5", 4},
 {"FX6", 5},
 { "", 127 }
};

const model_param_name_t ot_flex_names[] PROGMEM =
{
 {"PTC", 0},
 {"STR", 1},
 {"LEN", 2},
 {"RAT", 3},
 {"RTR", 4},
 {"RTM", 5},
 { "", 127 }
};

const model_param_name_t *ot_static_names = ot_flex_names;

const model_param_name_t ot_flex_setup_names[] PROGMEM =
{
 {"LOP", 0},
 {"SLC", 1},
 {"LEN", 2},
 {"RAT", 3},
 {"TST", 4},
 {"TSN", 5},
 { "", 127 }
};

const model_param_name_t *ot_static_setup_names  = ot_flex_setup_names;

const model_param_name_t ot_fx_filter_names[] PROGMEM =
{
 {"BAS", 0},
 {"WDT", 1},
 {"Q", 2},
 {"DPT", 3},
 {"ATK", 4},
 {"DEC", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_filter_setup_names[] PROGMEM =
{
 {"HP", 0},
 {"LP", 1},
 {"ENV", 2},
 {"HLD", 3},
 {"Q", 4},
 {"DST", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_eq_names[] PROGMEM =
{
 {"FQ1", 0},
 {"GN1", 1},
 {"Q1", 2},
 {"FQ2", 3},
 {"GN2", 4},
 {"Q2", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_eq_setup_names[] PROGMEM =
{
 {"TP1", 0},
 {"TP2", 2},
 { "", 127 }
};

const model_param_name_t ot_fx_djeq_names[] PROGMEM =
{
 {"LSF", 0},
 {"HSF", 2},
 {"LOG", 3},
 {"MIG", 4},
 {"HIG", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_phaser_names[] PROGMEM =
{
 {"CTR", 0},
 {"DEP", 1},
 {"SPF", 2},
 {"FB", 3},
 {"WID", 4},
 {"MIX", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_phaser_setup_names[] PROGMEM =
{
 {"NUM", 1},
 { "", 127 }
};

const model_param_name_t ot_fx_flanger_names[] PROGMEM =
{
 {"DEL", 0},
 {"DEP", 1},
 {"SPD", 2},
 {"FB", 3},
 {"WID", 4},
 {"MIX", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_chorus_names[] PROGMEM =
{
 {"DEL", 0},
 {"DEP", 1},
 {"SPD", 2},
 {"FB", 3},
 {"WID", 4},
 {"MIX", 5},
 {"", 127}
};

const model_param_name_t ot_fx_chorus_setup_names[] PROGMEM =
{
 {"TAP", 0},
 {"FLP", 3},
 { "", 127 }
};

const model_param_name_t ot_fx_compressor_names[] PROGMEM =
{
 {"ATK", 0},
 {"REK", 1},
 {"THR", 2},
 {"RAT", 3},
 {"GN", 4},
 {"MIX", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_compressor_setup_names[] PROGMEM =
{
 {"RMS", 0},
 { "", 127 }
};

const model_param_name_t ot_fx_lofi_names[] PROGMEM =
{
 {"DST", 0},
 {"AMF", 2},
 {"SRR", 3},
 {"BRR", 4},
 {"AMD", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_lofi_setup_names[] PROGMEM =
{
 {"AMP", 2},
 { "", 127 }
};

const model_param_name_t ot_fx_delay_names[] PROGMEM =
{
 {"TIM", 0},
 {"FB", 1},
 {"VOL", 2},
 {"BAS", 3},
 {"WDT", 4},
 {"SND", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_delay_setup_names[] PROGMEM =
{
 {"X", 0},
 {"TAP", 1},
 {"DIR", 2},
 {"SYN", 3},
 {"LCK", 4},
 {"PAS", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_reverb_names[] PROGMEM =
{
 {"TIM", 0},
 {"DMP", 1},
 {"GAT", 2},
 {"HP", 3},
 {"LP", 4},
 {"MIX", 5},
 { "", 127 }
};

const model_param_name_t ot_fx_reverb_setup_names[] PROGMEM =
{
 {"GVL", 0},
 { "", 127 }
};

const model_param_name_t ot_generic_names[] PROGMEM =
{
 { "LEV", 7 },
 { "BAL", 8 },
 { "ATK", 22 },
 { "HLD", 23 },
 { "REL", 24 },
 { "VOL", 25 },
 { "BAL", 26 },
 {"LP1", 28 },
 {"LP2", 29 },
 {"LP3", 30 },
 {"LD1", 31 },
 {"LD2", 32 },
 {"LD3", 33 },
 {"LEV", 46},
 {"CUL", 47},
 {"XF", 48},
 {"MUT", 49},
 {"SOL", 50},
 {"CUE", 51},
 { "", 127 }
};

model_to_param_names_t ot_model_param_names[] =
{
 { OT_MODEL_FLEX, ot_flex_names },
 { OT_MODEL_STATIC, ot_static_names },
// { OT_MACHINE_PICKUP, ot_pi
};

model_to_param_names_t ot_fx_param_names[] =
{
 { OT_FX_FILTER, ot_fx_filter_names },
 { OT_FX_EQ, ot_fx_eq_names },
 { OT_FX_DJEQ, ot_fx_djeq_names },
 { OT_FX_PHASER, ot_fx_phaser_names },
 { OT_FX_FLANGER, ot_fx_flanger_names },
 { OT_FX_CHORUS, ot_fx_chorus_names },
 { OT_FX_COMPRESSOR, ot_fx_compressor_names },
 { OT_FX_LOFI, ot_fx_lofi_names },
 { OT_FX_DELAY,ot_fx_delay_names },
 { OT_FX_REVERB, ot_fx_reverb_names }
};

static model_param_name_t const *get_model_param_names(uint8_t model) {
  for (uint8_t i = 0; i < countof(ot_model_param_names); i++) {
    if (model == ot_model_param_names[i].model) {
      return ot_model_param_names[i].names;
    }
  }
  return NULL;
}

static model_param_name_t const *get_fx_param_names(uint8_t fx) {
  for (uint8_t i = 0; i < countof(ot_fx_param_names); i++) {
    if (fx == ot_fx_param_names[i].model) {
      return ot_fx_param_names[i].names;
    }
  }
  return NULL;
}

PGM_P ot_track_param_name(OTTrack *track, uint8_t param) {
  if (IS_OT_FX_PARAM(param)) {
    if (param >= 40) {
      return ot_model_fx_name(track->fx2, param);
    } else {
      return ot_model_fx_name(track->fx1, param);
    }
  } else {
    return ot_model_param_name(track->model, param);
  }
}

PGM_P ot_model_param_name(uint8_t model, uint8_t param) {
  if (param >= OT_AMP_ATTACK) {
    return get_param_name(ot_generic_names, param);
  } else {
    return get_param_name(get_model_param_names(model), param);
  }
}

PGM_P ot_model_fx_name(uint8_t fx, uint8_t param) {
  return get_param_name(get_fx_param_names(fx), GET_OT_FX_PARAM(param));
}
