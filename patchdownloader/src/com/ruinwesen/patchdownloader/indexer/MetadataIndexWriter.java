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

import java.io.IOException;
import java.io.OutputStream;
import java.util.Date;

import name.cs.csutils.CSUtils;

import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.Tag;

public class MetadataIndexWriter extends IndexWriter {

    public MetadataIndexWriter(OutputStream outDocumentIndex,
            OutputStream outKeyIndex) {
        super(outDocumentIndex, outKeyIndex);
    }
    
    public void putMetadata(int docid, PatchMetadata metadata) throws IOException {
        Date date = metadata.getLastModifiedDate();
        if (date != null)
            putKey(docid, "date:"+CSUtils.dateToString(date));

        String title = metadata.getTitle();
        if (title != null) {
            title = normalizeTitle(title);
            if (!title.isEmpty()) {
                putKey(docid, "title:"+title);
            }
        }
        
        for (Tag tag: metadata.getTags()) {
            putKey(docid, tag.normalizedValue());
        }
    }

    private String normalizeTitle(final String str) {
        return str.trim().replaceAll("[\\s\\n\\r\\t]+", " "); 
    }

}
