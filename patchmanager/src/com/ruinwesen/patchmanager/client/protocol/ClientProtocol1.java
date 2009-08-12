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
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import name.cs.csutils.CSUtils;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo;
import com.ruinwesen.patch.metadata.Tagset;

public class ClientProtocol1 extends Protocol {

    public ClientProtocol1() {
        this("1.1");
    }
    
    protected ClientProtocol1(String protocolId) {
        super(protocolId);
    }

    @Override
    public Message decode(JSONObject obj) throws ProtocolException {
        checkProtocolId(obj);
        String action;
        try {
            action = obj.getString("action");
        } catch (JSONException ex) {
            throw new ProtocolException("field missing:action");
        }
        //if (enableClientProtocol) {
            if (Message.ACTION_RESPONSE.equalsIgnoreCase(action)) {
                return decodeResponse(obj);
            }
        //}
        /*
        if (enableServerProtocol) {
            if (Message.ACTION_DELETE_PATCH.equalsIgnoreCase(action)) {
                return decodeDeletePatchRequest(obj);
            } else if (Message.ACTION_GET_PATCH_SOURCE_LIST.equalsIgnoreCase(action)) {
                return decodeGetPatchRequest(obj);
            } else if (Message.ACTION_REGISTER_NEW_USER.equalsIgnoreCase(action)) {
                return decodeRegisterNewUserRequest(obj);
            } else if (Message.ACTION_STORE_NEW_PATCH.equalsIgnoreCase(action)) {
                return decodeStoreNewPatchPatchRequest(obj);
            }
        }
        */
        throw new ProtocolException("unsupported action:"+action);
    }

    private static final String getField(JSONObject obj, String name) throws ProtocolException {
        try {
            return obj.getString(name);
        } catch (JSONException ex) {
            throw new ProtocolException("reply is missing field: "+name);
        }
    }

    private static final String getField(JSONObject obj, String name, String alternative) throws ProtocolException {
        try {
            String value = obj.getString(name);
            if (value == null ) value = alternative;
            return value;
        } catch (JSONException ex) {
            return alternative;
        }
    }

    private static final int getIntField(JSONObject obj, String name) throws ProtocolException {
        try {
            return obj.getInt(name);
        } catch (JSONException ex) {
            throw new ProtocolException("reply is missing field: "+name);
        }
    }

    protected String decodeProtocolId(JSONObject obj) throws ProtocolException {
        return getField(obj, "protocol-id");
    }
    
    protected void checkProtocolId(JSONObject obj) throws ProtocolException {
        String id = decodeProtocolId(obj);
        if (!getProtocolId().equals(id)) { 
            throw new ProtocolException("protocol version unsupported: "+id);
        }
    }
    
    protected Response decodeResponse(JSONObject obj) throws ProtocolException {
        String requestedAction = getField(obj, "requested-action");
        String status = getField(obj, "status");
        String message = getField(obj, "message", "");

        Response response = new Response(decodeProtocolId(obj), requestedAction, status, message);
        
        if (Message.ACTION_STORE_NEW_PATCH.equals(requestedAction)) {


            JSONObject psobj;
            try {
                psobj = obj.getJSONObject("patch-source");
            } catch (JSONException ex) {
                throw new ProtocolException(ex);
            }
            PatchMetadata meta;
            try {
                meta = decodeMetadata(psobj.getJSONObject("patch-metadata"));
            } catch (JSONException ex) {
                throw new ProtocolException("could not read patch-metadata field", ex);
            }
            PatchSource source = new PatchSource(getField(psobj, "patch-id"), getField(psobj, "patch-url"),meta);
            List<PatchSource> list = new LinkedList<PatchSource>();
            list.add(source);
            response.setPatchSourceList(list);
            return response;
        } else if (Message.ACTION_GET_PATCH_SOURCE_LIST.equals(requestedAction)) {
            JSONArray ar;
            try {
                ar = obj.getJSONArray("patch-source-list");
            } catch (JSONException ex) {
                throw new ProtocolException("response is missing field: patch-source-list");
            }
            List<PatchSource> list = new ArrayList<PatchSource>(ar.length());
            for (int i=0;i<ar.length();i++) {
                JSONObject psobj;
                try {
                    psobj = ar.getJSONObject(i);
                } catch (JSONException ex) {
                    throw new ProtocolException(ex);
                }
                PatchMetadata meta;
                try {
                    meta = decodeMetadata(psobj.getJSONObject("patch-metadata"));
                } catch (JSONException ex) {
                    throw new ProtocolException("could not read patch-metadata field", ex);
                }
                PatchSource source = new PatchSource(getField(psobj, "patch-id"), getField(psobj, "patch-url"),meta);
                list.add(source);
            }
            response.setPatchSourceList(list);
            return response;
        } else if (Message.ACTION_GET_NEWS.equals(requestedAction)) {
            JSONArray ar;
            try {
                ar = obj.getJSONArray("news-list");
            } catch (JSONException ex) {
                throw new ProtocolException("response is missing field: news-list");
            }

            List<NewsItem> list = new ArrayList<NewsItem>(ar.length());
            for (int i=0;i<ar.length();i++) {
                JSONObject psobj;
                try {
                    psobj = ar.getJSONObject(i);
                    System.out.println(psobj);
                } catch (JSONException ex) {
                    throw new ProtocolException(ex);
                }
                NewsItem item = new NewsItem(
                        getIntField(psobj, "news-id"),
                        getField(psobj, "date"), 
                        getField(psobj, "title", ""),
                        getField(psobj, "message"));
                list.add(item);
            }
            response.setNewsList(list);
            return response;
        } else {
            return response;
        }
    }
    
