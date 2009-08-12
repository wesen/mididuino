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
package com.ruinwesen.patchmanager.swing.forms;

import java.awt.EventQueue;

import javax.swing.JProgressBar;
import javax.swing.SwingUtilities;

public class ProgressInfo {

    private int minimum = 0;
    private int maximum = 100;
    private int value = 0;
    private boolean indeterminate = false;
    private JProgressBar progressbar;
    
    public ProgressInfo() {
        this(0,10);
        progressbar.setIndeterminate(true);
    }

    public ProgressInfo(int minimum, int maximum) {
        this(minimum, maximum, null);
    }

    public ProgressInfo(int minimum, int maximum, JProgressBar bar) {
        this.minimum = minimum;
        this.maximum = maximum;
        setProgressBar(bar);
    }

    public synchronized int getMinimum() {
        return minimum;
    }
    
    public synchronized void setMinimum(int value) {
        if (this.minimum != value) {
            this.minimum = value;
            updateProgressbar();
        }
    }

    public synchronized int getMaximum() {
        return maximum;
    }
    
    public synchronized void setMaximum(int value) {
        if (this.maximum != value) {
            this.maximum = value;
            updateProgressbar();
        }
    }

    public synchronized int getValue() {
        return value;
    }
    
    public void setValue(int value) {
        if (this.value != value) {
            this.value = value;
            updateProgressbarValue();
        }
    }

    public synchronized boolean isIndeterminate() {
        return indeterminate;
    }
    
    public void setIndeterminate(boolean value) {
        if (this.indeterminate != value) {
            this.indeterminate = value;
            updateProgressbar();
        }
    }
    
    public synchronized void setProgressBar(JProgressBar bar) {
        this.progressbar = bar;
        if (bar != null)
            updateProgressbar();
    }
    
    public synchronized JProgressBar getProgressBar() {
        return progressbar;
    }

    private synchronized void updateProgressbar() {
        JProgressBar bar = progressbar;
        if (bar == null) {
            return ;
        }
        if (EventQueue.isDispatchThread()) {
            bar.setIndeterminate(indeterminate);
            bar.setMinimum(minimum);
            bar.setMaximum(maximum);
            bar.setValue(value);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    updateProgressbar();
                }
            });
        }
    }

    private synchronized void updateProgressbarValue() {
        JProgressBar bar = progressbar;
        if (bar == null) {
            return ;
        }
        if (EventQueue.isDispatchThread()) {
            bar.setValue(value);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    updateProgressbarValue();
                }
            });
        }
    }
    
}
