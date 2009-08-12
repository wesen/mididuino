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

import java.io.File;
import java.io.IOException;
import java.nio.ByteOrder;

/**
 * State machine 
 * @author Christian Schneider
 */
public final class HexFileValidator extends Validator {

    public static void main(String[] args) throws IOException {
        File file = new File("/home/christian/test/emptySketch.hex");
        new HexFileValidator().update(file);
        System.out.println("valid");
    }
    
    private static final int STATE_START = 0;
    private static final int STATE_VALID = -1;
    private static final int STATE_ERROR = -2;
    private static final int STATE_AFTER_HEX_DIGIT = 1;
    private static final int STATE_AFTER_COLON = 2;
    private static final int STATE_AFTER_WHITESPACE = 3;
    private int state;
    private int value;
    private int nibblecount;
    private ByteOrder byteorder;

    // record
    private int previouse_record_type;
    private int record_bytecount;
    private int record_address;
    private int record_type;
    private int record_checksum;
    private int record_computed_checksum;
    private int record_computed_checksum_tmp;
    private int record_count = 0;
    
    private static final int NIBBLECOUNT_BYTE_COUNT_END = 2;
    private static final int NIBBLECOUNT_ADDRESS_END = NIBBLECOUNT_BYTE_COUNT_END+4;
    private static final int NIBBLECOUNT_RECORD_TYPE_END = NIBBLECOUNT_ADDRESS_END+2;
    private static final int NIBBLECOUNT_DATA_BEGIN = NIBBLECOUNT_RECORD_TYPE_END;

    public static final int RECORD_TYPE_UNDEFINED =-1;
    public static final int RECORD_TYPE_DATA_RECORD = 0x00;
    public static final int RECORD_TYPE_EOF = 0x01;
    public static final int RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS = 0x02;
    public static final int RECORD_TYPE_START_SEGMENT_ADDRESS = 0x03;
    public static final int RECORD_TYPE_EXTENDED_LINEAR_ADDRESS = 0x04;
    public static final int RECORD_TYPE_START_LINEAR_ADDRESS = 0x05;
    
    private String error;
    private boolean failOnError;

    public HexFileValidator() {
        this(true);
    }
    
    public HexFileValidator(boolean failOnError) {
        this(failOnError, ByteOrder.BIG_ENDIAN);
    }
    
    public HexFileValidator(boolean failOnError, ByteOrder byteorder) {
        this.failOnError = failOnError;
        setByteOrder(byteorder);
        restart();
    }

    @Override
    public void validate() throws IOException {
        if (!isValid()) {
            throw new IOException("Not a valid intel HEX file (illegal state:"+state+")");
        }
    }
    
    @Override
    public boolean isValid() {
        return state == STATE_VALID;
    }
    
    public void setByteOrder(ByteOrder byteorder) {
        if (byteorder == null) {
            throw new IllegalArgumentException("byteorder:"+byteorder);
        }
        this.byteorder = byteorder;
    }
    
    public ByteOrder getByteOrder() {
        return byteorder;
    }
    
    public void restart() {
        state = STATE_START;
        value = 0;
        previouse_record_type = -1;
        record_count = 0;
    }
    
    private final void appendNibble(int n) {
        value = (value<<4)|n;
        nibblecount++;
    }

    private final void byteSwapValue16() {
        if (byteorder == ByteOrder.LITTLE_ENDIAN) {
            value = ((value&0xff)<<8)|((value>>8)&0xFf);
        }
    }

    private final void byteSwapValue32() {
        if (byteorder == ByteOrder.LITTLE_ENDIAN) {
            value = 
                 ((value<<24)&0xFf000000)
                |((value<<8)&0x00Ff0000)
                |((value>>8)&0x0000Ff00)
                |((value>>24)&0x000000Ff);
        }
    }
    
