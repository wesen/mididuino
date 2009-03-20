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
package com.ruinwesen.patchdownloader.repository;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;
import java.util.Properties;

import name.cs.csutils.CSHttpGet;
import name.cs.csutils.CSUtils;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class HTTPRepository extends Repository {

    private static Log log = LogFactory.getLog(Repository.class);

    public static final String KEY_REQUEST_PATCHES_SINCE =
        "remoterepository.requestpatches.since";

    public static final String KEY_REQUEST_PATCHES_ALL =
        "remoterepository.requestpatches.all";

    private String requestPatchesSinceFormat = null;
    private String requestPatchesAll = null;
    
    protected static Properties readProperties() {
        final String SOURCE = "/repository.properties";
        Properties props = new Properties();
        try {
            InputStream in = HTTPRepository.class.getResourceAsStream(SOURCE);
            if (in == null) {
                throw new FileNotFoundException("resource not found: "+SOURCE);
            }
            try {
                props.load(in);
            } finally {
                in.close();
            }
        } catch (IOException ex) {
            // no op
            if (log.isWarnEnabled()) {
                log.warn("could not read properties: "+SOURCE, ex);
            }
        }
        return props;
    }
    
    public HTTPRepository() {
        super();
        loadProperties();
    }

    protected void loadProperties() {
        setProperties(HTTPRepository.readProperties());
    }
    
    public String getRequestPatchesSinceRequestURL(Date since)  {
        return CSUtils.format(getRequestPatchesSinceFormat(), since);
    }

    public String getRequestPatchesAllRequestURL() {
        return getRequestPatchesAll();
    }

    public String getRequestPatchesSinceFormat() {
        return requestPatchesSinceFormat;
    }

    public String getRequestPatchesAll() {
        return requestPatchesAll;
    }

    public void setRequestPatchesSinceFormat(String value) {
        requestPatchesSinceFormat = value;
    }

    public void setRequestPatchesAll(String value) {
        requestPatchesAll = value;
    }
    
    protected void setProperties(Properties props) {
        requestPatchesSinceFormat = props.getProperty(KEY_REQUEST_PATCHES_SINCE); 
        requestPatchesAll = props.getProperty(KEY_REQUEST_PATCHES_ALL); 
    }

    protected static Properties getDefaultHTTPProperties() {
        Properties props = new Properties();
        props.put(KEY_REQUEST_PATCHES_SINCE,
        "http://ruinwesen.com/patchdownloader/respository/listpatches?since=%1$tY-%1$tm-%1$td" );
        props.put(KEY_REQUEST_PATCHES_ALL,
        "http://ruinwesen.com/patchdownloader/respository/listpatches");
        return props;
    }


    @Override
    public <C extends StoredPatchCollector> C collectPatches(C collector) throws IOException {
        return collectPatches(collector, null);
    }

    public <C extends StoredPatchCollector> C collectPatches(C collector, Date since) throws IOException {
        String requestURL = since != null 
            ? getRequestPatchesSinceRequestURL(since)
            : getRequestPatchesAllRequestURL();
        String patchListText;
        patchListText = new CSHttpGet().get(requestURL);
        
        String[] pathList = patchListText.split("[\r\n]+");
        for (String path: pathList) {
            path = path.trim();
            if (path.length() > 0) { // not an empty line
                if (!collector.takesMore()) {
                    break;
                }
                collector.collect(this, new StoredPatch.Remote(path));
            }
        }
        return collector;
    }

    public void export(StoredPatch patch, File dst) throws IOException {
        exportFile(patch, new File(dst, patch.getName()));
    }

    public void exportFile(StoredPatch patch, File dst) throws IOException {
        CSHttpGet httpget = new CSHttpGet();
        OutputStream out = new FileOutputStream(dst);
        try {
            httpget.get(patch.getPath(), out);
        } finally {
            out.flush();
            out.close();
        }
    }

    public void export(StoredPatch[] locatorList, File directory) throws IOException {
        CSHttpGet httpget = new CSHttpGet();
        for (StoredPatch locator: locatorList) {
            OutputStream out = new FileOutputStream(new File(directory, locator.getName()));
            try {
                httpget.get(locator.getPath(), out);
            } finally {
                out.flush();
                out.close();
            }
        }
    }

    @Override
    public void export(StoredPatch patch, OutputStream out) throws IOException {
        CSHttpGet httpget = new CSHttpGet();
        httpget.get(patch.getPath(), out);
    }

}
