#ifndef MD_PARAMS_H__
#define MD_PARAMS_H__


#define MD_ASSIGN_MACHINE_ID                             0x5b
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS                 0
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS         1
#define MD_ASSIGN_MACHINE_INIT_SYNTHESIS_EFFECTS_ROUTING 2

#define MD_CURRENT_GLOBAL_SLOT_REQUEST       0x01
#define MD_CURRENT_KIT_REQUEST               0x02
#define MD_CURRENT_PATTERN_REQUEST           0x04
#define MD_CURRENT_SONG_REQUEST              0x08
#define MD_CURRENT_SEQUENCER_MODE            0x10
#define MD_CURRENT_LOCK_MODE                 0x20

#define MD_GLOBAL_MESSAGE_ID                 0x50
#define MD_GLOBAL_REQUEST_ID                 0x51
#define MD_KIT_MESSAGE_ID                    0x52
#define MD_KIT_REQUEST_ID                    0x53
#define MD_LOAD_KIT_ID                       0x58
#define MD_LOAD_PATTERN_ID                   0x57
#define MD_LOAD_SONG_ID                      0x6c
#define MD_PATTERN_MESSAGE_ID                0x67
#define MD_PATTERN_REQUEST_ID                0x68
#define MD_RESET_MIDI_NOTE_MAP_ID            0x64
#define MD_SAVE_KIT_ID                       0x59
#define MD_SAVE_SONG_ID                      0x6d
#define MD_SET_ACTIVE_GLOBAL_ID              0x56
#define MD_SET_CURRENT_KIT_NAME_ID           0x55
#define MD_SET_DYNAMIX_PARAM_ID              0x60
#define MD_SET_EQ_PARAM_ID                   0x5f
#define MD_SET_GATE_BOX_PARAM_ID             0x5e
#define MD_SET_LFO_PARAM_ID                  0x62
#define MD_SET_MIDI_NOTE_TO_TRACK_MAPPING_ID 0x5a
#define MD_SET_MUTE_GROUP_ID                 0x66
#define MD_SET_RECEIVE_DUMP_POSITION_ID      0x6b
#define MD_SET_RHYTHM_ECHO_PARAM_ID          0x5d
#define MD_SET_STATUS_ID                     0x71
#define MD_SET_TEMPO_ID                      0x61
#define MD_SET_TRACK_ROUTING_ID              0x5c
#define MD_SET_TRIG_GROUP_ID                 0x65
#define MD_SONG_MESSAGE_ID                   0x69
#define MD_SONG_REQUEST_ID                   0x6a
#define MD_STATUS_REQUEST_ID                 0x70
#define MD_STATUS_RESPONSE_ID                0x72

#define GND_MODEL    0
#define GND_SN_MODEL 1
#define GND_NS_MODEL 2
#define GND_IM_MODEL 3

#define TRX_BD_MODEL 16
#define TRX_SD_MODEL 17
#define TRX_XT_MODEL 18
#define TRX_CP_MODEL 19
#define TRX_RS_MODEL 20
#define TRX_CB_MODEL 21
#define TRX_CH_MODEL 22
#define TRX_OH_MODEL 23
#define TRX_CY_MODEL 24
#define TRX_MA_MODEL 25
#define TRX_CL_MODEL 26
#define TRX_XC_MODEL 27
#define TRX_B2_MODEL 28

#define EFM_BD_MODEL 32
#define EFM_SD_MODEL 33
#define EFM_XT_MODEL 34
#define EFM_CP_MODEL 35
#define EFM_RS_MODEL 36
#define EFM_CB_MODEL 37
#define EFM_HH_MODEL 38
#define EFM_CY_MODEL 39

#define E12_BD_MODEL 48
#define E12_SD_MODEL 49
#define E12_HT_MODEL 50
#define E12_LT_MODEL 51
#define E12_CP_MODEL 52
#define E12_RS_MODEL 53
#define E12_CB_MODEL 54
#define E12_CH_MODEL 55
#define E12_OH_MODEL 56
#define E12_RC_MODEL 57
#define E12_CC_MODEL 58
#define E12_BR_MODEL 59
#define E12_TA_MODEL 60
#define E12_TR_MODEL 61
#define E12_SH_MODEL 62
#define E12_BC_MODEL 63

#define P_I_BD_MODEL 64
#define P_I_SD_MODEL 65
#define P_I_MT_MODEL 66
#define P_I_ML_MODEL 67
#define P_I_MA_MODEL 68
#define P_I_RS_MODEL 69
#define P_I_RC_MODEL 70
#define P_I_CC_MODEL 71
#define P_I_HH_MODEL 72

