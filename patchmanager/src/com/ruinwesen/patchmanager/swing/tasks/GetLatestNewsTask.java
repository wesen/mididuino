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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.MalformedURLException;
import java.net.URL;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.Launcher;
import name.cs.csutils.concurrent.SimpleSwingWorker;

import com.ruinwesen.patchmanager.client.news.NewsIndex;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;

public class GetLatestNewsTask extends SimpleSwingWorker {

    private static Log log = LogFactory.getLog(GetLatestNewsTask.class);
    /**
     * 
     */
    private static final long serialVersionUID = -7956077785238787070L;

    private SwingPatchManager patchmanager;
    private int newMessageCount = 0;
    
    public GetLatestNewsTask(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
    }

    private synchronized void setNewMessageCount(int value) {
        this.newMessageCount = value;
    }
    
    private synchronized int getNewMessageCount() {
        return newMessageCount;
    }
    
    @Override
    protected void setup() {
        patchmanager.setStatus("Downloading news...");
    }

    @Override
    protected void process() {
        NewsIndex newsIndex = patchmanager.getNewsIndex();
        try {
            setNewMessageCount(newsIndex.updateNews(patchmanager.getPatchManagerClient()));
        } catch (Exception ex) {
            patchmanager.setStatus("Downloading news failed: "+ex.getMessage());
            setNewMessageCount(0);
        }
        
    }

    @Override
    protected void finish() {
        NewsIndex newsIndex = patchmanager.getNewsIndex();

        newsIndex.writeProperties(patchmanager.getAppProperties());
        
        File newsDir = new File(patchmanager.getApplicationUserdataDir(), "news");
        if (!newsDir.exists()) newsDir.mkdir();
        File newsFile = new File(newsDir, "news.html");
        
        boolean trouble = false;
        PrintStream out = null;
        try {
            out = new PrintStream(new BufferedOutputStream(new FileOutputStream(newsFile)));
            newsIndex.compileNews(out);
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("Writing news index failed.", ex);
            }
            patchmanager.setStatus("Could not create news html file:"+ex.getMessage());
            trouble = true;
        } finally {
            if (out != null) out.close();
            if (trouble) {
                newsFile.delete();
            }
        }

        if (!trouble) {
            patchmanager.setStatus(getNewMessageCount()+" new messages");
            try {
                URL url = newsFile.toURI().toURL();
                boolean ok = Launcher.getInstance().launchURL(url);
                if (log.isErrorEnabled() && !ok) {
                    log.error("could not launch url: "+url);
                }
            } catch (MalformedURLException ex) {
                if (log.isErrorEnabled()) {
                    log.error("could not launch uri: "+newsFile, ex);
                }
            }
        }
    }

    @Override
    protected void cleanup() {
    }

}
