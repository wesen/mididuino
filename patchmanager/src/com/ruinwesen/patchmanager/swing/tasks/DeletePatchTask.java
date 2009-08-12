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

import java.io.File;

import com.ruinwesen.patchmanager.client.index.IndexedPatch;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestDeletePatch;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

import name.cs.csutils.concurrent.SimpleSwingWorker;

public class DeletePatchTask extends SimpleSwingWorker {

    /**
     * 
     */
    private static final long serialVersionUID = 2532354735539555702L;
    private SwingPatchManager patchmanager;
    private IndexedPatch patch;
    private Auth auth;

    public DeletePatchTask(SwingPatchManager patchmanager, IndexedPatch patch) {
        this.patchmanager = patchmanager;
        this.patch = patch;
    }
    
    @Override
    protected void setup() {
        this.auth = patchmanager.getUserAuthentication(false);
        if (auth == null) {
            cancel();
        } else {
            patchmanager.setStatus("Deleting patch id:"+patch.getMetadata().getPatchId());
        }
    }    
    
    @Override
    protected void process() {

        String patchid = patch.getMetadata().getPatchId();
        if (patchid == null) {
            patchid = "";
        }
        
        Request request = new RequestDeletePatch(auth, patchid);
        Response response;
        try {
            response = patchmanager.getPatchManagerClient().execute(request);
            if (response.isOkStatus()) {
                patchmanager.setStatus("Patch deleted.");
                File file = patch.getLocalFile();
                File parent = file.getParentFile();
                if (parent != null && parent.equals(patchmanager.getIndex().getRepositoryDir())) {
                    file.delete();
                }
            } else {
                cancel();
                throw new Exception("delete patch failed: "+response.getMessage());
            }
        } catch (Exception ex) {
            cancel();
            setException(ex);
        }
    }
    
    @Override
    protected void finish() {
    }

    @Override
    protected void cleanup() {
        patchmanager.regenerateIndex();

        Throwable t = getException();
        if (t != null) {
            SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(), 
                   t.getMessage());
        }
    }
    
}
