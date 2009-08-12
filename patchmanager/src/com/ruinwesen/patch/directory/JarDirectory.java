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
package com.ruinwesen.patch.directory;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

public class JarDirectory extends Directory {

    private static class JDEntry implements Entry {
        private ZipEntry zipentry;
        public JDEntry(ZipEntry zipentry) {
            if (zipentry == null) throw new NullPointerException();
            this.zipentry = zipentry;
        }
        @Override
        public boolean isDirectory() {
            return zipentry.isDirectory();
        }
        @Override
        public boolean isFile() {
            return !isDirectory();
        }
        @Override
        public String getName() {
            String name = zipentry.getName();
            return name;
        }
        @Override
        public File getLocalFile() {
            return null;
        }
    }

    private JarFile jar;

    public JarDirectory(File file) throws IOException {
        this(new JarFile(file));
    }

    public JarDirectory(JarFile jarFile) {
        if (jarFile == null) {
            throw new IllegalArgumentException("jarfile==null");
        }
        this.jar = jarFile;
    }

    @Override
    public void close() throws IOException {
        jar.close();
    }
    
    @Override
    public Entry getEntry(String path) {
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        ZipEntry ze = jar.getEntry(path);
        if (ze == null) {
            return null;
        }
        return new JDEntry(ze);
    }
    
    @Override
    public InputStream getInputStream(Entry entry) throws IOException {
        return jar.getInputStream(zipEntry(entry));
    }

    private ZipEntry zipEntry(Entry entry) {
        if (!(entry instanceof JDEntry)) {
            throw new IllegalArgumentException("invalid entry: "+entry);
        }
        return ((JDEntry)entry).zipentry;
    }

    @Override
    public Enumeration<Entry> entryEnumeration(Entry entry) {
        return filter(jar.entries(), entry == null ? null : zipEntry(entry));
    }

    private Enumeration<Entry> filter(Enumeration<JarEntry> entries,
            ZipEntry parentOrSelf) {
        return new FilterEnumeration(entries, parentOrSelf);
    }
    
    private class FilterEnumeration implements Enumeration<Entry> {
        private Enumeration<? extends ZipEntry> entries;
        private ZipEntry parentOrSelf;
        private ZipEntry next;
        
        public FilterEnumeration(Enumeration<? extends ZipEntry> entries,
                ZipEntry parentOrSelf) {
            this.entries = entries;
            this.parentOrSelf = parentOrSelf;
            selectNext();
        }
        
        private void selectNext() {
            next = null;
            while (entries.hasMoreElements()) {
                ZipEntry z = entries.nextElement();
                if (parentOrSelf == null || 
                        isPathParentOrSelf(parentOrSelf.getName(), z.getName())) {
                    next = z;
                    break;
                }
            }
        }
        @Override
        public boolean hasMoreElements() {
            return next != null;
        }
        @Override
        public Entry nextElement() {
            if (!hasMoreElements()) {
                throw new NoSuchElementException();
            }
            JDEntry res = new JDEntry(next);
            selectNext();
            return res;
        }
        
    }
    
    @Override
    public String toString() {
        return getClass().getName()+"[jar:"+jar.getName()+"]";
    }
    
    
}
 