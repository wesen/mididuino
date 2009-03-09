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
                return null;
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
        
    }

    public String toString() {
        return getClass().getSimpleName()+"[path="+getPath()+"]";
    }
        
}
