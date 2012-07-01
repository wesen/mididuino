class BeatRepeatPage2 : public EncoderPage {

  public: 
	EnumEncoder pitchEncoder;
        RangeEncoder volumeEncoder, decayEncoder, pitchDecayEncoder;
        BeatRepeatClass *beatRepeat;
   
        BeatRepeatPage2(BeatRepeatClass *_beatRepeat) :
        beatRepeat (_beatRepeat),
	pitchEncoder(BeatRepeatClass::pitchNames, countof(BeatRepeatClass::pitchNames), "PTC"),
	pitchDecayEncoder(100, 0, "PD%", 0), 
	volumeEncoder(127, 0, "VOL", 110), 
	decayEncoder(100, 0, "DC%", 0)
        {
	    encoders[0] = &pitchEncoder;
	    encoders[1] = &pitchDecayEncoder;
	    encoders[2] = &volumeEncoder;
	    encoders[3] = &decayEncoder;
	}

	virtual void loop() {
            if (pitchEncoder.hasChanged()) {
                beatRepeat->setPitch(pitchEncoder.getValue());
                beatRepeat->updatePitchVariables();                    
	    }
            if (pitchDecayEncoder.hasChanged()) {
                beatRepeat->setPitchDecay(pitchDecayEncoder.getValue());
                beatRepeat->updatePitchVariables();                    
	    }
            if (volumeEncoder.hasChanged()) {
                beatRepeat->setVolume(volumeEncoder.getValue());
                beatRepeat->updateVolumeVariables();                      
	    }
            if (decayEncoder.hasChanged()) {
                beatRepeat->setDecay(decayEncoder.getValue());
                beatRepeat->updateVolumeVariables();                      
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
            pitchEncoder.setValue(beatRepeat->pitch, true);
            pitchDecayEncoder.setValue(beatRepeat->pitchDecay, true);
            volumeEncoder.setValue(beatRepeat->volume, true);
            decayEncoder.setValue(beatRepeat->decay, true);                        
        }
    
	bool handleEvent(gui_event_t *event) {
		return false;
	}

};
