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
package com.ruinwesen.patchmanager.client.protocol;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import org.apache.commons.codec.binary.Base64;

public class DataInfo {

    private String data_base64;
    private byte[] md5digest;
    private String md5digest_base64;
    
    public DataInfo(InputStream is) throws NoSuchAlgorithmException, IOException {
        read(is);
    }
    
    private void read(InputStream is) throws IOException, NoSuchAlgorithmException {
        byte[] data;
        { 
            ByteArrayOutputStream dataOut = new ByteArrayOutputStream();
            byte[] tmp = new byte[1024];
            int r=0;
            while ((r=is.read(tmp, 0, tmp.length-r))!=-1) {
                dataOut.write(tmp, 0, r);
            }
            data = dataOut.toByteArray();
        }
        this.data_base64 = new String(Base64.encodeBase64(data));
        MessageDigest md5 = MessageDigest.getInstance("md5");
        md5.update(data);
        this.md5digest = md5.digest(new byte[32]);
        this.md5digest_base64 = new String(Base64.encodeBase64(md5digest));
    }
    
    public String getDataString() {
        return new String(data_base64);
    }
    
    public String getMD5String() {
        return new String(md5digest_base64);
    }
    
    public byte[] getMD5Digest() {
        return Arrays.copyOf(md5digest, md5digest.length);
    }
    
}
