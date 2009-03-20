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
package com.ruinwesen.patchdownloader.tasks;

import java.io.File;
import java.io.IOException;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSUtils;
import name.cs.csutils.directoryreader.DirectoryReader;

import com.ruinwesen.midisend.MidiSendException;
import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.repository.StoredPatch;
import com.ruinwesen.patchdownloader.swing.SwingPatchdownloader;
import com.ruinwesen.patchdownloader.swing.dialogs.ProgressDialog;

/**
 * Sends a midifile to the device.
 */
public class SendMidifileTask {
    
    private static Log log = LogFactory.getLog(SendMidifileTask.class);

    private StoredPatch patch;
    private PatchMetadata metadata;
    private boolean startFlag = false; 
    
    public SendMidifileTask(StoredPatch patch, PatchMetadata metadata) {
        if (patch == null) {
            throw new IllegalArgumentException("patch==null");
        }
        if (metadata == null) {
            throw new IllegalArgumentException("metadata==null");
        }
        this.patch = patch;
        this.metadata = metadata;
    }
    
    public void start() {
        if (startFlag) {
            throw new IllegalStateException("start() was already invoked");
        }
        startFlag = true;
        new PatchSender().start();
    }

    private class PatchSender implements Runnable {

        private int state = 0;
        private ProgressDialog dialog = new ProgressDialog(
                ((SwingPatchdownloader)PatchDownloader.getSharedInstance())
                .getFrame(), "Sending...");

        public void start() {
            if (state == 0)
                SwingUtilities.invokeLater(this);            
        }
        
        private synchronized int getState() {
            return state;
        }
        
        private synchronized void incState() {
            state++;
        }
        
        private void init() {
            dialog.setProgressBarIndeterminate(true);
            new Thread(this).start();
            dialog.showProgressDialog();
        }
        
        private void send() {
            try {                
                __sendPatch();
                SwingUtilities.invokeLater(this);
            } catch (Exception ex) {
                if (log.isDebugEnabled()) {
                    log.debug("sendPatch() failed", ex);
                }
                dialog.closeProgressDialogAnimated();
                JOptionPane.showMessageDialog(null, 
                        "Could not send patch: "+ex.getMessage(),
                        "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
        
        private void shutdown() {
            dialog.setProgressBarIndeterminate(false);
            dialog.setProgressBarProgress(1f);
            dialog.closeProgressDialogAnimated();
        }
        
        @Override
        public void run() {
            if (getState()==0) {
                incState();
                init();
            } else if (getState() == 1) {
                incState();
                send();
            } else if (getState() == 2) {
                shutdown();
            }
        }
        
    }
    
    public void __sendPatch() throws IOException, MidiSendException {

        String midifilepath = metadata.getPath(PatchMetadata.PATH_MIDIFILE);
        if (midifilepath == null) {
            return;
        }

        DirectoryReader reader;
        File tmp = null;
        try {
            reader = patch.getDirectoryReader();
            if (reader == null) {
                throw new IOException("no reader available for patch: "+patch.getPath());
            }
            try {
                tmp = File.createTempFile("send", CSUtils.getLastPathComponent(midifilepath));
            } catch (IOException ex) {
                throw new IOException("could not create temporary file", ex);
            }
            reader.copyFile(midifilepath, tmp);
            reader.close();
            
            PatchDownloader.getSharedInstance().getMidiSend().send(tmp);
        } finally {
            if (tmp != null && !tmp.delete()) {
                if (log.isWarnEnabled()) {
                    log.warn("Could not delete temporary file: "+tmp.getAbsolutePath());
                }
            }
        }
    }
    
}
