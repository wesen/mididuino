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

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Enumeration;

import name.cs.csutils.CSUtils;
import name.cs.csutils.iterator.FileEnumeration;
import name.cs.csutils.swing.FileFilterFactory;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public abstract class LocalRepository extends Repository {

    private Log log = LogFactory.getLog(LocalRepository.class);
    private File repositoryDir;
    private boolean compressed;

    public LocalRepository(File repositoryDir, boolean compressed) {
        this.repositoryDir = repositoryDir;
        this.compressed = compressed;
    }

    public LocalRepository(boolean compressed) {
        this.compressed = compressed;
        this.repositoryDir = null;
    }
    
    public boolean isCompressed() {
        return compressed;
    }
    
    public void validate() throws IOException {
        if (repositoryDir == null) {
            throw new IOException("base directory not set: "+repositoryDir);
        } else if (repositoryDir.isFile()) {
            throw new IOException("base directory is a file: "+repositoryDir);
        } else if (!repositoryDir.exists()) {
            throw new IOException("base directory does not exist");
        }
    }

    public void setBaseDir(File baseDir) {
        this.repositoryDir = baseDir;
    }

    public File getBaseDir() {
        return repositoryDir;
    }

    @Override
    public <C extends StoredPatchCollector> C collectPatches(C collector) {
        try {
            validate();
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("validate() failed", ex);
            }
            return collector;
        }
        
        if (compressed) {
            Enumeration<File> files = new FileEnumeration(repositoryDir, ".rwp");
            while (files.hasMoreElements()) {
                File file = files.nextElement();
                if (file.isDirectory() || 
                        !repositoryDir.equals(file.getParentFile())) {
                    continue;
                }
                if (!collector.takesMore()) {
                    break;
                }
                collector.collect(this, new StoredPatch.JarFilePatch(file));
            }
        } else {
            File[] files = repositoryDir.listFiles(FileFilterFactory.DirectoriesOnly());
            if (files == null) {
                if (log.isErrorEnabled()) {
                    log.error("could not retrieve directories");
                    return collector;
                }
            }
            for (File file: files) {
                if (!collector.takesMore()) {
                    break;
                }
                collector.collect(this, new StoredPatch.DirectoryPatch(file));
            }
        }
        return collector;
    }

    @Override
    public void export(StoredPatch patch, OutputStream out) throws IOException {
        if (compressed) {
            StoredPatch.JarFilePatch jp = (StoredPatch.JarFilePatch) patch;
            CSUtils.copy(jp.getFile(), out);
        } else {
            // TODO compress directory into a jar file and export it
            throw CSUtils.NotImplementedYet();
        }
    }

}
