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
package com.ruinwesen.patchdownloader.indexer;

import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.FileFilterFactory;

import com.ruinwesen.patch.DefaultPatchMetadata;
import com.ruinwesen.patch.PatchMetadata;
import com.ruinwesen.patch.StoredPatch;
import com.ruinwesen.patch.Tagset;

public class Index implements Serializable {
    
    private static final Log log = LogFactory.getLog(Index.class);
    
    private static final long serialVersionUID = 1440002812734809731L;
    private transient List<PatchMetadata> data;
    private File repodir;
    private TagStats tagstats = new TagStats();
    
    public Index() {
        data = new ArrayList<PatchMetadata>();
    }
    
    public Tagset getAllTags() {
        return tagstats.tags();
    }
    
    public void setRepositoryDir(File dir) {
        this.repodir = dir;
    }
    
    public Iterator<PatchMetadata> iterator() {
        return Collections.unmodifiableCollection(data).iterator();
    }

    public void update() throws IOException {
        data.clear();
        File[] files = repodir == null || !repodir.isDirectory()
            ? new File[0] :
            repodir.listFiles(FileFilterFactory.and(
                            FileFilterFactory.FilesOnly(),
                            FileFilterFactory.SuffixFilter(".rwp")));

        if (log.isDebugEnabled()) {
            log.debug("update index: "+repodir+", files:"+files.length);
        }
            
        for (File f: files) {
            StoredPatch patch = new StoredPatch.JarFilePatch(f);
            PatchMetadata data = patch.getMetadata();
            if (!(data instanceof DefaultPatchMetadata)) {
                data = new DefaultPatchMetadata(data);
            }
            this.data.add(data);
        }
        
        tagstats.reset();
        collect(tagstats, Scorer.ONE);
    }

    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    
    private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException {
        in.defaultReadObject();
        data = new ArrayList<PatchMetadata>();
        update();
    }
    
    public void collect(Collector collector, Scorer scorer) {
        if (scorer == Scorer.ONE) {
            for (PatchMetadata item: data) {
                collector.collect(item, 1f);
            }
        } else if (scorer == Scorer.ZERO) {
            for (PatchMetadata item: data) {
                collector.collect(item, 0f);
            }
        } else {
            for (PatchMetadata item: data) {
                collector.collect(item, scorer.score(item));
            }
        }
    }
    
}
