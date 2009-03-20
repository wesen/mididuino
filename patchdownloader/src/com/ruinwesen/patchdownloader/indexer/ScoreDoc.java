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

/**
 * An indexed document with a score value.
 * @author christian
 */
public class ScoreDoc extends Doc {

    /** the wrapped document */
    private Doc doc;
    
    /** the assigned score value */
    private float score;

    /**
     * Wrapps a new document and assignes the specified score to it.
     * @param doc the document 
     * @param score the score value
     */
    public ScoreDoc(Doc doc, float score) {
        this.doc = doc;
        this.score = score;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long date() {
        return doc.date();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int id() {
        return doc.id();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String title() {
        return doc.title();
    }

    /**
     * Returns the score assigned to the wrapped document where 0&lt;=score&lt;=1 .
     * The score value can be used to measure the relevance of a document
     * regarding a specific search query. 
     * @return the score assigned to the wrapped document
     * @see ScoreDoc
     */
    @Override
    public float score() {
        return score;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String path() {
        return doc.path();
    }
    
}
