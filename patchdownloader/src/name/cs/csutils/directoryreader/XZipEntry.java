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

import java.util.zip.ZipEntry;

/**
 * A java.util.zip.ZipEntry entry.
 *  
 * @author chresan
 */
public class XZipEntry implements Entry {
    
    // the path value
    private String path;
    
    // the zip entry
    private ZipEntry zipEntry;

    /**
     * Creates a new zip entry.
     * @param path the path name
     * @param zipEntry the zip entry
     */
    public XZipEntry(String path, java.util.zip.ZipEntry zipEntry) {
        this.path = path;
        this.zipEntry = zipEntry;
    }

    /**
     * Returns the zip entry.
     * @return the zip entry
     */
    public ZipEntry getZipEntry() {
        return zipEntry;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public String getName() {
        return ZipFileReader.getLastPathComponent(zipEntry.getName());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getPath() {
        return path;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirectory() {
        return zipEntry.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isFile() {
        return !zipEntry.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long size() {
        if (isDirectory()) {
            return -1;
        }
        return zipEntry.getSize();
    }

    /**
     * Returns the path name.
     */
    public String toString() {
        return getPath();
    }
    
}