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
package com.ruinwesen.patchmanager.client;

import java.io.File;
import java.io.IOException;

import name.cs.csutils.CSUtils;

import org.apache.http.HttpVersion;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;

import org.apache.http.client.HttpClient;
import org.apache.http.conn.ClientConnectionManager;
import org.apache.http.conn.params.ConnManagerParams;
import org.apache.http.conn.scheme.PlainSocketFactory;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.scheme.SchemeRegistry;

import com.ruinwesen.patchmanager.client.index.PatchIndex;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.ClientProtocol1;
import com.ruinwesen.patchmanager.client.protocol.DefaultPatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.ProtocolException;
import com.ruinwesen.patchmanager.client.repository.ClientRepository;
import com.ruinwesen.patchmanager.client.repository.PatchDownloadCallback;

public class PatchManager {

    private DefaultPatchManagerClient managerClient;
    private ClientRepository repository;
    private HttpClient httpclient;
    private File patchManagerDir;
    private File repositoryDir;
    private PatchIndex patchindex;
    
    public PatchManager(File patchManagerDir) {
        if (patchManagerDir == null) {
            patchManagerDir = CSUtils.getApplicationConfigDir("rw-patchmanager");
        }
        this.patchManagerDir = patchManagerDir;
        if (!patchManagerDir.exists()) {
            patchManagerDir.mkdir();
        }
        repositoryDir = new File(patchManagerDir, "repository");
        if (!repositoryDir.exists()) {
            repositoryDir.mkdir();
        }
        
        init();
    }
    
    public PatchIndex getIndex() {
        return patchindex;
    }
    
    public void syncRepository(Auth auth, PatchDownloadCallback cb) throws ProtocolException, IOException, InterruptedException {
        repository.sync(managerClient, auth, cb); // updates patch-index
    }
    
    public DefaultPatchManagerClient getPatchManagerClient() {
        return managerClient;
    }
    
    public HttpClient getHTTPClient() {
        return httpclient;
    }
    
    public ClientRepository getRepository() {
        return repository;
    }
    
    private void initHTTPClient() {
        HttpParams params = new BasicHttpParams();
        ConnManagerParams.setMaxTotalConnections(params, 100);
        ConnManagerParams.setTimeout(params, 5000);
        HttpProtocolParams.setVersion(params, HttpVersion.HTTP_1_1);

        // Create and initialize scheme registry 
        SchemeRegistry schemeRegistry = new SchemeRegistry();
        schemeRegistry.register(
                new Scheme("http", PlainSocketFactory.getSocketFactory(), 80));
        // Create an HttpClient with the ThreadSafeClientConnManager.
        // This connection manager must be used if more than one thread will
        // be using the HttpClient.
        ClientConnectionManager cm = new ThreadSafeClientConnManager(params, schemeRegistry);
        this.httpclient = new DefaultHttpClient(cm, params);
    }
    
    private void init() {
        initHTTPClient();
        managerClient = new DefaultPatchManagerClient(httpclient, new ClientProtocol1());
        patchindex = new PatchIndex(new File(patchManagerDir,"repository.index"),repositoryDir);
        repository = new ClientRepository(repositoryDir, patchindex);
    }
    
}
 