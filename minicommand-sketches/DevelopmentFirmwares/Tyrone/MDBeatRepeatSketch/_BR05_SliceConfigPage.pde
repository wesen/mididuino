class SliceConfigPage : public EncoderPage {

  public: 
	EnumEncoder numberOfBeatsInSampleEncoder, lengthOfSliceEncoder, sliceDirectionEncoder, sliceOrderEncoder;
        BeatRepeatClass *beatRepeat;
   
        SliceConfigPage(BeatRepeatClass *_beatRepeat) :
        beatRepeat (_beatRepeat),
	numberOfBeatsInSampleEncoder(BeatRepeatClass::numberOfBeatsInSampleNames, countof(BeatRepeatClass::numberOfBeatsInSampleNames), "BTS") ,
        sliceOrderEncoder(BeatRepeatClass::sliceOrderNames, countof(BeatRepeatClass::sliceOrderNames), "ORD"),
        sliceDirectionEncoder(BeatRepeatClass::sliceDirectionNames, countof(BeatRepeatClass::sliceDirectionNames), "DIR"),
        lengthOfSliceEncoder(BeatRepeatClass::lengthOfSliceNames, countof(BeatRepeatClass::lengthOfSliceNames), "LEN")

        {
	    encoders[0] = &numberOfBeatsInSampleEncoder;
	    encoders[1] = &sliceOrderEncoder;
	    encoders[2] = &sliceDirectionEncoder;
	    encoders[3] = &lengthOfSliceEncoder;
	}

	virtual void loop() {
	    if (numberOfBeatsInSampleEncoder.hasChanged()) {
                beatRepeat->setNumberOfBeatsInSample(numberOfBeatsInSampleEncoder.getValue());            
	    }
	    if (sliceOrderEncoder.hasChanged()) {
                beatRepeat->setSliceOrder(sliceOrderEncoder.getValue());            
	    }
	    if (sliceDirectionEncoder.hasChanged()) {
                beatRepeat->setSliceDirection(sliceDirectionEncoder.getValue());            
	    }
	    if (lengthOfSliceEncoder.hasChanged()) {
                beatRepeat->setLengthOfSlice(lengthOfSliceEncoder.getValue());                                  
	    }
	}

        void setup(){          
        	updateEncoders();
            //numberOfBeatsInSampleEncoder.setValue(1, true);
            //sliceOrderEncoder.setValue(0, true);
            //sliceDirectionEncoder.setValue(0, true);            
            //lengthOfSliceEncoder.setValue(1, true);
            //beatRepeat->setNumberOfBeatsInSample(numberOfBeatsInSampleEncoder.getValue());            
            //beatRepeat->setSliceOrder(sliceOrderEncoder.getValue());            
            //beatRepeat->setSliceDirection(sliceDirectionEncoder.getValue());            
            //beatRepeat->setLengthOfSlice(lengthOfSliceEncoder.getValue());                        
        }
        
        void setActiveBeatRepeat(BeatRepeatClass *_beatRepeat){
            beatRepeat = _beatRepeat;
            updateEncoders();
        }
        
        void updateEncoders(){
            numberOfBeatsInSampleEncoder.setValue(beatRepeat->numberOfBeatsInSample, true);
            sliceOrderEncoder.setValue(beatRepeat->sliceOrder, true);
            sliceDirectionEncoder.setValue(beatRepeat->sliceDirection, true);   
            lengthOfSliceEncoder.setValue(beatRepeat->lengthOfSlice, true);
        }   

	bool handleEvent(gui_event_t *event) {			
		return false;
	}

};
