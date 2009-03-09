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
package name.cs.csutils;

import java.awt.Font;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.channels.Channels;
import java.nio.channels.FileChannel;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Formatter;
import java.util.Iterator;

import javax.swing.JComponent;

import name.cs.csutils.platform.Platform;
import name.cs.csutils.platform.Platform.OS;

public class CSUtils {
    
    public static final String EMPTY_STRING = "";

    private static final
        SimpleDateFormat dateformat = new SimpleDateFormat("yyyy-MM-dd");
    
    public static File getApplicationConfigDir(String name) {
        File userhome = new File(System.getProperty("user.home"));
        if (Platform.isFlavor(OS.UnixFlavor) || Platform.isFlavor(OS.MacOSFlavor)) {
            return new File(userhome,'.'+name);
        } else {
            return new File(userhome,name);
        }
    }
    
    public static Date now() {
        return Calendar.getInstance().getTime();
    }
    
    public static String dateToString(Date date) {
        return dateformat.format(date);
    }

    public static Font changeFontSize(Font font, float factor) {
        int size = (int)Math.round(factor*font.getSize());
        if (size<6) size = Math.max(6, font.getSize());
        return new Font(font.getName(), font.getStyle(), size);
    }

    public static Font changeFontStyle(Font font, int style) {
        return new Font(font.getName(), style, font.getSize());
    }

    public static Font changeFontName(Font font, String name) {
        return new Font(name, font.getStyle(), font.getSize());
    }
    
    public static void changeFontSize(JComponent component, float factor) {
        component.setFont(changeFontSize(component.getFont(), factor));
    }

    public static void changeFontStyle(JComponent component, int style) {
        component.setFont(changeFontStyle(component.getFont(), style));
    }

    public static void changeFontName(JComponent component, String name) {
        component.setFont(changeFontName(component.getFont(), name));
    }
    
    public static Date parseDate(String str) {
        SimpleDateFormat dateformat = new SimpleDateFormat("yyyy-MM-dd");
        try {
            return dateformat.parse(str);
        } catch (ParseException e) {
            return null;
        }
        /*
        // yyyy-mm-dd
        if (str.length() != 10 || 
                str.charAt(4)!='-' || str.charAt(7)!='-') {
            return null;
        }
        int year, month, date;
        try {
            year = Integer.parseInt(str.substring(0,4));
            month = Integer.parseInt(str.substring(5,5+2));
            date = Integer.parseInt(str.substring(8,8+2));
        } catch (NumberFormatException ex) {
            return null;
        }
        Calendar cal = new GregorianCalendar(tz);

        cal.set(year, month, date, 0, 0, 0);  
        
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);

        // TODO 
        //if (!dateToString(cal.getTime()).equals(str))  
        //  throw new RuntimeException(str+","+dateToString(cal.getTime()));
        return cal.getTime();*/
    }
        
    public static String join(String delimiter, Object ... values) {
        if (delimiter == null) {
            delimiter = EMPTY_STRING;
        }
        if (values.length == 0) {
            return EMPTY_STRING;
        } else if (values.length == 1) {
            return String.valueOf(values[0]);
        } else if (values.length == 2) {
            return String.valueOf(values[0])
            +delimiter+String.valueOf(values[1]);
        }
        StringBuilder joined = new StringBuilder();
        joined.append(String.valueOf(values[0]));
        for (int i=1;i<values.length;i++) {
            joined.append(delimiter).append(values[i]);
        }
        return joined.toString();
    }

    public static String join(String delimiter, Iterator<?> iterator) {
        if (delimiter == null) {
            delimiter = EMPTY_STRING;
        }
        if (!iterator.hasNext()) {
            return EMPTY_STRING;
        }
        Object first = iterator.next();
        if (!iterator.hasNext()) {
            return String.valueOf(first);
        }
        StringBuilder joined = new StringBuilder();
        joined.append(first);
        while (iterator.hasNext()) {
            joined.append(delimiter).append(iterator.next());
        }
        return joined.toString();
    }
    
    public static String format(String format, Object ... args) {
        return format(null, format, args);
    }

    public static 
        String format(StringBuilder builder, String format, Object ... args) {
        if (builder == null) {
            builder = new StringBuilder();
        }
        new Formatter(builder).format(format, args);
        return builder.toString();
    }
    
    public static void copy(File file, OutputStream out) throws IOException {
        InputStream in = new BufferedInputStream(new FileInputStream(file));
        try {
            copy(in, out, file.length());
        } finally {
            in.close();
        }
    }

    public static void copy(InputStream in, OutputStream out, long size) throws IOException {
        if (in == null) {
            throw new IllegalArgumentException("in == null");
        }
        if (out == null) {
            throw new IllegalArgumentException("out == null");
        }
        if (size==0) {
            return ;
        }

        if (size>0) {
            if (FileInputStream.class.equals(in.getClass())) {
                FileChannel chin = ((FileInputStream)in).getChannel();
                try {
                    WritableByteChannel chout = Channels.newChannel(out);
                    try {
                        chin.transferTo(0, size, chout);
                    } finally {
                        chout.close();
                    }
                } finally {
                    chin.close();
                }
                return ;
            }
            if (FileOutputStream.class.equals(out.getClass())) {
                FileChannel chout = ((FileOutputStream)out).getChannel();
                try {
                    ReadableByteChannel chin = Channels.newChannel(in);
                    try {
                        chout.transferFrom(chin, 0, size);
                    } finally {
                        chin.close();
                    }
                } finally {
                    chout.close();
                }
                return ;
            }
        }
        
        final int MAX_TRANSFER_SIZE = 8192;
        byte[] buf = new byte[0];
        int r;
        while (true) {
            int available = in.available();
            if (available > 0) {
                if (available>MAX_TRANSFER_SIZE) {
                    available = MAX_TRANSFER_SIZE;
                }
                if (buf.length<available) {
                    buf = new byte[available];
                }
                r = in.read(buf, 0, available);
            } else {
                // if the buffer was used before, then keep its size
                if (buf.length<=0) {
                    buf = new byte[MAX_TRANSFER_SIZE];
                }
                r = in.read(buf, 0, buf.length);
            }
            if (r == -1) {
                break;
            }
            out.write(buf, 0, r);
        }
        
    }

    public static UnsupportedOperationException NotImplementedYet() {
        return new UnsupportedOperationException("TODO: not implemented yet");
    }

    public static String removeFileExtension(String name) {
        if (name.startsWith(".")) {
            return name;
        }
        int idx = name.lastIndexOf('.');
        return idx<0 ? name : name.substring(0, idx);
    }
    
}
