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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.jar.JarEntry;
import java.util.jar.JarOutputStream;
import java.util.zip.ZipOutputStream;

public class JarFileBuilder {

    private static final class Node<E> {
        public E item;
        public String pathComponent;
        public String path;
        public List<Node<E>> children;
        public Node(String path, String pathComponent, E item) {
            this.path = path;
            this.pathComponent = pathComponent;
            this.item = item;
        }
        public boolean add(String path, E item) {
            if (path.length()==0 || path.equals("/")) {
                return false;
            }
            
            int begin = 0;
            int end = path.length();
            if (path.startsWith("/")) {
                begin = 2;
            }
            if (path.endsWith("/")) {
                end --;
            }
        
            Node<E> parent = this;
            for(;;) {
                int idx = path.indexOf('/', begin);
                if (idx<0) { idx = end; } 
             //   System.out.printf("path'%s',begin:%d idx:%d\n", path,begin,idx);
                String pathComponent = path.substring(begin, idx);
                
                Node<E> child = parent.findChildOrCreate(pathComponent);
                if (idx >= end) {
                    if (child.item != null) {
                        return false;// collision 
                    } else {
                        child.item = item;
                        return true;
                    }
                }
                parent = child;
                begin = idx+1;
            }
        }
        private Node<E> findChildOrCreate(String pathComponent) {
            if (children == null) {
                children = new LinkedList<Node<E>>();
            }
            ListIterator<Node<E>> iter = children.listIterator();
            for (int i=0;i<children.size();i++) {
                Node<E> n = iter.next();
                int cmp = n.pathComponent.compareTo(pathComponent);
                if (cmp == 0) {
                    return n;
                } else if (cmp>0) {
                    iter.previous(); // select insert location
                    break;
                }
            }
            // create new node
            Node<E> n = new Node<E>(path+(path.length()==0?"":"/")+pathComponent, pathComponent, null);
            iter.add(n); // insert child
            return n;
        }
        
    }
    
    private ZipOutputStream zipstream;
    private boolean writePerformed = false;
    private Node<Content> root = new Node<Content>("","",null);
    
    public JarFileBuilder(File file) throws IOException {
        this(new BufferedOutputStream(new FileOutputStream(file)));
    }
    
    public JarFileBuilder(OutputStream out) throws IOException {
        if (out == null) {
            throw new NullPointerException();
        }
        this.zipstream = new JarOutputStream(out);
        this.zipstream.setLevel(9);
        this.zipstream.setMethod(ZipOutputStream.DEFLATED);
    }

    public void add(Directory dir) {
        add(dir, dir.entryEnumeration());
    }
    
    public void add(
            Directory dir, Enumeration<Entry> entryEnumeration) {
        add("", dir, entryEnumeration);
    }
    
    public void add(String pathPrefix, Directory dir) {
        add(pathPrefix, dir, dir.entryEnumeration());
    }
    
    public void add(
            String pathPrefix,
            Directory dir, Enumeration<Entry> entryEnumeration) {
        if (pathPrefix.startsWith("/")) {
            pathPrefix = pathPrefix.substring(1);
        }
        if ((pathPrefix.length()!=0) && (!pathPrefix.endsWith("/"))) {
            pathPrefix = pathPrefix+"/";
        }
        
        while (entryEnumeration.hasMoreElements()) {
            Entry entry = entryEnumeration.nextElement();
            String name = pathPrefix+entry.getName();
            root.add(name, entry.isFile() ? new Content(dir, entry) : null);
        }   
    }
    
    public void add(String pathName, InputStream is) {
        if (is == null) {
            throw new IllegalArgumentException("is==null");
        }
        root.add(pathName, new Content(is));
    }
    
    public void write() throws IOException {
        if (writePerformed) {
            throw new IllegalStateException("write may be called only once");
        }
        
        writePerformed = true;
        writeChildren(root.children);
        root.children = null;
        zipstream.flush();
    }
    
    private void writeChildren(List<Node<Content>> children) throws IOException {
        if (children == null) {
            return;
        }
        for (Node<Content> node: children) {
            Content content = node.item;
            if (content != null && content.is != null) { // input stream
                zipstream.putNextEntry(new JarEntry(node.path));
                try {
                    write(zipstream, content.is);
                } finally {
                    content.is.close();
                }
                zipstream.closeEntry();
            } else if (content != null && content.entry != null && content.entry.isFile()) { // file
                zipstream.putNextEntry(new JarEntry(node.path));
                InputStream is = content.dir.getInputStream(content.entry);
                try {
                    write(zipstream, is);
                } finally {
                    is.close();
                }
                zipstream.closeEntry();
            } else { // directory (path must end with '/'
                zipstream.putNextEntry(new JarEntry(node.path+"/"));
                writeChildren(node.children);
                zipstream.closeEntry();
            }
        }
    }

    private final byte[] buffer = new byte[8096];
    
    private void write(OutputStream out, InputStream is) throws IOException {
        int r;
        while ((r=is.read(buffer))!=-1) {
            out.write(buffer, 0, r);
        }
    }

    public void close() throws IOException {
        zipstream.close();
    }

    private static final class Content {
        Entry entry;
        Directory dir;
        InputStream is;
        public Content(InputStream is) {
            this.is = is;
        }
        public Content(Directory dir, Entry entry) {
            this.dir = dir;
            this.entry = entry;
        }
    }
    
}
