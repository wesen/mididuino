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
import java.io.Serializable;
import java.util.regex.Pattern;

public final class FileFilterFactory {

    private static FileFilter DIRECTORIES_ONLY = new DirectoryFilter();
    private static FileFilter FILES_ONLY = new FilesFilter();
    private static FileFilter ANY = new ConstFilter(true);
    private static FileFilter NONE = new ConstFilter(false);

    // no instances
    private FileFilterFactory() {
        super();
    }

    /**
     * FILTER:= (*<suffix>)|<name>
     * 
     * @param filterString
     * @return
     */
    public static FileFilter parseFilter(String filterString) {
        if ("*".equals(filterString)) {
            return ANY;
        }
        if (filterString.startsWith("*") && filterString.indexOf('*', 1)<0) {
            return new FileSuffixFilter(filterString.substring(1), true);
        } else if (filterString.indexOf('*')>=0) {
            return new WildcardFilter(filterString, true);
        } else {
            return new FileNameIs(filterString, true);
        }
    }

    /**
     * FILTER_LIST := (FILTER(,FILTER)*)?
     * FILTER:= [-]?(*<suffix>)|<name>
     * 
     * @param filterString
     * @return
     */
    public static FileFilter parseFilterList(String filterString) {
        FileFilter excludeFilter = NONE;
        FileFilter includeFilter = NONE;
        for (String fstr: filterString.split(",")) {
            if (fstr.isEmpty()) {
                continue;
            }
            if (fstr.charAt(0)=='-') {
                excludeFilter = or(excludeFilter, not(parseFilter(fstr.substring(1))));
            } else {
                includeFilter = or(includeFilter, parseFilter(fstr));
            }
        }
        FileFilter filter = and(includeFilter, not(excludeFilter));
        return filter;
    }
    
    public static javax.swing.filechooser.FileFilter ForFileChooser(FileFilter filter,
            String description) {
        return new ForChooser(filter, description);
    }
    
    public static FileFilter nameIs(String name) {
        return nameIs(name,true);
    }
    
    private static FileFilter nameIs(String name, boolean ignoreCase) {
        return new FileNameIs(name, ignoreCase);
    }

    public static FileFilter SuffixFilter(String suffix) {
        return new FileSuffixFilter(suffix, true);
    }

    public static FileFilter SuffixFilter(String suffix, boolean ignorecase) {
        return new FileSuffixFilter(suffix, ignorecase);
    }

    public static FileFilter PrefixFilter(String suffix) {
        return new PrefixFilter(suffix, true);
    }

    public static FileFilter PrefixFilter(String suffix, boolean ignorecase) {
        return new PrefixFilter(suffix, ignorecase);
    }
    
    public static FileFilter not(FileFilter f) {
        if (f == ANY) return NONE;
        if (f == NONE) return ANY;
        if (f  instanceof InverseFilter) {
            return ((InverseFilter)f).filter;
        }
        return new InverseFilter(f);
    }

    public static FileFilter any() {
        return ANY;
    }

    public static FileFilter none() {
        return NONE;
    }

    public static FileFilter DirectoriesOnly() {
        return DIRECTORIES_ONLY;
    }

    public static FileFilter FilesOnly() {
        return FILES_ONLY;
    }

    public static FileFilter and(FileFilter a, FileFilter b) {
        if (a == b) return a;
        if (a == NONE || b == NONE) return NONE;
        if (a == ANY) return b;
        if (b == ANY) return a;
        return new ANDFilter(a,b);
    }

    public static FileFilter or(FileFilter a, FileFilter b) {
        if (a == b) return a;
        if (a == NONE) return b;
        if (b == NONE) return a;
        if (a == ANY || b == ANY) return ANY;
        return new ORFilter(a,b);
    }
    
