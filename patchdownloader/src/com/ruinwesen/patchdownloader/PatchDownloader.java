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
package com.ruinwesen.patchdownloader;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.MissingResourceException;
import java.util.Properties;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;
import name.cs.csutils.LockFile;
import name.cs.csutils.LockFile.MultipleApplicationInstancesException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.RWMidiSend;
import com.ruinwesen.patchdownloader.indexer.IndexReader;
import com.ruinwesen.patchdownloader.indexer.MetadataIndexWriter;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.repository.HTTPRepository;
import com.ruinwesen.patchdownloader.repository.LocalRepository;
import com.ruinwesen.patchdownloader.repository.RemoteBackupRepository;
import com.ruinwesen.patchdownloader.repository.StoredPatch;
import com.ruinwesen.patchdownloader.repository.StoredPatchCollector;
import com.ruinwesen.patchdownloader.repository.WorkspaceRepository;

public class PatchDownloader {
    
    private static final String PATCH_DOWNLOADER_VERSION_STRING = 
        "1.0";

    public static final String KEY_WORKSPACE = "workspace";
    public static final String KEY_MIDISEND = "midisend";

    private static Log alog;
    private static Log getLog() {
        if (alog == null) {
            alog = LogFactory.getLog(PatchDownloader.class);
        }
        return alog;
    }
    
    private File configDir;
    protected LocalRepository remoteRepositoryBackup;
    protected LocalRepository workspaceRepository;
    protected HTTPRepository remoteRepository;
    private IndexReader patchIndex = new IndexReader();
    private MidiSend midiSend;
    private volatile long stopping = 0;
    private boolean debugModeOn = false;
    
    private static PatchDownloader INSTANCE;
    
    public PatchDownloader() {
        super();
        if (PatchDownloader.INSTANCE != null) {
            throw new RuntimeException("only one patchdownloader instance permitted");
        }
        
        PatchDownloader.INSTANCE = this;
    }
    
    public static boolean isDebugModeOn() {
        return INSTANCE.debugModeOn;
    }
    
    public static PatchDownloader getSharedInstance() {
        if (INSTANCE == null) {
            throw new RuntimeException("patchdownloader instance unavailable");
        }
        return INSTANCE;
    }

    public MidiSend getMidiSend() {
        return midiSend;
    }
    
    public void setMidiSend(MidiSend midiSend) {
        if (midiSend == null) {
            throw new IllegalArgumentException("midiSend == null");
        }
        this.midiSend = midiSend;
    }
    
    public static final String PatchdownloaderVersion() {
        return  PATCH_DOWNLOADER_VERSION_STRING;
    }
    
    public LocalRepository getWorkspaceRepository() {
        return workspaceRepository;
    }

    public LocalRepository getRemoteRepositoryBackup() {
        return remoteRepositoryBackup;
    }

    public HTTPRepository getRemoteRepository() {
        return remoteRepository;
    }
    
    public File getConfigDir() {
        return configDir;
    }

    public IndexReader getPatchIndex() {
        return patchIndex;
    }
    
    public int getAvailablePatches(Date since) {
        return remoteRepository
        .collectPatches(new StoredPatchCollector.Counter())
        .getCount();
    }
    
    private Properties getConfig() throws IOException {
        try {
            return readProperties("config.properties");
        } catch (FileNotFoundException ex) {
            // it does not matter if the file exists or not
            return new Properties();
        } catch (IOException ex) {
            if (getLog().isDebugEnabled()) {
                getLog().debug("could not read config file", ex);
            }
            throw ex;
        }
    }
    
    private void storeConfig(Properties db) throws IOException {
        writeProperties("config.properties", db);
    }
    
    private transient Properties applicationProperties; 
    
    private Properties getApplicationProperties() {
        if (applicationProperties == null) { 
            applicationProperties = new Properties();
            InputStream in = getClass().getResourceAsStream("/application.properties");
            if (in != null) {
                if (getLog().isDebugEnabled()) {
                    getLog().debug("Application properties available.");
                }
                try {
                    applicationProperties.load(new BufferedInputStream(in));
                    in.close();
                } catch (IOException ex) {
                    if (getLog().isDebugEnabled()) {
                        getLog().debug("Could not read application properties.", ex);
                    }
                } 
            } else {
                if (getLog().isDebugEnabled()) {
                    getLog().debug("Application properties not available.");
                }
            }
        }
        // read only
        return new Properties(applicationProperties); 
    }
    
    private Properties readProperties(String name) throws IOException {
        return readProperties(configDir, name);
    }

    private Properties readProperties(File dir, String name) throws IOException {
        Properties props = new Properties();
        InputStream in = new BufferedInputStream(new FileInputStream(
                new File(dir, name)));
        try {
            props.load(in);
        } finally {
            in.close();
        }
        return props;
    }
    
