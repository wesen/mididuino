#include <Platform.h>
#include <CCHandler.h>

class OctatrackLiveSketch : public Sketch{  

    public:
    bool muted, groupAPage1, groupBPage1;
    DjEqPage djEqPage;
    DelayControlPage delayControlPage;
    RecorderPage recorderPage;
    
    OctatrackLiveSketch() : djEqPage(){
    }  
       
//    void setupSwitchPage(){
//       switchPage.initPages(&djEqPage, &filterPage, &recorderPage, NULL);
//       switchPage.parent = this;
//    }
      
    void setupPages(){
        djEqPage.setup();
        djEqPage.setShortName("EQ");
        delayControlPage.setup();
        delayControlPage.setShortName("DLY");
        recorderPage.setup();
        recorderPage.setShortName("REC");        
    }
       
    void getName(char *n1, char *n2) {
        m_strncpy_p(n1, PSTR("OT  "), 5);
        m_strncpy_p(n2, PSTR("LIV "), 5);
      }   
     
    void setup() {
       muted = false;
       ccHandler.setup();
       setupPages();
//       setupSwitchPage();
       groupAPage1 = true;
       groupBPage1 = true;
       ccHandler.addEncoder(&djEqPage.mdEqLowEncoder);
       ccHandler.addEncoder(&djEqPage.mdEqHighEncoder);
       ccHandler.addEncoder(&djEqPage.mnmEqLowEncoder);        
       ccHandler.addEncoder(&djEqPage.mnmEqHighEncoder);   
//       ccHandler.addEncoder(&filterPage.mdFilterBaseEncoder);
//       ccHandler.addEncoder(&filterPage.mdFilterWidthEncoder);
//       ccHandler.addEncoder(&filterPage.mnmFilterBaseEncoder);        
//       ccHandler.addEncoder(&filterPage.mnmFilterWidthEncoder);         
       ccHandler.addEncoder(&recorderPage.recEqLowEncoder);
       ccHandler.addEncoder(&recorderPage.recEqHighEncoder);
       ccHandler.addEncoder(&recorderPage.recFilterBaseEncoder);
       ccHandler.addEncoder(&recorderPage.recFilterWidthEncoder);       
    }
        
    virtual void show() {
//        if (currentPage() == &switchPage){
//            popPage(&switchPage);
//        }
        if (currentPage() == NULL){
            setPage(&djEqPage);
        }
    }   
    
    virtual void hide() {
//        if (currentPage() == &switchPage){
//            popPage(&switchPage);
//        }
    }    
    
    virtual void mute(bool pressed) {
      if (pressed) {
          muted = !muted;
          if (muted) {
              GUI.flash_strings_fill("OCTATRACK LIVE:", "MUTED");
          } else {
              GUI.flash_strings_fill("OCTATRACK LIVE:", "UNMUTED");
          }
      }
    }  
  
    virtual Page *getPage(uint8_t i) {
      if (i == 0) {
        return &djEqPage;
      } 
      if (i == 1) {
        return &delayControlPage;
      }
      if (i == 2) {
        return &recorderPage;
      } else {
        return NULL;
      }
    }  
    

    
    void displayPageGroupA(){
        if (currentPage() == &djEqPage || currentPage() == &delayControlPage){
            groupAPage1 = !groupAPage1;
        }
        if (groupAPage1){
            GUI.setPage(&djEqPage);
        } else {
            GUI.setPage(&delayControlPage);
        }
    }
    
    void displayPageGroupB(){
        
        // Currently there is only one page for "Group B" - recorderPage
//        if (currentPage() == &recorderPage || currentPage() == &recorderPage){
//            groupBPage1 = !groupBPage1;
//        }
//        if (groupBPage1){
//            GUI.setPage(&recorderPage);
//        } else {
//            GUI.setPage(&recorderPage);
//        }
        GUI.setPage(&recorderPage);        
    }    
       
    bool handleEvent(gui_event_t *event) {       
      
           
           // Top Left button
           if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
               displayPageGroupA();
               return true;
           }
           
           // Top Right button
           if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
               displayPageGroupB();
               return true;
           }           
           
           // Bottom Right button
           if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
                recorderPage.startSupatrigga();
           } else if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
                recorderPage.stopSupatrigga();
           }         
       return false;
    }   
    
    

};
    
