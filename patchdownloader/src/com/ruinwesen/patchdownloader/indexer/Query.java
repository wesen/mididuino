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

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Query {

    private String queryString;
    private transient List<String> tokenizedList;
  //  private Query matchAllQuery; 
/*
    public Query(Query matchAll, String queryString) {
        this(queryString);
     //   this.matchAllQuery = flattteNmatchAll;
    } */
    
    public Query(String queryString) {
        this.queryString = queryString == null ? "" : queryString; 
    }

    private static final String norm(String key) {
        return key.trim().toLowerCase();
    }
    
    private List<String> __tokenize() {
        List<String> keys = new ArrayList<String>();
        for (String key: queryString.split("[\\s]+")) {
            key = norm(key);
            if (key.length()>0 && !keys.contains(key)) keys.add(key);
        }
        return keys;
    }
    
    public List<String> tokenize() {
        if (tokenizedList == null) {
            tokenizedList = __tokenize();
        }
        return Collections.unmodifiableList(tokenizedList);
    }
    
    @Override
    public boolean equals(Object o) {
        if (o == this) return true;
        if (o instanceof Query) {
            List<String> la = this.__tokenize(); 
            List<String> lb = ((Query)o).__tokenize();
            return la.containsAll(lb) && lb.containsAll(la);
        }
        return false;
    }

    public boolean isEmpty() {
        return __tokenize().isEmpty();
    }
    
    public String highlight(String text, String hlPrefix, String hlSuffix) {
        if (text.trim().length() == 0) {
            return text;
        }
        List<String> tokenLists = __tokenize();       
        if (tokenLists.isEmpty()) {
            return text;
        }
        
        String lowercaseText = text.toLowerCase(); 
        StringBuilder highlighted = null;
        int index = 0;
        while (index<text.length()) {
            String selectedToken = null;
            int nextIndex = Integer.MAX_VALUE;
            for (String token: tokenLists) {
                int idx = lowercaseText.indexOf(token, index);
                if (idx>=0 && idx<nextIndex && idx<text.length()) {
                    nextIndex = idx;
                    selectedToken = token;
                }
            }
            if (selectedToken == null) {
                break;
            }
            if (highlighted == null) {
                highlighted = new StringBuilder(text.length());
            }
            highlighted.append(text.substring(index, nextIndex));
            index = nextIndex;
            highlighted.append(hlPrefix);
            highlighted.append(text.substring(index, index+selectedToken.length()));
            highlighted.append(hlSuffix);
            index+=selectedToken.length();
        }
        if (highlighted != null) {
            if (index<text.length()) {
                highlighted.append(text.substring(index));
            }
            return highlighted.toString();
        }
        return text;
    }
    
    @Override
    public String toString() {
        return "Query[query="+queryString+"]";
    }
    
}