    private void writeProperties(String name, Properties db) throws IOException {
        File file = new File(configDir, name);
        if (getLog().isDebugEnabled()) {
            getLog().debug("writing properties: "+file.getAbsolutePath());
        }
        OutputStream out = new BufferedOutputStream(new FileOutputStream(file));
        try {
            db.store(out, name);
            out.flush();
        } finally {
            out.close();
        }
    }
    
    public void updateRemoteRepositoryBackup() throws IOException {
        String DB_NAME = "remotebackup.properties";
        String KEY_LASTUPDATE_DATE = "remotebackup.lastupdate.date";
        Date since = null; // TODO get and store this date
        Properties db = null;
        try {
            db = readProperties(DB_NAME);
            String datestr = db.getProperty(KEY_LASTUPDATE_DATE);
            since = datestr == null ? null : CSUtils.parseDate(datestr);
            if (since == null) {
                if (getLog().isErrorEnabled()) {
                    getLog().error("reading the '"+KEY_LASTUPDATE_DATE+"' key failed");
                }
            }
        } catch (FileNotFoundException ex) {
            // ignore;
        } catch (IOException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("reading the properties failed: "+DB_NAME, ex);
            }
            // since is null => we download everything again
        }
        if (db == null) {
            db = new Properties();
        }
        
        List<StoredPatch> srclist = new ArrayList<StoredPatch>();
        remoteRepository.collectPatches(new StoredPatchCollector.Collection(srclist),since);
        
        MetadataIndexWriter writer = patchIndex.createAppendableIndexWriter();
        
        // write new documents to index
        for (StoredPatch patch: srclist) {
            // the target file name, we assume there are no collisions
            File dstfile = new File(remoteRepositoryBackup.getBaseDir(),
                    patch.getName());
            // export the patch from the repository
            remoteRepository.exportFile(patch, dstfile);
            
            StoredPatch dstStoredPatch = new StoredPatch.JarFilePatch(dstfile);
            
            // get the metadata
            PatchMetadata metadata;
            try {
                metadata = dstStoredPatch.getMetadata();
            } catch (IOException ex) {
                if (getLog().isErrorEnabled()) {
                    getLog().error("could not read metadata: "+dstfile.getAbsolutePath(), ex);
                }
                // clean up
                dstfile.delete();
                continue; // go on
            }
            // create a document using the metadata
            String path = dstfile.getName();
            int docid = writer.newDocument(path);
            writer.putMetadata(docid, metadata);
        }

        // commit the changes
        writer.flush();
        writer.close();
        
        // memorize when we performed this operation
        db.put(KEY_LASTUPDATE_DATE, CSUtils.dateToString(CSUtils.now()));
        
