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

import java.io.IOException;
import java.io.UnsupportedEncodingException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.util.EntityUtils;
import org.json.JSONException;
import org.json.JSONObject;

public class DefaultPatchManagerClient extends PatchManagerClient {

    private static Log log = LogFactory.getLog(DefaultPatchManagerClient.class);
    
    private HttpClient httpclient;
    private Protocol protocol;
    private String postURI;
    public static final String DEFAULT_POSTURI = "http://ruinwesen.com/patch-manager";

    public DefaultPatchManagerClient(HttpClient httpclient, Protocol protocol) {
        this(httpclient, protocol, DEFAULT_POSTURI);
    }
    
    public DefaultPatchManagerClient(HttpClient httpclient, Protocol protocol, String postURI) {
        this.httpclient = httpclient;
        this.protocol = protocol;
        this.postURI = postURI;
    }
    
    public HttpClient getHttpclient() {
        return httpclient;
    }

    public Protocol getProtocol() {
        return protocol;
    }

    @Override
    public Response execute(Request request) throws ProtocolException {
        if (log.isDebugEnabled()) {
            log.debug("Sending request:"+request);
        }
        
        HttpPost httppost = new HttpPost(postURI);
        String requestString;
        try {
            requestString = protocol.encode(request).toString();
        } catch (IOException ex) {
            throw new ProtocolException("Could not encode request: "+request);
        }
        
        if (log.isDebugEnabled()) {
            log.debug("Request post-uri:'"+postURI+"', body:"+requestString);
        }
        
      //  System.out.println("REQUEST: "+requestString);
        
        try {
            httppost.setEntity(new StringEntity(requestString,"UTF8"));
        } catch (UnsupportedEncodingException ex) {
            throw new ProtocolException("Could not set http-post body.", ex);
        }
        
        HttpResponse response;
        try {
            response = httpclient.execute(httppost);
        } catch (Exception ex) {
            throw new ProtocolException("Could not execute request:"+requestString);
        }
        HttpEntity resEntity = response.getEntity();

        if (response.getStatusLine().getStatusCode() == HttpStatus.SC_OK) {
            if (resEntity != null) {
                Response robj;
                try {
                    robj = createResponseObject(request.getAction(), EntityUtils.toString(resEntity));
                } catch (Exception ex) {
                    throw new ProtocolException("could not parse response", ex);
                } finally {
                    try {
                        resEntity.consumeContent();
                    } catch (IOException ex) {
                        if (log.isDebugEnabled()) {
                            log.debug(ex);
                        }
                    }
                }
                if (!request.getAction().equals(robj.getRequestedAction())) {
                    throw new ProtocolException("unexpected response: "+robj);
                }
                if (log.isErrorEnabled() && !robj.isOkStatus()) {
                    log.error("Response indicates error: "+robj);
                }
                return robj;
            } else {
                if (log.isErrorEnabled()) {
                    log.error("response is not a JSON object");
                }
                throw new ProtocolException("response is not a JSON object");
            }
        } else {
            if (log.isErrorEnabled()) {
                log.error("error in http response:"+response.getStatusLine());
            }
            throw new ProtocolException(response.getStatusLine().toString());
        }
    }

    private Response createResponseObject(String action, String jsonResponse) throws ProtocolException {        
        try {
        Message message = protocol.decode(new JSONObject(jsonResponse));
        if (!message.isResponse()) {
            throw new ProtocolException("not a response"); 
        }
        return (Response) message;
        } catch (JSONException ex) {
            throw new ProtocolException("Could not parse reponse.", ex);
        }
    }

}
