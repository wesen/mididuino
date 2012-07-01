class BeatRepeatPage3 : public EncoderPage {
 
  public:
        MDTrackFlashEncoder trackEncoder;        
        RangeEncoder chanceEncoder;
        EnumEncoder variationEncoder;
        BoolEncoder sliceEnabledEncoder;
        
        BeatRepeatClass *beatRepeat;
  
        BeatRepeatPage3(BeatRepeatClass *_beatRepeat) :
        beatRepeat (_beatRepeat),
        trackEncoder("TRK"),
        chanceEncoder(100, 0, " % ", 100), 
	variationEncoder(BeatRepeatClass::variationNames, countof(BeatRepeatClass::variationNames), "VAR"),
	    sliceEnabledEncoder("SLC", true)

        {
            encoders[0] = &trackEncoder;
            encoders[1] = &chanceEncoder;
            encoders[2] = &variationEncoder;        
            encoders[3] = &sliceEnabledEncoder;     
        }
 
        virtual void loop() {
            if (trackEncoder.hasChanged()) {
              
                // Restore old [source track] volume
                MD.setTrackParam(beatRepeat->sourceTrack, MODEL_VOL, MD.kit.params[beatRepeat->sourceTrack][MODEL_VOL]);
                
                // Set new [source track] and copy machine to [target track]
                beatRepeat->setSourceTrack(trackEncoder.getValue());
                beatRepeat->setVolume(MD.kit.params[beatRepeat->sourceTrack][MODEL_VOL]);
                MD.assignMachine(beatRepeat->targetTrack, MD.kit.models[beatRepeat->sourceTrack]);                
                beatRepeat->updatePitchVariables();           
                beatRepeat->updateVolumeVariables();                      
            }
	    if (chanceEncoder.hasChanged()) {
                beatRepeat->setChance(chanceEncoder.getValue());
	    }
            if (variationEncoder.hasChanged()) {
                beatRepeat->setVariation(variationEncoder.getValue());
	    }
	    
	    	if (sliceEnabledEncoder.hasChanged()) {
                beatRepeat->setSliceEnabled(sliceEnabledEncoder.getValue());
	    }
            
        }
 
        void setup(){                     
            updateEncoders();
        }
        
        void setActiveBeatRepeat(BeatRepeatClass *_beatRepeat){
            beatRepeat = _beatRepeat;
            updateEncoders();
        }

        void updateEncoders(){
            trackEncoder.setValue(beatRepeat->sourceTrack, true);
            chanceEncoder.setValue(beatRepeat->chance, true);
            variationEncoder.setValue(beatRepeat->variation, true);   
            sliceEnabledEncoder.setValue(beatRepeat->sliceEnabled, true);
        }                 
   
        bool handleEvent(gui_event_t *event) {
            if (EVENT_PRESSED(event, Buttons.ENCODER1)){
                MD.requestKit(MD.currentKit);
            }
            return false;
        }
 
};
