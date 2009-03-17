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
package com.ruinwesen.patchdownloader.patch;

import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import name.cs.csutils.CSUtils;

public class DefaultPatchMetadata implements PatchMetadata {

    private String title;
    private String name;
    private String author;
    private Date lastModifiedDate;
    private String comment;
    String metadataFormatVersion = "1.0";
    private Tagset tags = new Tagset();
    private Map<String,String> paths = new HashMap<String, String>();
    
    public DefaultPatchMetadata() {
        super();
        setLastModiedDate();
    }
    
    public DefaultPatchMetadata(PatchMetadata metadata) {
        setMetadata(metadata);
    }
    
    public void setLastModiedDate() {
        this.lastModifiedDate = CSUtils.now();
    }

    @Override
    public String getComment() {
        return comment;
    }

    @Override
    public void setComment(String comment) {
        this.comment = comment;
    }

    @Override
    public String getTitle() {
        return title;
    }

    @Override
    public void setTitle(String title) {
        this.title = title;
    }

    @Override
    public void setLastModifiedDate(Date date) {
        this.lastModifiedDate = date;
    }

    @Override
    public Date getLastModifiedDate() {
        return lastModifiedDate;
    }

    @Override
    public String getName() {
        return name;
    }
    @Override
    public void setName(String name) {
        this.name = name;
    }
    @Override
    public String getAuthor() {
        return author;
    }
    @Override
    public void setAuthor(String author) {
        this.author = author;
    }

    @Override
    public String getMetadataFormatVersion() {
        return metadataFormatVersion;
    }

    @Override
    public void setMetadata(PatchMetadata metadata) {
        if (metadata == this) {
            return;
        }
        setTitle(metadata.getTitle());
        setName(metadata.getName());
        setAuthor(metadata.getAuthor());
        setLastModifiedDate(metadata.getLastModifiedDate());
        setComment(metadata.getComment());
        setTags(metadata.getTags());
        this.paths = new HashMap<String, String>(metadata.getPaths());
        this.metadataFormatVersion = metadata.getMetadataFormatVersion();
    }

    @Override
    public Tagset getTags() {
        return new Tagset(tags);
    }

    @Override
    public void removeTag(String tag) {
        tags.remove(tag);
    }

    @Override
    public void addAllTags(Tagset tag) {
        tags.addAll(tag);
    }

    @Override
    public void removeAllTags(Tagset tag) {
        tags.removeAll(tag);
    }

    @Override
    public void setTags(Tagset tag) {
        this.tags = new Tagset(tag);
    }

    @Override
    public String getPath(String name) {
        if (name == null) {
            throw new IllegalArgumentException("name must not be null");
        }
        
        return paths.get(name);
    }

    @Override
    public void setPath(String name, String path) {
        if (path == null) {
            paths.remove(name);
        } else {
            paths.put(name, path);
        }   
    }

    @Override
    public Map<String, String> getPaths() {
        return Collections.unmodifiableMap(new HashMap<String,String>(paths));
    }

}
