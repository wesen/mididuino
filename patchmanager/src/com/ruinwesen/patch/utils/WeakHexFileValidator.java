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

import java.io.FileNotFoundException;
import java.io.IOException;

public class WeakHexFileValidator extends Validator {

    private static final int STATE_ERROR = -2;
    private static final int STATE_VALIDATED = -1;
    private static final int STATE_START = 0;
    private static final int STATE_WHITESPACE = 0;
    private static final int STATE_AFTER_COLON = 1;
    private static final int STATE_HEXDIGITS = 2;
    
    
    private int state = STATE_START;
    private boolean failOnError;

    public WeakHexFileValidator() throws FileNotFoundException {
        this(true);
    }

    public WeakHexFileValidator(boolean failOnError) throws FileNotFoundException {
        this.failOnError = failOnError;
    }

    @Override
    public void validate() throws IOException {
        if (!isValid()) {
            throw new IOException("Not a valid intel HEX file (illegal state:"+state+")");
        }
    }

    @Override
    public boolean isValid() {
        return state == STATE_VALIDATED;
    }

    private void enterErrorState() throws IOException {
        state = STATE_ERROR;
        if (failOnError) {
            throw new IOException("not a intel hex file");
        }
    }
    
    private void validateCharacter(int c) throws IOException {
        switch (c) {
        case -1: {
            if (state == STATE_WHITESPACE || state == STATE_HEXDIGITS) {
                state = STATE_VALIDATED;
            } else {
                enterErrorState();
            } 
            break;
        }
        case ':': {
            if (state != STATE_WHITESPACE && state != STATE_START) {
                enterErrorState();
            } else {
                state = STATE_AFTER_COLON;
            }
            break;
        }
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
        case '0':case '1':case '2':case '3':case '4':case '5':
        case '6':case '7':case '8':case '9': {
            if (state == STATE_AFTER_COLON) {
                state = STATE_HEXDIGITS;
            } else if (state != STATE_HEXDIGITS) {
                enterErrorState();
                return;
            }
            break;
        }
        default: {
            if (Character.isWhitespace((char)c)) {
                if (state == STATE_HEXDIGITS) {
                    state = STATE_WHITESPACE;
                } else if (state != STATE_WHITESPACE) {
                    enterErrorState();
                }
            } else {
                enterErrorState();
                return;
            }
            break;
        }
        }
    }

    @Override
    protected boolean shouldFinish() {
        return state<0;
    }

    @Override
    public void update(int c) throws IOException {
        validateCharacter(c);
    }
    
}
