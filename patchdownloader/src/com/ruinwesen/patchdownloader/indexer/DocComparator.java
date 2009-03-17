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

import java.util.Comparator;

public abstract class DocComparator<T extends Doc> implements Comparator<Doc> {

    private static int cmpString(String a, String b) {
        if (a == null) {
            if (b == null) return 0;
            else return +1;
        } else {
            if (b==null) return -1;
            else return a.compareTo(b);
        }
    }
    
    private static int cmpId(int a, int b) {
        if (a<b) return -1;
        if (a>b) return +1;
        return 0;
    }
    
    public static final class ByDate extends DocComparator<Doc> {

        @Override
        public int compare(Doc a, Doc b) {
            if (a.date()>b.date()) return -1;
            if (a.date()<b.date()) return +1;
            
            if (a.score()>b.score()) return -1;
            if (a.score()<b.score()) return +1;

            int cmpTitle = DocComparator.cmpString(a.title(), b.title());
            if (cmpTitle != 0) return cmpTitle;
            
            return DocComparator.cmpId(a.id(), b.id());
        }
        
    }

    public static final class ByRelevance extends DocComparator<Doc> {

        @Override
        public int compare(Doc a, Doc b) {
            if (a.score()>b.score()) return -1;
            if (a.score()<b.score()) return +1;

            int cmpTitle = DocComparator.cmpString(a.title(), b.title());
            if (cmpTitle != 0) return cmpTitle;
            
            if (a.date()>b.date()) return -1;
            if (a.date()<b.date()) return +1;
            
            return DocComparator.cmpId(a.id(), b.id());
        }
        
    }

}
