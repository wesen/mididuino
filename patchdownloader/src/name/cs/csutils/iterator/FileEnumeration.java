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
package name.cs.csutils.iterator;

import java.io.File;
import java.io.FileFilter;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.NoSuchElementException;

public class FileEnumeration implements Enumeration<File> {

    private static class FEFilter implements FileFilter {
        private String extension;

        public FEFilter(String extension) {
            this.extension = extension;
        }

        @Override
        public boolean accept(File pathname) {
            return pathname.isFile() && pathname.getName().endsWith(extension)
            || pathname.isDirectory();
        }
    }
    
    private LinkedList<File> list = new LinkedList<File>();
    private FileFilter filter;
    
    public FileEnumeration(File start, String extension) {
        this(start, true, new FEFilter(extension), true);
    }
    
    public FileEnumeration(File start, boolean includeStartFile) {
        this(start, includeStartFile, null, false);
    }
    
    public FileEnumeration(File start, boolean includeStartFile,
            FileFilter filter, boolean filterStartFile) {
        if (start == null) {
            throw new IllegalArgumentException("start==null");
        }
        this.filter = filter;
        if (!filterStartFile || (filter != null && filter.accept(start))) {
            if (includeStartFile) {
                list.add(start);
            } else if (start.isDirectory()) {
                addChildrenOf(start);
            }    
        }
    }
    
    private void addChildrenOf(File file) {
        File[] children = filter != null ? file.listFiles(filter) : file.listFiles();
        for (int i=children.length-1;i>=0;i--) {
            list.addFirst(children[i]);
        }
    }

    @Override
    public boolean hasMoreElements() {
        return !list.isEmpty();
    }

    @Override
    public File nextElement() {
        if (!hasMoreElements()) {
            throw new NoSuchElementException();
        }
        File next = list.removeFirst();
        if (next.isDirectory()) {
            addChildrenOf(next);
        }
        return next;
    }
}