    private static class ForChooser extends javax.swing.filechooser.FileFilter
        implements Serializable {

        /**
         * 
         */
        private static final long serialVersionUID = 5012444320133193693L;
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

    public static javax.swing.filechooser.FileFilter createNamedFileFilter(String name, FileFilter filter) {
        return new NamedFilter(name, filter);
    }

    private static final class NamedFilter extends javax.swing.filechooser.FileFilter implements FileFilter, Serializable {
        private FileFilter delegate;
        private String name;
        public NamedFilter(String name, FileFilter delegate) {
            if (name == null || delegate == null) {
                throw new IllegalArgumentException("name:"+name+";delegate:"+delegate);
            }
            this.name = name;
            this.delegate = delegate;
        }
        @Override
        public boolean accept(File pathname) {
            return delegate.accept(pathname);
        }
        @Override
        public String toString() {
            return name;
        }
        @Override
        public String getDescription() {
            return name;
        }
    }
    
    private static final class DirectoryFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = 5843798416264775275L;
        @Override
        public boolean accept(File pathname) {
            return pathname.isDirectory();
        }
        @Override
        public String toString() {
            return "<Directories>";
        }
    }

    private static final class FilesFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -1065456507027087969L;
        @Override
        public boolean accept(File pathname) {
            return pathname.isFile();
        }
        @Override
        public String toString() {
            return "<Files>";
        }
    }

    private static final class InverseFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -1688558554756176982L;
        private FileFilter filter;
        public InverseFilter(FileFilter filter) {
            this.filter = filter;
        }
        @Override
        public boolean accept(File pathname) {
            return !filter.accept(pathname);
        }
        @Override
        public String toString() {
            return "(not "+filter.toString()+")";
        }
    }


    private static final class ConstFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = 8025700872479650848L;
        private boolean accept;
        public ConstFilter(boolean accept) {
            this.accept = accept;
        }
        @Override
        public boolean accept(File pathname) {
            return accept;
        }
        @Override
        public String toString() {
            return accept ? "*" : "<none>";
        }
    }

    private static final class FileNameIs implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -5784802998120019959L;
        private String name;
        private boolean ignoreCase;
        public FileNameIs(String name, boolean ignoreCase) {
            this.name = name;
            this.ignoreCase = ignoreCase;
        }
        @Override
        public boolean accept(File pathname) {
            if (ignoreCase)
                return pathname.getName().equalsIgnoreCase(name);
            else
                return pathname.getName().equals(name);
        }
        @Override
        public String toString() {
            return name;
        }
    }
    
    private static final class WildcardFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -5387779677809441110L;
        private Pattern pattern;
        private String filter;
        public WildcardFilter (String filter, boolean ignoreCase) {
            this.pattern = Pattern.compile(filter.replaceAll("\\*+", ".*"),
                    ignoreCase ? Pattern.CASE_INSENSITIVE : 0);
            this.filter = filter;
        }
        @Override
        public boolean accept(File pathname) {
            return pattern.matcher(pathname.getName()).matches();
        }
        @Override
        public String toString() {
            return filter;
        }
    }
    private static final class FileSuffixFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = 4328282331008997765L;
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

    private static final class PrefixFilter implements FileFilter, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -1206153327994489103L;
        private String prefix;
        private boolean ignoreCase;
        public PrefixFilter (String prefix, boolean ignoreCase) {
            this.prefix = ignoreCase ? prefix.toLowerCase() : prefix;
            this.ignoreCase = ignoreCase;
        }
        @Override
        public boolean accept(File pathname) {
            if (ignoreCase) {
                return  pathname.getName().toLowerCase().startsWith(prefix);
            }
            return pathname.getName().startsWith(prefix);
        }
        @Override
        public String toString() {
            return prefix+"*";
        }
    }
    private static final class ORFilter implements FileFilter, Serializable {

        /**
         * 
         */
        private static final long serialVersionUID = 4390675060950126936L;
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

    private static final class ANDFilter implements FileFilter, Serializable {

        /**
         * 
         */
        private static final long serialVersionUID = -3763026691428078028L;
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
