#include "helpers.h"
#include "MD.h"

machine_name_t machine_names[] PROGMEM = {
  { "GND---", 0},
  { "GND-SN", 1},
  { "GND-NS", 2},
  { "GND-IM", 3},
  { "TRX-BD", 16},
  { "TRX-SD", 17},
  { "TRX-XT", 18},
  { "TRX-CP", 19},
  { "TRX-RS", 20},
  { "TRX-RS", 21},
  { "TRX-CH", 22},
  { "TRX-OH", 23},
  { "TRX-CY", 24},
  { "TRX-MA", 25},
  { "TRX-CL", 26},
  { "TRX-XC", 27},
  { "TRX-B2", 28},
  { "EFM-BD", 32},
  { "EFM-SD", 33},
  { "EFM-XT", 34},
  { "EFM-CP", 35},
  { "EFM-RS", 36},
  { "EFM-CB", 37},
  { "EFM-HH", 38},
  { "EFM-CY", 39},
  { "E12-BD", 48},
  { "E12-SD", 49},
  { "E12-HT", 50},
  { "E12-LT", 51},
  { "E12-CP", 52},
  { "E12-RS", 53},
  { "E12-CB", 54},
  { "E12-CH", 55},
  { "E12-OH", 56},
  { "E12-RC", 57},
  { "E12-CC", 58},
  { "E12-BR", 59},
  { "E12-TA", 60},
  { "E12-TR", 61},
  { "E12-SH", 62},
  { "E12-BC", 63},
  { "P-I-BD", 64},
  { "P-I-SD", 65},
  { "P-I-MT", 66},
  { "P-I-ML", 67},
  { "P-I-MA", 68},
  { "P-I-RS", 69},
  { "P-I-RC", 70},
  { "P-I-CC", 71},
  { "P-I-HH", 72},
  { "INP-GA", 80},
  { "INP-GB", 81},
  { "INP-FA", 82},
  { "INP-FB", 83},
  { "INP-EA", 84},
  { "INP-EB", 85},
  { "MID-01", 96},
  { "MID-02", 97},
  { "MID-03", 98},
  { "MID-04", 99},
  { "MID-05", 100},
  { "MID-06", 101},
  { "MID-07", 102},
  { "MID-08", 103},
  { "MID-09", 104},
  { "MID-10", 105},
  { "MID-11", 106},
  { "MID-12", 107},
  { "MID-13", 108},
  { "MID-14", 109},
  { "MID-15", 110},
  { "MID-16", 111},
  { "CTR-AL", 112},
  { "CTR-8P", 113},
			
  { "CTR-RE", 120},
  { "CTR-GB", 121},
  { "CTR-EQ", 122},
  { "CTR-DX", 123},
			
  { "ROM-01", 128},
  { "ROM-02", 129},
  { "ROM-03", 130},
  { "ROM-04", 131},
  { "ROM-05", 132},
  { "ROM-06", 133},
  { "ROM-07", 134},
  { "ROM-08", 135},
  { "ROM-09", 136},
  { "ROM-10", 137},
  { "ROM-11", 138},
  { "ROM-12", 139},
  { "ROM-13", 140},
  { "ROM-14", 141},
  { "ROM-15", 142},
  { "ROM-16", 143},
  { "ROM-17", 144},
  { "ROM-18", 145},
  { "ROM-19", 146},
  { "ROM-20", 147},
  { "ROM-21", 148},
  { "ROM-22", 149},
  { "ROM-23", 150},
  { "ROM-24", 151},
  { "ROM-25", 152},
  { "ROM-26", 153},
  { "ROM-27", 154},
  { "ROM-28", 155},
  { "ROM-29", 156},
  { "ROM-30", 157},
  { "ROM-31", 158},
  { "ROM-32", 159},
  { "RAM-R1", 160},
  { "RAM-R2", 161},
  { "RAM-P1", 162},
  { "RAM-P2", 163},
			
  { "RAM-R3", 165},
  { "RAM-R4", 166},
  { "RAM-P3", 167},
  { "RAM-P4", 168},

  { "ROM-33", 176},
  { "ROM-34", 177},
  { "ROM-35", 178},
  { "ROM-36", 179},
  { "ROM-37", 180},
  { "ROM-38", 181},
  { "ROM-39", 182},
  { "ROM-40", 183},
  { "ROM-41", 184},
  { "ROM-42", 185},
  { "ROM-43", 186},
  { "ROM-44", 187},
  { "ROM-45", 188},
  { "ROM-46", 189},
  { "ROM-47", 190},
  { "ROM-48", 191}
};

