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


import java.awt.Component;
import javax.swing.JOptionPane;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSProperties;
import name.cs.csutils.CSUtils;
import name.cs.csutils.concurrent.SimpleSwingWorker;

import com.ruinwesen.patchmanager.client.protocol.RequestGetClientInfo;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

public class CheckForUpdatesTask extends SimpleSwingWorker {

    /**
     * 
     */
    private static final long serialVersionUID = -124826352064173417L;

    private Log log = LogFactory.getLog(CheckForUpdatesTask.class);
    
    public static final String KEY_DAYS_BETWEEN_CHECK 
        = CheckForUpdatesTask.class.getName()+".days-between-check";
    public static final String KEY_LAST_CHECK 
    = CheckForUpdatesTask.class.getName()+".last-check";
    public static final String KEY_LAST_CHECK_CLIENT_VERSION
        = CheckForUpdatesTask.class.getName()+".last-check.client-version";
    public static final String KEY_USER_NOTIFIED_ABOUT_NEW_VERSION
        = CheckForUpdatesTask.class.getName()+".user-notified-about-new-version";
    
    public static final int DEFAULT_DAYS_BETWEEN_CHECK = 7;
    
    private boolean quietMode = false;
    private int daysBetweenCheck = DEFAULT_DAYS_BETWEEN_CHECK;
    private Component parentComponent;

    private SwingPatchManager patchmanager;
    private String latestVersionResponse = null;
    
    public CheckForUpdatesTask(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
        daysBetweenCheck = Math.max(
                patchmanager
                .getAppProperties()
                .getIntegerProperty(KEY_DAYS_BETWEEN_CHECK, DEFAULT_DAYS_BETWEEN_CHECK), 1);
    }
    
    public void setQuietModeEnabled(boolean value) {
        this.quietMode = value;
    }
    
    public void setDaysBetweenCheck(int value) {
        this.daysBetweenCheck = value;
    }
    
    public boolean isQuietModeEnabled() {
        return quietMode;
    }
    
    public int getDaysBetweenCheck() {
        return daysBetweenCheck;
    }

    @Override
    protected void process() {
        String latestVersion = null;
        try {
            Response response = patchmanager.getPatchManagerClient().execute(new RequestGetClientInfo());
            if (response.isOkStatus()) {
                latestVersion = response.getProperty(Response.KEY_CLIENT_VERSION);
                if (log.isDebugEnabled()) {
                    log.debug("request new client version ok:"+latestVersion);
                }
            }
        } catch (Exception ex) {
            if (log.isDebugEnabled()) {
                log.debug("request new client version failed", ex);
            }
        }

        synchronized (this) {
            latestVersionResponse = latestVersion;
        }
    }
    
    @Override
    protected synchronized void finish() {
        if (latestVersionResponse == null) {
            if (!quietMode) {
                SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(),  
                        "Checking for updates failed.");
            }
            return;
        } else {
            CSProperties properties = patchmanager.getAppProperties();
            // remember when we checked the last time
            properties.setDateProperty(KEY_LAST_CHECK, CSUtils.now());
            // remember the results of the last check
            properties.put(KEY_LAST_CHECK_CLIENT_VERSION, latestVersionResponse);
        }   

        // are updates available ?
        final boolean isUpdateAvailable; 
            isUpdateAvailable = 
                latestVersionResponse != null && (!latestVersionResponse.equals(patchmanager.getVersion()));

        if (isUpdateAvailable) {            
            JOptionPane.showMessageDialog(parentComponent, "New version of Patchmanager is available.", "Check Update", JOptionPane.INFORMATION_MESSAGE);
        } else if (!quietMode) {
            JOptionPane.showMessageDialog(parentComponent, "No updates available", "Check Update", JOptionPane.INFORMATION_MESSAGE);
        }
    }
    
}
