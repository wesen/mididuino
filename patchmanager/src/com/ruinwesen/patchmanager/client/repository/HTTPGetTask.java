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
package com.ruinwesen.patchmanager.client.repository;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.Callable;

import name.cs.csutils.CSUtils;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;

public class HTTPGetTask implements Callable<File> {

    private HttpClient httpclient;
    private HttpGet httpget;
    private File dst;

    public HTTPGetTask(HttpClient httpclient, HttpGet httpget, File dst) {
        if (httpclient == null || httpget == null || dst == null) {
            throw new IllegalArgumentException("httpclient:"+httpclient+",httpget:"+httpget+",dst:"+dst);
        }
        this.httpclient = httpclient;
        this.httpget = httpget;
        this.dst = dst;
    }

    @Override
    public File call() throws IOException, InterruptedException {
        HttpResponse httpresponse = httpclient.execute(httpget);
        HttpEntity httpentity = httpresponse.getEntity();
        

        if (httpresponse.getStatusLine().getStatusCode() != HttpStatus.SC_OK) {
            if (httpentity != null) {
                httpentity.consumeContent();
            }
            throw new IOException(httpresponse.getStatusLine().toString());
        }
        
        if (httpentity == null) {
            throw new IOException(new Exception("no content in response: "+httpresponse.getStatusLine()));
        }

        try {
            InputStream is = httpentity.getContent();
            try {
                CSUtils.copy(is, dst, true);
            } finally {
                is.close();
            }
        } finally {
            httpentity.consumeContent();
        }
        return dst;
    }

}
