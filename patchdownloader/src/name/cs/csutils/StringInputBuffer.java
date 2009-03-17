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
package name.cs.csutils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.Reader;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class StringInputBuffer implements Runnable {

    private static Log log = LogFactory.getLog(StringInputBuffer.class);
    private BufferedReader reader;
    private StringBuffer buffer = new StringBuffer();
    private PrintStream out;
    private volatile long stopRequested = 0;
    private int maxAppendLines = -1;
    private int lineno = 0;
    private String prefix;

    public StringInputBuffer(Reader reader) {
        this(reader, null, null);
    }
    
    public StringInputBuffer(Reader reader, String prefix, PrintStream out) {
        if (reader == null) {
            throw new IllegalArgumentException("reader == null");
        }
        if (reader instanceof BufferedReader) {
            this.reader = (BufferedReader) reader;
        } else {
            this.reader = new BufferedReader(reader);
        }
        this.out = out;
        this.prefix = prefix;
    }
    
    public StringInputBuffer(InputStream in) {
        if (in == null) {
            throw new IllegalArgumentException("in == null");
        }
        this.reader = new BufferedReader(new InputStreamReader(in));
    }
    
    public Thread start() {
        Thread thread = new Thread(this);
        thread.start();
        return thread;
    }
    
    public void stop() {
        stopRequested = 1;
    }
    
    public void close() throws IOException {
        stop();
        reader.close();
    }
    
    public void run() {
        boolean interruped = false;
        try {
            while (stopRequested == 0) {
                String line = reader.readLine();
                if (line == null) {
                    break;
                }
                emit(line);
            }
        } catch (IOException ex) {
            if (log.isDebugEnabled()) {
                log.debug("error while reading", ex);
            }
            // no op
        } finally {
            if (interruped) {
                Thread.currentThread().interrupt();
            }
        }
    }
    
    private void emit(String line) {
        lineno++;
        if (maxAppendLines != -1 && lineno>=maxAppendLines) {
            return;
        }
        buffer.append(line).append('\n');
        if (out != null) {
            if (prefix != null) {
                out.print(prefix);
            }
            out.println(line);
        }
    }
    
    public String getOutput() {
        return buffer.toString();
    }
    
}
