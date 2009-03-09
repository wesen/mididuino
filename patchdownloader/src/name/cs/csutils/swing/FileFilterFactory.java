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
package name.cs.csutils.swing;

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
        public String toString() {
            return "<Directories>";
        }
    }

    private static final class FilesFilter implements FileFilter {
        @Override
        public boolean accept(File pathname) {
            return pathname.isFile();
        }
        public String toString() {
            return "<Files>";
        }
    }

    private static final class AnyFilter implements FileFilter {
        @Override
        public boolean accept(File pathname) {
            return true;
        }
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
        public String toString() {
            return "("+a+" and "+b+")";
        }
    }
    
    
    
}
