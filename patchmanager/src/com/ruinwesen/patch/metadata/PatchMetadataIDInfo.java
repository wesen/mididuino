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
package com.ruinwesen.patch.metadata;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class PatchMetadataIDInfo {

    private static Log log = LogFactory.getLog(PatchMetadataIDInfo.class);
    
    private PatchMetadataIDInfo() {
        super(); 
    }
    
    static class NamedId<K extends NamedId<K>> implements Comparable<K> {
        private String value;
        public NamedId(String value) {
            if (value == null) {
                throw new NullPointerException();
            }
            this.value = value;
        }
        public String value() {
            return value;
        }
        @Override
        public int hashCode() {
            return value.hashCode();
        }
        @Override
        public String toString() {
            return value;
        }
        @Override
        public boolean equals(Object o) {
            return o!= null && getClass() == o.getClass()
            && ((NamedId<?>)o).value.equals(value);
        }
        @Override
        public int compareTo(K o) {
            return value.compareTo(o.value);
        }
    }

    public static final class DeviceId extends NamedId<DeviceId> {
        public DeviceId(String value) {
            super(value);
        }
    }

    public static final class EnvironmentId extends NamedId<EnvironmentId> {
        public EnvironmentId(String value) {
            super(value);
        }
    }

    public static final EnvironmentId ENVIRONMENT_MIDIDUINO_013 = new EnvironmentId("mididuino-013");
    public static final DeviceId MINICOMMAND_1_1 = new DeviceId("minicommand-1.1");
    public static final List<DeviceId> DEVICE_ID_LIST;
    public static final List<EnvironmentId> ENVIRONMENT_ID_LIST;
    
    static {
        {
            List<DeviceId> list = new ArrayList<DeviceId>();
            list.add(MINICOMMAND_1_1);
            addDeviceIds(list, "/resources/device-id-list.txt");
            Collections.sort(list);
            DEVICE_ID_LIST = Collections.unmodifiableList(list);
        }
        {
            List<EnvironmentId> list = new ArrayList<EnvironmentId>();
            list.add(ENVIRONMENT_MIDIDUINO_013);
            addEnvIds(list, "/resources/environment-id-list.txt");
            Collections.sort(list);
            ENVIRONMENT_ID_LIST = Collections.unmodifiableList(list);
        }
    }

    private static void addDeviceIds(List<DeviceId> dst, String src) {
        for (String name: parseIDList(src)) {
            DeviceId id = new DeviceId(name);
            if (!dst.contains(id))
                dst.add(id);
        }
    }

    private static void addEnvIds(List<EnvironmentId> dst, String src) {
        for (String name: parseIDList(src)) {
            EnvironmentId id = new EnvironmentId(name);
            if (!dst.contains(id))
                dst.add(id);
        }
    }
    
    private static List<String> parseIDList(String name) {
       
       URL url = PatchMetadataIDInfo.class.getResource(name);
       List<String> list = null;
       if (url != null) {
           try {
               InputStream is = url.openStream();
               try {
                   list = parseIDList(is);
               } finally {
                   is.close();
               }
           } catch (IOException ex) {
               if (log.isErrorEnabled()) {
                   log.error("could not parse id list: "+name, ex);
               }
           }
       }
       if (list == null) {
           if (log.isWarnEnabled()) {
               log.warn("could not read id list:"+name);
           }
           list = Collections.emptyList();
       }
       return list;
    }
    
    private static List<String> parseIDList(InputStream in) throws IOException {
        List<String> list = new LinkedList<String>();
        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
        String line;
        while ((line = reader.readLine()) != null) {
            int comment_index = line.indexOf('#');
            if (comment_index>=0) {
                line = line.substring(0, comment_index);
            }
            line = line.trim();
            if (!line.isEmpty()) {
                list.add(line);
            }
        }
        return list;
    }

    public static DeviceId getDeviceId(String name) {
        DeviceId id=getDefinedDeviceId(name);
        return id != null ? id : new DeviceId(name);
    }

    public static EnvironmentId getEnvironmentId(String name) {
        EnvironmentId id=getDefinedEnvironmentId(name);
        return id != null ? id : new EnvironmentId(name);
    }

    public static DeviceId getDefinedDeviceId(String name) {
        return getDefinedId(DEVICE_ID_LIST, name);
    }

    public static EnvironmentId getDefinedEnvironmentId(String name) {
        return getDefinedId(ENVIRONMENT_ID_LIST, name);
    }

    private static final <K extends NamedId<K>> K getDefinedId(Collection<K> c, String name) {
        if (name == null || name.isEmpty()) {
            throw new IllegalArgumentException("name:"+name);
        }
        for (K k: c) {
            if (k.value.equals(name)) {
                return k;
            }
        }
        return null;
    }
    
}
