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

/**
 * A thread save class to measure a specific amount of time.
 * This class is intented to be used to (repeatedly) update the user interface
 * after a specific amount of time, usually when a background {@link Thread} is
 * operating and the UI should indicate the progress of the thread. 
 * 
 * <strong>Example:</strong>
 * 
 * <pre><code>Runnable myRunnable = new Runnable() {
 *  public void run() {
 *     UpdateTimer uiupdate = new UpdateTimer();
 *     while (true) {
 *          ... do someting ...
 *          
 *          // test if the time-interval is completed
 *          // if true, the timer is reset automatically to measure
 *          // the next time interval 
 *          if (uiupdate.needsUpdateAndReset()) {
 *             updateUI(); // update the UI
 *          }
 *     }
 *  }
 *  }
 * 
 * </code></pre>
 * @author Christian Schneider 
 */
public final class UpdateTimer {

    /** the default interval length in milliseconds */
    public static final long DEFAULT_INTERVAL = 250;
    
    /** the interval length */
    private long intervalLength;
    
    /** the time when the next update should be performed */
    private long nextupdate = 0;

    /** 
     * Create a new timer with {@link #DEFAULT_INTERVAL} as interval.
     */
    public UpdateTimer() {
        this(DEFAULT_INTERVAL);
    }
    
    /**
     * Create a timer with the specified interval length. 
     * 
     * @param intervalLength the length of the measured time-interval in milliseconds
     * @throws IllegalArgumentException if the specified interval length is &lt;= 0
     */
    public UpdateTimer(long intervalLength) {
        if (intervalLength <= 0) {
            throw new IllegalArgumentException("intervalLength <= 0");
        }
        this.intervalLength = intervalLength;
    }
 
    /**
     * Returns true if the current interval is complete. The interval is complete
     * if <code>interval length</code>-milliseconds since the last call to
     * {@link #reset()} or {@link #needsUpdateAndReset()} have passed or
     * if this is a new instance of the class. If the result is true
     * then the timer is reset and begins measuring the next interval.
     * @return true if the current interval is complete
     */
    public synchronized boolean needsUpdateAndReset() {
        long time = System.currentTimeMillis();
        if (time>=nextupdate) {
            nextupdate = time+intervalLength;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns true if the current interval is complete. The interval is complete
     * if <code>interval length</code>-milliseconds since the last call to
     * {@link #reset()} or {@link #needsUpdateAndReset()} have passed or
     * if this is a new instance of the class. 
     * @return true if the current interval is complete
     */
    public synchronized boolean needsUpdate() {
        long time = System.currentTimeMillis();
        return (time>=nextupdate);
    }

    /**
     * Resets this timer to measure a new interval.
     */
    public synchronized void reset() {
        long time = System.currentTimeMillis();
        nextupdate = time+intervalLength;
    }
    
}
