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

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * Reads files and directories in a zip file. 
 * 
 * @author chresan
 */
public class ZipFileReader<Z extends ZipFile> extends DirectoryReader {

    
    /**
     * The zip file
     */
    private Z zipFile;

    /**
     * Reads files and directories in the specified zip file.
     * @param zipfile the zip file
     */
    public ZipFileReader(Z zipfile) {
        this.zipFile = zipfile;
    }

    public Z getZipFile() {
        return zipFile;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public Enumeration<XZipEntry> entryEnumeration(String path,
            boolean relativePaths) {
        return new EntryEnumeration(path, relativePaths, zipFile.entries());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public InputStream open(Entry entry) throws IOException {
        return zipFile.getInputStream(((XZipEntry) entry).getZipEntry());
    }

    /**
     * Enumerates over a set of zip entries, creates the necessary wrapper
     * objects .
     * @author chresan
     */
    private static class EntryEnumeration implements Enumeration<XZipEntry> {

        // the zip entry enumeration
        private Enumeration<? extends ZipEntry> enumeration;
        
        // the base path
        private String basePath;
        
        // use relative paths or nut
        private boolean relativePaths;
        
        // the next entry
        private XZipEntry next;

        /**
         * Creates a zip entry enumeration.
         * @param path the path name
         * @param relativePaths true if the entries should have relative paths,
         *                      false if the entries should have absolute paths
         * @param enumeration the zip entries
         */
        public EntryEnumeration(String basePath, boolean relativePaths,
                Enumeration<? extends ZipEntry> enumeration) {
            // normalize the path name
            if (basePath != null) {
                if ((basePath.length() == 0 || basePath.equals("/"))) {
                    // the condition indicates that this is the root path name
                    basePath = null; 
                } else if (!basePath.endsWith("/")) {
                    // ensure that the path name ends with a slash
                    basePath = basePath + '/';
                }
            }
            
            this.basePath = basePath;
            this.relativePaths = relativePaths;
            this.enumeration = enumeration;
        }

        /**
         * {@inheritDoc}}
         */
        @Override
        public boolean hasMoreElements() {
            // align: find the next element 
            while (next == null && enumeration.hasMoreElements()) {
                // next candidate
                ZipEntry zipEntry = enumeration.nextElement();
                // full path of an entry: this/is/a/path
                String entryPath = zipEntry.getName();
                
                // path filter is disabled ? -> return element
                if (basePath == null) { 
                    next = new XZipEntry(entryPath, zipEntry);
                    return true;
                } 
                
                if (entryPath.startsWith(basePath)) {                    
                    // translate path to base path
                    if (relativePaths) {
                        
                        if (entryPath.length() == basePath.length()) {
                            entryPath = getLastPathComponent(entryPath);
                        } else {
                            entryPath = entryPath.substring(basePath.length());
                        }
                    }
                    
                    // finished
                    next = new XZipEntry(entryPath, zipEntry);
                }
            }
            return next != null;
        }

        /**
         * {@inheritDoc}}
         */
        @Override
        public XZipEntry nextElement() {
            if (!hasMoreElements()) {
                throw new NoSuchElementException();
            }
            XZipEntry result = next;
            next = null;
            return result;
        }
        
    }
    
    /**
     * Returns the last path component of the specified path.
     * The components are separated using the slash character '/'
     * @param path the path
     * @return the last path component of the specified path
     */
    static String getLastPathComponent(String path) {
        int separator = path.lastIndexOf('/');
        if (separator>0) { // keep name if it contains only a single '/'
            return path.substring(separator+1);
        }
        return path;
    }

}
