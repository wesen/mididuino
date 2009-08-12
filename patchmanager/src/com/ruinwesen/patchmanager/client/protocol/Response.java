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

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Response extends Message {

    public static final String KEY_CLIENT_VERSION = "client-version";
    
    private String requestedAction;
    private String status;
    private String message;
    private List<PatchSource> patchSourceList;
    private List<NewsItem> newsList;
    private Map<String, String> properties = new HashMap<String, String>();
    
    public Response(String protocolId, 
            String requestedAction, String status, String message) {
        super(protocolId, "response");
        if (requestedAction == null || status == null) {
            throw new IllegalArgumentException("requestedAction:"+requestedAction+",status:"
                    +status);
        }
        this.requestedAction = requestedAction;
        this.status = status;
        this.message = message == null ? "" : message;
    }
    
    public String getRequestedAction() {
        return requestedAction;
    }

    public String getStatus() {
        return status;
    }
    
    public boolean isOkStatus() {
        return "ok".equalsIgnoreCase(status);
    }
    
    public String getMessage() {
        return message;
    }
    
    @Override
    public String toString() {
        return getClass().getName()+"[protocolid="+getProtocolId()+",requestedaction="
        +requestedAction+",status='"+status+"',message='"+message+"']";
    }

    void setPatchSourceList(List<PatchSource> list) {
        this.patchSourceList = list;
    }
 
    void setNewsList(List<NewsItem> list) {
        this.newsList = list;
    }
    
    public List<NewsItem> getNewsList() {
        return newsList;
    }
    
    public List<PatchSource> getPatchSourceList() {
        return patchSourceList;
    }

    public PatchSource getPatchSource() {
        if (patchSourceList != null && !patchSourceList.isEmpty()) {
            return patchSourceList.get(0);
        }
        return null;
    }

    public String getProperty(String value) {
        return properties.get(value);
    }
    
}
