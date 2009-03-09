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