    @Override
    public void update(int c) throws IOException {
        switch (c) {
        case -1: {
            handleEOFCharacter();
            break;
        }
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f': {
            updateNibble(c-'a'+10);
            break;
        }
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F': {
            updateNibble(c-'A'+10);
            break;
        }
        case '0':case '1':case '2':case '3':case '4':case '5':
        case '6':case '7':case '8':case '9': {
            updateNibble(c-'0');
            break;
        }
        case ':': {
            beginRecord();
            break;
        }
        case ' ':case '\t':case '\r':case '\n':case '\f': {
            handleWhitespace((char)c);
            break;
        }   
        default: {
            if (Character.isWhitespace(c)) {
                handleWhitespace((char)c);
            } else {
                handleError();
            }
            break;
        }
        }
    }

    private void updateNibble(int nibble) throws IOException {
        if (state == STATE_AFTER_COLON) {
            nibblecount = 0;
            state = STATE_AFTER_HEX_DIGIT;
        } else if (state != STATE_AFTER_HEX_DIGIT) {
            handleError(); // error
            return;
        }
        appendNibble(nibble);
        
        if (nibblecount>=2 && (nibblecount%2==0)) { 
            // make sure we ignore the last byte (the checksum byte)
            record_computed_checksum = record_computed_checksum_tmp;
            record_computed_checksum_tmp = (record_computed_checksum_tmp+(value&0xff))&0xFf;
            
            switch (nibblecount) {
            case NIBBLECOUNT_ADDRESS_END: {
                byteSwapValue16();
                record_address = value;
                value = 0;
                break;
            }
            case NIBBLECOUNT_BYTE_COUNT_END: {
                record_bytecount = value;
                value = 0;
                break;
            }
            case NIBBLECOUNT_RECORD_TYPE_END: {
                record_type = value;
                value = 0;
                break;
            }
            default: {
                break;
            }
            }
        }
    }

    private void beginRecord() throws IOException {
        if (state == STATE_START || state == STATE_AFTER_WHITESPACE) {
            state = STATE_AFTER_COLON;
        } else {
            handleError();
            return;
        }

        if (previouse_record_type == RECORD_TYPE_EOF) {
            handleError(); // there should be no more data
            return;
        }
        record_count++;
        
        // reset record
        nibblecount = 0;
        value = 0;
        record_address = 0;
        record_bytecount = 0;
        record_checksum = 0;
        record_computed_checksum = 0;
        record_computed_checksum_tmp = 0;
        record_type = RECORD_TYPE_UNDEFINED;
    }
    
    private void endRecord() throws IOException {
        int actual_nibblecount =
            nibblecount
            - NIBBLECOUNT_DATA_BEGIN
            - 2; // checksum

        record_checksum = value&0xff;
        record_computed_checksum = (((record_computed_checksum&0xff)^0xff)+1)&0xff;
        previouse_record_type = record_type;
        
        // compare specified and actual bytecount 
        if (actual_nibblecount != record_bytecount*2) {
            handleError("specified bytecount is wrong in record["+(record_count-1)+"]: specified="+record_bytecount+",expected="+(actual_nibblecount/2));
            return;
        }
        
        if (record_computed_checksum != record_checksum) {
            handleError("record["+(record_count-1)+"]: checksum error found:0x"+(Integer.toHexString(record_checksum)
                    +" expected:0x"+Integer.toHexString(record_computed_checksum)));
        }
        
        // clear record type so we know whats going on
        record_type = RECORD_TYPE_UNDEFINED; 
    }
    
    private void handleWhitespace(char c) throws IOException {
        if (state == STATE_AFTER_HEX_DIGIT) {
            state = STATE_AFTER_WHITESPACE;
            endRecord();
            return; // ok
        } else if (state == STATE_AFTER_WHITESPACE) {
            return; // ok
        } else {
            handleError(); // error
        }
    }
           
    private void handleEOFCharacter() throws IOException {
        switch (state) {
            case STATE_ERROR:
            case STATE_VALID: { break; }
            case STATE_AFTER_HEX_DIGIT: {
                endRecord(); // finish record
                if (previouse_record_type == RECORD_TYPE_EOF) {
                    state = STATE_VALID;
                } else {
                    handleError("EOF record is missing");
                }
                break;    
            }
            case STATE_AFTER_WHITESPACE: {
                if (previouse_record_type == RECORD_TYPE_EOF) {
                    state = STATE_VALID;
                } else {
                    handleError("EOF record is missing");
                }
                break;
            }
            default: {
                handleError();
                break;
            }
        }
    }
    
    private void handleError() throws IOException {
        handleError("not an intel hex file");
    }
           
    private void handleError(String error) throws IOException {
        this.state = STATE_ERROR;
        this.error = error;
        if (failOnError) {
            throw new IOException(error);
        }
    }

    @Override
    protected boolean shouldFinish() {
        return state<0;
    }
       
}
