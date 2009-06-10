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
package com.ruinwesen.patch;

import java.util.Date;
import java.util.Map;

public abstract class WrappedPatchMetadata implements PatchMetadata {

    protected abstract PatchMetadata getDelegate();
    
    @Override
    public void addAllTags(Tagset tag) {
        ensureIsModifieable();
        getDelegate().addAllTags(tag);
    }

    @Override
    public String getAuthor() {
        return getDelegate().getAuthor();
    }

    @Override
    public String getComment() {
        return getDelegate().getComment();
    }

    @Override
    public String getDeviceId() {
        return getDelegate().getDeviceId();
    }

    @Override
    public Date getLastModifiedDate() {
        return getDelegate().getLastModifiedDate();
    }

    @Override
    public String getMetadataFormatVersion() {
        return getDelegate().getMetadataFormatVersion();
    }

    @Override
    public String getName() {
        return getDelegate().getName();
    }

    @Override
    public String getPath(String name) {
        return getDelegate().getPath(name);
    }

    @Override
    public Map<String, String> getPaths() {
        return getDelegate().getPaths();
    }

    @Override
    public Tagset getTags() {
        return getDelegate().getTags();
    }

    @Override
    public String getTitle() {
        return getDelegate().getTitle();
    }

    @Override
    public void removeAllTags(Tagset tag) {
        ensureIsModifieable();
        getDelegate().removeAllTags(tag);
    }

    @Override
    public void removeTag(String tag) {
        ensureIsModifieable();
        getDelegate().removeTag(tag);
    }

    @Override
    public void setAuthor(String author) {
        ensureIsModifieable();
        getDelegate().setAuthor(author);
    }

    @Override
    public void setComment(String comment) {
        ensureIsModifieable();
        getDelegate().setComment(comment);
    }

    @Override
    public void setDeviceId(String deviceId) {
        ensureIsModifieable();
        getDelegate().setDeviceId(deviceId);
    }

    @Override
    public void setLastModifiedDate(Date date) {
        ensureIsModifieable();
        getDelegate().setLastModifiedDate(date);
    }

    @Override
    public void setMetadata(PatchMetadata metadata) {
        ensureIsModifieable();
        getDelegate().setMetadata(metadata);
    }

    @Override
    public void setName(String name) {
        ensureIsModifieable();
        getDelegate().setName(name);
    }

    @Override
    public void setPath(String name, String path) {
        ensureIsModifieable();
        getDelegate().setPath(name, path);
    }

    @Override
    public void setTags(Tagset tag) {
        ensureIsModifieable();
        getDelegate().setTags(tag);
    }

    @Override
    public void setTitle(String title) {
        ensureIsModifieable();
        getDelegate().setTitle(title);
    }

    protected void ensureIsModifieable() {
        // no op
    }
    
    @Override
    public void setPath(String path) {
        ensureIsModifieable();
        getDelegate().setPath(path);
    }
    
    public String getPath() {
        return getDelegate().getPath();
    }
    
}
