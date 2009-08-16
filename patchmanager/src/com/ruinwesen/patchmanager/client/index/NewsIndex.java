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
package com.ruinwesen.patchmanager.client.index;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;

import name.cs.csutils.CSProperties;
import name.cs.csutils.CSUtils;

import com.ruinwesen.patchmanager.client.protocol.NewsItem;
import com.ruinwesen.patchmanager.client.protocol.PatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestGetNews;
import com.ruinwesen.patchmanager.client.protocol.Response;

public class NewsIndex {

    public static final String KEY_LAST_UPDATE = 
        NewsIndex.class.getName()+".lastupdate";
    private List<NewsItem> newsList = new ArrayList<NewsItem>();
    private Date lastUpdate = null;
    private File indexFile;
    private boolean indexFileLoaded = false;
    private final Object INDEX_FILE_LOCK = new Object();
    private final Object NEWS_LIST_LOCK = new Object();
    
    public NewsIndex(File indexFile) {
        super();
        this.indexFile = indexFile;
    }

    public void compileNews(PrintStream out) throws IOException {
        String lu = "-";
        if (lastUpdate != null)
            lu = CSUtils.dateToString(lastUpdate);
        
        URL cssURL = getClass().getResource("/resources/news-client.css");
        String cssStmt = cssURL == null ? "" : (
                "<link rel=\"stylesheet\" type=\"text/css\" href=\""+cssURL+"\"/>");
        
        out.println("<html><head>"+cssStmt+"<title>News - last updated: "+lu+"</title></head><body>");
        synchronized (NEWS_LIST_LOCK) {
            for (NewsItem item: newsList) {
                out.print("<h1>");
                out.print(item.getDateAsString());
                out.print(" - ");
                out.print(item.getTitle());
                out.println("</h1>");
                out.println(item.getMessage());
                out.println();
            }
        }
        out.println("</body></html>\n");
    }
    
    public List<NewsItem> getNewsList() {
        synchronized (NEWS_LIST_LOCK) {
            return Collections.unmodifiableList(new ArrayList<NewsItem>(newsList));
        }
    }

    public void readProperties(CSProperties properties) {
        this.lastUpdate = properties.getDateProperty(KEY_LAST_UPDATE, CSUtils.now());
    }

    public void writeProperties(CSProperties properties) {
        if (lastUpdate != null)
        properties.setDateProperty(KEY_LAST_UPDATE, lastUpdate);
    }
    
    private boolean newsIdExists(int id) {
        for (NewsItem item: newsList)
            if (item.getNewsId() == id) 
                return true;
        return false;
    }
    
    public void readIndex() throws IOException {
        synchronized (NEWS_LIST_LOCK) {
        this.newsList.clear();
        }
        synchronized (INDEX_FILE_LOCK) {
            if (!indexFile.exists()) { 
                indexFileLoaded = true;
                return;
            }
        }
        
        List<NewsItem> list = new ArrayList<NewsItem>();
        synchronized (INDEX_FILE_LOCK) {
            RecordIO io = new RecordIO(indexFile, RecordIO.READ);
            try {
                while (io.hasMore()) {
                    list.add(readNewsRecord(io));
                }
            } finally {
                io.close();
            }
        }
        Collections.sort(list);
        synchronized (NEWS_LIST_LOCK) {
        this.newsList = list;
        }
        synchronized (INDEX_FILE_LOCK) {
            indexFileLoaded = true;
        }
    }
    
    public int updateNews(PatchManagerClient client) throws Exception {
        synchronized (NEWS_LIST_LOCK) {
            if (!indexFileLoaded) {
                readIndex();
            }
        }
        
        Request request = new RequestGetNews(lastUpdate);
        Response response;
        try {
            response = client.execute(request);
        } catch (Exception ex) {
            throw ex;
        }
        
        int newMessagesCount = 0;
        if (response.isOkStatus() && !response.getNewsList().isEmpty()) {
            synchronized (NEWS_LIST_LOCK) {
                int newsbefore= newsList.size();
                appendToIndexFile(response.getNewsList());
                newMessagesCount = Math.max(0, newsList.size()-newsbefore);
            }
        }        
        
        lastUpdate = CSUtils.now();
        return newMessagesCount;
    }

    private void appendToIndexFile(List<NewsItem> list) throws IOException {
        if (list.isEmpty()) {
            return;
        }
        synchronized (NEWS_LIST_LOCK) {
            synchronized (INDEX_FILE_LOCK) {
                if (indexFile == null) {
                    for (NewsItem item: list) {
                        if (!newsIdExists(item.getNewsId())) {
                            newsList.add(item);
                        }
                    }
                    Collections.sort(newsList);
                }
                try {
                    RecordIO io = null;
                    try {
                        io = new RecordIO(indexFile, RecordIO.APPEND);
                        for (NewsItem item: list) {
                            if (!newsIdExists(item.getNewsId())) {
                                newsList.add(item);
                                writeNewsRecord(io, item);
                            }
                        }      
                        io.flush();
                        Collections.sort(newsList);
                    } finally {
                        io.close();
                    }
                } catch (IOException ex) {
                    indexFile.delete();
                    throw ex;
                }
            }
        }
    }

    private NewsItem readNewsRecord(RecordIO io) throws IOException {        
        int newsid = io.readInteger();
        String date = io.readString();
        String title = io.readString();
        String message = io.readString();
        return new NewsItem(newsid, date, title, message);
    }
    
    private void writeNewsRecord(RecordIO io, NewsItem record) throws IOException {
        io.writeInteger(record.getNewsId());
        io.writeString(record.getDateAsString(),"");
        io.writeString(record.getTitle(),"");
        io.writeString(record.getMessage(),"");
    }
    
    
}