    @Override
    public JSONObject encode(Message message) throws ProtocolException, IOException {
        Map<String, Object> map = createMap();
        encodeProtocolId(map);
        final String action = message.getAction();
        map.put("action", action);
        //if (enableClientProtocol) {
            if (message instanceof AuthenticatingRequest) {
                boolean encodeUsermail = Message.ACTION_REGISTER_NEW_USER.equals(action);
                encodeAuth(map, ((AuthenticatingRequest)message).getAuthentication(), encodeUsermail);
            }
            if (Message.ACTION_SEND_BUG_REPORT.equals(action)) {
                map.put("bug-report", ((RequestSendBugReport)message).getBugreport());
                encodeAuthForBugReport(map, ((RequestSendBugReport)message).getAuth());
            } else if (Message.ACTION_DELETE_USER.equals(action)) {
                map.put("username", ((RequestDeleteUser)message).getUsername());
            } else if (Message.ACTION_GET_CLIENT_INFO.equals(action)) {
                // no operation
            } else if (Message.ACTION_GET_SERVER_INFO.equals(action)) {
                // no operation
            } else if (Message.ACTION_REGISTER_NEW_USER.equals(action)) {
                // no operation
            } else if (Message.ACTION_GET_PATCH_SOURCE_LIST.equals(action)) {
                map.put("date-since", ((RequestGetPatchSourceList)message).getDateSinceString());
            } else if (Message.ACTION_GET_NEWS.equals(action)) {
                map.put("date-since", ((RequestGetNews)message).getDateSinceString());
            } else if (Message.ACTION_STORE_NEW_PATCH.equals(action)) {
                RequestStorePatch r = (RequestStorePatch) message;
                encodeMetadata(map, r.getMetadata());
                try {
                    encodeData(map, r.getDataInfo());
                } catch (NoSuchAlgorithmException ex) {
                    throw new ProtocolException(ex);
                }
            } else if (Message.ACTION_DELETE_PATCH.equals(action)) {
                map.put("patch-id",  ((RequestDeletePatch)message).getPatchId());
            } else {
                throw new ProtocolException("unsupported action:"+action);
            }
            
        //}
           
        /*
        if (enableServerProtocol) {
            if (Message.ACTION_RESPONSE.equalsIgnoreCase(action)) {
                Response response = (Response) message;
                String requestedAction = response.getRequestedAction();
                // TODO
            } else {
                throw new ProtocolException("unsupported action:"+action);
            }
        }*/
        
        return new JSONObject(map);
    }
    
    protected void encodeProtocolId(Map<String, Object> map) {
        map.put("protocol-id", getProtocolId());
    }

    protected Map<String, Object> createMap() {
        return new HashMap<String, Object>();
    }

    private void encodeData(Map<String, Object> map, DataInfo dataInfo) {
        map.put("data", dataInfo.getDataString());
        map.put("data-md5", dataInfo.getMD5String());
    }

    protected void encodeMetadata(Map<String,Object> map, PatchMetadata metadata) {
        Map<String,Object> metamap = createMap();
        metamap.put("title", metadata.getTitle());
        metamap.put("comment", metadata.getComment());
        metamap.put("name", metadata.getName());
        metamap.put("author", metadata.getAuthor());
        metamap.put("metadata-format-version", metadata.getVersion());
        metamap.put("tags", new JSONArray(metadata.getTags()));
        metamap.put("device-id", metadata.getDeviceId());
        metamap.put("environment-id", metadata.getEnvironmentId());
        
        map.put("patch-metadata", new JSONObject(metamap));
    }
    
    protected PatchMetadata decodeMetadata(JSONObject obj) throws ProtocolException {
        PatchMetadata meta = new DefaultPatchMetadata();
        meta.setTitle(getField(obj, "title", ""));
        meta.setAuthor(getField(obj, "author", ""));
        meta.setName(getField(obj, "name", ""));
        meta.setComment(getField(obj, "comment", ""));
        String id = getField(obj, "device-id", "");
        meta.setDeviceId(id.isEmpty() ? null : PatchMetadataIDInfo.getDeviceId(id));
        id = getField(obj, "environment-id", "");
        meta.setEnvironmentId(id.isEmpty()?null:PatchMetadataIDInfo.getEnvironmentId(id));        
        
        try {
            Date date =CSUtils.parseDate(getField(obj, "last-modified-date", ""));
            meta.setLastModifiedDate(date); 
        } catch (NumberFormatException ex) {
            // ignored
        }
        
        try {
            JSONArray ta = obj.getJSONArray("tags");
            Tagset tags = new Tagset();
            for (int i=0;i<ta.length();i++) {
                Object item = ta.get(i);
                if (item instanceof String)
                    tags.add((String)item);
            }
            meta.setTags(tags);
        } catch (JSONException ex) {
            
        }
        
        return meta;
    }
    
    protected void encodeAuthForBugReport(Map<String,Object> map, Auth auth) throws ProtocolException {
        Map<String, Object> map2 = createMap();
        map2.put("username", auth.getUsername());
        if (auth.getUsermail() != null) {
            map2.put("email", auth.getPassword());
        }
        map.put("auth", new JSONObject(map2));
    }
    protected void encodeAuth(Map<String,Object> map, Auth auth, boolean encodeUsermail) throws ProtocolException {
        Map<String, Object> map2 = createMap();
        map2.put("username", auth.getUsername());
        map2.put("password", auth.getPassword());
        if (encodeUsermail) {
            if (auth.getUsermail() == null) {
                throw new ProtocolException("usermail is missing in Auth object");
            }
            map2.put("email", auth.getPassword());
        }
        map.put("auth", new JSONObject(map2));
    }
    
}
 