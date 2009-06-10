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

import java.util.Collection;
import java.util.Map;
import java.util.HashMap;

import com.ruinwesen.patch.PatchMetadata;
import com.ruinwesen.patch.Tag;
import com.ruinwesen.patch.Tagset;

public class TagStats implements Collector {

    private Map<String, Integer> tagCounter = new HashMap<String, Integer>();
    
    public void reset() {
        tagCounter.clear();
    }
    
    public Tagset tags() {
        Collection<String> tags = tagCounter.keySet();
        return new Tagset(tags.toArray(new String[tags.size()]));
    }
    
    public TagStats intersection(Tagset tagset) {
        TagStats stats = new TagStats();
        stats.tagCounter = new HashMap<String, Integer>(this.tagCounter);
        for (Tag tag: tagset) {
            stats.tagCounter.remove(tag.normalizedValue());
        }
        return stats;
    }
    
    public int getCount(String tag) {
        return getCount(new Tag(tag));
    }
    
    public int getCount(Tag tag) {
        Integer count = tagCounter.get(tag.normalizedValue());
        return count == null ? 0 : count.intValue();
    }

    @Override
    public void collect(PatchMetadata data, float score) {
        if (score>0) {
            for (Tag tag: data.getTags()) {
                tagCounter.put(tag.normalizedValue(), getCount(tag)+1);
            }
        }
    }
    
}
