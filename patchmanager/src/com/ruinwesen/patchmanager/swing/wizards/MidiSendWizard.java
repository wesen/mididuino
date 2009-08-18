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
package com.ruinwesen.patchmanager.swing.wizards;

import java.io.File;

import javax.swing.JFrame;

import name.cs.csutils.CSProperties;
import name.cs.csutils.concurrent.SimpleSwingWorker;

import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendProcess;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;
import com.ruinwesen.patchmanager.swing.form.FormContainer;
import com.ruinwesen.patchmanager.swing.form.Wizard;
import com.ruinwesen.patchmanager.swing.forms2.MidiSendForm;

public class MidiSendWizard extends Wizard {

    public static final String KEY_INPUT_DEVICE = 
        MidiSendWizard.class.getName()+"input-device";
    public static final String KEY_OUTPUT_DEVICE = 
        MidiSendWizard.class.getName()+"output-device";
    
    private MidiSendForm midisendForm;
    private SendThread sendThread;
    private CSProperties properties = null;
    
    public MidiSendWizard() {
        super();
        midisendForm = new MidiSendForm();
        FormContainer ms = new FormContainer(midisendForm);
        ms.setValidationHintsEnabled(false);
        add(ms);
        acFinish.useName("Send");
        setResizable(false);
        setTitle("midi-send");
        setConfirmOnCancelEnabled(false);
        setNextCapable(false);
        setFinishCapable(true);
    }

    public void setProperties(CSProperties properties) {
        this.properties = properties;
        if (properties != null) {
            String in = properties.getProperty(KEY_INPUT_DEVICE);
            String out = properties.getProperty(KEY_OUTPUT_DEVICE);
            if (in != null) {
                midisendForm.selectInputDeviceByName(in);
            }
            if (out != null) {
                midisendForm.selectOutputDeviceByName(out);
            }
        }
    }
    
    
    public void setSourceFileSelectable(boolean value) {
        midisendForm.setSourceFileSelectable(value);
    }

    public void setSourceFile(File file) {
        midisendForm.setSourceFile(file);
    }

    public static void main(String[] args) {
        Wizard w = new MidiSendWizard();
        w.showDialog((JFrame)null);
    }

    @Override
    protected void handle_finish() {
        midisendForm.setInputControlsEnabled(false);
        doSend();
        acFinish.setEnabled(false);
       // super.handle_finish();
    }
    
    @Override
    protected void handle_cancel() {
        if (sendThread != null) {
            sendThread.cancel();
            acCancel.setEnabled(false);
            return;
        } else {
            super.handle_cancel();
        }
    }
    
    private void doSend()  {
        setHint("Sending ...");
        MidiSend midisend = midisendForm.getMidiSend();

        MidiDevice input = midisendForm.getInputDevice();
        MidiDevice output = midisendForm.getOutputDevice();
        
        if (properties != null) {
            properties.put(KEY_INPUT_DEVICE, input.getName());
            properties.put(KEY_OUTPUT_DEVICE, output.getName());
        }
        
        midisend.setInputDevice(input);
        midisend.setOutputDevice(output);
        
        MidiSendProcess proc;
        try {
            // validate hex file + unpack hex file from patch
            byte[] hexFileData = midisendForm.getHexFileBytes();
            proc = midisend.send(hexFileData);
        } catch (Exception ex) {
            SwingPatchManagerUtils.showErrorDialog(getContainer(), 
                    "Error: "+ex
            );
            return;
        }
        
        this.sendThread = new SendThread(proc);
        new Thread(sendThread).start();
    }
    
    private class SendThread extends SimpleSwingWorker {
        /**
         * 
         */
        private static final long serialVersionUID = -5313078745023679652L;
        private MidiSendProcess proc;
        public SendThread(MidiSendProcess proc) {
            this.proc = proc;
        }

        @Override
        protected void setup() {
            midisendForm.getProgressBar().setValue(
                    midisendForm.getProgressBar().getMinimum());
            midisendForm.getProgressBar().setIndeterminate(true);
        }
        
        @Override
        public void cancel() {
            super.cancel();
            proc.cancel();
        }
        
        @Override
        protected void process() throws Exception {
            boolean interrupted = false;
            try {
                while (!proc.isTerminated()) {
                    try {
                        proc.waitFor(1000);
                    } catch (InterruptedException ex) {
                        interrupted = true;
                    }
                }
            
                Throwable t = proc.getError();
                if (t != null) {
                    if (t instanceof Exception) {
                        throw (Exception) t;
                    } else if (t instanceof RuntimeException) {
                        throw (RuntimeException) t;
                    } else {
                        throw new Exception(t);
                    }
                }
            } finally {
                if (interrupted) {
                    Thread.currentThread().interrupt();
                }
            }
        }
        
        @Override
        protected void finish() {
            midisendForm.getProgressBar().setValue(
                    midisendForm.getProgressBar().getMaximum());
        }

        @Override
        protected void cleanup() {
            acCancel.setEnabled(true);
            midisendForm.getProgressBar().setIndeterminate(false);
            midisendForm.setInputControlsEnabled(true);
            sendThread = null;
            acFinish.setEnabled(true);
            if (getException() != null) {
                setHint("Error:"+getException().getMessage());
            }
        }
        
    }

}
