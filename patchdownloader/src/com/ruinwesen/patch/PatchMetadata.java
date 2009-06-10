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

public interface PatchMetadata {

    public static final String PATH_SOURCECODE = "sourcecode";
    public static final String PATH_MIDIFILE = "midifile";

    String getMetadataFormatVersion();
    String getComment();
    void setComment(String comment);
    String getDeviceId();
    void setDeviceId(String deviceId);
    String getTitle();
    void setTitle(String title);
    void setLastModifiedDate(Date date);
    Date getLastModifiedDate();
    String getName();
    void setName(String name);
    String getAuthor();
    void setAuthor(String author);
    void setMetadata(PatchMetadata metadata);
    Tagset getTags();
    void setTags(Tagset tag);
    void removeTag(String tag);
    void removeAllTags(Tagset tag);
    void addAllTags(Tagset tag);

    String getPath(String name);
    void setPath(String name, String path);
    
    Map<String,String> getPaths();
    
    // path to the patch file (set at runtime)
    String getPath();
    void setPath(String path);
}