        try {
            writeProperties(DB_NAME, db);
        } catch (IOException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("updating the '"+KEY_LASTUPDATE_DATE+"' key failed: "+DB_NAME, ex);
            }
            throw ex;
        }

        try {
            // update the in-memory index
            patchIndex.update();
        } catch (IOException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("could not load the index", ex);
            }
            throw ex;
        }
        
    }
    
    public final void start() throws IOException {
        try {
            if (getLog().isInfoEnabled()) {
                Properties sys = System.getProperties();
                String[] keys = {
                        "os.name",
                        "os.arch",
                        "os.version",
                        "java.version",
                        "java.vendor",
                        "java.vendor.url",
                        "java.home",
                        "java.vm.specification.version",
                        "java.vm.specification.vendor",
                        "java.vm.specification.name",
                        "java.vm.version",
                        "java.vm.vendor",
                        "java.vm.name",
                        "java.specification.version",
                        "java.specification.vendor",
                        "java.specification.name",
                        "java.class.version",
                        "java.class.path",
                        "java.library.path",
                        "java.io.tmpdir",
                        "java.compiler"
                        };
                for (String key: keys) {
                    getLog().info(key+"="+sys.getProperty(key));
                }
            }
        } catch (RuntimeException ex) {
            // ignore
        }
        
        try {
            String debug = getApplicationProperties().getProperty("debug");
            debugModeOn = debug != null && "true".equalsIgnoreCase(debug);
            
            configDir = CSUtils.getApplicationConfigDir("patchdownloader");
            if (!configDir.exists() && !configDir.mkdir()) {
                throw new IOException("could not create config directory");
            }
            // tests if the application is already running
            File lock = new File(configDir, "patchdownloader.lock");
            if (new LockFile(lock).isLockActive()) {
                throw new LockFile.MultipleApplicationInstancesException();
            }
            
            // get the config file
            Properties config = getConfig();
            start(config);
        } catch (RuntimeException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("error in start()", ex);
            }
            startExceptionHandler(ex);
        } catch (IOException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("error in start()", ex);
            }
            startExceptionHandler(ex);
        } catch (MultipleApplicationInstancesException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error("error in start()", ex);
            }
            startExceptionHandler(ex);
        }
    }
    
    protected void start(Properties config) throws IOException {
        // configure the remote repository backup
        File rrb = new File(configDir, "remote-repository");
        if (getLog().isDebugEnabled()) {
            getLog().debug("remote-repository dir: "+rrb.getAbsolutePath());
        }
        if (!rrb.exists() && !rrb.mkdir()) {
            throw new IOException("could not create the remote repository backup directory: "+rrb.getAbsolutePath());
        }
        // create the repository - only jar files are permittet
        remoteRepositoryBackup = new RemoteBackupRepository();
        remoteRepositoryBackup.setBaseDir(rrb);
        
        // configure the workspace repository
        workspaceRepository = new WorkspaceRepository();

        String workspaceFileName = config.getProperty(KEY_WORKSPACE);
        if (workspaceFileName != null) {
            // note: the location does not necessarily exist
            workspaceRepository.setBaseDir(new File(workspaceFileName));
        }
        if (getLog().isDebugEnabled()) {
            getLog().debug("workspace dir: "+workspaceFileName);
        }
        
        remoteRepository = new HTTPRepository();
        
        // configure patch index
        File indexDir = new File(configDir, "index");
        if (getLog().isDebugEnabled()) {
            getLog().debug("patch index directory: "+indexDir.getAbsolutePath());
        }
        if (!indexDir.exists() && !indexDir.mkdir()) {
            throw new IOException("could not create index directory");
        }
        if (!indexDir.exists() && !indexDir.mkdir()) {
            handleFatalExceptionOnStart("could not create index directory", 
                    new FileNotFoundException("could not create index directory: "+indexDir.getAbsolutePath()));
        }
        patchIndex.setIndexDir(indexDir);
        try {
            patchIndex.update();
        } catch (FileNotFoundException ex) {
            // no op - no index, then it will be created later
        } catch (IOException ex) {
            if (getLog().isWarnEnabled()) {
                getLog().warn("could not read the patch index, please rebuild the index", ex);
            }
        }
        
        // create midisend
        /*String selectedMidisend = config.getProperty(KEY_MIDISEND);
        if (RWMidiSend.class.getName().equals(selectedMidisend)) {
            this.midiSend = new RWMidiSend();
        } else if (JavaMidiSend.class.getName().equals(selectedMidisend)) {
            this.midiSend = new JavaMidiSend();
        } else {
            this.midiSend = new RWMidiSend(); // default
        }*/
        this.midiSend = new RWMidiSend(); // default
        
        try {
            I18N.getSharedInstance().setResourceBundleName("resources/lang/MessageBundle");
        } catch (MissingResourceException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error(ex);
            }
        }
    }

    protected void handleFatalExceptionOnStart(String message, Throwable  ex) {
        getLog().fatal(message, ex);
        System.exit(1);
    }
    
    public boolean isStopping() {
        return stopping != 0;
    }

    public synchronized final void stop() {
        if (isStopping()) {
            if (getLog().isDebugEnabled()) {
                getLog().debug("attempt to stop() again");
            }
            
            return;
        }
        stopping = 1;
        if (getLog().isDebugEnabled()) {
            getLog().debug("stopping...");
        }
        
        Properties config = null;
        try {
            try {
                config = getConfig();
            } catch (FileNotFoundException ex) {
                // ignore - we write the new config anyway
            } catch (IOException ex) {
                // ignore - we write the new config anyway
            }
            if (config == null) {
                config = new Properties();
            }
            stop(config);
            try {
                storeConfig(config);
            } catch (IOException ex) {
                if (getLog().isErrorEnabled()) {
                    getLog().error("could not store config", ex);
                }
            }
        } catch (Throwable ex) {
            try {
                if (getLog().isErrorEnabled()) {
                    getLog().error("error in exit()", ex);
                }
                stopExceptionHandler(ex);
            } finally {
                System.exit(1);
            }
        } finally {
            System.exit(0);
        }
    }

    protected void startExceptionHandler(Throwable ex) {
        // no op
        if (ex instanceof MultipleApplicationInstancesException) {
            System.exit(1);
        }
    }

    protected void stopExceptionHandler(Throwable ex) {
        // no op
    }

    protected void stop(Properties config) throws IOException {
        // update the configuration
        // workspace basedir
        File workspaceFile = workspaceRepository.getBaseDir();
        if (workspaceFile == null) {
            config.remove(KEY_WORKSPACE);
        } else {
            String path;
            try {
                path = workspaceFile.getCanonicalPath();
            } catch (IOException ex) {
                path = workspaceFile.getAbsolutePath();
            }
            config.setProperty(KEY_WORKSPACE, path);
        }
        // midisend implementation
        config.setProperty(KEY_MIDISEND, midiSend.getClass().getName());
    }

}
