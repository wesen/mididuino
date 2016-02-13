class BeatRepeatPage1 : public EncoderPage {

  public: 

	EnumEncoder intervalEncoder, offsetEncoder, gridEncoder, gateEncoder;
        BeatRepeatClass *beatRepeat;
   
        BeatRepeatPage1(BeatRepeatClass *_beatRepeat) :
        beatRepeat (_beatRepeat),
	intervalEncoder(BeatRepeatClass::intervalNames, countof(BeatRepeatClass::intervalNames), "INT"), 
	offsetEncoder(BeatRepeatClass::offsetNames, countof(BeatRepeatClass::offsetNames), "OFS"), 
	gateEncoder(BeatRepeatClass::gateNames, countof(BeatRepeatClass::gateNames), "GTE"), 
	gridEncoder(BeatRepeatClass::gridNames, countof(BeatRepeatClass::gridNames), "GRD")
        {
	    encoders[0] = &intervalEncoder;
	    encoders[1] = &offsetEncoder;
	    encoders[2] = &gateEncoder;
	    encoders[3] = &gridEncoder;
	}

	virtual void loop() {
	    if (intervalEncoder.hasChanged()) {
                beatRepeat->setInterval(intervalEncoder.getValue());
                beatRepeat->setInterval32(intervalEncoder.getValue());    
                beatRepeat->setTotalNumberOfRepeats();            
                beatRepeat->updatePitchVariables();                
                beatRepeat->updateVolumeVariables();                
	    }
            if (offsetEncoder.hasChanged()) {
                beatRepeat->setOffset32(offsetEncoder.getValue());     
                beatRepeat->setOffset(offsetEncoder.getValue());                  
	    }
            if (gridEncoder.hasChanged()) {
                beatRepeat->setOriginalGrid32(gridEncoder.getValue());   
                beatRepeat->setGrid32(gridEncoder.getValue());      
                beatRepeat->setGrid(gridEncoder.getValue());            
                beatRepeat->setTotalNumberOfRepeats();                
                beatRepeat->updatePitchVariables();                
                beatRepeat->updateVolumeVariables();                                
	    }
            if (gateEncoder.hasChanged()) {
                beatRepeat->setGate(gateEncoder.getValue());  
                beatRepeat->setGate32(gateEncoder.getValue());                
                beatRepeat->setTotalNumberOfRepeats();             
                beatRepeat->updatePitchVariables();                 
                beatRepeat->updateVolumeVariables();                                
	    }
	}

        void setup(){          
            updateEncoders();
//            beatRepeat->setInterval(intervalEncoder.getValue());
//            beatRepeat->setInterval32(intervalEncoder.getValue());            
//            beatRepeat->setOffset(offsetEncoder.getValue());  
//            beatRepeat->setOffset32(offsetEncoder.getValue());   
//            beatRepeat->setOriginalGrid32(gridEncoder.getValue());            
//            beatRepeat->setGrid(gridEncoder.getValue());    
//            beatRepeat->setGrid32(gridEncoder.getValue());    
//            beatRepeat->setGate(gateEncoder.getValue());                
//            beatRepeat->setGate32(gateEncoder.getValue());    
//            beatRepeat->setTotalNumberOfRepeats();            
//            beatRepeat->updatePitchVariables();                
//            beatRepeat->updateVolumeVariables();                            
        }
        
        void setActiveBeatRepeat(BeatRepeatClass *_beatRepeat){
            beatRepeat = _beatRepeat;
            updateEncoders();
        }
        
        void updateEncoders(){
            intervalEncoder.setValue(beatRepeat->interval, true);
            offsetEncoder.setValue(beatRepeat->offset, true);
            gridEncoder.setValue(beatRepeat->grid, true);
            gateEncoder.setValue(beatRepeat->gate, true);                        
        }        
    
	bool handleEvent(gui_event_t *event) {
		return false;
	}

};


