class ConfigPage1 : public EncoderPage {

  public: 
	RangeEncoder lengthEncoder, offsetEncoder, intervalEncoder; 
        CCRandomizerClass *ccRandomizer;
   
        ConfigPage1(CCRandomizerClass *_ccRandomizer) :
        ccRandomizer (_ccRandomizer),
        lengthEncoder(64, 1, "LEN", 16),  
        offsetEncoder(16, 0, "OFS", 0),  
        intervalEncoder(16, 1, "INT", 1)
        {
	    encoders[0] = &lengthEncoder;
	    encoders[1] = &offsetEncoder;
	    encoders[2] = &intervalEncoder;
	}

	virtual void loop() {
            if (lengthEncoder.hasChanged()) {
                ccRandomizer->setLength(lengthEncoder.getValue());    
	    }
            if (offsetEncoder.hasChanged()) {
                ccRandomizer->setOffset(offsetEncoder.getValue());    
	    }
            if (intervalEncoder.hasChanged()) {
                ccRandomizer->setInterval(intervalEncoder.getValue());    
	    }
	}

        void setup(){         
            updateEncoders();
        }      
      
        void setActiveRandomizer(CCRandomizerClass *_ccRandomizer){
            ccRandomizer = _ccRandomizer;
            updateEncoders();
        }
        
        void updateEncoders(){
            lengthEncoder.setValue(ccRandomizer->length, true);
            offsetEncoder.setValue(ccRandomizer->offset, true);
            intervalEncoder.setValue(ccRandomizer->interval, true);                        
        }        
    
	bool handleEvent(gui_event_t *event) {
		return false;
	}

};
