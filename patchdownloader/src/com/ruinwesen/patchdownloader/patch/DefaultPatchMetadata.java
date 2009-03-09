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
