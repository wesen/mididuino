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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.nio.channels.OverlappingFileLockException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Implements a lock file mechanism.
 */
public class LockFile {

    public static final class MultipleApplicationInstancesException extends Exception {
        private static final long serialVersionUID = -145283693213936163L;
    }
    
    private Log log = LogFactory.getLog(LockFile.class);
    
    // the lock file name
    private File file;
    private FileChannel channel;
    private FileLock lock;

    /**
     * Creates a new lock file.
     * @param file the lock file name
     * @throws IllegalArgumentException the file argument is <code>null</code>
     */
    public LockFile(File file) {
        if (file == null) {
            throw new IllegalArgumentException("file==null");
        }
        this.file = file;
    }
    
    /**
     * 
     * @return
     */
    public boolean isLockActive() {
        try {
            // access the lock file, may throw a FileNotFoundException
            RandomAccessFile raf = new RandomAccessFile(file, "rw"); 
            channel = raf.getChannel();

            try {
                lock = channel.tryLock();
            }
            catch (OverlappingFileLockException e) {
                // already locked
                closeLock();
                return true;
            }

            if (lock == null) {
                closeLock();
                return true;
            }

            // destroy the lock when the JVM is closing
            Runtime.getRuntime().addShutdownHook(new Thread() {
                    @Override
                    public void run() {
                        closeLock();
                        deleteFile();
                    }
                });
            return false;
        } catch (FileNotFoundException ex) {
            if (log.isWarnEnabled()) {
                log.warn("Could not access lockfile:"+file.getAbsolutePath(), ex);
            }
            closeLock();
            return true;   
        } catch (IOException ex) {
            if (log.isWarnEnabled()) {
                log.warn("Could not access lockfile:"+file.getAbsolutePath(), ex);
            }
            closeLock();
            return true;
        }
    }

    private void closeLock() {
        try { 
            lock.release();  
        }
        catch (IOException ex) {  
            if (log.isWarnEnabled()) {
                log.warn("Could not release lockfile:"+file.getAbsolutePath(), ex);
            }
        }
        try { 
            channel.close(); 
        } catch (IOException ex) { 
            if (log.isWarnEnabled()) {
                log.warn("Could not close lockfile channel:"+file.getAbsolutePath(), ex);
            }
        }
    }
    
    private void deleteFile() {
        file.delete(); 
    }

    
}
 