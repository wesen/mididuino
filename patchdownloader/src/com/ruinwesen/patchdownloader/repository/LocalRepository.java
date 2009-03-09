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
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import name.cs.csutils.CSUtils;
import name.cs.csutils.iterator.FileEnumeration;
import name.cs.csutils.swing.FileFilterFactory;

public abstract class LocalRepository extends Repository {

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
    public StoredPatch[] listPatches() throws IOException {
        validate();
        if (compressed) {
            List<StoredPatch> list = new ArrayList<StoredPatch>(300);
            Enumeration<File> files = new FileEnumeration(repositoryDir, ".rwp");
            while (files.hasMoreElements()) {
                File file = files.nextElement();
                if (file.isDirectory() || 
                        !repositoryDir.equals(file.getParentFile())) {
                    continue;
                }
                list.add(new StoredPatch.JarFilePatch(file));
            }
            return list.toArray(new StoredPatch[list.size()]);
        } else {
            File[] files = repositoryDir.listFiles(FileFilterFactory.DirectoriesOnly());
            if (files == null) {
                throw new IOException("could not retrieve directories");
            }
            List<StoredPatch> list = new ArrayList<StoredPatch>(files.length);
            for (File file: files) {
                list.add(new StoredPatch.DirectoryPatch(file));
            }
            return list.toArray(new StoredPatch[list.size()]);
        }
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
