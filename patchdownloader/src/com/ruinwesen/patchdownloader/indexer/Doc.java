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

import java.util.Date;

import name.cs.csutils.CSUtils;

/**
 * An indexed document.
 * 
 * @author chresan
 */
public abstract class Doc {

    /**
     * Returns the document id.
     * @return the document id
     */
    public abstract int id();
    
    /**
     * Returns the date of this document.
     * If the document date is unavailable this method returns 0.
     * 
     * To get a Date object do:
     * Date dateObj = doc.date() == 0 ? null : new Date(doc.date());
     * 
     * @return the date of this document
     * @see Date#Date(long)
     */
    public abstract long date();
    
    /**
     * Returns the title of this document.
     * @return the title of this document
     */
    public abstract String title();
    
    /**
     * Returns the location this document is stored.
     * @return the location this document is stored
     */
    public abstract String path();
    
    /**
     * Returns the score of this document where 0&lt;=score&lt;=1 .
     * The score value can be used to measure the relevance of a document
     * regarding a specific search query. By default the score is 1.
     * @return the score of this document
     * @see ScoreDoc
     */
    public float score() {
        return 1.0f;
    }
    
    /**
     * Returns a string representation of the date.
     * @return a string representation of the date
     * @see #date()
     */
    public String dateToString() {
        return CSUtils.dateToString(new Date(date()));
    }

}
