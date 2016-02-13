class ConfigPage2 : public EncoderPage {

  public: 
	RangeEncoder trackEncoder, ccMinEncoder, ccMaxEncoder; 
        EnumEncoder parameterEncoder;
        CCRandomizerClass *ccRandomizer;
   
        ConfigPage2(CCRandomizerClass *_ccRandomizer) :
        ccRandomizer (_ccRandomizer),  
        trackEncoder(8, 1, "TRK", 1), 
        parameterEncoder(ccRandomizer->parameterShortNames, 30, "PAR", 0),         
        ccMinEncoder(127, 0, "MIN", 0),  
        ccMaxEncoder(127, 0, "MAX", 127)
        {
	    encoders[0] = &trackEncoder;
	    encoders[1] = &parameterEncoder;
	    encoders[2] = &ccMinEncoder;
	    encoders[3] = &ccMaxEncoder;
	}

	virtual void loop() {
            if (trackEncoder.hasChanged()) {
                ccRandomizer->setTrack(trackEncoder.getValue());         
                ccRandomizer->updateOutputEncoder();              
	    }
            if (parameterEncoder.hasChanged()) {
                ccRandomizer->setParameter(parameterEncoder.getValue());   
                ccRandomizer->setParameterCC(parameterEncoder.getValue());    
                ccRandomizer->updateOutputEncoder();           
                GUI.setLine(GUI.LINE2);      
                GUI.flash_string_fill(ccRandomizer->parameterNames[parameterEncoder.getValue()]);                
	    }
            if (ccMinEncoder.hasChanged()) {
                ccRandomizer->setCCMin(ccMinEncoder.getValue());                       
	    }
            if (ccMaxEncoder.hasChanged()) {
                ccRandomizer->setCCMax(ccMaxEncoder.getValue());    
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
            trackEncoder.setValue(ccRandomizer->track, true);          
            parameterEncoder.setValue(ccRandomizer->parameter, true);
            ccMinEncoder.setValue(ccRandomizer->ccMin, true);
            ccMaxEncoder.setValue(ccRandomizer->ccMax, true);                       
        }        
    
	bool handleEvent(gui_event_t *event) {
		return false;
	}

};
