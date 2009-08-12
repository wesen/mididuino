package com.ruinwesen.patchmanager.client.news;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
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
        
        NewsIO io = new NewsIO();
        List<NewsItem> list = new ArrayList<NewsItem>();
        synchronized (INDEX_FILE_LOCK) {
            BufferedReader reader = new BufferedReader(new FileReader(indexFile));
            try {
                for(;;) {
                    NewsItem item = io.read(reader);
                    if (item == null) break;
                    list.add(item);
                }
            } finally {
                reader.close();
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
                PrintWriter os = new PrintWriter(new BufferedWriter(new FileWriter(indexFile, true)));
                try {
                    NewsIO io = new NewsIO();
                    for (NewsItem item: list) {
                        if (!newsIdExists(item.getNewsId())) {
                            newsList.add(item);
                            io.write(item, os);
                        }
                    }        
                    os.flush();
                    Collections.sort(newsList);
                } catch (IOException ex) {
                    os.close();
                    os = null;
                    indexFile.delete();
                    throw ex;
                } finally {
                    if (os != null)
                        os.close();
                }
            }
        }
    }
    
    
}