PGM_P MDClass::getMachineName(uint8_t machine) {
  for (uint8_t i = 0; i < countof(machine_names); i++) {
    if (pgm_read_byte(&machine_names[i].id) == machine) {
      return machine_names[i].name;
    }
  }
  return NULL;
}

model_param_name_t gnd_sn_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3}, {"", 127} };
model_param_name_t gnd_ns_model_names[] PROGMEM = { { "DEC", 0}, {"", 127} };
model_param_name_t gnd_im_model_names[] PROGMEM = { { "UP", 0},
						    { "UVL", 1},
						    { "DWN", 2},
						    { "DVL", 3}, {"", 127} };

model_param_name_t trx_bd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3},
						    { "STR", 4},
						    { "NS", 5},
						    { "HRM", 6},
						    { "CLP", 7}, {"", 127} };
model_param_name_t trx_b2_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "HLD", 3},
						    { "TCK", 4},
						    { "NS", 5},
						    { "DRT", 6},
						    { "DST", 7}, {"", 127} };
model_param_name_t trx_sd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "BMP", 2},
						    { "BNV", 3},
						    { "SNP", 4},
						    { "TON", 5},
						    { "TUN", 6},
						    { "CLP", 7}, {"", 127} };
model_param_name_t trx_xt_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3},
						    { "DMP", 4},
						    { "DST", 5},
						    { "DTP", 6}, {"", 127} };
model_param_name_t trx_cp_model_names[] PROGMEM = { { "CLP", 0},
						    { "TON", 1},
						    { "HRD", 2},
						    { "RCH", 3},
						    { "RAT", 4},
						    { "ROM", 5},
						    { "RSZ", 6},
						    { "RTN", 7}, {"", 127} };
model_param_name_t trx_rs_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "DST", 2}, {"", 127} };
model_param_name_t trx_cb_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 2},
						    { "ENH", 3},
						    { "DMP", 4},
						    { "TON", 5},
						    { "BMP", 6}, {"", 127} };
model_param_name_t trx_ch_model_names[] PROGMEM = { { "GAP", 0},
						    { "DEC", 1},
						    { "HPF", 2},
						    { "LPF", 3},
						    { "MTL", 4}, {"", 127} };
model_param_name_t trx_oh_model_names[] PROGMEM = { { "GAP", 0},
						    { "DEC", 1},
						    { "HPF", 2},
						    { "LPF", 3},
						    { "MTL", 4}, {"", 127} };
model_param_name_t trx_cy_model_names[] PROGMEM = { { "RCH", 0},
						    { "DEC", 1},
						    { "TOP", 2},
						    { "TTU", 3},
						    { "SZ", 4},
						    { "PK", 5}, {"", 127} };
model_param_name_t trx_ma_model_names[] PROGMEM = { { "ATT", 0},
						    { "SUS", 1},
						    { "REV", 2},
						    { "DMP", 3},
						    { "RTL", 4},
						    { "RTP", 5},
						    { "TON", 6},
						    { "HRD", 7}, {"", 127} };
model_param_name_t trx_cl_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "DUA", 2},
						    { "ENH", 3},
						    { "TUN", 4},
						    { "CLC", 5}, {"", 127} };
model_param_name_t trx_xc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3},
						    { "DMP", 4},
						    { "DST", 5},
						    { "DTP", 6}, {"", 127} };

model_param_name_t efm_bd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6},
						    { "MFB", 7}, {"", 127} };
model_param_name_t efm_sd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "NS", 2},
						    { "NDC", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6},
						    { "HPF", 7}, {"", 127} };
model_param_name_t efm_xt_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "RMP", 2},
						    { "RDC", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6},
						    { "CLC", 7}, {"", 127} };
model_param_name_t efm_cp_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "CLP", 2},
						    { "CDC", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6},
						    { "HPF", 7}, {"", 127} };
