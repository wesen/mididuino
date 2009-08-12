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
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.NoSuchElementException;

import name.cs.csutils.CSUtils;

public class FSDirectory extends Directory {

    private static class FSEntry implements Entry {
        private File root;
        private File file;
        private String name = null;
        public FSEntry(File root, File file) {
            if (root == null || file == null) throw new NullPointerException();
            this.root = root;
            this.file = file;
        }
        @Override
        public boolean isDirectory() {
            return file.isDirectory();
        }
        @Override
        public boolean isFile() {
            return file.isFile();
        }
        @Override
        public String getName() {
            if (name == null) {
                LinkedList<String> names = new LinkedList<String>();
                if (file.isDirectory()) {
                    names.add("");
                }
                File pos = file;
                while (!pos.equals(root)) {
                    names.addFirst(pos.getName());
                    pos = pos.getParentFile();
                }
                this.name = CSUtils.join("/", names.iterator());
            }
            return name;
        }
        @Override
        public File getLocalFile() {
            return file;
        }
    }
    
    private File root;
    private FileFilter includeFilter;
    public static final FileFilter DEFAULT_FILTER = new DefaultIncludeFilter();
    
    private static final class DefaultIncludeFilter implements FileFilter {
        @Override
        public boolean accept(File file) {
            if ((!file.exists())|| file.isHidden()) {
                return false;
            }
            return true;
        }
    }

    public FSDirectory(File root) {
        this(root, DEFAULT_FILTER);
    }
    
    public FSDirectory(File root, FileFilter includeFilter) {
        this.root = root;
        this.includeFilter = includeFilter;
    }

    @Override
    public Entry getEntry(String path) {
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        File file = new File(root, path);
        if ((!file.exists()) || (!includeFilter.accept(file))) {
            return null;
        }
        return new FSEntry(root, file);
    }

    private FSEntry fsentry(Entry entry) {
        if (!(entry instanceof FSEntry)) {
            throw new IllegalArgumentException("entry:"+entry);
        }
        return ((FSEntry)entry);
    }
    
    @Override
    public InputStream getInputStream(Entry entry) throws IOException {
        return new FileInputStream(fsentry(entry).file);
    }

    @Override
    public Enumeration<Entry> entryEnumeration(Entry entry) {
        File file = entry == null ? root : fsentry(entry).file;
        return new FileEnumeration(file, includeFilter);
    }

    private class FileEnumeration implements Enumeration<Entry> {

        private LinkedList<File> files = new LinkedList<File>();
        private FileFilter includeFilter;
        private File next;
        
        public FileEnumeration(File file, FileFilter includeFilter) {
            files.add(file);
            this.includeFilter = includeFilter;
            selectNext();
        }
        
        private void selectNext() {
            next = null;
            while (!files.isEmpty()) {
                File file = files.removeFirst();
                if (includeFilter.accept(file)) {   
                    if (file.isDirectory()) { // add all children
                        files.addAll(0, Arrays.asList(file.listFiles(includeFilter)));
                    } 
                    next = file;
                    break;
                }
            }
        }

        @Override
        public boolean hasMoreElements() {
            return next != null;
        }

        @Override
        public FSEntry nextElement() {
            if (!hasMoreElements()) {
                throw new NoSuchElementException();
            }
            FSEntry r = new FSEntry(root, next);
            selectNext();
            return r;
        }
        
    }
    
    @Override
    public String toString() {
        return getClass().getName()+"[file:"+root.getAbsolutePath()+"]";
    }
    
}
