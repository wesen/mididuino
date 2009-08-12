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
package com.ruinwesen.patch.utils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

public abstract class Validator {

    public abstract boolean isValid();
    
    public abstract void validate() throws IOException;
    
    public final void update(File file) throws IOException {
        InputStream is = new BufferedInputStream(new FileInputStream(file));
        try  {
            update(is);
        } finally {
            is.close();
        }
    }
    
    public final void update(InputStream in) throws IOException {
        int c;
        while ((c=in.read())!=-1 && !shouldFinish()) {
            update(c);
        }
        if (!shouldFinish()) {
            update(-1); // EOF character
        }
    }
    
    protected abstract boolean shouldFinish();

    public final void update(CharSequence seq) throws IOException {
        int len = seq.length();
        for (int i=0;i<len;i++)
            update((int)seq.charAt(i));
    }

    public final void update(byte[] data) throws IOException {
        for (int i=0;i<data.length;i++)
            update(data[i]);
        update(-1);
    }
   
    public final void update(char c) throws IOException {
        update((int)c);
    }
    
    public final void update(byte b) throws IOException {
        update(b&0xFf);
    }

    public abstract void update(int c) throws IOException;
    
    public void reset() {
        // no op
    }
    
}
