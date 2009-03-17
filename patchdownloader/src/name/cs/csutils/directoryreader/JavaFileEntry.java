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
package name.cs.csutils.directoryreader;

import java.io.File;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * A java.io.File entry.
 *  
 * @author chresan
 */
public class JavaFileEntry implements Entry {

    /**
     * The cached logger.
     */
    private static Log log = LogFactory.getLog(JavaFileEntry.class);

    // the file we compute the relative path names to
    private File relativeBasePath;
    // the file 
    private File file;
    // the cached path name
    private transient String path;

    /**
     * Creates a new entry.
     * 
     * @param relativeBasePath the base path
     * @param file the file
     */
    public JavaFileEntry(File relativeBasePath, File file) {
        if (relativeBasePath == null) {
            throw new IllegalArgumentException("relative base path file is null");
        }
        if (file == null) {
            throw new IllegalArgumentException("file is null");
        }
        
        this.relativeBasePath = relativeBasePath;
        this.file = file;
    }

    /**
     * Returns the file.
     * @return the file
     */
    public File getFile() {
        return file;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public String getName() {
        return file.getName();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getPath() {
        if (path == null) {
            path = JavaFileReader.translateToPath(relativeBasePath, file);
            if (path == null) {
                if (log.isDebugEnabled()) {
                    log.debug("translation to base path failed:"
                            +"base:"+relativeBasePath.getAbsolutePath()
                            +",file:"+file.getAbsolutePath());
                }
            }
        }
        return path;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirectory() {
        return file.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isFile() {
        return file.isFile();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long size() {
        return file.length();
    }
    
    /**
     * Returns the path name.
     */
    @Override
    public String toString() {
        return getPath();
    }
    
}