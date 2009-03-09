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
package name.cs.csutils.directoryreader;

import java.io.File;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * A java.io.File entry.
 *  
 * @author chresan
 */
public class JavaFileEntry implements Entry {

    /**
     * The cached logger.
     */
    private static Log log = LogFactory.getLog(JavaFileEntry.class);

    // the file we compute the relative path names to
    private File relativeBasePath;
    // the file 
    private File file;
    // the cached path name
    private transient String path;

    /**
     * Creates a new entry.
     * 
     * @param relativeBasePath the base path
     * @param file the file
     */
    public JavaFileEntry(File relativeBasePath, File file) {
        if (relativeBasePath == null) {
            throw new IllegalArgumentException("relative base path file is null");
        }
        if (file == null) {
            throw new IllegalArgumentException("file is null");
        }
        
        this.relativeBasePath = relativeBasePath;
        this.file = file;
    }

    /**
     * Returns the file.
     * @return the file
     */
    public File getFile() {
        return file;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public String getName() {
        return file.getName();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getPath() {
        if (path == null) {
            path = JavaFileReader.translateToPath(relativeBasePath, file);
            if (path == null) {
                if (log.isDebugEnabled()) {
                    log.debug("translation to base path failed:"
                            +"base:"+relativeBasePath.getAbsolutePath()
                            +",file:"+file.getAbsolutePath());
                }
            }
        }
        return path;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirectory() {
        return file.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isFile() {
        return file.isFile();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long size() {
        return file.length();
    }
    
    /**
     * Returns the path name.
     */
    public String toString() {
        return getPath();
    }
    
}