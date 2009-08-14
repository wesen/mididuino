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
package com.ruinwesen.patchmanager.swing.components;

import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Highlighter implements Serializable {
    
    private static final long serialVersionUID = 4846683601790665829L;
    private String words;
    private transient List<String> tokenList;

    public Highlighter(String words) {
        this.words = words.trim();
        init();
    }

    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException {
        in.defaultReadObject();
        init();
    }
    
    private void init() {
        this.tokenList = createTokenList();
    }
    
    private List<String> createTokenList() {
        if (words.isEmpty()) {
            return Collections.emptyList();
        }
        // set ensures that list contains no duplicates
        Set<String> keySet = new HashSet<String>();
        for (String key: words.split("[\\s]+")) {
            key = norm(key);
            if (!key.isEmpty()) {
                keySet.add(key);
            }
        }
        return new ArrayList<String>(keySet);
    }
    
    private static final String norm(String key) {
        return key.trim().toLowerCase();
    }

    public String highlight(String text, String hlPrefix, String hlSuffix) {
        if (text.trim().isEmpty() || tokenList.isEmpty()) {
            return text;
        }
        
        String lowercaseText = text.toLowerCase(); 
        StringBuilder highlighted = null;
        int index = 0;
        while (index<text.length()) {
            String selectedToken = null;
            int nextIndex = Integer.MAX_VALUE;
            for (String token: tokenList) {
                int idx = lowercaseText.indexOf(token, index);
                if (idx>=0 && idx<nextIndex) {
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
            // append non-highlighted suffix
            highlighted.append(text.substring(index));
            return highlighted.toString();
        }
        return text;
    }
    
    
}
