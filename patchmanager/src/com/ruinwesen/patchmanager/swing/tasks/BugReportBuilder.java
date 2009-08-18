package com.ruinwesen.patchmanager.swing.tasks;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.File;
import java.util.Formatter;

import name.cs.csutils.SystemProperties;

public class BugReportBuilder {

    private StringBuilder sb = new StringBuilder();
    private Formatter fmt = new Formatter(sb); 
    
    public StringBuilder getStringBuilder() {
        return sb;
    }
    
    public void format(String format, Object...args) {
        fmt.format(format, args);
    }
    
    @Override
    public String toString() {
        return sb.toString();
    }
    
    public BugReportBuilder appendSystemInfo() {
        appendSystemProperty(SystemProperties.JAVA_VERSION);
        appendSystemProperty(SystemProperties.JAVA_VENDOR);
        appendSystemProperty(SystemProperties.JAVA_VENDOR_URL);
        appendSystemProperty(SystemProperties.JAVA_HOME);
        appendSystemProperty(SystemProperties.JAVA_VM_SPECIFICATION_VERSION);
        appendSystemProperty(SystemProperties.JAVA_VM_SPECIFICATION_VENDOR);
        appendSystemProperty(SystemProperties.JAVA_VM_SPECIFICATION_NAME);
        appendSystemProperty(SystemProperties.JAVA_VM_VERSION);
        appendSystemProperty(SystemProperties.JAVA_VM_VENDOR);
        appendSystemProperty(SystemProperties.JAVA_VM_NAME);
        appendSystemProperty(SystemProperties.JAVA_SPECIFICATION_VERSION);
        appendSystemProperty(SystemProperties.JAVA_SPECIFICATION_VENDOR);
        appendSystemProperty(SystemProperties.JAVA_SPECIFICATION_NAME);
        appendSystemProperty(SystemProperties.JAVA_CLASS_VERSION);
        appendSystemProperty(SystemProperties.JAVA_CLASS_PATH);
        appendSystemProperty(SystemProperties.JAVA_LIBRARY_PATH);
        appendSystemProperty(SystemProperties.JAVA_IO_TMPDIR);
        appendSystemProperty(SystemProperties.JAVA_COMPILER);
        appendSystemProperty(SystemProperties.JAVA_EXT_DIRS);
        appendSystemProperty(SystemProperties.OS_NAME);
        appendSystemProperty(SystemProperties.OS_ARCH);
        appendSystemProperty(SystemProperties.OS_VERSION);
        appendSystemProperty(SystemProperties.FILE_SEPARATOR);
        appendSystemProperty(SystemProperties.PATH_SEPARATOR);
        appendSystemProperty(SystemProperties.USER_HOME);
        return this;
    }

    private void appendSystemProperty(final String property) {
        try {
            sb.append(property+"="+System.getProperty(property)+"\n"); 
        } catch (SecurityException ex) {
            sb.append("could not read property '"+property+"':"+ex+"\n");
        }
    }

    public void appendLogFile(File file) {
        String logString = "could not load logfile";
        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            InputStream in = new BufferedInputStream(new FileInputStream(file));
            byte[] buf = new byte[1024];
            int r;
            while((r=in.read(buf)) != -1) {
                out.write(buf, 0, r);
            }
            logString = new String(out.toByteArray());
        } catch (Exception ex) {
            // no op
        }

        sb.append("logfile: "+file.getName()+"\n");
        sb.append(logString);
    }

    public void append(String string) {
        sb.append(string);
    }
    
}
