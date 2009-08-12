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
package com.ruinwesen.patchmanager.swing.model;

import com.ruinwesen.patchmanager.client.index.IndexedPatch;

import java.util.Comparator;

public final class Order {

    private static final Comparator<IndexedPatch> DATE_A = new DateOrder();
    private static final Comparator<IndexedPatch> DATE_D = new ReverseOrder<IndexedPatch>(DATE_A); 
    private static final Comparator<IndexedPatch> SCORE_D = new PatchScoreOrder(); 
    private static final Comparator<IndexedPatch> TITLE_A = new PatchTitleOrder();
    private static final Comparator<IndexedPatch> AUTHOR_A = new AuthorOrder();  
    

    public static final Comparator<IndexedPatch> BY_DATE = 
        join(DATE_D, join(SCORE_D, join(TITLE_A, AUTHOR_A)));
    public static final Comparator<IndexedPatch> BY_RELEVANCE = 
        join(SCORE_D, join(TITLE_A, join(AUTHOR_A, DATE_D)));
    public static final Comparator<IndexedPatch> BY_AUTHOR = 
        join(AUTHOR_A, join(SCORE_D, join(TITLE_A, DATE_D)));
    public static final Comparator<IndexedPatch> BY_TITLE = 
        join(TITLE_A, join(SCORE_D, join(DATE_D, AUTHOR_A)));
    
    private static final Comparator<IndexedPatch> join(Comparator<IndexedPatch> a, 
            Comparator<IndexedPatch> b) {
        return new JoinedOrder<IndexedPatch>(a,b);
    }
    
}
