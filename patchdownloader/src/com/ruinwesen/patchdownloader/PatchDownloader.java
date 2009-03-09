/** 
 * Copyright (C) 2009 Christian Schneider
 *
 * This file is part of Patchdownloader.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
import java.util.Date;
import java.util.MissingResourceException;
import java.util.Properties;

import javax.xml.parsers.ParserConfigurationException;

import name.cs.csutils.CSUtils;
import name.cs.csutils.i18n.I18N;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.store.LockObtainFailedException;

import com.ruinwesen.midisend.JavaMidiSend;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.RWMidiSend;
import com.ruinwesen.patchdownloader.lucene.LuceneIndex;
import com.ruinwesen.patchdownloader.patch.PatchDocument;
import com.ruinwesen.patchdownloader.patch.XMLPatchMetadata;
import com.ruinwesen.patchdownloader.repository.HTTPRepository;
import com.ruinwesen.patchdownloader.repository.LocalRepository;
import com.ruinwesen.patchdownloader.repository.RemoteBackupRepository;
import com.ruinwesen.patchdownloader.repository.StoredPatch;
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
    private LuceneIndex luceneIndex = new LuceneIndex();
    private boolean luceneIndexCorrupted = false;
    private boolean stopping = false;
    private MidiSend midiSend;
    
    private static PatchDownloader INSTANCE;
    
    public PatchDownloader() {
        super();
        if (PatchDownloader.INSTANCE != null) {
            throw new RuntimeException("only one patchdownloader instance permitted");
        }
        
        PatchDownloader.INSTANCE = this;
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
    
    public boolean getLuceneIndexCorruptedFlag() {
        return luceneIndexCorrupted;
    }
    
    public LocalRepository getWorkspaceRepository() {
        return workspaceRepository;
    }

    public LocalRepository getWorkspaceRepositoryBackup() {
        return workspaceRepository;
    }

    public HTTPRepository getRemoteRepository() {
        return remoteRepository;
    }
    
    public File getConfigDir() {
        return configDir;
    }

    public LuceneIndex getLuceneIndex() {
        return luceneIndex;
    }
    
    public int getAvailablePatches(Date since) throws IOException {
        return remoteRepository.listPatches(since).length;
    }
    
    private Properties getConfig() throws IOException {
        try {
            return readProperties("config");
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
        writeProperties("config", db);
    }
    
    private Properties readProperties(String name) throws IOException {
        Properties props = new Properties();
        InputStream in = new BufferedInputStream(new FileInputStream(
                new File(configDir, name)));
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
        
        StoredPatch[] srclist = remoteRepository.listPatches(since);
        
        for (StoredPatch patch: srclist) {
            // the target file name, we assume there are no collisions
            File dstfile = new File(remoteRepositoryBackup.getBaseDir(),
                    patch.getName());
            // export the patch from the repository
            remoteRepository.export(patch, dstfile);
            // get the metadata
            XMLPatchMetadata metadata;
            try {
                metadata = new XMLPatchMetadata(dstfile);
            } catch (ParserConfigurationException ex) {
                if (getLog().isErrorEnabled()) {
                    getLog().error("could not instantiate xml parser", ex);
                }
                // clean up
                dstfile.delete();
                // we cannot continue
                throw new IOException("could not instantiate xml parser", ex);
            }
            // create a lucene document using the metadata
            Document luceneDocument = PatchDocument.Document(metadata);
            // the unique document id
            String documentId = "remote:"+dstfile.getName();
            // index the document
            luceneIndex.index(documentId, luceneDocument);
        }

        // commit the changes
        try {
            luceneIndex.setOptimizeAfterCommitEnabled(true);
            luceneIndex.commit();
        } finally {
            luceneIndex.setOptimizeAfterCommitEnabled(false);
        }
        
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
    }
    
    public final void start() throws IOException {
        try {
            configDir = CSUtils.getApplicationConfigDir("patchdownloader");
            if (!configDir.exists() && !configDir.mkdir()) {
                throw new IOException("could not create config directory");
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
        
        // configure lucene
        File indexDir = new File(configDir, "index");
        if (getLog().isDebugEnabled()) {
            getLog().debug("lucene index dir: "+indexDir.getAbsolutePath());
        }
        if (!indexDir.exists() && !indexDir.mkdir()) {
            throw new IOException("could not create index directory");
        }
        luceneIndex.setIndexDir(indexDir);
        try {
            luceneIndex.open();
        } catch (LockObtainFailedException ex) {
            this.luceneIndexCorrupted = true;
            if (getLog().isWarnEnabled()) {
                getLog().warn("lucene index is corrupted", ex);
            }
            handleLockObtainFailedOnStartException(ex);
        } catch (CorruptIndexException ex) {
            this.luceneIndexCorrupted = true;
            if (getLog().isWarnEnabled()) {
                getLog().warn("lucene index is corrupted", ex);
            }
        } catch (IOException ex) {
            this.luceneIndexCorrupted = true;
            if (getLog().isWarnEnabled()) {
                getLog().warn("lucene index IO error", ex);
            }
        }
        
        // create midisend
        String selectedMidisend = config.getProperty(KEY_MIDISEND);
        if (RWMidiSend.class.getName().equals(selectedMidisend)) {
            this.midiSend = new RWMidiSend();
        } else if (JavaMidiSend.class.getName().equals(selectedMidisend)) {
            this.midiSend = new JavaMidiSend();
        } else {
            this.midiSend = new RWMidiSend(); // default
        }
        
        try {
            I18N.getSharedInstance().setResourceBundleName("resources/lang/MessageBundle");
        } catch (MissingResourceException ex) {
            if (getLog().isErrorEnabled()) {
                getLog().error(ex);
            }
        }
    }

    protected void handleLockObtainFailedOnStartException(
            LockObtainFailedException ex) {
        String message = "Please ensure no other application instance is running.\n"
        +"If this problem still exists try to delete the lock-file.\n"
        +"Exiting...";
        getLog().fatal(message);
        System.exit(1);
    }

    public synchronized final void stop() {
        if (stopping) {
            if (getLog().isDebugEnabled()) {
                getLog().debug("attempt to stop() again");
            }
            
            return;
        }
        stopping = true;
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
    }

    protected void stopExceptionHandler(Throwable ex) {
        // no op
    }

    protected void stop(Properties config) throws IOException {
        // stop lucene
        luceneIndex.close();
        
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
