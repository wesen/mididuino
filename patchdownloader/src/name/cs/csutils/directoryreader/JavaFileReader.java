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
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.NoSuchElementException;

import name.cs.csutils.iterator.EmptyEnumeration;
import name.cs.csutils.iterator.FileEnumeration;

/**
 * Reads files and directories on a filesystem.
 * @author chresan
 */
public class JavaFileReader extends DirectoryReader {

    /**
     * The base file.
     * All file entries are descendants of this file.
     */
    private File baseFile;

    /**
     * Reads from the specified directory.
     * @param basePath the root directory.
     */
    public JavaFileReader(String basePath) {
        this(new File(basePath));
    }

    /**
     * Reads from the specified directory.
     * @param baseFile the root directory.
     */
    public JavaFileReader(File baseFile) {
        this.baseFile = baseFile;
    }

    /**
     * Returns the base file.
     * @return the base file
     */
    public File getBaseFile() {
        return baseFile;
    }
    
    /**
     * {@inheritDoc}
     */
    public Enumeration<JavaFileEntry> entryEnumeration() {
        return entryEnumeration((String)null);
    }

    /**
     * Returns all file- and directory entries matching the specified path.
     * 
     * @param path the path
     * @return all file- and directory entries matching the specified path
     * @see #entryEnumeration(String, boolean)
     */
    public Enumeration<JavaFileEntry> entryEnumeration(File path) {
        return entryEnumeration(path, true);
    }

    /**
     * Returns all file- and directory entries matching the specified path.
     * 
     * @param path the path
     * @param relativePaths true if the entries should have relative paths,
     *                      false if the entries should have absolute paths
     * @return all file- and directory entries matching the specified path
     * @see #entryEnumeration(String, boolean)
     */
    public Enumeration<JavaFileEntry> entryEnumeration(File path, boolean relativePaths) {
        String pathName = translateToPath(baseFile, path);
        if (pathName == null) {
            // the path is not descendant or equals the basefile
            return EmptyEnumeration.getInstance();
        }
        return entryEnumeration(pathName, relativePaths);
    }

    /**
     * {@inheritDoc}
     */
    public Enumeration<JavaFileEntry> entryEnumeration(String path) {
        return entryEnumeration(path, true);
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public Enumeration<JavaFileEntry> entryEnumeration(String path,
            boolean relativePaths) {
        
        File root;
        File translationBaseFile;
        if (path == null) {
            root = baseFile;
            translationBaseFile = baseFile;
        } else {
            root = new File(baseFile, path);
            translationBaseFile = relativePaths ? root : baseFile;
        }
        
        return new FileEntryEnumeration(translationBaseFile,root);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public InputStream open(Entry entry) throws IOException {
        return new FileInputStream(((JavaFileEntry)entry).getFile());
    }
    
    /**
     * Computes the path name of the specified file relative to the base file. 
     * 
     * @param basefile the base file
     * @param file the file
     * @return returns the relative path or null if the file is not a descendant of the
     * basefile
     */
    static String translateToPath(File basefile, File file) {
        if (file.equals(basefile)) {
            return file.isDirectory() ? "" : file.getName();
        }
        // do path translation
        String str = "";
        while (true) {
            str = file.getName()+str;
            File parent = file.getParentFile();
            
            if (parent == null) {
                return null;
            } else if (parent.equals(basefile)) {
                return str;
            } else {
                file = parent;
                str = '/'+str;
            }
        }
    }
    
    
    /**
     * Enumerates over a set of files, creates the necessary entries .
     * @author chresan
     */
    private static class FileEntryEnumeration implements Enumeration<JavaFileEntry> {

        // the remaining file entries
        private Enumeration<File> enumeration;
        
        // the file we compute the relative path names to
        private File relativeBasePath;

        public FileEntryEnumeration(File relativeBasePath, File root) {
            this.relativeBasePath = relativeBasePath;
            // ignore the directory entry with an empty path name ("")
            boolean ignoreFirst = root.isDirectory() && relativeBasePath.equals(root);
            
            this.enumeration = new FileEnumeration(root, !ignoreFirst);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean hasMoreElements() {
            return enumeration.hasMoreElements();
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public JavaFileEntry nextElement() {
            if (!hasMoreElements()) {
                throw new NoSuchElementException();
            }
            return new JavaFileEntry(relativeBasePath,enumeration.nextElement());
        }

    }

}
