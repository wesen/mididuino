#include <MD.h>
#include <BreakdownPage.h>

/**
*
*    Xone4D must be in Midi Map 2 (Ableton default) mode  !!!
*
**/
// Xone 4D Midi Channels (0 based, so add 1 to get "real" channel number)
#define mdBaseMidiChannel 0
#define mnmBaseMidiChannel 4
#define mnmAutoTrackMidiChannel 10
#define mnmTransposeMidiChannel 12
#define tetraMidiChannel 11
//#define dp4MidiChannel 12
#define abletonMidiChannel 13
#define oberheimMidiChannel 14
#define xone4DMidiChannel 15 

// Xone4D Fader CC Global Vars
#define ccLeftFader1 19
#define ccLeftFader2 18
#define ccLeftFader3 17
#define ccLeftFader4 16
#define ccRightFader1 31
#define ccRightFader2 30
#define ccRightFader3 29
#define ccRightFader4 28

// Xone PotName CcValue 
#define ccTopLeftPot1 9
#define ccTopLeftPot2 11
#define ccTopLeftPot3 13
#define ccTopLeftPot4 15
#define ccBottomLeftPot1 8
#define ccBottomLeftPot2 10
#define ccBottomLeftPot3 12
#define ccBottomLeftPot4 14
#define ccTopRightPot1 21
#define ccTopRightPot2 23
#define ccTopRightPot3 25
#define ccTopRightPot4 27
#define ccBottomRightPot1 20
#define ccBottomRightPot2 22
#define ccBottomRightPot3 24
#define ccBottomRightPot4 26

// Xone EncoderName / CcValues 
#define ccTopLeftEncoder1 32
#define ccTopLeftEncoder2 33
#define ccTopLeftEncoder3 34
#define ccTopLeftEncoder4 35
#define ccTopRightEncoder1 40
#define ccTopRightEncoder2 41
#define ccTopRightEncoder3 42
#define ccTopRightEncoder4 43
#define ccBottomLeftEncoder1 36
#define ccBottomRightEncoder1 44

//Xone4D JogName / CcValues 
#define ccLeftJogScroll 37
#define ccRightJogScroll 45

// Xone EncoderName / NoteValues 
#define noteTopLeftEncoder1 28
#define noteTopLeftEncoder2 14
#define noteTopLeftEncoder3 16
#define noteTopLeftEncoder4 26
#define noteTopRightEncoder1 60
#define noteTopRightEncoder2 46
#define noteTopRightEncoder3 48
#define noteTopRightEncoder4 58
#define noteBottomLeftEncoder1 24
#define noteBottomRightEncoder1 56

//Xone4D ButtonName / NoteValues 
#define noteTopLeftButton1 29
#define noteTopLeftButton2 15
#define noteTopLeftButton3 17
#define noteTopLeftButton4 27
#define noteMiddleLeftButton1 36
#define noteMiddleLeftButton2 21
#define noteMiddleLeftButton3 18
#define noteMiddleLeftButton4 32
#define noteBottomLeftButton1 37
#define noteBottomLeftButton2 22
#define noteBottomLeftButton3 19
#define noteBottomLeftButton4 33
#define noteTopRightButton1 61
#define noteTopRightButton2 47
#define noteTopRightButton3 49
#define noteTopRightButton4 59
#define noteMiddleRightButton1 68
#define noteMiddleRightButton2 53
#define noteMiddleRightButton3 50
#define noteMiddleRightButton4 64
#define noteBottomRightButton1 69
#define noteBottomRightButton2 54
#define noteBottomRightButton3 51
#define noteBottomRightButton4 65

//Xone4D JogName / NoteValues  
#define noteLeftJogUpClick 10
#define noteLeftJogRightClick 9
#define noteLeftJogDownClick 13
#define noteLeftJogLeftClick 11
#define noteRightJogUpClick 42
#define noteRightJogRightClick 41
#define noteRightJogDownClick 45
#define noteRightJogLeftClick 43

//Xone4D LightRingName / NoteValue
#define noteBottomLeftLightRing1 38
#define noteBottomLeftLightRing2 23
#define noteBottomLeftLightRing3 20
#define noteBottomLeftLightRing4 34
#define noteBottomRightLightRing1 70
#define noteBottomRightLightRing2 55
#define noteBottomRightLightRing3 52
#define noteBottomRightLightRing4 66
#define noteLeftFilterModeLightRing1 84
#define noteLeftFilterModeLightRing2 83
#define noteLeftFilterModeLightRing3 82
#define noteLeftFilterOnLightRing 80
#define noteRightFilterModeLightRing1 87
#define noteRightFilterModeLightRing2 86
#define noteRightFilterModeLightRing3 85 
#define noteRightFilterOnLightRing 81 

// Track Names & Numbers
#define MD_TRACK_1 0
#define MD_TRACK_2 1
#define MD_TRACK_3 2
#define MD_TRACK_4 3
#define MD_TRACK_5 4
#define MD_TRACK_6 5
#define MD_TRACK_7 6
#define MD_TRACK_8 7
#define MD_TRACK_9 8 
#define MD_TRACK_10 9
#define MD_TRACK_11 10
#define MD_TRACK_12 11
#define MD_TRACK_13 12 
#define MD_TRACK_14 13
#define MD_TRACK_15 14
#define MD_TRACK_16 15
#define MNM_TRACK_1 0
#define MNM_TRACK_2 1 
#define MNM_TRACK_3 2
#define MNM_TRACK_4 3 
#define MNM_TRACK_5 4
#define MNM_TRACK_6 5
#define OB_TRACK_1 6
#define TETRA_TRACK_1 7
#define TRACK_VOL 33
#define MIDI_TRACK_VOL 7
#define NOTE_REPEAT 0
#define TRANSPOSE 1
#define XONE4D_FADER_1 0
#define XONE4D_FADER_2 1
#define XONE4D_FADER_3 2
#define XONE4D_FADER_4 3
#define FADER_COUNT 4
#define MD_TRACK_COUNT 16
#define SYNTH_TRACK_COUNT 8

// MD Track Mute CC Numbers
#define MD_TRACK_MUTE_1 12
#define MD_TRACK_MUTE_2 13
#define MD_TRACK_MUTE_3 14
#define MD_TRACK_MUTE_4 15
#define MNM_TRACK_MUTE 3
