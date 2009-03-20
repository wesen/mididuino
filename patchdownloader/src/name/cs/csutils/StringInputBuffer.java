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
import java.io.Reader;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Reads the string-content of a {@link Reader} stores it in a buffer.
 * The content is read in a seperate thread. The class is intented to be
 * used to read the output and error stream of a {@link Process}.
 * 
 * @author Christian Schneider 
 */
public class StringInputBuffer implements Runnable {

    /** log instance for this class */
    private static Log log = LogFactory.getLog(StringInputBuffer.class);
    
    /** the reader */
    private BufferedReader reader;
    
    /** the string buffer (thread safe) */
    private StringBuffer buffer = new StringBuffer();
    
    /** stopRequested != 0 indicates if stopping the thread is requested */
    private volatile long stopRequested = 0;
    
    /**
     * Only maxAppendLines are written to the buffer. <code>-1</code> indicates
     * that the number of lines written to the buffer is unlimited.
     * This variable is currently not modifyable.
     */
    private int maxAppendLines = -1;
    
    /** counts the number of lines. */
    private int lineno = 0;
    
    /** the reader thread */
    private Thread readerThread = null;

    /**
     * Creates a new StringInputBuffer which is reading from the specified {@link Reader}.
     * @param reader a reader
     * @throws IllegalArgumentException if the specified argument is null
     */
    public StringInputBuffer(Reader reader) {
        if (reader == null) {
            throw new IllegalArgumentException("reader == null");
        }
        if (reader instanceof BufferedReader) {
            this.reader = (BufferedReader) reader;
        } else {
            this.reader = new BufferedReader(reader);
        }
    }

    /**
     * Creates a new StringInputBuffer which is reading from the specified
     * {@link InputStream}.
     * @param in a InputStream
     * @throws IllegalArgumentException if the specified argument is null
     */
    public StringInputBuffer(InputStream in) {
        if (in == null) {
            throw new IllegalArgumentException("in == null");
        }
        this.reader = new BufferedReader(new InputStreamReader(in));
    }
    
    /**
     * Starts a thread which reads the content and writes it to the buffer.
     * This method can be called only once.
     * @return the new thread
     */
    public synchronized Thread start() {
        if (readerThread != null) {
            throw new IllegalStateException("start() can be called only once");
        }
        
        readerThread = new Thread(this);
        readerThread.start();
        return readerThread;
    }
    
    /**
     * Requests that the thread should stop.
     */
    public void requestStop() {
        stopRequested = 1;
    }
    
    /**
     * Stops a running thread and closes the reader.
     * @throws IOException If an I/O error occurs 
     */
    public void close() throws IOException {
        requestStop();
        reader.close();
    }
    
    /**
     * The run method which is executed by the thread.
     */
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
    
    /**
     * Adds the specified line to the buffer.
     * @param line a string
     */
    private void emit(String line) {
        if (maxAppendLines == -1 || lineno<maxAppendLines) {
            lineno++;
            buffer.append(line).append('\n');
        }
    }
    
    /**
     * Returns the currently buffered output.
     * The method can be called while a thread is running.
     * @return the currently buffered output
     */
    public String getOutput() {
        return buffer.toString();
    }
    
}
