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
package com.ruinwesen.patchdownloader.repository;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

import javax.xml.parsers.ParserConfigurationException;

import name.cs.csutils.directoryreader.DirectoryReader;
import name.cs.csutils.directoryreader.JarFileReader;
import name.cs.csutils.directoryreader.JavaFileReader;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patchdownloader.patch.DefaultPatchMetadata;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.XMLPatchMetadata;

public abstract class StoredPatch {
    
    private static Log log = LogFactory.getLog(JarFilePatch.class);
    
    public static final String DEFAULT_METADATA = "patch-metadata.xml";

    protected StoredPatch() {
        super();
    }
    
    public abstract String getName();
    
    public abstract String getPath();
    
    public abstract DirectoryReader getDirectoryReader() throws IOException;
    
    /**
     * Returns the default patch metadata of this patch or <code>null</code> if unavailable.
     * @return the default patch metadata of this patch or <code>null</code> if unavailable
     * @throws IOException could not read the metadata file although it was available
     */
    public PatchMetadata getMetadata() throws IOException {
        return getMetadata(DEFAULT_METADATA);
    }
    
    /**
     * Returns the patch metadata of this patch or <code>null</code> if unavailable.
     * @param path the path of the metadata file
     * @return the patch metadata of this patch or <code>null</code> if unavailable
     * @throws IOException could not read the metadata file although it was available
     */
    public abstract PatchMetadata getMetadata(String path) throws IOException ;
    
    public static final class DirectoryPatch extends StoredPatch {

        private File baseDir;

        public DirectoryPatch(File baseDir) {
            if (baseDir == null) {
                throw new IllegalArgumentException("baseDir == null");
            }
            this.baseDir = baseDir;
        }
        
        public File getBaseDir() {
            return baseDir;
        }
        
        @Override
        public PatchMetadata getMetadata(String path) throws IOException {
            if (path == null) {
                throw new IllegalArgumentException("path==null");
            }
            validateBaseDir();
            File file = new File(baseDir, path);
            InputStream in = new BufferedInputStream(
                    new FileInputStream(file));
            PatchMetadata metadata;
            try {
                metadata = read(in);
            } finally {
                in.close();
            }
            return metadata;
        }

        private void validateBaseDir() throws IOException {
            if (!baseDir.exists()) {
                throw new IOException("basedir does not exist: "+baseDir.getAbsolutePath());
            } 
            if (!baseDir.isDirectory()) {
                throw new IOException("basedir must be a directory: "+baseDir.getAbsolutePath());
            }
        }

        @Override
        public String getPath() {
            return baseDir.getAbsolutePath();
        }

        @Override
        public String getName() {
            return baseDir.getName();
        }

        @Override
        public DirectoryReader getDirectoryReader() {
            return new JavaFileReader(baseDir);
        }
        
    }
    
    protected PatchMetadata read(InputStream in) throws IOException {
        PatchMetadata copy;
        try {
            PatchMetadata source;
            try {
                source = new XMLPatchMetadata(in);
            } catch (ParserConfigurationException ex) {
                if (log.isErrorEnabled()) {
                    log.error("could not parse XML metadata", ex);
                }
                throw new IOException("could not read metadata", ex);
            }
            copy = new DefaultPatchMetadata(source);
        } finally {
            in.close();
        }
        return copy;
    }
    
    public static final class JarFilePatch extends StoredPatch {
        
        private File file;
        
        public JarFilePatch(File file) {
            if (file == null) {
                throw new IllegalArgumentException("file==null");
            }
            this.file = file;
        }
        
        public File getFile() {
            return file;
        }

        @Override
        public String getPath() {
            return file.getAbsolutePath();
        }
        
        @Override
        public PatchMetadata getMetadata(String path) throws IOException {
            if (path == null) {
                throw new IllegalArgumentException("path==null");
            }
            PatchMetadata metadata;
            JarFile jar = new JarFile(file);
            try {
                ZipEntry entry = jar.getEntry(path);
                if (entry == null) {
                    return null; // no such metadata
                }
                InputStream in = jar.getInputStream(entry);
                try {
                    metadata = read(in);
                } finally {
                    in.close();
                }
            } finally {
                jar.close();
            }
            return metadata;
        }

        @Override
        public String getName() {
            return file.getName();
        }


        @Override
        public DirectoryReader getDirectoryReader() throws IOException {
            return new JarFileReader(new JarFile(file));
        }
    }
    
    public static final class Remote extends StoredPatch {

        private static Log log = LogFactory.getLog(Remote.class);
        
        private String uri;

        public Remote(String uri) {
            this.uri = uri;
        }
        
        @Override
        public String getPath() {
            return uri;
        }
        
        /**
         * Metadata is not available.
         */
        @Override
        public PatchMetadata getMetadata(String path) throws IOException {
            if (path == null) {
                throw new IllegalArgumentException("path==null");
            }
            return null;
        }

        @Override
        public String getName() {
            String path = null;
            try {
                path = new URI(uri).getPath();
            } catch (URISyntaxException ex) {
                if (log.isDebugEnabled()) {
                    log.debug("invalid uri syntax", ex);
                }
                path = uri;
            }
            
            // get last path component
            int idx = path.lastIndexOf('/');
            if (idx<0) idx = path.lastIndexOf('\\');
            path = idx < 0 ? path : path.substring(idx+1);
            return path;
        }

        @Override
        public DirectoryReader getDirectoryReader() throws IOException {
            return null;
        }
        
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+"[path="+getPath()+"]";
    }
        
}
