package com.ruinwesen.patchmanager.client.news;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import com.ruinwesen.patchmanager.client.protocol.NewsItem;

public class NewsIO {

    private StringBuilder sb = new StringBuilder();
    
    private String escape(String str) {
        if (str.indexOf('\n')<0 && str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                sb.append("\\\\");
            } else if (ch == '\n') {
                sb.append("\\n");
            } else {
                sb.append(ch);
            }
        }
        return sb.toString();
    }
    
    private String unescape(String str) {
        if (str.indexOf('\\')<0) {
            return str;
        }
        sb.setLength(0);
        for (int i=0;i<str.length();i++) {
            char ch = str.charAt(i);
            if (ch == '\\') {
                if (i+1<str.length()) {
                    if (str.charAt(i+1)=='n') {
                        sb.append('\n');
                        continue;
                    } else if (str.charAt(i+1)=='\\') {
                        sb.append('\\');
                        continue;
                    }
                }
            }
            sb.append(ch);
        }
        return sb.toString();
    }
    
    private final String getLine(BufferedReader reader) throws IOException {
        String line = reader.readLine();
        if (line == null) {
            throw new IOException("incomplete record");
        }
        return unescape(line);
    }

    public NewsItem read(BufferedReader reader) throws IOException {
        String newsidStr = reader.readLine();
        if (newsidStr == null) { // last line
            return null;
        }
        
        int newsid;
        try {
            newsid = Integer.parseInt(newsidStr);
        } catch (NumberFormatException ex) {
            throw new IOException("index corrupted");
        }
        String date = getLine(reader);
        String title = getLine(reader);
        String message = getLine(reader);
        return new NewsItem(newsid, date, title, message);
    }
    
    public void write(NewsItem record, PrintWriter w) throws IOException {
        writeLine(w, Integer.toString(record.getNewsId()));
        writeLine(w, record.getDateAsString());
        writeLine(w, escape(record.getTitle()));
        writeLine(w, escape(record.getMessage()));
    }
    
    private void writeLine(PrintWriter writer, Object str) throws IOException {
        writer.println(str == null ? "" : escape(String.valueOf(str)));
    }
    
}