model_param_name_t efm_rs_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "MOD", 2},
						    { "HPF", 3},
						    { "SNR", 4},
						    { "SPT", 5},
						    { "SDC", 6},
						    { "SMD", 7}, {"", 127} };
model_param_name_t efm_cb_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "SNP", 2},
						    { "FB", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6}, {"", 127} };
model_param_name_t efm_hh_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "TRM", 2},
						    { "TFQ", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6},
						    { "FB", 7}, {"", 127} };
model_param_name_t efm_cy_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "FB", 2},
						    { "HPF", 3},
						    { "MOD", 4},
						    { "MFQ", 5},
						    { "MDC", 6}, {"", 127} };

model_param_name_t e12_bd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "SNP", 2},
						    { "SPL", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_sd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "RNG", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_ht_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_lt_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "RNG", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_cp_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_rs_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "RTL", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_cb_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_ch_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_oh_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "STP", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_rc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "BEL", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_cc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_br_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "REL", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_ta_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_tr_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "HPQ", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_sh_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "SLW", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };
model_param_name_t e12_bc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HP", 2},
						    { "BC", 3},
						    { "STR", 4},
						    { "RTG", 5},
						    { "RTM", 6},
						    { "BND", 7}, {"", 127} };

model_param_name_t p_i_bd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "HMR", 3},
						    { "TNS", 4},
						    { "DMP", 5}, {"", 127} };
model_param_name_t p_i_sd_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "TNS", 3},
						    { "RVL", 4},
						    { "RDC", 5},
						    { "RNG", 6}, {"", 127} };
model_param_name_t p_i_mt_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "HMR", 3},
						    { "TUN", 4},
						    { "DMP", 5},
						    { "SZ", 6},
						    { "POS", 7}, {"", 127} };
model_param_name_t p_i_ml_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "TNS", 3}, {"", 127} };
model_param_name_t p_i_ma_model_names[] PROGMEM = { { "GRN", 0},
						    { "DEC", 1},
						    { "GLN", 2},
						    { "SZ", 4},
						    { "HRD", 5}, {"", 127} };
model_param_name_t p_i_rs_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "RNG", 3},
						    { "RVL", 4},
						    { "RDC", 5}, {"", 127} };
model_param_name_t p_i_rc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "RNG", 3},
						    { "AG", 4},
						    { "AU", 5},
						    { "BR", 6},
						    { "GRB", 7}, {"", 127} };
model_param_name_t p_i_cc_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "HRD", 2},
						    { "RNG", 3},
						    { "AG", 4},
						    { "AU", 5},
						    { "BR", 6},
						    { "GRB", 7}, {"", 127} };
model_param_name_t p_i_hh_model_names[] PROGMEM = { { "PTC", 0},
						    { "DEC", 1},
						    { "CLS", 2},
						    { "RNG", 3},
						    { "AG", 4},
						    { "AU", 5},
						    { "BR", 6},
						    { "CLS", 7}, {"", 127} };

model_param_name_t inp_ga_model_names[] PROGMEM = { { "VOL", 0},
						    { "GAT", 1},
						    { "ATK", 2},
						    { "HLD", 3},
						    { "DEC", 4}, {"", 127} };
model_param_name_t inp_fa_model_names[] PROGMEM = { { "ALV", 0},
						    { "GAT", 1},
						    { "FAT", 2},
						    { "FHL", 3},
						    { "FDC", 4},
						    { "FDP", 5},
						    { "FFQ", 6},
						    { "FQ", 7}, {"", 127} };
model_param_name_t inp_ea_model_names[] PROGMEM = { { "AVL", 0},
						    { "AHL", 1},
						    { "ADC", 2},
						    { "FQ", 3},
						    { "FDP", 4},
						    { "FHL", 5},
						    { "FDC", 6},
						    { "FFQ", 7}, {"", 127} };

model_param_name_t mid_model_names[] PROGMEM = { { "NOT", 0},
						 { "N2", 1},
						 { "N3", 2},
						 { "LEN", 3},
						 { "VEL", 4},
						 { "PB", 5},
						 { "MW", 6},
						 { "AT", 7},
						 { "C1D", 8},
						 { "C1V", 9},
						 { "C2D", 10},
						 { "C2V", 11},
						 { "C3D", 12},
						 { "C3V", 13},
						 { "C4D", 14},
						 { "C4V", 15},
						 { "C5D", 16},
						 { "C5V", 17},
						 { "C6D", 18},
						 { "C6V", 19},
						 { "PCG", 20},
						 { "LFS", 21},
						 { "LFD", 22},
						 { "LFM", 23}, {"", 127} };

