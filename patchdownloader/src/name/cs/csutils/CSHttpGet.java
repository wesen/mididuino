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

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;


import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

public class CSHttpGet {

    private HttpClient httpclient = new DefaultHttpClient();
    
    public CSHttpGet() {
        super();
    }

    public String get(URL url) throws ClientProtocolException, IOException, URISyntaxException {
        return get(url.toURI());
    }

    public String get(String uri) throws ClientProtocolException, IOException {
        return get(new HttpGet(uri), uri);
    }

    public String get(URI uri) throws ClientProtocolException, IOException {
        return get(new HttpGet(uri), uri);
    }

    private String get(HttpGet httpget, Object path) throws ClientProtocolException, IOException {
        return get(httpget, null, path);
    }
    
    private String get(HttpGet httpget, OutputStream out, Object path)
        throws ClientProtocolException, IOException {
        // Execute the request
        HttpResponse response = httpclient.execute(httpget);

        // Examine the response status
        // System.out.println(response.getStatusLine());
        int status = response.getStatusLine().getStatusCode();
        // handle the response status code
        switch (status) {
            case HttpStatus.SC_NOT_FOUND: {
                throw new FileNotFoundException("file not found: "+path);
            }
            case HttpStatus.SC_OK: {
                break;
            }
        }

        // Get hold of the response entity
        HttpEntity entity = response.getEntity();

        String result = null;
        // If the response does not enclose an entity, there is no need
        // to worry about connection release
        if (entity != null) {
            InputStream instream = entity.getContent();
            try {
                if (out != null) {
                    CSUtils.copy(instream, out, -1);
                } else {
                    ByteArrayOutputStream bytestream = 
                        new ByteArrayOutputStream();
                    CSUtils.copy(instream, bytestream, -1);
                    result = new String(bytestream.toByteArray());
                }
                
            } catch (IOException ex) {

                // In case of an IOException the connection will be released
                // back to the connection manager automatically
                throw ex;
                
            } catch (RuntimeException ex) {

                // In case of an unexpected exception you may want to abort
                // the HTTP request in order to shut down the underlying 
                // connection and release it back to the connection manager.
                httpget.abort();
                throw ex;
                
            } finally {

                // Closing the input stream will trigger connection release
                instream.close();
                
            }
        }
        
        return result;
    }

    public void get(URL url, OutputStream targetStream) throws ClientProtocolException, IOException, URISyntaxException {
        get(url.toURI(), targetStream);
    }

    public void get(URI uri, OutputStream targetStream) throws ClientProtocolException, IOException {
        get(new HttpGet(uri), targetStream, uri);
    }

    public void get(String uri, OutputStream targetStream) throws ClientProtocolException, IOException {
        get(new HttpGet(uri), targetStream, uri);
    }

}
