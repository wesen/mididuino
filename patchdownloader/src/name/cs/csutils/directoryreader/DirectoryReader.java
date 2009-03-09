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
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.channels.Channels;
import java.nio.channels.FileChannel;
import java.util.Enumeration;

/**
 * Reads files and directories.
 * 
 * @author chresan
 */
public abstract class DirectoryReader {

    /**
     * Reads files and directories.
     */
    public DirectoryReader() {
        super();
    }
    
    /**
     * Returns all files and directories.
     * @return all files and directories
     */
    public Enumeration<? extends Entry> entryEnumeration() {
        return entryEnumeration(null);
    }

    /**
     * Returns all file- and directory entries matching the specified path.
     * 
     * @param path the path
     * @return all file- and directory entries matching the specified path
     * @see #entryEnumeration(String, boolean)
     */
    public Enumeration<? extends Entry> entryEnumeration(String path) {
        return entryEnumeration(path, true);
    }

    /**
     * Returns all file- and directory entries matching the specified path.
     * 
     * A file entry is matched if it's path name is equal to the specified path
     * or if the file is inside a directory which has a path name equals to
     * the specified path.
     * 
     * A directory entry is matched if there is an ancestor directory entry
     * with the same path name as the specified path.
     * 
     * If relativePaths is false then the path names of all returned entries
     * are relative to this readers root path name (meaning absolute), 
     * otherwise the path names of all returned entries are relative to the
     * specified path. 
     * 
     * @param path the path to which the entries are matched
     * @param relativePaths true if the entries should have relative paths,
     *                      false if the entries should have absolute paths
     * @return all file- and directory entries matching the specified path
     */
    public abstract Enumeration<? extends Entry> 
        entryEnumeration(String path, boolean relativePaths);

    /**
     * Opens a stream to read the contents of the specified file entry.
     * The property {@link Entry#isFile()} of the specified entry must be true,
     * otherwise an {@link IOException} is thrown. If the Entry class is not
     * supported by this directory reader an {@link ClassCastException} is
     * thrown.
     * 
     * @param entry the entry
     * @throws IOException error while opening/reading the file
     * @throws ClassCastException unsupported entry class 
     * @return a stream to read the contents of the specified file entry
     */
    public abstract InputStream open(Entry entry) throws IOException;
    
    /**
     * Opens a stream to read the contents of the file at the specified path.
     * 
     * @param path the path name of the file
     * @return a stream to read the contents of the specified file
     * @throws IOException could not open the stream or the entry is not a file
     * entry or the entry does not exist
     */
    public InputStream open(String path) throws IOException {
        Enumeration<? extends Entry> enumeration = entryEnumeration(path);
        if (!enumeration.hasMoreElements()) {
            throw new FileNotFoundException("file not found: "+path);
        }
        Entry entry = enumeration.nextElement();
        if (!entry.isFile()) {
            throw new FileNotFoundException("not a file: "+path);
        }
        return open(entry);
    }

    /**
     * Copies the contents of the file at the specified path and writes them
     * to the specified target file.
     * 
     * @param path the path name of the file
     * @param target the target file
     * @throws IOException could not open or read from the source stream 
     */
    public void copyFile(String path, File target) throws IOException {
        Enumeration<? extends Entry> enumeration = entryEnumeration(path);
        if (!enumeration.hasMoreElements()) {
            throw new FileNotFoundException("file not found: "+path);
        }
        Entry entry = enumeration.nextElement();
        if (!entry.isFile()) {
            throw new FileNotFoundException("not a file: "+path);
        }
        transfer(target, entry, null);
    }

    /**
     * Copies the complete directory to the specified target directory.
     * 
     * @param targetDirectory the target directory
     * @throws IOException could not perform a necessary I/O operation
     */
    public void copyTo(File targetDirectory) throws IOException {
        copyTo("", targetDirectory, true);
    }

    /**
     * Copies the entries relative to the specified path to the specified
     * target directory.
     * 
     * @param path the source path
     * @param targetDirectory the target directory
     * @throws IOException could not perform a necessary I/O operation
     */
    public void copyTo(String path, File targetDirectory) throws IOException {
        copyTo(path, targetDirectory, true);
    }
    
    /**
     * Copies the entry at the specified path and it's descendants to the
     * specified target directory.
     * 
     * @param path the source path
     * @param targetDirectory the target directory
     * @param relativePaths true if the target files are relative to the specified path,
     *  false if the target files are relative to this directory readers root path
     * @throws IOException could not perform a necessary I/O operation
     */
    public void copyTo(String path, File targetDirectory, boolean relativePaths) 
        throws IOException {
        if (!targetDirectory.exists()) {
            throw new IOException("target directory not found: "+targetDirectory);
        }
        
        byte[] buffer = null;
        Enumeration<? extends Entry> enumeration =
            entryEnumeration(path, relativePaths);
        while (enumeration.hasMoreElements()) {
            Entry entry = enumeration.nextElement();
            if (entry.isDirectory()) {
                // replicate the directory
                File newDir = new File(targetDirectory, entry.getPath()); 
                if (!newDir.mkdirs()) {
                    throw new IOException("could not create directory: "
                            +newDir.getAbsolutePath());
                }
            } else if (entry.isFile()) {
                // replicate the file
                File newFile = new File(targetDirectory, entry.getPath());
                buffer = transfer(newFile, entry, buffer);
            }
        }
    }
    
    /**
     * Helper method to write the contents of a file entry to the
     * specified target file.
     * 
     * @param target the target file
     * @param entry the file entry to copy
     * @param buffer a byte buffer which can be reused in subsequent calls
     * @return the specified byte buffer or a new byte buffer
     * @throws IOException could not perform a necessary I/O operation
     * @throws IllegalArgumentException the entry is not a file entry
     * @throws NullPointerException if the target or entry argument is null 
     */
    private byte[] transfer(File target, Entry entry,
            byte[] buffer) throws IOException {
        if (!entry.isFile()) {
            throw new IllegalArgumentException("not a file entry: "+entry);
        }
        
        File parent = target.getParentFile();
        if (!parent.exists() && !parent.mkdirs()) {
            throw new IOException("could not create directory: "
                    +parent.getAbsolutePath());
        }
        FileOutputStream out = new FileOutputStream(target);
        try {
            InputStream inputStream = open(entry);
            try {
                long size = entry.size();
                if (size>=0) {
                    FileChannel chout = out.getChannel();
                    try {
                        chout.transferFrom(Channels.newChannel(inputStream), 0, size);
                    } finally {
                        chout.close();
                    }
                } else {
                    if (buffer == null) {
                        buffer = new byte[8192];
                    }
                    int r;
                    while ((r=inputStream.read(buffer))!=-1) {
                        out.write(buffer, 0, r);
                    }
                }
            } finally {
                inputStream.close();
            }
        } finally {
            out.close();
        }
        return buffer;
    }
    
    
    /* TODO
    public void copyTo(ZipOutputStream out, String path, boolean relativePaths) {
        new ZipEntry()
    }*/
    
}
