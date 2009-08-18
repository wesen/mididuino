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
package com.ruinwesen.patchmanager.client.protocol;

public class NewsItem implements Comparable<NewsItem> {

    private String date;
    private String message;
    private String title;
    private int newsid;
    
    public NewsItem(int newsid, String date, String title, String message) {
        if (date == null || message == null) {
            throw new IllegalArgumentException("date:"+date+",message:message");
        }
        this.newsid = newsid;
        this.title = title == null ? "" : title;
        this.date = date;
        this.message = message;
    }
    public int getNewsId() {
        return newsid;
    }
    
    public String getDateAsString() {
        return date;
    }
    
    public String getMessage() {
        return message;
    }
    
    public String getTitle() {
        return title;
    }
    
    @Override
    public String toString() {
        return "NewsItem[date="+date+",title='"+title+"',message='"+message+"']";
    }

    public int compareTo(NewsItem b) {
        int cmp = -date.compareTo(b.date);
        if (cmp == 0) {
            cmp = -(newsid-b.newsid);
        }
        return cmp;
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        }
        if (o == this) {
            return true;
        }
        if (!(o instanceof NewsItem)) {
            return false;
        }
        NewsItem i = (NewsItem)o;
        
        return date.equals(i.date) && title.equals(i.title) && message.equals(i.message);
    }
    
}
