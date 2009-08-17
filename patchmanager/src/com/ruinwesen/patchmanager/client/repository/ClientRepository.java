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
package com.ruinwesen.patchmanager.client.repository;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import name.cs.csutils.CSProperties;
import name.cs.csutils.CSUtils;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;

import com.ruinwesen.patchmanager.client.index.PatchIndex;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.DefaultPatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.PatchSource;
import com.ruinwesen.patchmanager.client.protocol.ProtocolException;
import com.ruinwesen.patchmanager.client.protocol.RequestGetPatchSourceList;
import com.ruinwesen.patchmanager.client.protocol.Response;

public class ClientRepository {

    private static Log log = LogFactory.getLog(ClientRepository.class);
    public static final String LAST_SYNCED = "last-synced";
    
    private File dir;
    private File settingsFile;
    private Object LOCK = new Object();
    private Object SETTINGS_LOCK = new Object();
    private PatchIndex patchindex;
    
    public ClientRepository(File dir, PatchIndex patchindex) {
        if (dir == null) {
            throw new NullPointerException();
        }
        this.dir = dir;
        this.settingsFile = new File(dir, ".settings");
        this.patchindex = patchindex;
    }

    public void sync(DefaultPatchManagerClient client, Auth auth, PatchDownloadCallback cb) throws ProtocolException, IOException, InterruptedException {
        try {
            synchronized (LOCK) {
                __sync(client, auth, cb);
            }
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error(ex);
            }
            throw ex;
        } catch (InterruptedException ex) {
            if (log.isErrorEnabled()) {
                log.error(ex);
            }
            throw ex;
        }
    }
    
    private void __sync(DefaultPatchManagerClient client, Auth auth, PatchDownloadCallback cb) throws ProtocolException, IOException, InterruptedException {
        
        Date lsd = getLastSyncDate();

        if (log.isDebugEnabled()) {
            log.debug("using last sync date:"+((lsd == null) ? null : CSUtils.dateToString(lsd)));
        }
        
        Response response = 
            client.execute(new RequestGetPatchSourceList(lsd, auth)); 
        if (!response.isOkStatus()) {
            throw new IOException(response.getMessage());
        }

        setLastSyncDate(CSUtils.now());
        List<PatchSource> list = response.getPatchSourceList();
        

        if (log.isDebugEnabled()) {
           log.debug("received patch list containing "+
                  list.size()
                   +" patches");
        }
        
        HttpClient hc = client.getHttpclient();
        
        ExecutorService executorService = Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors()+2);
        List<PatchDownloadTask> taskList = new ArrayList<PatchDownloadTask>(list.size());
        HashSet<String> deletedPatchIds = new HashSet<String>();
        
        int numDownloadFiles = 0;
        for (PatchSource source: list) {
            if (!source.isDeleteFlagSet()) {
                String name = source.getPatchId();
                File dstfile = new File(dir, name);
                if (!dstfile.exists()) {
                    numDownloadFiles++;
                }
            }
        }
        
        int noDownloadPatch = 0;
        for (PatchSource source: list) {
            String name = source.getPatchId();
            File dstfile = new File(dir, name);
            if (source.isDeleteFlagSet()) {
                boolean delete_ok = dstfile.delete();
                if (log.isDebugEnabled()) {
                    log.debug("Deleting patch file:"+dstfile+" ok:"+delete_ok
                            +" file exists:"+dstfile.exists());
                }
                deletedPatchIds.add(name);
                continue;
            }
            if (dstfile.exists()) {
                if (log.isDebugEnabled()) {
                   log.debug("sync:dst file exists:"+dstfile);
                }
                continue;
            } 
            File tmpFile = File.createTempFile("patch-", "");
            tmpFile.deleteOnExit();
            
            HTTPGetTask getTask =
                new HTTPGetTask(hc, new HttpGet(source.getPatchURL()), tmpFile);
            PatchDownloadTask dlTask =  
                new PatchDownloadTask(getTask, dstfile, source.getPatchId());
            if (cb != null) {
                noDownloadPatch++;
                executorService.submit(new TaskWithUpdate(cb,
                        "Downloaded patch "+noDownloadPatch+" of "+numDownloadFiles,
                        dlTask));
            } else {
                executorService.submit(dlTask);
            }
            taskList.add(dlTask);
        }
        executorService.shutdown();
        executorService.awaitTermination(1, TimeUnit.HOURS);
        
        List<File> fileList = new ArrayList<File>(taskList.size());
        for (PatchDownloadTask dltask: taskList) {
            if (dltask.isSuccess()) {
                fileList.add(dltask.getTargetFile());
            }
        }
        
        if (patchindex != null) {
            patchindex.addToIndex(fileList, deletedPatchIds);
        }
    }
    
    private static final class TaskWithUpdate implements Callable<File>  {
        private PatchDownloadCallback cb;
        private String message;
        private Callable<File> call;

        public TaskWithUpdate(PatchDownloadCallback cb, String message, Callable<File> call) {
            this.cb = cb;
            this.message = message;
            this.call = call;
        }

        @Override
        public File call() throws Exception {
            File result = call.call();
            cb.patchDownloaded(message);
            return result;
        }
    }
    
    private void storePatch(PatchSource source, InputStream is, File file) {
        try {
            CSUtils.copy(is, file, true);
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("could not store patch", ex);
            }
        }
    }

    public File getDir() {
        return dir;
    }
    
    public File getSettingsFile() {
        return settingsFile;
    }
    
    public CSProperties readSettings()  {
        synchronized (SETTINGS_LOCK) {
        return CSUtils.loadProperties(settingsFile);
        }
    }
    
    public Date getLastSyncDate() {
        return readSettings().getDateProperty(LAST_SYNCED, CSUtils.parseDate("1990-01-01"	));
    }
    
    public void setLastSyncDate(Date value) {
        if (log.isDebugEnabled()) {
            log.debug("setting last sync date:"+CSUtils.dateToString(value));
         }
        CSProperties p = readSettings();
        p.setDateProperty(LAST_SYNCED, value);
        synchronized (SETTINGS_LOCK) {
        CSUtils.storeProperties(p, settingsFile);
        }
    }
    
}