model_param_name_t ctr_al_model_names[] PROGMEM = { { "SN1", 0},
						    { "SN2", 1},
						    { "SN3", 2},
						    { "SN4", 3},
						    { "SN5", 4},
						    { "SN6", 5},
						    { "SN7", 6},
						    { "SN8", 7}, {"", 127} };
model_param_name_t ctr_8p_model_names[] PROGMEM = { { "P1", 0},
						    { "P2", 1},
						    { "P3", 2},
						    { "P4", 3}, 
						    { "P5", 4},
						    { "P6", 5},
						    { "P7", 6},
						    { "P8", 7},
						    { "P1T", 8},
						    { "P1P", 9},
						    { "P2T", 10},
						    { "P2P", 11},
						    { "P3T", 12},
						    { "P3P", 13},
						    { "P4T", 14},
						    { "P4P", 15},
						    { "P5T", 16},
						    { "P5P", 17},
						    { "P6T", 18},
						    { "P6P", 19},
						    { "P7T", 20},
						    { "P7P", 21},
						    { "P8T", 22},
						    { "P8P", 23}, {"", 127} };
model_param_name_t rom_model_names[] PROGMEM = { { "PTC", 0},
						 { "DEC", 1},
						 { "HLD", 2},
						 { "BRR", 3},
						 { "STR", 4},
						 { "END", 5},
						 { "RTG", 6},
						 { "RTM", 7}, {"", 127} };
model_param_name_t ram_r_model_names[] PROGMEM = { { "MLV", 0},
						   { "MBL", 1},
						   { "ILV", 2},
						   { "IBL", 3},
						   { "CU1", 4},
						   { "CU2", 5},
						   { "LEN", 6},
						   { "RAT", 7}, {"", 127} };

model_param_name_t generic_param_names[] PROGMEM = { { "AMD", 8 },
						      { "AMF", 9 },
						      { "EQF", 10 },
						      { "EQG", 11 },
						      { "FLF", 12 },
						      { "FLW", 13 },
						      { "FLQ", 14 },
						      { "SRR", 15 },
						      { "DST", 16 },
						      { "VOL", 17 },
						      { "PAN", 18 },
						      { "DEL", 19 },
						      { "REV", 20 },
						      { "LFS", 21 },
						      { "LFD", 22 },
						      { "LFM", 23 }, { "", 127 } };
  
