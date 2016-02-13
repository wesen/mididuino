class BeatRepeatSketch : public Sketch, public MDCallback, ClockCallback {

public:
    bool muted, activateBr1;      
    BeatRepeatClass beatRepeat1, beatRepeat2;
    BeatRepeatPage1 beatRepeatPage1;
    BeatRepeatPage2 beatRepeatPage2;
    BeatRepeatPage3 beatRepeatPage3;
    SliceConfigPage sliceConfigPage;
 
    BeatRepeatSketch() :
    beatRepeatPage1(&beatRepeat1),
    beatRepeatPage2(&beatRepeat1), 
    beatRepeatPage3(&beatRepeat1),
    sliceConfigPage(&beatRepeat1)
    {
     
    }
   
    void setupPages(){
       beatRepeat1.setup();
       beatRepeat1.setTargetTrack(12);
       beatRepeat1.setName("BEAT REPEAT 1:");
       beatRepeat2.setup();
       beatRepeat2.setTargetTrack(13);       
       beatRepeat2.setName("BEAT REPEAT 2:");
       beatRepeatPage1.setShortName("RPT");
       beatRepeatPage2.setShortName("DCY");    
       beatRepeatPage3.setShortName("TRK");     
       sliceConfigPage.setShortName("SLC");
       debugPage.setShortName("DBG");
    }
           
    // Switch Pages
    SwitchPage switchPage;
    void setupSwitchPage(){
       switchPage.initPages(&beatRepeatPage1, &beatRepeatPage2, &beatRepeatPage3, &sliceConfigPage);
       switchPage.parent = this;
    }
   
    void getName(char *n1, char *n2) {
        m_strncpy_p(n1, PSTR("BT  "), 5);
        m_strncpy_p(n2, PSTR("RPT "), 5);
      }   
   
    void setup() {
       muted = false;
       activateBr1 = true;
       setupPages();
       setupSwitchPage();
    }
    
    void toggleActiveBeatRepeat(){
        activateBr1 = !activateBr1;
        if (activateBr1){
            beatRepeatPage1.setActiveBeatRepeat(&beatRepeat1);
            beatRepeatPage2.setActiveBeatRepeat(&beatRepeat1);
            beatRepeatPage3.setActiveBeatRepeat(&beatRepeat1);      
            sliceConfigPage.setActiveBeatRepeat(&beatRepeat1); 
            beatRepeat1.setActive();
        } else {
            beatRepeatPage1.setActiveBeatRepeat(&beatRepeat2);
            beatRepeatPage2.setActiveBeatRepeat(&beatRepeat2);
            beatRepeatPage3.setActiveBeatRepeat(&beatRepeat2);     
            sliceConfigPage.setActiveBeatRepeat(&beatRepeat2);  
            beatRepeat2.setActive();          
        }
    }
   
    virtual void show() {
        if (currentPage() == &switchPage){
            popPage(&switchPage);
        }
        if (currentPage() == NULL){
            setPage(&sliceConfigPage);
            setPage(&beatRepeatPage3);                        
            setPage(&beatRepeatPage2);           
            setPage(&beatRepeatPage1);           
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
                GUI.flash_strings_fill("BEAT REPEAT:", "MUTED");
            } else {
                GUI.flash_strings_fill("BEAT REPEAT:", "UNMUTED");
            }
        }
      }
         
      virtual Page *getPage(uint8_t i) {
        if (i == 0) {
          return &beatRepeatPage1;
        } else {
          return NULL;
        }
      }
   
    bool handleEvent(gui_event_t *event) {  
      
       if (EVENT_PRESSED(event, Buttons.ENCODER4)) {
           GUI.pushPage(&debugPage);
       } else if (EVENT_RELEASED(event, Buttons.ENCODER4)) {  
          GUI.popPage(&debugPage);        
       }  
      
       // Top Left button
       else if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
           GUI.pushPage(&switchPage);
       } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {  
          GUI.popPage(&switchPage);        
       } else {
           // Top Right button
           if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
               beatRepeat1.toggleEnabled();
               return true;          
           // Bottom Right button         
           }else if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
               beatRepeat2.toggleEnabled();
               return true;          
           // Bottom Left button         
           } else if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
               // Toggle between BR1 & BR2    
               toggleActiveBeatRepeat();    
               return true;                   
           } else {
               return false;
           }                   
       }          
       return true;
    }   
    
};
