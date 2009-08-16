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
import java.io.FileFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import name.cs.csutils.CSUtils;
import name.cs.csutils.FileFilterFactory;
import name.cs.csutils.collector.CollectionCollector;
import name.cs.csutils.collector.Collector;
import name.cs.csutils.collector.MultiCollector;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.metadata.Tagset;

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
    
    public void readIndex(Collector<IndexedPatch> collector) {
        List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>();
        try {
            Collector<IndexedPatchRecord> collectionCollector =
                new CollectionCollector<IndexedPatchRecord>(list);
            
            Collector<IndexedPatchRecord> passedCollector 
                = collector == null ? collectionCollector
                        : new MultiCollector<IndexedPatchRecord>(collectionCollector, collector);
            
            synchronized (PATCH_FILE_LOCK) {
                RecordIO io = new RecordIO(indexFile, RecordIO.READ);
                readRecords(io, passedCollector);
            }
            synchronized (PATCH_LIST_LOCK) {
                this.patchList = list;
            }
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("could not read index", ex);
            }
            // rebuild index
            rebuildIndex();
        }
    }
    
    public void rebuildIndex() {
        if (log.isDebugEnabled()) {
            log.debug("rebuilding index");
        }
        synchronized (PATCH_LIST_LOCK) {
            this.patchList.clear();
        }
        File[] files = repositoryDir.listFiles(patchFileFilter);
        try {
            RecordIO io = new RecordIO(indexFile, RecordIO.WRITE);
            try {
                List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>();
                rebuildIndex(io, Arrays.asList(files), new CollectionCollector<IndexedPatchRecord>(list));
                synchronized (PATCH_LIST_LOCK) {
                    this.patchList = list;
                }
                io.flush();
            } finally {
                io.close();
            }
        } catch (IOException  ex) {
            if (log.isErrorEnabled()) {
                log.error("Could not write index", ex);
            }
            indexFile.delete(); // delete corrupted index
        }
    }

    public void writeIndex() {
        List<IndexedPatchRecord> copy = copyList();
        synchronized (PATCH_FILE_LOCK) {
            try {
                RecordIO io = new RecordIO(indexFile, RecordIO.WRITE);
                try {
                    for (IndexedPatchRecord record: copy) {
                        writeRecord(io, record);
                    }
                    io.flush();
                } finally {
                    io.close();
                }
            } catch (IOException ex) {
                if (log.isErrorEnabled()) {
                    log.error("Writing index failed", ex);
                }
                indexFile.delete();
            }
        }
    }
    
    public void addToIndex(List<File> files) {
        // append to existing index file
        try {
            List<IndexedPatchRecord> list = new ArrayList<IndexedPatchRecord>(files.size());
            synchronized (PATCH_FILE_LOCK) {
                RecordIO io = new RecordIO(indexFile, RecordIO.APPEND);
                try {
                    rebuildIndex(io, files, new CollectionCollector<IndexedPatchRecord>(list));
                    io.flush();
                } finally {
                    io.close();
                }
            }
            // add new items
            synchronized (PATCH_LIST_LOCK) {
                this.patchList.addAll(list);
            }
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("Appending to index failed: deleting index", ex);
            }
            indexFile.delete(); // delete corrupted index
            
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
    
    
    // record io

    private IndexedPatchRecord readRecord(RecordIO io) throws IOException {
        IndexedPatchRecord record = new IndexedPatchRecord(repositoryDir);
        record.patchfileName = io.readString();
        int pathcount = io.readInteger();
        PatchMetadata meta = new DefaultPatchMetadata();
        for (int i=0;i<pathcount;i++) {
            String name = io.readString();
            String path = io.readString();
            meta.addPath(new Path(name, path));
        }
        meta.setAuthor(io.readString());
        meta.setComment(io.readString());
        String l = io.readString();
        meta.setDeviceId(l.isEmpty() ? null : PatchMetadataIDInfo.getDeviceId(l));
        l = io.readString();
        meta.setEnvironmentId(l.isEmpty() ? null:PatchMetadataIDInfo.getEnvironmentId(l));
        try {
        meta.setLastModifiedDate(CSUtils.parseDate(io.readString()));
        } catch (NumberFormatException ex) {
            throw new IOException("index corrupted", ex);
        }
        meta.setName(io.readString());
        meta.setPatchId(io.readString());
        meta.setTags(Tagset.parseTags(io.readString()));
        meta.setTitle(io.readString());
        meta.setVersion(io.readString());
        record.meta = meta;
        return record;
    }
    
    private void writeRecord(RecordIO io, PatchRecord record) throws IOException {
        io.writeString(record.patchfileName);
        PatchMetadata meta = record.meta;
        Collection<Path> paths = meta.getPaths().values();
        io.writeInteger(paths.size());
        for (Path path: paths) {
            io.writeString(path.getName());
            io.writeString(path.getPath());
        }
        io.writeString(meta.getAuthor(),"");
        io.writeString(meta.getComment(),"");
        io.writeString(meta.getDeviceId() == null ? "" : meta.getDeviceId().value());
        io.writeString(meta.getEnvironmentId() == null ? "" : meta.getEnvironmentId().value());
        io.writeString(meta.getLastModifiedDateString(),"");
        io.writeString(meta.getName(),"");
        io.writeString(meta.getPatchId(),"");
        io.writeString(meta.getTags().toString(),"");
        io.writeString(meta.getTitle(),"");
        io.writeString(meta.getVersion(),"");
    }
    
    private void rebuildIndex(RecordIO io, Collection<File> files, Collector<IndexedPatchRecord> collector) throws IOException {
        PatchRecord record = new PatchRecord();
        for (File file: files) {
            try {
                record.meta = new DefaultPatch(file).getMetadata();
            } catch (Exception ex) {
                continue;
            }
            record.patchfileName = file.getName();
            writeRecord(io, record);
            if (collector != null) {
                if (collector.collect(new IndexedPatchRecord(repositoryDir, record.patchfileName, record.meta)) == Collector.FINISHED) {
                    collector = null;
                }
            }
        }
    }
    
    private int readRecords(RecordIO io, Collector<IndexedPatchRecord> collector) throws IOException {
        int count = 0;
        for(;io.hasMore();count++) {
            IndexedPatchRecord patch = readRecord(io);
            if (collector != null && collector.collect(patch)==Collector.FINISHED) {
                collector = null;
            }
        }
        return count;
    }
}
 