#define INP_GA_MODEL 80
#define INP_GB_MODEL 81
#define INP_FA_MODEL 82
#define INP_FB_MODEL 83
#define INP_EA_MODEL 84
#define INP_EB_MODEL 85

#define MID_MODEL 96

#define MID_01_MODEL 96
#define MID_02_MODEL 97
#define MID_03_MODEL 98
#define MID_04_MODEL 99
#define MID_05_MODEL 100
#define MID_06_MODEL 101
#define MID_07_MODEL 102
#define MID_08_MODEL 103
#define MID_09_MODEL 104
#define MID_10_MODEL 105
#define MID_11_MODEL 106
#define MID_12_MODEL 107
#define MID_13_MODEL 108
#define MID_14_MODEL 109
#define MID_15_MODEL 110
#define MID_16_MODEL 111

#define CTR_AL_MODEL 112
#define CTR_8P_MODEL 113
			
#define CTR_RE_MODEL 120
#define CTR_GB_MODEL 121
#define CTR_EQ_MODEL 122
#define CTR_DX_MODEL 123
			
#define ROM_01_MODEL 128
#define ROM_02_MODEL 129
#define ROM_03_MODEL 130
#define ROM_04_MODEL 131
#define ROM_05_MODEL 132
#define ROM_06_MODEL 133
#define ROM_07_MODEL 134
#define ROM_08_MODEL 135
#define ROM_09_MODEL 136
#define ROM_10_MODEL 137
#define ROM_11_MODEL 138
#define ROM_12_MODEL 139
#define ROM_13_MODEL 140
#define ROM_14_MODEL 141
#define ROM_15_MODEL 142
#define ROM_16_MODEL 143
#define ROM_17_MODEL 144
#define ROM_18_MODEL 145
#define ROM_19_MODEL 146
#define ROM_20_MODEL 147
#define ROM_21_MODEL 148
#define ROM_22_MODEL 149
#define ROM_23_MODEL 150
#define ROM_24_MODEL 151
#define ROM_25_MODEL 152
#define ROM_26_MODEL 153
#define ROM_27_MODEL 154
#define ROM_28_MODEL 155
#define ROM_29_MODEL 156
#define ROM_30_MODEL 157
#define ROM_31_MODEL 158
#define ROM_32_MODEL 159

#define RAM_R1_MODEL 160
#define RAM_R2_MODEL 161
#define RAM_P1_MODEL 162
#define RAM_P2_MODEL 163
			
#define RAM_R3_MODEL 165
#define RAM_R4_MODEL 166
#define RAM_P3_MODEL 167
#define RAM_P4_MODEL 168

#define ROM_33_MODEL 176
#define ROM_34_MODEL 177
#define ROM_35_MODEL 178
#define ROM_36_MODEL 179
#define ROM_37_MODEL 180
#define ROM_38_MODEL 181
#define ROM_39_MODEL 182
#define ROM_40_MODEL 183
#define ROM_41_MODEL 184
#define ROM_42_MODEL 185
#define ROM_43_MODEL 186
#define ROM_44_MODEL 187
#define ROM_45_MODEL 188
#define ROM_46_MODEL 189
#define ROM_47_MODEL 190 
#define ROM_48_MODEL 191

#define ROM_MODEL    128

#define MD_ECHO_TIME 0
#define MD_ECHO_MOD  1
#define MD_ECHO_MFRQ 2
#define MD_ECHO_FB   3
#define MD_ECHO_FLTF 4
#define MD_ECHO_FLTW 5
#define MD_ECHO_MONO 6
#define MD_ECHO_LEV  7

#define MD_REV_DVOL  0
#define MD_REV_PRED  1
#define MD_REV_DEC   2
#define MD_REV_DAMP  3
#define MD_REV_HP    4
#define MD_REV_LP    5
#define MD_REV_GATE  6
#define MD_REV_LEV   7

#define MD_EQ_LF     0
#define MD_EQ_LG     1
#define MD_EQ_HF     2
#define MD_EQ_HG     3
#define MD_EQ_PF     4
#define MD_EQ_PG     5
#define MD_EQ_PQ     6
#define MD_EQ_GAIN   7

#define MD_DYN_ATCK  0
#define MD_DYN_REL   1
#define MD_DYN_TRHD  2
#define MD_DYN_RTIO  3
#define MD_DYN_KNEE  4
#define MD_DYN_HP    5
#define MD_DYN_OUTG  6
#define MD_DYN_MIX   7

#endif /* MD_PARAMS_H__ */
