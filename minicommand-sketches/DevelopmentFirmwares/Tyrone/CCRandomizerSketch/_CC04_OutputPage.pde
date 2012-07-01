//class OutputPage : public EncoderPage {
//
//  public: 
//        CCRandomizerClass *ccRandomizer0,  *ccRandomizer1,  *ccRandomizer2,  *ccRandomizer3;
//        
//        OutputPage(CCRandomizerClass *_ccRandomizer0, CCRandomizerClass *_ccRandomizer1, CCRandomizerClass *_ccRandomizer2, CCRandomizerClass *_ccRandomizer3) :
//        ccRandomizer0 (_ccRandomizer0),  
//        ccRandomizer1 (_ccRandomizer1),  
//        ccRandomizer2 (_ccRandomizer2),  
//        ccRandomizer3 (_ccRandomizer3)          
//        {
////	    encoders[0] = &ccMinEncoder;
////	    encoders[1] = &ccMaxEncoder;
//	    //encoders[2] = &offsetEncoder;
//	    //encoders[3] = &intervalEncoder;
//	}
//
//	virtual void loop() {
//	}
//
//        void setup(){         
//            encoders[0] = ccRandomizer0->getOutputEncoder();  
//            encoders[1] = ccRandomizer1->getOutputEncoder();  
//            encoders[2] = ccRandomizer2->getOutputEncoder();  
//            encoders[3] = ccRandomizer3->getOutputEncoder();              
//        }      
//              
//        void updateEncoders(){
//        }        
//    
//	bool handleEvent(gui_event_t *event) {
//		return false;
//	}
//
//};


class OutputPage : public EncoderPage {

  public: 
        CCRandomizerClass *ccRandomizer[4];
        
        OutputPage(CCRandomizerClass *_ccRandomizer0, CCRandomizerClass *_ccRandomizer1, CCRandomizerClass *_ccRandomizer2, CCRandomizerClass *_ccRandomizer3) 
        {
            ccRandomizer[0] = _ccRandomizer0;  
            ccRandomizer[1] = _ccRandomizer1;  
            ccRandomizer[2] = _ccRandomizer2;  
            ccRandomizer[3] = _ccRandomizer3;  
	}

	virtual void loop() {
	}

        void setup(){         
            for (uint8_t i=0; i<4; i++){
                encoders[i] = ccRandomizer[i]->getOutputEncoder();    
            }
        }      
              
        void updateEncoders(){
        }        
    
	bool handleEvent(gui_event_t *event) {
            return false;
	}

};
