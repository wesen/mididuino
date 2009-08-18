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
package com.ruinwesen.patchmanager.client.index;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.EOFException;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.text.DateFormat;
import java.text.ParseException;
import java.util.Date;

public class RecordIO {

    public static final int READ = 1<<0;
    public static final int WRITE = 1<<1;
    public static final int APPEND = 1<<2;
    public static final int WRITE_MASK = WRITE|APPEND;

    private DateFormat dateFormat = DateFormat.getInstance();
    private BufferedReader is;
    private PrintStream os;
    private int mode;
    private StringBuilder sb = new StringBuilder(); // used to (un)escape strings
    private String bufferedLine = null;
    
    public RecordIO(File file, int mode) throws IOException {
        this.mode = mode;
        this.is = null;
        this.os = null;
        switch (mode) {
        case READ: { 
            is = new BufferedReader(new FileReader(file));
            bufferedLine = is.readLine();
            break; }
        case WRITE: { os = new PrintStream(new BufferedOutputStream(new FileOutputStream(file))); break; }
        case APPEND: { os = new PrintStream(new BufferedOutputStream(new FileOutputStream(file, true))); break; }
        default: { throw new IllegalArgumentException("unsupported mode: "+mode); }
        }
    }
    
    protected final void ensureMode(int mask) throws IOException {
        if ((mode & mask) == 0) {
            throw new IOException("operation not supported by mode:"+mode);
        }
    }

    private String escape(String str) {
        if (str.indexOf('\n')<0 && str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                sb.append("\\\\");
            } else if (ch == '\n') {
                sb.append("\\n");
            } else {
                sb.append(ch);
            }
        }
        return sb.toString();
    }
    
    private String unescape(String str) {
        if (str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                if (i+1<str.length()) {
                    if (str.charAt(i+1)=='n') {
                        sb.append('\n');
                        continue;
                    } else if (str.charAt(i+1)=='\\') {
                        sb.append('\\');
                        continue;
                    }
                }
            }
            sb.append(ch);
        }
        return sb.toString();
    }
    
    public boolean hasMore() throws IOException {
        ensureMode(READ);
        return bufferedLine != null;
    }
    
    public String readString() throws IOException {
        ensureMode(READ);
        String line = bufferedLine;
        if (line == null) {
            throw new EOFException("corrupted index");
        }
        bufferedLine = is.readLine();
        return unescape(line);
    }

    public int readInteger() throws IOException {
        try {
            return Integer.parseInt(readString());
        } catch (NumberFormatException ex) {
            throw new IOException("corrupted index");
        }
    }

    public long readLong() throws IOException {
        try {
            return Long.parseLong(readString());
        } catch (NumberFormatException ex) {
            throw new IOException("corrupted index");
        }
    }

    public void writeString(String str) throws IOException {
        ensureMode(WRITE_MASK);
        os.println(escape(str));
    }

    public void writeString(String str, String defaultValue) throws IOException {
        writeString(str != null ? str : defaultValue);
    }

    public void writeInteger(int value) throws IOException {
        writeString(Integer.toString(value));
    }

    public void writeLong(long value) throws IOException {
        writeString(Long.toString(value));
    }

    public void writeDate(Date date) throws IOException {
        writeString(dateFormat.format(date));
    }

    public Date readDate() throws IOException {
        try {
            return dateFormat.parse(readString());
        } catch (ParseException ex) {
            throw new IOException("corrupted index");
        }
    }
    
    public void flush() throws IOException {
        ensureMode(WRITE_MASK);
        os.flush();
    }
    
    public void close() throws IOException {
        if (is != null) {
            is.close();
            is = null;
        }
        if (os != null) {
            os.close();
            os = null;
        }
    }
    
}
