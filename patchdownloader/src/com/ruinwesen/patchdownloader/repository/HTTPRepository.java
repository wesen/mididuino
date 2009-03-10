/** 
 * Copyright (C) 2009 Christian Schneider
 *
 * This file is part of Patchdownloader.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

import name.cs.csutils.CSUtils;
import name.cs.csutils.net.CSHttpGet;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.http.client.ClientProtocolException;

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
    public <C extends StoredPatchCollector> C collectPatches(C collector) {
        return collectPatches(collector, null);
    }

    public <C extends StoredPatchCollector> C collectPatches(C collector, Date since) {
        String requestURL = since != null 
            ? getRequestPatchesSinceRequestURL(since)
            : getRequestPatchesAllRequestURL();
        String patchListText;
        try {
            patchListText = new CSHttpGet().get(requestURL);
        } catch (ClientProtocolException ex) {
            if (log.isErrorEnabled()) {
                log.error("could not get url: "+requestURL, ex);
            }
            return collector;
        } catch (IOException ex) {
            log.error("could not get url: "+requestURL, ex);
            return collector;
        }
        
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
        CSHttpGet httpget = new CSHttpGet();
        OutputStream out = new FileOutputStream(new File(dst, patch.getName()));
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
