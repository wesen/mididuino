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

import com.ruinwesen.patch.PatchMetadata;

import java.io.Serializable;
import java.util.Comparator;
import java.util.Date;

public final class Order {

    private static final Comparator<PatchMetadata> DATE_A = new DateOrder();
    private static final Comparator<PatchMetadata> DATE_D = reverse(DATE_A); 
    private static final Comparator<PatchMetadata> SCORE_D = new ScoreOrder(); 
    private static final Comparator<PatchMetadata> TITLE_A = new TitleOrder();
    private static final Comparator<PatchMetadata> AUTHOR_A = new AuthorOrder();  
    

    public static final Comparator<PatchMetadata> BY_DATE = 
        combine(DATE_D, combine(SCORE_D, combine(TITLE_A, AUTHOR_A)));
    public static final Comparator<PatchMetadata> BY_RELEVANCE = 
        combine(SCORE_D, combine(TITLE_A, combine(AUTHOR_A, DATE_D)));
    public static final Comparator<PatchMetadata> BY_AUTHOR = 
        combine(AUTHOR_A, combine(SCORE_D, combine(TITLE_A, DATE_D)));
    public static final Comparator<PatchMetadata> BY_TITLE = 
        combine(TITLE_A, combine(SCORE_D, combine(DATE_D, AUTHOR_A)));
    
    private Order() {
        super();
    }
    
    public static <E> Comparator<E> combine(Comparator<E> a, Comparator<E> b) {
        return new CombinedOrder<E>(a,b);
    }
    
    public static <E> Comparator<E> reverse(Comparator<E> a) {
        return new ReverseOrder<E>(a);
    }
    
    private static class CombinedOrder<E> implements Comparator<E> {
        private Comparator<E> comparatora;
        private Comparator<E> comparatorb;
        public CombinedOrder(Comparator<E> a, Comparator<E> b) {
            if (a == null || b == null) 
                throw new IllegalArgumentException("a:"+a+",b:"+b);
            this.comparatora = a;
            this.comparatorb = b;
        }
        @Override
        public int compare(E a, E b) {
            int cmp = comparatora.compare(a, b);
            if (cmp == 0) 
                cmp = comparatorb.compare(a, b);
            return cmp;
        }
    }
    
    private static int nullOrder(Object a, Object b) {
        if (a == null && b == null) {
            return 0;
        }
        if (a == null) return +1;
        if (b == null) return -1;
        return 0;
    }
    
    private static class TitleOrder implements Comparator<PatchMetadata> {
        @Override
        public int compare(PatchMetadata a, PatchMetadata b) {
            String da = a.getTitle();
            String db = b.getTitle();
            int cmp = nullOrder(da, db);
            if (cmp == 0)
                cmp = da.compareToIgnoreCase(db);
            return cmp;
        }
    }

    private static class ScoreOrder implements Comparator<PatchMetadata> {
        
        private float scoreOf(PatchMetadata data) {
            return (data instanceof ScoredPatchMetadata) 
            ? ((ScoredPatchMetadata)data).score()
            : 1f;
        }
        @Override
        public int compare(PatchMetadata a, PatchMetadata b) {
            return (int)Math.signum(scoreOf(b)-scoreOf(a));
        }
    }
    
    private static class AuthorOrder implements Comparator<PatchMetadata> {
        @Override
        public int compare(PatchMetadata a, PatchMetadata b) {
            String da = a.getAuthor();
            String db = b.getAuthor();
            if (da == null && db == null) {
                return 0;
            }
            if (da == null) return +1;
            if (db == null) return -1;
            return da.compareToIgnoreCase(db);
        }
    }
    
    private static class DateOrder implements Comparator<PatchMetadata> {
        @Override
        public int compare(PatchMetadata a, PatchMetadata b) {
            Date da = a.getLastModifiedDate();
            Date db = b.getLastModifiedDate();
            if (da == null && db == null) {
                return 0;
            }
            if (da == null) return +1;
            if (db == null) return -1;
            return da.compareTo(db);
        }
    }
    
    private static class ReverseOrder<E> implements Comparator<E>, Serializable {
        /**
         * 
         */
        private static final long serialVersionUID = -2892143950487012576L;
        private Comparator<E> comparator;
        public ReverseOrder(Comparator<E> comparator) {
            this.comparator = comparator;
        }
        @Override
        public int compare(E a, E b) {
            return -Integer.signum(comparator.compare(a, b));
        }
    }
    
}
