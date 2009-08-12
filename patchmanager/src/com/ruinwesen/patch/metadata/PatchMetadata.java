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
package com.ruinwesen.patch.metadata;
import java.util.Collection;
import java.util.Date;
import java.util.Map;

import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;

public interface PatchMetadata {

    public static final String VERSION_1_0 = "1.0";
    public static final String NS_PATCH_METDATA = "http://ruinwesen.com/ns/patchmetadata";
    public static final String FILENAME = "metadata.xml";
    public static final String CATEGORY_TAG_PREFIX = "category:";
    public static final String DEFAULT_MIDIFILE_PATH_NAME = "midifile";
    public static final String DEFAULT_SOURCE_PATH_NAME = "source";

    DeviceId getDeviceId();

    PatchMetadata copy();
    
    void setDeviceId(DeviceId value);

    EnvironmentId getEnvironmentId();

    void setEnvironmentId(EnvironmentId value);
    
    String getVersion();

    void setVersion(String value);
    
    String getTitle();

    void setTitle(String value);
    
    String getName();

    void setName(String value);
    
    String getAuthor();

    void setAuthor(String value);
    
    String getComment();

    void setComment(String value);
    
    Path getPath(String name);
    
    void setPath(String name, Path value);
    
    void removePath(String name);

    Map<String, Path> getPaths();
    
    void setAll(PatchMetadata src);

    void setPaths(Map<String, Path> paths);

    void addPath(Path path);
    
    Tagset getTags();
    
    void setTags(Tagset set);
    
    void addTag(Tag tag);
    
    void addTag(String tagname);
 
    public String getLastModifiedDateString();
    
    Date getLastModifiedDate();

    void setLastModifiedDate(Date d);

    void setPaths(Collection<Path> paths);

    void setPatchId(String patchId);
    
    String getPatchId();
    
}
