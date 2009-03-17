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
package name.cs.csutils;

import java.io.File;
import java.io.FileFilter;

public final class FileFilterFactory {

    private static FileFilter DIRECTORIES_ONLY = new DirectoryFilter();
    private static FileFilter FILES_ONLY = new FilesFilter();
    private static FileFilter FILES_AND_DIRECTORIES = new AnyFilter();

    // no instances
    private FileFilterFactory() {
        super();
    }
    
    public static javax.swing.filechooser.FileFilter ForFileChooser(FileFilter filter,
            String description) {
        return new ForChooser(filter, description);
    }
    
    public static FileFilter SuffixFilter(String suffix) {
        return new FileSuffixFilter(suffix, true);
    }

    public static FileFilter SuffixFilter(String suffix, boolean ignorecase) {
        return new FileSuffixFilter(suffix, ignorecase);
    }

    public static FileFilter FilesAndDirectories() {
        return FILES_AND_DIRECTORIES;
    }

    public static FileFilter DirectoriesOnly() {
        return DIRECTORIES_ONLY;
    }

    public static FileFilter FilesOnly() {
        return FILES_ONLY;
    }

    public static FileFilter and(FileFilter a, FileFilter b) {
        return new ANDFilter(a,b);
    }

    public static FileFilter or(FileFilter a, FileFilter b) {
        return new ORFilter(a,b);
    }
    
    private static class ForChooser extends javax.swing.filechooser.FileFilter {

        private FileFilter filter;
        private String description;

        public ForChooser(FileFilter filter, String description) {
            this.filter = filter;
            this.description = description;
        }

        @Override
        public boolean accept(File f) {
            return filter.accept(f);
        }

        @Override
        public String getDescription() {
            return description;
        }
        
    }
    
    private static final class DirectoryFilter implements FileFilter {
        @Override
        public boolean accept(File pathname) {
            return pathname.isDirectory();
        }
        @Override
        public String toString() {
            return "<Directories>";
        }
    }

    private static final class FilesFilter implements FileFilter {
        @Override
        public boolean accept(File pathname) {
            return pathname.isFile();
        }
        @Override
        public String toString() {
            return "<Files>";
        }
    }

    private static final class AnyFilter implements FileFilter {
        @Override
        public boolean accept(File pathname) {
            return true;
        }
        @Override
        public String toString() {
            return "*.*";
        }
    }

    private static final class FileSuffixFilter implements FileFilter {
        private String suffix;
        private boolean ignoreCase;
        public FileSuffixFilter (String suffix, boolean ignoreCase) {
            this.suffix = ignoreCase ? suffix.toLowerCase() : suffix;
            this.ignoreCase = ignoreCase;
        }
        @Override
        public boolean accept(File pathname) {
            if (ignoreCase) {
                return  pathname.getName().toLowerCase().endsWith(suffix);
            }
            return pathname.getName().endsWith(suffix);
        }
        @Override
        public String toString() {
            return "*"+suffix;
        }
    }
    
    private static final class ORFilter implements FileFilter {

        private FileFilter a;
        private FileFilter b;

        public ORFilter(FileFilter a, FileFilter b) {
            this.a = a;
            this.b = b;
        }
        
        @Override
        public boolean accept(File pathname) {
            return a.accept(pathname) || b.accept(pathname); 
        }
        @Override
        public String toString() {
            return "("+a+" or "+b+")";
        }
    }

    private static final class ANDFilter implements FileFilter {

        private FileFilter a;
        private FileFilter b;

        public ANDFilter(FileFilter a, FileFilter b) {
            this.a = a;
            this.b = b;
        }
        
        @Override
        public boolean accept(File pathname) {
            return a.accept(pathname) && b.accept(pathname); 
        }
        @Override
        public String toString() {
            return "("+a+" and "+b+")";
        }
    }
    
    
    
}
