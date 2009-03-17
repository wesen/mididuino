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
package name.cs.csutils.directoryreader;

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import name.cs.csutils.CSUtils;

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
     * Returns the path inside the zip file.
     */
    @Override
    protected String resolvePath(String path) {
        return zipFile.getName()+"!"+path;
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
                } else if (basePath.endsWith("/")) {
                    // ensure that the path name ends with a slash
                    basePath = basePath.substring(0, basePath.length()-1);
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
                
                if (entryPath.startsWith(basePath+'/')) {                    
                    // translate path to base path
                    if (relativePaths) {
                        
                        if (entryPath.length() == basePath.length()) {
                            entryPath = CSUtils.getLastPathComponent(entryPath);
                        } else {
                            entryPath = entryPath.substring(basePath.length()+1);
                        }
                    }
                    
                    // finished
                    next = new XZipEntry(entryPath, zipEntry);
                } else if (entryPath.equals(basePath)) {                    
                    // translate path to base path
                    if (relativePaths) {
                        
                        entryPath = CSUtils.getLastPathComponent(entryPath);
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
    
    @Override
    public void close() throws IOException {
        zipFile.close();
    }
    

}
