class TransposeConfigPage : public EncoderPage {

  public: 
	RangeEncoder trackSelectEncoder, offsetEncoder;
        BoolEncoder trackTransposeEnabledEncoder;
        OctatrackTransposeClass *octatrackTranspose;
        uint8_t trackType, trackIndex;
        bool updateAllTracksMode;
   
        TransposeConfigPage
        (
            OctatrackTransposeClass *_octatrackTranspose, 
            uint8_t _trackType 
        ):
            octatrackTranspose(_octatrackTranspose),       
            trackType(_trackType), 
            trackSelectEncoder(8, 1, "TRK", 1), 
            trackTransposeEnabledEncoder("TRN", octatrackTranspose->transposeTrackEnabled[trackType][trackIndex]),            
            offsetEncoder(12, -12, "OFS", 0)  
        {
	    encoders[0] = &trackSelectEncoder;
	    encoders[1] = &trackTransposeEnabledEncoder;
	    encoders[2] = &offsetEncoder;
            trackType = _trackType;
//	    encoders[3] = &debugParam1;
	}

	virtual void loop() {
            if (trackSelectEncoder.hasChanged()) {
                trackIndex = (trackSelectEncoder.getValue() - 1);
                updateEncoders();
	    }
            if (trackTransposeEnabledEncoder.hasChanged()) {
                if(updateAllTracksMode){
                    // Update value for all tracks
                    for (int trackNumber=0; trackNumber<8; trackNumber++){
                        octatrackTranspose->setTransposeTrackEnabled(trackType, trackNumber, trackTransposeEnabledEncoder.getValue());     
                    } 
                } else {
                    // Just update the track selected by the Track Encoder
                    octatrackTranspose->setTransposeTrackEnabled(trackType, trackIndex, trackTransposeEnabledEncoder.getValue());    
                }
	    }
            if (offsetEncoder.hasChanged()) {
                if(updateAllTracksMode){
                    // Update value for all tracks
                    for (int trackNumber=0; trackNumber<8; trackNumber++){
                        octatrackTranspose->setOffset(trackType, trackNumber, offsetEncoder.getValue());    
                    } 
                } else {                
                    // Just update the track selected by the Track Encoder
                    octatrackTranspose->setOffset(trackType, trackIndex, offsetEncoder.getValue());    
                }
	    }
	}

        void setup(){         
            updateAllTracksMode = false;
            trackIndex = (trackSelectEncoder.getValue() - 1);       
            updateEncoders();                           
        }        
        
        void updateEncoders(){
            trackTransposeEnabledEncoder.setValue(octatrackTranspose->getTransposeTrackEnabled(trackType,trackIndex), true);   
            offsetEncoder.setValue(octatrackTranspose->getOffset(trackType,trackIndex), true);  
        }
        
        void setUpdateAllTracksMode(bool _value){
            updateAllTracksMode = _value;
            if(updateAllTracksMode){
                GUI.flash_strings_fill("UPDATE ALL TRKS:", "ON", 400);              
            } else {
                GUI.flash_strings_fill("UPDATE ALL TRKS:", "OFF", 400);                            
            }
        }
    
	bool handleEvent(gui_event_t *event) {
  
            if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
                setUpdateAllTracksMode(true);
                return true;                 
            } else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {  
                setUpdateAllTracksMode(false);
                return true; 
            } 
              
            return false;

	}

};


