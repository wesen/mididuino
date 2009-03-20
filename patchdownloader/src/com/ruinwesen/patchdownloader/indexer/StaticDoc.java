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
 * A concrete indexed document.
 * @author chresan
 */
public class StaticDoc extends Doc {

    /** the document id */
    int id;
    /** the date */
    long date;
    /** the path where the document is stored */
    String path;
    /** the document title */
    String title;

    /**
     * Creates a new document.
     * @param id the document id
     * @param date the date 
     * @param path the path where the document is stored
     * @param title the document title
     */
    public StaticDoc(int id, long date, String path, String title) {
        this.id = id;
        this.date = date;
        this.path = path;
        this.title = title;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public long date() {
        return date;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int id() {
        return id;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String path() {
        return path;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String title() {
        return title;
    }

}
