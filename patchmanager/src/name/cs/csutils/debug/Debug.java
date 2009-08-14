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
package name.cs.csutils.debug;

import java.io.Serializable;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class Debug implements Serializable {

    private static final Log log = LogFactory.getLog(Debug.class);
    
    private static Debug GLOBAL = null;
    static {
        GLOBAL = DebugFactory.getDebug(Debug.class);
    }
    
    private static final long serialVersionUID = -3091309901467023003L;
    private Class<?> debugClass;
    private boolean enabled;
    
    private Debug() {
        super();
    }
    
    Debug(Class<?> aclass) {
        this();
        this.debugClass = aclass;
        this.enabled = 
            (Debug.GLOBAL != null && Debug.GLOBAL.isDebugEnabled())
         || "true".equals(System.getProperty(aclass.getName()+"#debug"));
        if (this.enabled && log.isDebugEnabled()) {
            if (Debug.class == aclass) {
                log.debug("debug mode is enabled globally");
            } else {
                log.debug("debug mode enabled for "+aclass);
            }
        }
    }
    
    public Class<?> getDebugClass() {
        return debugClass;
    }
    
    public final boolean isDebugEnabled() {
        return enabled;
    }
    
}
