package com.ruinwesen.midisend;

public interface MidiSendProcess {

    Throwable getError();
    
    void cancel();
    
    boolean isTerminated();
    
    void waitFor(long timeout) throws InterruptedException;
    
}
