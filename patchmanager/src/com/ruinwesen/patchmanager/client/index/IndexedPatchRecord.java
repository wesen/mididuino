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
package com.ruinwesen.patchmanager.client.index;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarDirectory;
import com.ruinwesen.patch.metadata.PatchMetadata;

class IndexedPatchRecord extends PatchRecord implements IndexedPatch {

    private File indexDir;

    public IndexedPatchRecord(File indexDir, String patchfileName, PatchMetadata meta) {
        this(indexDir);
        this.patchfileName = patchfileName;
        this.meta = meta;
    }
    
    public IndexedPatchRecord(File indexDir) {
        this.indexDir = indexDir;
    }
    
    @Override
    public Directory openDirectory() throws IOException {
        if (indexDir == null) {
            throw new IOException("index directory undefined");
        }
        File file = getLocalFile();
        if (file.isFile()) {
            return new JarDirectory(file);
        } else if (file.isDirectory()) {
            return new FSDirectory(file);
        } else {
            throw new FileNotFoundException("file not found: "+file);
        }
    }

    @Override
    public File getLocalFile() {
        return new File(indexDir, patchfileName);
    }
    
    @Override
    public PatchMetadata getMetadata() {
        return meta;
    }

    public float score() {
        return 1f;
    }

    @Override
    public int hashCode() {
        return patchfileName.hashCode();
    }

    @Override
    public String toString() {
        return "IndexedPatch[patch="+patchfileName+"]";
    }

    @Override
    public String internalId() {
        return patchfileName;
    }

    @Override
    public boolean equals(Object o) {
        return compareEquals(this, o);
    }
    
    static final boolean compareEquals(IndexedPatch a, Object o) {
        if (a == o) return true;
        if (o == null || !(o instanceof IndexedPatch)) return false;
        return a.internalId().equals(((IndexedPatch)o).internalId());
    }

}
