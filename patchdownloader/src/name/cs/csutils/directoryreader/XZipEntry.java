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

import java.util.zip.ZipEntry;

import name.cs.csutils.CSUtils;

/**
 * A java.util.zip.ZipEntry entry.
 *  
 * @author chresan
 */
public class XZipEntry implements Entry {
    
    // the path value
    private String path;
    
    // the zip entry
    private ZipEntry zipEntry;

    /**
     * Creates a new zip entry.
     * @param path the path name
     * @param zipEntry the zip entry
     */
    public XZipEntry(String path, java.util.zip.ZipEntry zipEntry) {
        this.path = path;
        this.zipEntry = zipEntry;
    }

    /**
     * Returns the zip entry.
     * @return the zip entry
     */
    public ZipEntry getZipEntry() {
        return zipEntry;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public String getName() {
        return CSUtils.getLastPathComponent(zipEntry.getName());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getPath() {
        return path;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isDirectory() {
        return zipEntry.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isFile() {
        return !zipEntry.isDirectory();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long size() {
        if (isDirectory()) {
            return -1;
        }
        return zipEntry.getSize();
    }

    /**
     * Returns the path name.
     */
    @Override
    public String toString() {
        return getPath();
    }
    
}