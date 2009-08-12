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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileFilter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.FileFilterFactory;
import name.cs.csutils.collector.CollectionCollector;
import name.cs.csutils.collector.Collector;
import name.cs.csutils.collector.MultiCollector;

import com.ruinwesen.patch.Patch;

public class PatchIndex {
    private static Log log = LogFactory.getLog(PatchIndex.class);

    private File repositoryDir;
    private File indexFile;
    private static final FileFilter patchFileFilter =
        FileFilterFactory.FilesOnly();
        //FileFilterFactory.and(FileFilterFactory.FilesOnly(), FileFilterFactory.SuffixFilter(".rwp"));

    private List<IndexedPatchRecord> patchList = new ArrayList<IndexedPatchRecord>();
    private final Object PATCH_LIST_LOCK = new Object();
    private final Object PATCH_FILE_LOCK = new Object();
    
    public PatchIndex(File indexFile, File repositoryDir) {
        if (indexFile==null||repositoryDir == null) {
            throw new IllegalArgumentException("indexFile:"+indexFile+",repository-dir:"+repositoryDir);
        }
        this.repositoryDir = repositoryDir;
        this.indexFile = indexFile;
    }
    
    public File getRepositoryDir() {
        return repositoryDir;
    }
    
    public int computeEstimatedPatchCount() {
        return repositoryDir.listFiles(patchFileFilter).length;
    }
    
    public void writeIndex() {
        PatchRecordIO io = new PatchRecordIO();
        
        List<IndexedPatchRecord> copy = copyList();
        synchronized (PATCH_FILE_LOCK) {
            try {
                PrintWriter os = new PrintWriter(new BufferedWriter(new FileWriter(indexFile)));
                try {
                    io.writeIndex(os, copy);
                } finally {
                    os.close();
                }
            } catch (IOException ex) {
                if (log.isErrorEnabled()) {
                    log.error("Writing index failed", ex);
                }
                indexFile.delete();
            }
        }
    }
    
    public void readIndex(Collector<IndexedPatch> collector) {
        PatchRecordIO io = new PatchRecordIO();
        List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>();
        try {
            Collector<IndexedPatchRecord> collectionCollector =
                new CollectionCollector<IndexedPatchRecord>(list);
            
            Collector<IndexedPatchRecord> passedCollector 
                = collector == null ? collectionCollector
                        : new MultiCollector<IndexedPatchRecord>(collectionCollector, collector);
            
            synchronized (PATCH_FILE_LOCK) {
                io.readIndex(repositoryDir, indexFile, passedCollector);
            }
            synchronized (PATCH_LIST_LOCK) {
                this.patchList = list;
            }
            return;
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("could not read index", ex);
            }
        }
        
        // rebuild index
        rebuildIndex();
    }
    
    public void rebuildIndex() {
        synchronized (PATCH_LIST_LOCK) {
            this.patchList.clear();
        }
        PatchRecordIO io = new PatchRecordIO();
        File[] files = repositoryDir.listFiles(patchFileFilter);
        try {
            PrintWriter os = new PrintWriter(new BufferedWriter(new FileWriter(indexFile)));
            try {
                List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>();
                io.rebuildIndex(repositoryDir, os, Arrays.asList(files), new CollectionCollector<IndexedPatchRecord>(list));
                synchronized (PATCH_LIST_LOCK) {
                    this.patchList = list;
                }
                os.flush();
            } finally {
                os.close();
            }
        } catch (IOException  ex) {
            if (log.isErrorEnabled()) {
                log.error("Could not write index", ex);
            }
        }
    }
    
    public void addToIndex(List<File> files) {
        // append to existing index file
        try {
            List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>(files.size());
            synchronized (PATCH_FILE_LOCK) {
                if (!indexFile.exists()) {
                    return;
                }   
                PrintWriter os =
                    new PrintWriter(new BufferedWriter(new FileWriter(indexFile, true)));
                try {
                    PatchRecordIO io = new PatchRecordIO();
                    io.rebuildIndex(indexFile, os, files, new CollectionCollector<IndexedPatchRecord>(list));
                    os.flush();
                } finally {
                    os.close();
                }
            }
            // add new items
            synchronized (PATCH_LIST_LOCK) {
                this.patchList.addAll(list);
            }
            
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("Appending to index failed", ex);
            }
            return;
        }
        
        
        // TODO
        /*
        List<IndexedPatchRecord> list = update(files, true, false, null);
        synchronized (PATCH_LIST_LOCK) {
            patchList.addAll(list);   
        }*/
    }
    
    private List<IndexedPatchRecord> copyList() {
        synchronized (PATCH_LIST_LOCK) {
            return new ArrayList<IndexedPatchRecord>(patchList);
        }
    }
    
    public void collect(Collector<IndexedPatch> collector) {
        for (IndexedPatch patch: copyList()) {
            if (collector.collect(patch) == Collector.FINISHED) {
                break;
            }
        }
    }
    
    public void collect(Collector<IndexedPatch> collector, ScoreBuilder<Patch> scoreBuilder,
            float minScore) {
        for (IndexedPatch patch: copyList()) {
            float score = scoreBuilder.score(patch);
            if (score<minScore) {
                continue;
            }
            if (score<1) { // the original indexedpatch has score 1.0f, we don't have to create a new object
                patch = new ScoredPatch(patch, score);
            }
            if (collector.collect(patch) == Collector.FINISHED) {
                break;
            }
        }
    }

    public int getPatchCount() {
        synchronized (PATCH_LIST_LOCK) {
            return patchList.size();
        }
    }
    
}
 