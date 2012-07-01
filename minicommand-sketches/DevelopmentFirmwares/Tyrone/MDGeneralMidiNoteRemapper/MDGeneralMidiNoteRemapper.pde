#define NUM_MAPPINGS 47
#define FIRST_NOTE 35
#define LAST_NOTE 81
#include <MidiClockPage.h>

uint8_t noteMappings[NUM_MAPPINGS][2] = {  
    {35,36},  //Bass Drum 2 -> MD Track 1 (LOW1)
    {36,36},  //Bass Drum 1 -> MD Track 1 (LOW1)
    {37,47},  //Side Stick/Rimshot -> MD Track 7 (MID3)
    {38,43},  //Snare Drum 1 -> MD Track 5 (MID1)
    {39,45},  //Hand Clap -> MD Track 6 (MID2)
    {40,43},  //Snare Drum 2 -> MD Track 5 (MID1)
    {41,38},  //Low Tom 2 -> MD Track 2 (LOW2)
    {42,50},  //Closed Hi-hat -> MD Track 9 (HIGH1)
    {43,38},  //Low Tom 1 -> MD Track 2 (LOW2)
    {44,50},  //Pedal Hi-hat -> MD Track 9 (HIGH1)
    {45,40},  //Mid Tom 2 -> MD Track 3 (LOW3)
    {46,52},  //Open Hi-hat -> MD Track 10 (HIGH2)
    {47,40},  //Mid Tom 1 -> MD Track 3 (LOW3)
    {48,41},  //High Tom 2 -> MD Track 4 (LOW4)
    {49,55},  //Crash Cymbal 1 -> MD Track 12 (HIGH4)
    {50,41},  //High Tom 1 -> MD Track 4 (LOW4)
    {51,53},  //Ride Cymbal 1 -> MD Track 11 (HIGH3)
    {52,53},  //Chinese Cymbal -> MD Track 11 (HIGH3)
    {53,53},  //Ride Bell -> MD Track 11 (HIGH3)
    {54,53},  //Tambourine -> MD Track 11 (HIGH3)
    {55,55},  //Splash Cymbal -> MD Track 12 (HIGH4)
    {56,47},  //Cowbell -> MD Track 7 (MID3)
    {57,55},  //Crash Cymbal 2 -> MD Track 12 (HIGH4)
    {58,45},  //Vibra Slap -> MD Track 6 (MID2)
    {59,53},  //Ride Cymbal 2 -> MD Track 11 (HIGH3)
    {60,48},  //High Bongo -> MD Track 8 (MID4)
    {61,47},  //Low Bongo -> MD Track 7 (MID3)
    {62,48},  //Mute High Conga -> MD Track 8 (MID4)
    {63,48},  //Open High Conga -> MD Track 8 (MID4)
    {64,45},  //Low Conga -> MD Track 6 (MID2)
    {65,40},  //High Timbale -> MD Track 3 (LOW3)
    {66,38},  //Low Timbale -> MD Track 2 (LOW2)
    {67,55},  //High Agogô -> MD Track 12 (HIGH4)
    {68,53},  //Low Agogô -> MD Track 11 (HIGH3)
    {69,53},  //Cabasa -> MD Track 11 (HIGH3)
    {70,53},  //Maracas -> MD Track 11 (HIGH3)
    {71,53},  //Short Whistle -> MD Track 11 (HIGH3)
    {72,55},  //Long Whistle -> MD Track 12 (HIGH4)
    {73,53},  //Short Güiro -> MD Track 11 (HIGH3)
    {74,55},  //Long Güiro -> MD Track 12 (HIGH4)
    {75,53},  //Claves -> MD Track 11 (HIGH3)
    {76,55},  //High Wood Block -> MD Track 12 (HIGH4)
    {77,53},  //Low Wood Block -> MD Track 11 (HIGH3)
    {78,53},  //Mute Cuíca -> MD Track 11 (HIGH3)
    {79,55},  //Open Cuíca -> MD Track 12 (HIGH4)
    {80,53},  //Mute Triangle -> MD Track 11 (HIGH3)
    {81,53}  //Open Triangle -> MD Track 11 (HIGH3)
};


  bool enabled;



  void onNoteOn(uint8_t *msg) {  
    if (!enabled){
      return;  
    }
    
    uint8_t newPitch, oldPitch;
    oldPitch = msg[1];
    
    if ((oldPitch >= FIRST_NOTE) && (oldPitch <= LAST_NOTE)){
       oldPitch -= FIRST_NOTE;
       newPitch = noteMappings[oldPitch][1];
    } else {
       newPitch = oldPitch;
    }
    
    GUI.setLine(GUI.LINE2);
    GUI.flash_printf_fill("NOTE %b -> %b", msg[1], newPitch);
    MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
  }
  
  void onNoteOff(uint8_t *msg) {
    if (!enabled){
      return;  
    }   
    
    uint8_t newPitch, oldPitch;
    oldPitch = msg[1];
    
    if ((oldPitch >= FIRST_NOTE) && (oldPitch <= LAST_NOTE)){
       oldPitch -= FIRST_NOTE;
       newPitch = noteMappings[oldPitch][1];
    } else {
       newPitch = oldPitch;
    }
    MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
  }
  
  void setup() {
    enabled = false;    
    GUI.setLine(GUI.LINE1);
    GUI.put_string_fill("GM->MD MAP:");
    toggleOnOff();
    initClockPage();
  }
  
  void toggleOnOff(){
     enabled = !enabled;
     GUI.setLine(GUI.LINE1);     
     if (enabled){   
       GUI.put_string_at(11, " ON ");
     } else {
       GUI.put_string_at(11, " OFF");
     }
  }
  
  bool handleEvent(gui_event_t *event) {  

        // Pressing button 2 (bottom left) toggles firmware on/off
        if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
            toggleOnOff();
            return true;
        }  
        return false;
  }   


  
