#include <WProgram.h>
#include <CCHandler.h>

class CCRandomizerSketch : public Sketch{  

    public:
    bool muted;  
    CCRandomizerClass ccRandomizer[4];
    ConfigPage1 configPage1;
    ConfigPage2 configPage2;
    OutputPage outputPage;
    SwitchPage switchPage;
    
    CCRandomizerSketch() : 
    configPage1(&ccRandomizer[0]),
    configPage2(&ccRandomizer[0]),
    outputPage(&ccRandomizer[0], &ccRandomizer[1], &ccRandomizer[2], &ccRandomizer[3])
    {
    }  
       
    void setupSwitchPage(){
       switchPage.initPages(&configPage1, &configPage2, NULL, &outputPage);
       switchPage.parent = this;
    }
      
    void setupPages(){
        configPage1.setup();
        configPage1.setShortName("LEN");
        configPage2.setup();
        configPage2.setShortName("TRK");
        outputPage.setup();
        outputPage.setShortName("OUT");
    }
    
    void setupRandomizers(){
        ccRandomizer[0].setName("CC RANDOMIZER 1:");         
        ccRandomizer[1].setName("CC RANDOMIZER 2:");         
        ccRandomizer[2].setName("CC RANDOMIZER 3:");         
        ccRandomizer[3].setName("CC RANDOMIZER 4:");    
        ccRandomizer[0].setShortName(" 1 ");         
        ccRandomizer[1].setShortName(" 2 ");         
        ccRandomizer[2].setShortName(" 3 ");         
        ccRandomizer[3].setShortName(" 4 ");            
        for (int i = 0; i < 4; i++) {
            ccRandomizer[i].outputEncoder.setName("   ");
            ccRandomizer[i].outputEncoder.name[1] = '1' + i;           
            ccRandomizer[i].setup();
        }
    }
       
    void getName(char *n1, char *n2) {
        m_strncpy_p(n1, PSTR("CC  "), 5);
        m_strncpy_p(n2, PSTR("RND "), 5);
      }   
     
    void setup() {
       muted = false;
       setupPages();
       setupSwitchPage();
       setupRandomizers();
    }
        
    virtual void show() {
        if (currentPage() == &switchPage){
            popPage(&switchPage);
        }
        if (currentPage() == NULL){
            setPage(&configPage1);
        }
    }   
    
    virtual void hide() {
        if (currentPage() == &switchPage){
            popPage(&switchPage);
        }
    }    
    
    virtual void mute(bool pressed) {
      if (pressed) {
          muted = !muted;
          if (muted) {
              GUI.flash_strings_fill("CC RANDOMIZER:", "MUTED");
          } else {
              GUI.flash_strings_fill("CC RANDOMIZER:", "UNMUTED");
          }
      }
    }  
  
    virtual Page *getPage(uint8_t i) {
      if (i == 0) {
        return &configPage1;
      } 
      if (i == 1) {
        return &configPage2;
      } else {
        return NULL;
      }
    }  
    
//    void togglePageDisplay(){
//        if (currentPage() == &djEqPage){
//            GUI.setPage(&filterPage);
//        } else {
//            GUI.setPage(&djEqPage);
//        }
//    }
       
    bool handleEvent(gui_event_t *event) {       
          
        // Top Left button
        if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
            GUI.pushPage(&switchPage);
        } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {  
            GUI.popPage(&switchPage);        
        }           

        // Bottom Left button
        if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
            displayRandomizerSelectText("SELECT RANDOMIZR");
        } else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {  
            GUI.currentPage()->redisplayPage();    
            return true; 
        }       

        // Bottom Right button
        if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
            displayRandomizerSelectText("RANDOMIZE VALUES");
        } else if (EVENT_RELEASED(event, Buttons.BUTTON3)) {  
            GUI.currentPage()->redisplayPage();     
            return true;
        }

        // Bottom Right button
        if (BUTTON_DOWN(Buttons.BUTTON3)) {
            
            // Click Encoder 1 - 4
            for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
                if (EVENT_PRESSED(event, i)) {
                    ccRandomizer[i].randomizeValues();
                    return true;
                }
            }          
        }       

        // Bottom Left button
        if (BUTTON_DOWN(Buttons.BUTTON2)) {
                
            // Click Encoder 1 - 4
            for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
                if (EVENT_PRESSED(event, i)) {
                    configPage1.setActiveRandomizer(&ccRandomizer[i]);
                    configPage2.setActiveRandomizer(&ccRandomizer[i]);                    
                    ccRandomizer[i].setActive();
                    return true;                    
                }
            }          
        } else {

            // Click Encoder 1 - 4
            for (uint8_t i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
                if (EVENT_PRESSED(event, i)) {
                    ccRandomizer[i].toggleEnabled();       
                    return true;                    
//                    GUI.setLine(GUI.LINE1);                    
//                    if (ccRandomizer[i].enabled){
//                        GUI.put_string_at((4 * (i+1))-1, "*");
//                    } else {
//                        GUI.put_string_at((4 * (i+1))-1, " ");          
//                    }                               
                }
            }
        }
        
        return false;
    }   
    
    void displayRandomizerSelectText(const char *_line1Text){
        GUI.setLine(GUI.LINE1);
        GUI.put_string_at_fill(0, _line1Text);
        GUI.setLine(GUI.LINE2);
        for (int i = 0; i < 4; i++) {
            GUI.put_string_fill(i, ccRandomizer[i].shortName);
        }
    }

};
    
