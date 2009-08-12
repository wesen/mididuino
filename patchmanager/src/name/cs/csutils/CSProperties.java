package name.cs.csutils;

import java.text.DateFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.Properties;

public class CSProperties extends Properties {

    private static final long serialVersionUID = -5222149302146445139L;

    public int getIntegerProperty(String key, int defaultValue) {
        try {
            String value = getProperty(key);
            if (value == null) {
                return defaultValue;
            }
            return Integer.parseInt(value);
        } catch (NumberFormatException ex) {
            return defaultValue;
        }
    }

    public Date getDateProperty(String key, Date defaultValue) {
        try {
            String value = getProperty(key);
            if (value == null) {
                return defaultValue;
            }
            return DateFormat.getDateTimeInstance().parse(value);
        } catch (ParseException ex) {
            return defaultValue;
        }
    }

    public boolean getBooleanProperty(String key, boolean defaultValue) {
        try {
            String value = getProperty(key);
            if (value == null) {
                return defaultValue;
            }
            return Boolean.parseBoolean(value);
        } catch (NumberFormatException ex) {
            return defaultValue;
        }
    }

    public void setDateProperty(String key, Date value) {
        if (value == null) {
            remove(key);
        } else {
            setProperty(key, DateFormat.getDateTimeInstance().format(value));
        }
    }

    public void setIntegerProperty(String key, int value) {
        setProperty(key, Integer.toString(value));
    }

    public void setBooleanProperty(String key, boolean value) {
        setProperty(key, Boolean.toString(value));
    }
    
}
