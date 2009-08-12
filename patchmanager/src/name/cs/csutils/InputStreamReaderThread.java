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
package name.cs.csutils;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

public class InputStreamReaderThread implements Runnable {
    
    private static final int BUFFER_SIZE = 1024;

    private InputStream is;
    private StringBuffer sb = new StringBuffer();
    private boolean isRunning = false;
    private boolean isCompleted = false;
    private boolean isCanceled = false;
   // private Thread thread;

    private boolean onlyAvailable;
    
    public InputStreamReaderThread(InputStream is, boolean onlyAvailable) {
        this.is = is;
        this.onlyAvailable = onlyAvailable;
    }
    
    public void start() {
        Thread t = new Thread(this);
        t.setDaemon(true);
        t.start();
        synchronized (this) {
      //      this.thread = t;
        }
    }

    public synchronized boolean isRunning() {
        return isRunning;
    }

    public synchronized boolean isCompleted() {
        return isCompleted;
    }
    
    public synchronized void cancel() {
        isCanceled = true;
    }
    
    public void run() {
        synchronized (this) {
            isRunning = true;
            isCompleted = false;
            isCanceled = false;
        }
        
        boolean interrupted = false;
        try {
            byte[] bytes = new byte[BUFFER_SIZE];
            BufferedInputStream itmp = new BufferedInputStream(is, 1024);
            int count = -2;
            while (count != -1) {
                synchronized (this) {
                    if (isCanceled) {
                        break;
                    }
                }
                int available_without_blocking = itmp.available();
                if (available_without_blocking > 0) {
                    count = itmp.read(bytes, 0, 
                            Math.min(available_without_blocking, BUFFER_SIZE));
                    if (count > 0) {
                        sb.append(new String(bytes, 0, count));
                    }
                } else {
                    if (onlyAvailable) {
                        break;
                    }
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException ex) {
                        interrupted = true;
                        break;
                    }
                }
            }
        } catch (IOException ex) {
            // ignore
        } finally {
            try {
                is.close();
            } catch (IOException ex) {
                // no op
            }
            synchronized (this) {
                isRunning = false;
                isCompleted = true;
            }
            if (interrupted) {
                Thread.currentThread().interrupt();
            }
        }
    }
    
    public String getBuffer() {
        return sb.toString();
    }
    
}
