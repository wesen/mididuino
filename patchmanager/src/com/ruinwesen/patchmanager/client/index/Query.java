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

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;

public class Query implements ScoreBuilder<Patch> {

    private String sinceDate;
    private String[] token;
    private String expectedAuthor;
    private Set<DeviceId> expectedDeviceIds;
    private Set<EnvironmentId> expectedEnvIds;
    private Tagset expectedTags;
    private Tagset forbiddenTags;
    private float minScore;
    
    public Query(Query q) {
        expectedDeviceIds = q.expectedDeviceIds == null ? null : new HashSet<DeviceId>(q.expectedDeviceIds);
        expectedEnvIds = q.expectedEnvIds == null ? null : new HashSet<EnvironmentId>(q.expectedEnvIds);
        expectedAuthor = q.expectedAuthor;
        token = q.token;
        sinceDate = q.sinceDate;
        expectedTags = q.expectedTags == null ? null : new Tagset(q.expectedTags);
        forbiddenTags = q.forbiddenTags == null ? null : new Tagset(q.forbiddenTags);
        minScore = q.minScore;
    }
    
    public Query(String sinceDate, String queryString, float minScore) {
        this.minScore = minScore;
        this.sinceDate = sinceDate;
        
        if (queryString != null && queryString.trim().length()!=0) {
        Set<String> tokenList = new HashSet<String>();
            for (String str: queryString.split("[\\s]+")) {
                if (str.length()!=0) {
                    tokenList.add(str);
                }
            }
            this.token = tokenList.toArray(new String[tokenList.size()]);
        } else {
            this.token = new String[0];
        }
    }
    
    public float minScore() {
        return minScore;
    }
    
    public void setExpectedDeviceIds(DeviceId ... idlist) {
        setExpectedDeviceIds(idlist.length==0?null:Arrays.asList(idlist));
    }

    public void setExpectedEnvironmentIds(EnvironmentId ... idlist) {
        setExpectedEnvironmentIds(idlist.length==0?null:Arrays.asList(idlist));
    }

    public void setExpectedDeviceIds(Collection<DeviceId> c) {
        this.expectedDeviceIds = c == null ? null : new HashSet<DeviceId>(c);
    }

    public void setExpectedEnvironmentIds(Collection<EnvironmentId> c) {
        this.expectedEnvIds = c == null ? null : new HashSet<EnvironmentId>(c);
    }
    
    public void setExpectedAuthor(String value) {
        this.expectedAuthor = value;
    }
    
    public void setExpectedTags(Tagset value) {
        this.expectedTags = value;
    }
    
    public void setForbiddenTags(Tagset value) {
        this.forbiddenTags = value;
    }
    

    public float score(Patch item) {
        PatchMetadata meta;
        try { 
            meta = item.getMetadata();
        } catch (Exception ex) {
            return 0f;
        }

        // if before sinceDate return score:0
        if (sinceDate != null && (sinceDate.compareTo(meta.getLastModifiedDateString())>0)) {
            return 0f;
        }

        String author = nonNullString(meta.getAuthor());
        if (expectedAuthor != null && !expectedAuthor.equalsIgnoreCase(author)) {
            return 0f;
        }
        DeviceId devId = meta.getDeviceId(); // may be null
        if (expectedDeviceIds != null && !expectedDeviceIds.contains(devId)) {
            return 0f;
        }
        EnvironmentId envId = meta.getEnvironmentId(); // may be null
        if (expectedEnvIds != null && !expectedEnvIds.contains(envId)) {
            return 0f;
        }
        Tagset tags = meta.getTags();
        if (expectedTags != null && !tags.containsAll(expectedTags)) {
            return 0f;
        }
        if (forbiddenTags != null && tags.containsSomeOf(forbiddenTags)) {
            return 0f;
        }
        
        if (token.length == 0) {
            return 1f;
        }
        
        int count = 0;
        count += countToken(meta.getComment());
        count += countToken(meta.getTitle());
        count += countToken(meta.getName());
        count += countToken(meta.getAuthor());
        count += countToken(tags.toString());
        for (String t: token) {
            if (tags.contains(t)) {
                count++;
            }
        }
        
        float score = count == 0 ? 0f : Math.min(1f, count/(float)token.length);
        return score;
    }
    
    private final int countToken(String str) {
        if (str == null) {
            return 0;
        }
        int count = 0;
        for (String t: this.token) {
            if (str.indexOf(t)>=0) {
                count++;
            }
        }
        return count;
    }
    
    private static final String nonNullString(Object obj) {
        return obj == null ? "" : String.valueOf(obj);
    }

}
