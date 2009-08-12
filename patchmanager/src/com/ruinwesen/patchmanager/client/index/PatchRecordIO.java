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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Collection;

import name.cs.csutils.CSUtils;
import name.cs.csutils.collector.Collector;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.metadata.Tagset;

public final class PatchRecordIO {

    private StringBuilder sb = new StringBuilder();
    
    public PatchRecordIO() {
        super();
    }

    public void rebuildIndex(File indexDir, PrintWriter os, Collection<File> files, Collector<IndexedPatchRecord> collector) throws IOException {
        PatchRecord record = new PatchRecord();
        for (File file: files) {
            record.patchfileName = file.getName();
            try {
            DefaultPatch patch = new DefaultPatch(file);
            record.meta = patch.getMetadata();
            } catch (Exception ex) {
                continue;
            }
            write(record, os);
            if (collector != null) {
                if (collector.collect(new IndexedPatchRecord(indexDir, record.patchfileName, record.meta)) == Collector.FINISHED) {
                    collector = null;
                }
            }
        }
    }
    
    public void writeIndex(PrintWriter os, Collection<? extends PatchRecord> data) throws IOException {
        for (PatchRecord record: data) {
            write(record, os);
        }
    }
    
    public int readIndex(File indexDir, File file, Collector<IndexedPatchRecord> collector) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(file));
        int count=0;
        try {
            count = readIndex(indexDir, reader, collector);
        } finally {
            reader.close();
        }
        return count;
    }

    public int readIndex(File indexDir, BufferedReader reader, Collector<IndexedPatchRecord> collector) throws IOException {
        for(int count=0;;) {
            IndexedPatchRecord patch = new IndexedPatchRecord(indexDir);
            if (!read(patch, reader)) {
                return count;
            }
            if (collector != null && collector.collect(patch)==Collector.FINISHED) {
                collector = null;
            }
        }
    }
    
    private String escape(String str) {
        if (str.indexOf('\n')<0 && str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                sb.append("\\\\");
            } else if (ch == '\n') {
                sb.append("\\n");
            } else {
                sb.append(ch);
            }
        }
        return sb.toString();
    }
    
    private String unescape(String str) {
        if (str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                if (i+1<str.length()) {
                    if (str.charAt(i+1)=='n') {
                        sb.append('\n');
                        continue;
                    } else if (str.charAt(i+1)=='\\') {
                        sb.append('\\');
                        continue;
                    }
                }
            }
            sb.append(ch);
        }
        return sb.toString();
    }
    
    private final String getLine(BufferedReader reader) throws IOException {
        String line = reader.readLine();
        if (line == null) {
            throw new IOException("incomplete record");
        }
        return unescape(line);
    }

    private final int getInteger(BufferedReader reader) throws IOException {
        try {
            return Integer.parseInt(getLine(reader));
        } catch (NumberFormatException ex) {
            throw new IOException("corrupted index", ex);
        }
    }
    
    public boolean read(PatchRecord dst, BufferedReader reader) throws IOException {
        dst.patchfileName = reader.readLine();
        if (dst.patchfileName == null) { // last line
            return false;
        }
        
        int pathcount = getInteger(reader);
        PatchMetadata meta = new DefaultPatchMetadata();
        for (int i=0;i<pathcount;i++) {
            String name = getLine(reader);
            String path = getLine(reader);
            meta.addPath(new Path(name, path));
        }
        meta.setAuthor(getLine(reader));
        meta.setComment(getLine(reader));
        String l = getLine(reader);
        meta.setDeviceId(l.isEmpty() ? null : PatchMetadataIDInfo.getDeviceId(l));
        l = getLine(reader);
        meta.setEnvironmentId(l.isEmpty() ? null:PatchMetadataIDInfo.getEnvironmentId(l));
        try {
            meta.setLastModifiedDate(CSUtils.parseDate(getLine(reader)));
        } catch (NumberFormatException ex) {
            throw new IOException("corrupted index");
        }
        meta.setName(getLine(reader));
        meta.setPatchId(getLine(reader));
        meta.setTags(Tagset.parseTags(getLine(reader)));
        meta.setTitle(getLine(reader));
        meta.setVersion(getLine(reader));
        dst.meta = meta;
        return true;
    }
    
    public void write(PatchRecord record, PrintWriter w) throws IOException {
        writeLine(w, record.patchfileName);
        PatchMetadata meta = record.meta;
        Collection<Path> paths = meta.getPaths().values();
        writeLine(w, Integer.toString(paths.size()));
        for (Path path: paths) {
            writeLine(w, path.getName());
            writeLine(w, path.getPath());
        }
        writeLine(w, meta.getAuthor());
        writeLine(w, meta.getComment());
        writeLine(w, meta.getDeviceId());
        writeLine(w, meta.getEnvironmentId());
        writeLine(w, meta.getLastModifiedDateString());
        writeLine(w, meta.getName());
        writeLine(w, meta.getPatchId());
        writeLine(w, meta.getTags().toString());
        writeLine(w, meta.getTitle());
        writeLine(w, meta.getVersion());
    }
    
    private void writeLine(PrintWriter writer, Object str) throws IOException {
        writer.println(str == null ? "" : escape(String.valueOf(str)));
    }
    
}
