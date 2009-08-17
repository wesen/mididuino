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
package com.ruinwesen.patchmanager.swing.tasks;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.concurrent.SimpleSwingWorker;

import com.ruinwesen.patchmanager.client.repository.PatchDownloadCallback;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

public class SynchronizeRepositoryTask extends SimpleSwingWorker implements PatchDownloadCallback {

    /**
     * 
     */
    private static final long serialVersionUID = 6513521337985896762L;
    private SwingPatchManager patchmanager;
    private boolean trouble = false;
    private static Log log = LogFactory.getLog(SynchronizeRepositoryTask.class);
    
    public SynchronizeRepositoryTask(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
    }
    
    @Override
    protected void setup() {
        patchmanager.setStatus("Checking for new patches...");
    }
    
    @Override
    protected void process() {
        try {    
            if (log.isDebugEnabled()) {
                log.debug("Sending synchronize request");
            }
            
            patchmanager.getPatchManager().syncRepository(
            		SwingPatchManager.adminMode ?
            				patchmanager.getUserAuthentication(false) : null,
            				this);
            patchmanager.indexUpdated();
        } catch (Exception ex) {
            if (log.isErrorEnabled()) {
            log.error("error while synchronizing", ex);
            }
            
            synchronized (this) {
                this.trouble = true;
            }
        }
    }

    @Override
    protected synchronized void finish() {
        if (trouble) {
            patchmanager.setStatus(null);
            SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(),
                    "Synchronisation failed. Please try again later.");
        } else {
            patchmanager.setStatus("Checking for new patches done.");
        }
    }

    @Override
    public void patchDownloaded(String message) {
        patchmanager.setStatus(message);
    }
    
}
