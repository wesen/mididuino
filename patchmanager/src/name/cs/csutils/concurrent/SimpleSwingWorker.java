/**
 * Copyright (c) 2009, Christian Schneider
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the names of the authors nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package name.cs.csutils.concurrent;

import java.awt.EventQueue;
import java.io.IOException;
import java.io.Serializable;
import java.lang.reflect.InvocationTargetException;

import javax.swing.SwingUtilities;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.Flag;

public class SimpleSwingWorker implements Runnable, Serializable {

    private static Log log = LogFactory.getLog(SimpleSwingWorker.class);

    private static final long serialVersionUID = 2907327668177136627L;
    
    // cache update event
    private transient EventQueueCall eqcUpdate;
    
    private Flag canceledFlag = new Flag.Atomic(false);
    
    private Throwable exception;
    
    public synchronized Throwable getException() {
        return exception;
    }
    
    public synchronized void setException(Throwable t) {
        this.exception = t;
    }
    
    public boolean isCanceled() {
        return canceledFlag.isset();
    }
    
    public void cancel() {
        canceledFlag.set();
    }
    
    public void run() {
        try {
            callSetup();
            if  (!isCanceled()) {
                process();
                if  (!isCanceled()) {
                    callFinish();
                }
            }
        } catch (Exception ex) {
            setException(ex);
            cancel();
        } finally {
            callCleanup();
        }
    }

    /**
     * Called on the event dispatch thread.
     */
    protected void setup() {
        
    }

    /**
     * Called on the event dispatch thread.
     */
    protected void finish() {
        
    }

    /**
     * Called in an arbitrary thread.
     */
    protected void process() throws Exception {
        
    }

    /**
     * Called on the event dispatch thread.
     */
    protected void cleanup() {
        
    }

    /**
     * Called on the event dispatch thread by process() .
     * Use this to provide visual feedback of the progress.
     */
    protected void update() {
        
    }

    protected void callSetup() {
        if (EventQueue.isDispatchThread()) {
            setup();
        } else {
            try {
                SwingUtilities.invokeAndWait(new EventQueueCall(this, EventQueueCall.SETUP));
            } catch (InterruptedException ex) {
                cancel();
                if (log.isErrorEnabled()) {
                    log.error(ex);
                }
            } catch (InvocationTargetException ex) {
                cancel();
                if (ex.getCause() instanceof RuntimeException) {
                    throw (RuntimeException) ex.getCause();
                } else {
                    if (log.isErrorEnabled()) {
                        log.error(ex);
                    }
                }
            }
        }
    }

    protected void callCleanup() {
        if (EventQueue.isDispatchThread()) {
            cleanup();
        } else {
            SwingUtilities.invokeLater(new EventQueueCall(this, EventQueueCall.CLEANUP));
        }
    }

    protected void callFinish() {
        if (EventQueue.isDispatchThread()) {
            finish();
        } else {
            SwingUtilities.invokeLater(new EventQueueCall(this, EventQueueCall.FINISH));
        }
    }

    protected void callUpdate() {
        if (EventQueue.isDispatchThread()) {
            update();
        } else {
            if (eqcUpdate== null) {
                eqcUpdate = new EventQueueCall(this, EventQueueCall.UPDATE);
            }
            SwingUtilities.invokeLater(eqcUpdate);
        }
    }
    
    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    
    private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException {
        in.defaultReadObject();
    }

    private static final class EventQueueCall implements Runnable {
        public static final int SETUP = 0;
        public static final int FINISH = 1;
        public static final int UPDATE = 2;
        public static final int CLEANUP = 3;
        private int type;
        private SimpleSwingWorker t;
        public EventQueueCall(SimpleSwingWorker t, int type) {
            this.t = t;
            this.type = type;
        }
        @Override
        public void run() {
            switch (type) {
            case SETUP: { t.setup(); break; }
            case FINISH: { t.finish(); break; }
            case UPDATE: { t.update(); break; }
            case CLEANUP: { t.cleanup(); break; }
            default: break;
            }
        }
    }
    
}
