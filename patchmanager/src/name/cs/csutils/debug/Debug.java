package name.cs.csutils.debug;

import java.io.Serializable;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class Debug implements Serializable {
    private static final Log log = LogFactory.getLog(Debug.class);
    
    private static final long serialVersionUID = -3091309901467023003L;
    private Class<?> debugClass;
    private boolean enabled;
    
    private Debug() {
        super();
    }
    
    Debug(Class<?> aclass) {
        this();
        this.debugClass = aclass;
        this.enabled = "true".equals(System.getProperty(aclass.getName()+"#debug"));
        if (this.enabled && log.isDebugEnabled()) {
            log.debug("debug mode enabled for "+aclass);
        }
    }
    
    public Class<?> getDebugClass() {
        return debugClass;
    }
    
    public final boolean isDebugEnabled() {
        return enabled;
    }
    
}
