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
package com.ruinwesen.patchdownloader.repository;

import java.util.ArrayList;

public abstract class StoredPatchCollector {

    public boolean takesMore() {
        return true;
    }

    public abstract void collect(Repository repository, StoredPatch patch);
    
    public static final class Collection extends StoredPatchCollector {

        private java.util.Collection<StoredPatch> collection;

        public Collection(java.util.Collection<StoredPatch> collection) {
            this.collection = collection;
        }

        public Collection() {
            this(new ArrayList<StoredPatch>(100));
        }
        
        public java.util.Collection<StoredPatch> getCollection() {
            return collection;
        }
        
        @Override
        public void collect(Repository repository, StoredPatch patch) {
            collection.add(patch);
        }
        
    }
    
    public static final class MaxCount extends StoredPatchCollector {
        private int countdown;
        private StoredPatchCollector collector;
        public MaxCount(int max, StoredPatchCollector collector) {
            if (max<0 ) {
                throw new IllegalArgumentException("max<0");
            }
            this.countdown = max;
            this.collector = collector;
        }
        
        @Override
        public boolean takesMore() {
            return countdown>0 && collector.takesMore();
        }

        @Override
        public void collect(Repository repository, StoredPatch patch) {
            if (takesMore()) {
                collector.collect(repository, patch);
            }
            countdown--;
        } 
    }
    
    public static final class Counter extends StoredPatchCollector {
        
        private int count = 0;
        
        public void setCount(int count) {
            this.count = count;
        }
        
        public int getCount() {
            return count;
        }
        
        @Override
        public void collect(Repository repository, StoredPatch patch) {
            count ++;
        }
    }
    
    
}
