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
package com.ruinwesen.patch.utils;

import java.io.File;
import java.io.IOException;
import java.util.Date;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;

import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.JarFileBuilder;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;

public class PatchMetadataModifier {

    private Patch sourcePatch;
    private PatchMetadata meta;
    private boolean preserveSourceCode = true;
    private boolean preserveMidifile = true;
    private boolean preserveOtherfiles = true;
    
    public PatchMetadataModifier() {
        super();
    }

    public void setPreserveSourceCodeEnabled(boolean value) {
        this.preserveSourceCode = value;
    }

    public void setPreserveMidiFileEnabled(boolean value) {
        this.preserveMidifile = value;
    }

    public void setPreserveOtherFilesEnabled(boolean value) {
        this.preserveOtherfiles = value;
    }

    public boolean isPreserveSourceCodeEnabled() {
        return this.preserveSourceCode;
    }

    public boolean isPreserveMidiFileEnabled() {
        return this.preserveMidifile;
    }

    public boolean isPreserveOtherFilesEnabled() {
        return this.preserveOtherfiles;
    }
    
    public PatchMetadataModifier(Patch patch) throws IOException, ParserConfigurationException, SAXException {
        this();
        loadPatch(patch);
    }
    
    public void loadPatch(Patch patch) throws IOException, ParserConfigurationException, SAXException {
        this.sourcePatch = patch;
        this.meta = null; // clear old metadata
        this.meta = sourcePatch.getMetadata(); // get original metadata  
    }

    public void writePatchFile(File file) throws IllegalStateException, Exception {
        if (sourcePatch == null) {
            throw new IllegalStateException("no source patch selected");
        }
        
        Directory srcDir = sourcePatch.openDirectory();
        JarFileBuilder jfb = new JarFileBuilder(file);
        
      //  jfb.add(srcDir, )
        
 //       srcDir.entryEnumeration()    
    }
    
 //   private static final FilterMetadataEnumeration
    
    
    
    private boolean isMetadataAvailable() {
        return meta != null;
    }

    public String getAuthor() {
        return isMetadataAvailable() ? meta.getAuthor() : null;
    }
    
    public void setAuthor(String value) {
        if (isMetadataAvailable()) {
            meta.setAuthor(value);
        }
    }

    public String getName() {
        return isMetadataAvailable() ? meta.getName() : null;
    }
    
    public void setName(String value) {
        if (isMetadataAvailable()) {
            meta.setName(value);
        }
    }

    public String getTitle() {
        return isMetadataAvailable() ? meta.getTitle() : null;
    }
    
    public void setTitle(String value) {
        if (isMetadataAvailable()) {
            meta.setTitle(value);
        }
    }

    public DeviceId getDeviceId() {
        return isMetadataAvailable() ? meta.getDeviceId() : null;
    }
    
    public void setDeviceId(DeviceId value) {
        if (isMetadataAvailable()) {
            meta.setDeviceId(value);
        }
    }

    public EnvironmentId getEnvironmentId() {
        return isMetadataAvailable() ? meta.getEnvironmentId() : null;
    }
    
    public void setEnvironmentId(EnvironmentId value) {
        if (isMetadataAvailable()) {
            meta.setEnvironmentId(value);
        }
    }

    public String getComment() {
        return isMetadataAvailable() ? meta.getComment() : null;
    }
    
    public void setComment(String value) {
        if (isMetadataAvailable()) {
            meta.setComment(value);
        }
    }

    public Tagset getTags() {
        return isMetadataAvailable() ? meta.getTags() : null;
    }
    
    public void setTags(Tagset value) {
        if (isMetadataAvailable()) {
            meta.setTags(value);
        }
    }

    public Date getLastModifiedDate() {
        return isMetadataAvailable() ? meta.getLastModifiedDate() : null;
    }
    
    public void setLastModifiedDate(Date value) {
        if (isMetadataAvailable()) {
            meta.setLastModifiedDate(value);
        }
    }
    
    public String getPatchId() {
        return isMetadataAvailable() ? meta.getPatchId() : null;
    }
    
    public void setPatchId(String value) {
        if (isMetadataAvailable()) {
            meta.setPatchId(value);
        }
    }


}
