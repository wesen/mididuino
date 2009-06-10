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

import com.ruinwesen.patch.Tagset;

public class Query {

    private String authorName;
    private Tagset expectedTags = new Tagset();
    private String textsearch;
    private float minScoreThreshold = 0.5f;
    
   // private Date minDate;
    //private Date maxDate;
    
    public Query() {
        super();
    }
/*
    public void setMinimumDate(Date date) {
        this.minDate = date;
    }

    public void setMaximumDate(Date date) {
        this.maxDate = date;
    }
    
    public void setDate(Date date) {
        this.minDate = date;
        this.maxDate = date;
    }
  */  
    public static Query parse(String queryString) {
        Query q = new Query(); 
        q.setTextSearch(queryString);
        return q;
    }
    
    public void setTextSearch(String textsearch) {
        this.textsearch = textsearch;
    }
    
    public void setAuthor(String name) {
        this.authorName = name;
    }

    public Tagset expectedTags() {
        return expectedTags;
    }

    public void setMinScoreThreshold(float minscore) {
        if (minscore<0 || minscore>1) {
            throw new IllegalArgumentException("minscore:"+minscore);
        }
        this.minScoreThreshold = minscore;
    }
    
    public Scorer createScorer() {
        Scorer scorer = Scorer.ONE;
        if (authorName != null) {
            scorer = Scorer.logicAnd(scorer, Scorer.authorIs(authorName));
        }
        if (!expectedTags.isEmpty()) {
            scorer = Scorer.logicAnd(scorer, Scorer.containsAllTags(expectedTags));
        }
        if (textsearch != null) {
            scorer = Scorer.logicAnd(scorer, Scorer.fullTextSearch(textsearch));
            // the more matched tags the higher the score 
            scorer = Scorer.weight(scorer, Scorer.maxTagMatch(textsearch));
        }
        if (minScoreThreshold != 0f) {
            scorer = Scorer.minThreshold(minScoreThreshold, scorer);
        }
        /*
        if (minDate != null) {
            //scorer = Scorer.logicAnd(scorer, Scorer.lastmodifiedDate(date, type))
        }*/
        return scorer;
    }

    public Highlighter createHighlighter() {
        StringBuilder text = new StringBuilder();
        if (authorName != null)
            text.append(authorName).append(' ');
        if (textsearch != null) 
            text.append(textsearch).append(' ');
        return new Highlighter(text.toString());
    }
    
}