model_to_param_names_t model_param_names[] = {
  { GND_SN_MODEL, gnd_sn_model_names },
  { GND_NS_MODEL, gnd_ns_model_names },
  { GND_IM_MODEL, gnd_im_model_names },
  
  { TRX_BD_MODEL, trx_bd_model_names },
  { TRX_B2_MODEL, trx_b2_model_names },
  { TRX_SD_MODEL, trx_sd_model_names },
  { TRX_XT_MODEL, trx_xt_model_names },
  { TRX_CP_MODEL, trx_cp_model_names },
  { TRX_RS_MODEL, trx_rs_model_names },
  { TRX_CH_MODEL, trx_ch_model_names },
  { TRX_OH_MODEL, trx_oh_model_names },
  { TRX_CY_MODEL, trx_cy_model_names },
  { TRX_MA_MODEL, trx_ma_model_names },
  { TRX_CL_MODEL, trx_cl_model_names },
  { TRX_XC_MODEL, trx_xc_model_names },

  { EFM_BD_MODEL, efm_bd_model_names },
  { EFM_SD_MODEL, efm_sd_model_names },
  { EFM_XT_MODEL, efm_xt_model_names },
  { EFM_CP_MODEL, efm_cp_model_names },
  { EFM_RS_MODEL, efm_rs_model_names },
  { EFM_CB_MODEL, efm_cb_model_names },
  { EFM_HH_MODEL, efm_hh_model_names },
  { EFM_CY_MODEL, efm_cy_model_names },

  { E12_BD_MODEL, e12_bd_model_names },
  { E12_SD_MODEL, e12_sd_model_names },
  { E12_HT_MODEL, e12_ht_model_names },
  { E12_LT_MODEL, e12_lt_model_names },
  { E12_CP_MODEL, e12_cp_model_names },
  { E12_RS_MODEL, e12_rs_model_names },
  { E12_CB_MODEL, e12_cb_model_names },
  { E12_CH_MODEL, e12_ch_model_names },
  { E12_OH_MODEL, e12_oh_model_names },
  { E12_RC_MODEL, e12_rc_model_names },
  { E12_CC_MODEL, e12_cc_model_names },
  { E12_BR_MODEL, e12_br_model_names },
  { E12_TA_MODEL, e12_ta_model_names },
  { E12_TR_MODEL, e12_tr_model_names },
  { E12_SH_MODEL, e12_sh_model_names },
  { E12_BC_MODEL, e12_bc_model_names },

  { P_I_BD_MODEL, p_i_bd_model_names },
  { P_I_SD_MODEL, p_i_sd_model_names },
  { P_I_MT_MODEL, p_i_mt_model_names },
  { P_I_ML_MODEL, p_i_ml_model_names },
  { P_I_MA_MODEL, p_i_ma_model_names },
  { P_I_RS_MODEL, p_i_rs_model_names },
  { P_I_RC_MODEL, p_i_rc_model_names },
  { P_I_CC_MODEL, p_i_cc_model_names },
  { P_I_HH_MODEL, p_i_hh_model_names },

  { INP_GA_MODEL, inp_ga_model_names },
  { INP_FA_MODEL, inp_fa_model_names },
  { INP_EA_MODEL, inp_ea_model_names },

  { INP_GB_MODEL, inp_ga_model_names },
  { INP_FB_MODEL, inp_fa_model_names },
  { INP_EB_MODEL, inp_ea_model_names },

  { MID_MODEL,    mid_model_names },

  { CTR_AL_MODEL, ctr_al_model_names },
  { CTR_8P_MODEL, ctr_8p_model_names },
  { ROM_MODEL,    rom_model_names },
  { RAM_R1_MODEL,  ram_r_model_names },
  { RAM_R2_MODEL,  ram_r_model_names },
  { RAM_R3_MODEL,  ram_r_model_names },
  { RAM_R4_MODEL,  ram_r_model_names }
};

static PGM_P get_param_name(model_param_name_t *names, uint8_t param) {
  uint8_t i = 0;
  uint8_t id;
  if (names == NULL)
    return NULL;
  
  while ((id = pgm_read_byte(&names[i].id)) != 127 && i < 24) {
    if (id == param) {
      return names[i].name ;
    }
    i++;
  }
  return NULL;
}

static model_param_name_t *get_model_param_names(uint8_t model) {
  for (uint16_t i = 0; i < countof(model_param_names); i++) {
    if (model == model_param_names[i].model) {
      return model_param_names[i].names;
    }
  }
  return NULL;
}

PGM_P model_param_name(uint8_t model, uint8_t param) {
  if (param == 32) {
    return PSTR("MUT");
  } else if (param == 33) {
    return PSTR("LEV");
  }
  
  if (model >= MID_MODEL && model <= MID_16_MODEL) {
    return get_param_name(mid_model_names, param);
  }
  if (model >= CTR_8P_MODEL && model < ROM_01_MODEL) {
    return get_param_name(get_model_param_names(model), param);
  }
  if (param >= 8) {
    return get_param_name(generic_param_names, param);
  } else {
    if ((model >= ROM_01_MODEL && model <= ROM_32_MODEL) ||
	(model >= ROM_33_MODEL && model <= ROM_48_MODEL)) {
      return get_param_name(rom_model_names, param);
    }
    if (model == RAM_R1_MODEL ||
	model == RAM_R2_MODEL ||
	model == RAM_R3_MODEL ||
	model == RAM_R4_MODEL) {
      return get_param_name(ram_r_model_names, param);
    }
    if (model == RAM_P1_MODEL ||
	model == RAM_P2_MODEL ||
	model == RAM_P3_MODEL ||
	model == RAM_P4_MODEL) {
      return get_param_name(rom_model_names, param);
    }
    return get_param_name(get_model_param_names(model), param);
  }
}
