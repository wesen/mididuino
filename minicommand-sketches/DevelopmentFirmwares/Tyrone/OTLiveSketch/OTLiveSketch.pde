// See other tabs...
//
//  OT Live Sketch 

// Note - this sketch assumes that you have:
//    RAM RECORDER machine on OT Track 5
//    MD running through a THRU machine on OT Track 6
//    MNM running through a THRU machine on OT Track 7
//    Filter FX assigned to FX block 1 on the THRU and RAM RECORDER machines
//    DJ EQ FX assigned to FX block 2 on the THRU and RAM RECORDER machines
//

// Notes on Variables:  update midi channels for your setup if required.  Variables are defined in _OT01_DjEqPage
//#define RECORDER_MIDI_CHANNEL (5 - 1)  // OT Track / Midi Channel 5
//#define MD_THRU_MIDI_CHANNEL (6 - 1)  // OT Track / Midi Channel 6 
//#define MNM_THRU_MIDI_CHANNEL (7 - 1) // OT Track / Midi Channel 7 
//#define TETRA_MIDI_CHANNEL (12 - 1) // Midi Channel 12
//#define CC_FX_1_PARAM_1 34  //FILTER BASE
//#define CC_FX_1_PARAM_2 35  //FILTER WIDTH
//#define CC_FX_2_PARAM_4 43  //EQ LOW GAIN
//#define CC_FX_2_PARAM_6 45  //EQ HI GAIN
//#define CC_PLAYBACK_PARAM_2 17  //RECORDER START
//#define CC_PLAYBACK_PARAM_4 19  //RECORDER RATE

// General Notes - Firmware has two main "modes":  
// 1.0  Mode 1 - "THRU" machine mode.  Give control of FX on THRU machines.  
// 1.01 There are two pages in "THRU" machine mode.  To toggle between the two pages, press button 1 (top left) when in "THRU" machine mode.
// 1.1  Page 1 - DJEq Page - control of Low and Hi EQ for "MD THRU" and "MNM THRU"
// 1.11 Pressing an Encoder will "cut" the EQ to 0 or toggle it back to its previous value.  An asterix is displayed next to the encoder name to show when EQ "cut" is active
// 1.12 Holding Button 2 (bottom left) and pressing an encoder will restore the EQ back to 64 and disable "cut" mode
// 1.2  Page 2 - Filter Page - control of Filter Cutoffs for "MD THRU" and "MNM THRU"
// 1.21 Pressing an Encoder will "cut" the cutoff to 0 (BAS) or 127 (WDT), or toggle it back to its previous value.  An asterix is displayed next to the encoder name to show when EQ "cut" is active
// 1.22 Holding Button 2 (bottom left) and pressing an encoder will restore the cutoff back to 0 (BAS) or 127 (WDT) and disable "cut" mode
// 2.0  Mode 2 - "RAM RECORDER" mode.  Give control of FX on RAM RECORDER machine.
// 2.01 There is only one page for this mode which controls both the DJ EQ and Filter FX.
// 2.02 Functionality is same as above for fx controls
// 3.0  To switch from "THRU" to "RAM RECORD" mode, press button 4 (top right).  
// 3.1  To switch from "RAM RECORD" to "THRU" mode, press button 1 (top left).  Once back in "THRU" mode, subsequent presses of button 1 will switch between the two pages of "THRU" mode
// 4.0  Press button three to activate the wesen supatrigga functionality for the RAM RECORDER machine.  This can be activated from any mode/page.
