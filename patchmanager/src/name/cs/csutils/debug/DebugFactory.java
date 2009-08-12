package name.cs.csutils.debug;

public class DebugFactory {

    private DebugFactory() {
        super();
    }
    
    public static final Debug getDebug(Class<?> aclass) {
        return new Debug(aclass);
    }
    
}
