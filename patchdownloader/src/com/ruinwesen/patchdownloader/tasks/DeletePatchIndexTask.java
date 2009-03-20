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
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Collections;
import java.util.LinkedList;

import name.cs.csutils.FileFilterFactory;
import name.cs.csutils.UpdateTimer;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patchdownloader.indexer.Doc;
import com.ruinwesen.patchdownloader.indexer.IndexReader;
import com.ruinwesen.patchdownloader.swing.SwingPatchdownloader;
import com.ruinwesen.patchdownloader.swing.SearchController;
import com.ruinwesen.patchdownloader.swing.dialogs.ProgressDialog;

/**
 * Deletes the current patch index.
 */
public class DeletePatchIndexTask implements Runnable {

    private static Log log = LogFactory.getLog(DeletePatchIndexTask.class);

    private ProgressDialog dialog;
    
    public void start() {
        dialog = new ProgressDialog(SwingPatchdownloader.getSharedInstance().getFrame(),
                "Deleting index...");
        new Thread(this).start();
        dialog.showProgressDialog();
    }
    
    public void run() {
        try {
            deleteIndex();
        } finally {
            dialog.setProgressBarProgress(1f);
            dialog.closeProgressDialogAnimated();
        }
    }
    
    private void deleteIndex() {
        SwingPatchdownloader patchdownloader = SwingPatchdownloader.getSharedInstance();

        // clear all items so the list cell renderer does not try to 
        // load items we have deleted
        patchdownloader.getPatchListView().getListModel().setItems(
                Collections.<Doc>emptyList());
        
        IndexReader ir = patchdownloader.getPatchIndex();
        LinkedList<File> deleteList = new LinkedList<File>();
        deleteList.add(new File(ir.getIndexDir(), "key.index"));
        deleteList.add(new File(ir.getIndexDir(), "document.index"));
        if (patchdownloader.getRemoteRepositoryBackup().getBaseDir() != null) {
            deleteList.add(patchdownloader.getRemoteRepositoryBackup().getBaseDir());
        }
        
        Log log = DeletePatchIndexTask.log.isDebugEnabled() 
            ? DeletePatchIndexTask.log : null;
        
        UpdateTimer uiupdate = new UpdateTimer();
        
        int count = 0;
        int total = 1;
        while (!deleteList.isEmpty()) {
            File file = deleteList.removeFirst();
            if (!file.exists()) {
                continue;
            } else if (file.isFile()) {
                if (file.delete()) {
                    count ++;
                } else {
                    if (log != null) {
                        log.debug("Could not delete: "+file.getAbsolutePath());
                    }
                }
            } else if (file.isDirectory()) {
                File[] children = file.listFiles(
                        FileFilterFactory.and(FileFilterFactory.FilesOnly(),
                                FileFilterFactory.SuffixFilter(".rwp"))
                        );
                if (children != null) {
                    total += children.length;
                    dialog.setProgressBarProgress(count/(float)total);
                    for (File child: children) {
                        if (child.delete()) {
                            count ++;
                            if (uiupdate.needsUpdateAndReset()) {
                                dialog.setProgressBarProgress(count/(float)total);
                            }
                        } else {
                            if (log != null) {
                                log.debug("Could not delete: "+file.getAbsolutePath());
                            }
                        }
                    }
                }
            }
        }

        if (log != null) {
            log.debug(count+" files deleted");
        }
        try {
            ir.update();
        } catch (FileNotFoundException ex) {
            // no surprise: we deleted the files
        } catch (IOException ex) {
            if (log != null) {
                log.debug("could not update index", ex);
            }
        }
        
        SearchController searchController = patchdownloader.getSearchController();
        searchController.indexChanged();
        searchController.update();
    }

}
