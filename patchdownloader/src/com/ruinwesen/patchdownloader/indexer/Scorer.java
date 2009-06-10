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
package com.ruinwesen.patchdownloader.indexer;

import java.io.Serializable;
import java.util.Arrays;
import java.util.Date;
import java.util.HashSet;
import java.util.Set;
import java.util.regex.Pattern;

import com.ruinwesen.patch.PatchMetadata;
import com.ruinwesen.patch.Tag;
import com.ruinwesen.patch.Tagset;

public abstract class Scorer implements Serializable {

    /**
     * 
     */
    private static final long serialVersionUID = -1587949558946560377L;
    public static final Scorer ONE = new ConstScorer(1f);
    public static final Scorer ZERO = new ConstScorer(0f);
    
    public static final int BEFORE_DATE = 0;
    public static final int EQUALS_DATE = 1;
    public static final int AFTER_DATE = 2;

    public abstract float score(PatchMetadata data);

    public static Scorer authorIs(String name) {
        return new AuthorIs(name);
    }

    public static Scorer fullTextSearch(String search) {
        return new FullTextSearch(search);
    }

    public static Scorer maxTagMatch(String search) {
        return new TagSearchScorer(tokenList(search));
    }

    public static Scorer lastmodifiedDate(Date date, int type) {
        return new LastModifiedDateScorerer(date, type);
    }

    public static Scorer minThreshold(float minScore, Scorer scorer) {
        if (scorer == ZERO || scorer == ONE) return scorer;
        return new MinThresholdScorer(minScore, scorer);
    }

    public static Scorer containsAllTags(Tagset tags) {
        return new ContainsAllTags(tags);
    }

    public static Scorer inverse(Scorer s) {
        if (s == ONE) return ZERO;
        if (s == ZERO) return ONE;
        return new Inverse(s);
    }

    public static Scorer logicAnd(Scorer a, Scorer b) {
        if (a == ONE) return b;
        if (b == ONE) return a;
        if (a == ZERO || b == ZERO) return ZERO;
        return new LogicAnd(a,b);
    }

    public static Scorer weight(Scorer moreImportant, Scorer lessImportant) {
        if (moreImportant == ZERO) {
            return ZERO;
        }
        if (moreImportant == ONE) {
            return lessImportant;
        }
        if (lessImportant == ONE && lessImportant == ZERO) {
            return moreImportant;
        }
        return new WeightScorer(moreImportant, lessImportant);
    }

    private static final Pattern PATTERN =
        Pattern.compile("([^\\w\\d_-]+)"); 
    private static String[] tokenList(String text) {
        String[] split = PATTERN.split(text.toLowerCase());
        Set<String> set = new HashSet<String>(
                Arrays.asList(split));
        set.remove(""); // remove empty string
        return set.toArray(new String[set.size()]);
    }

    private static final class FullTextSearch extends Scorer {
        
        /**
         * 
         */
        private static final long serialVersionUID = 7687452170352190814L;
        private String[] token;
        
        public FullTextSearch(String search) {
            this.token = tokenList(search);
        }
        @Override
        public float score(PatchMetadata data) {
            if (this.token.length == 0) {
                return 1f;
            }
            String[] split = PATTERN.split(getText(data).toLowerCase());
            float score = 0f;
            for (String token: this.token) {
                float tscore = 0f;
                for (String s: split) {
                    int i = s.indexOf(token);
                    if (i>=0) {
                        tscore = Math.max(tscore, (token.length()-i)/(float)token.length());
                        if (tscore == 1) break;
                    }
                }
                score+=tscore;
            }
            return score;
        }
        protected String getText(PatchMetadata data) {
            StringBuilder text = new StringBuilder();
            String author = data.getAuthor();
            if (author != null) {
                text.append(author).append(' ');
            }
            String comment = data.getComment();
            if (comment != null) {
                text.append(comment).append(' ');
            }
            String title = data.getTitle();
            if (title != null) {
                text.append(title).append(' ');
            }
            for (Tag tag: data.getTags()) {
                text.append(tag.value()).append(' ');
            }
            String name = data.getName();
            if (name != null) {
                text.append(name).append(' ');
            }
            return text.toString();
        }
    }
    
    private static abstract class UnaryScorer extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = -5308152071280534862L;
        protected Scorer a;
        public UnaryScorer(Scorer a) {
            this.a = a;
        }
    }
    
    private static abstract class BinaryScorer extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = 2611521047735086746L;
        protected Scorer a;
        protected Scorer b;
        public BinaryScorer(Scorer a, Scorer b) {
            this.a = a;
            this.b = b;
        }
    }
    private static final class Inverse extends UnaryScorer {
        /**
         * 
         */
        private static final long serialVersionUID = -1008238641804205071L;
        public Inverse(Scorer a) {
            super(a);
        }
        @Override
        public float score(PatchMetadata data) {
            return 1f-a.score(data);
        }
    }

    private static final class WeightScorer extends BinaryScorer {
        /**
         * 
         */
        private static final long serialVersionUID = -4776112192823485960L;
        public WeightScorer(Scorer a, Scorer b) {
            super(a, b);
        }
        @Override
        public float score(PatchMetadata data) {
            float s = a.score(data);
            if (s == 0) return 0;
            return (s*.5f+.5f)+(.5f*b.score(data));
        }
    }
    
    
    private static final class LogicAnd extends BinaryScorer {
        /**
         * 
         */
        private static final long serialVersionUID = 7787177708835929775L;
        public LogicAnd(Scorer a, Scorer b) {
            super(a, b);
        }
        @Override
        public float score(PatchMetadata data) {
            float s = a.score(data);
            return s == 0 ? 0 : (s*b.score(data));
        }
    }
    
    private static final class ConstScorer extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = -8278685611746473885L;
        private float score;
        public ConstScorer(float score) {
            this.score = score;
        }
        @Override
        public float score(PatchMetadata data) {
            return score;
        }
    }
    
    private static final class MinThresholdScorer extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = 5432483566441789722L;
        private float minScore;
        private Scorer scorer;
        public MinThresholdScorer(float minScore, Scorer scorer) {
            this.minScore = minScore;
            this.scorer = scorer;
        }
        @Override
        public float score(PatchMetadata data) {
            float score = scorer.score(data);
            return score < minScore ? 0f: score;
        }
    }
    
    public static abstract class DateScorerer extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = 3006924624867948536L;
        private Date date;
        private int type;
        public DateScorerer(Date date, int type) {
            this.date = date;
            this.type = type;
        }
        @Override
        public float score(PatchMetadata data) {
            Date d = getDate(data);
            if (d == null) {
                return 0f;
            }
            int cmp = this.date.compareTo(d);
            return ((type == BEFORE_DATE && cmp>0)
                 || (type == EQUALS_DATE && cmp==0)
                 || (type == AFTER_DATE && cmp>0)) ? 1f : 0f;
        }
        protected abstract Date getDate(PatchMetadata data) ;
    }

    public static class ContainsAllTags extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = -3884063006464633812L;
        private Tagset tags;
        public ContainsAllTags(Tagset tags) {
            this.tags = new Tagset(tags);
        }
        @Override
        public float score(PatchMetadata data) {
            return data.getTags().containsAll(tags) ? 1f:0f;
        }
    }

    private static class TagSearchScorer extends Scorer {
        private static final long serialVersionUID = 1787556662235500892L;
        private String[] token;

        public TagSearchScorer(String[] token) {
            Set<String> list = new HashSet<String>();
            for (int i=0;i<token.length;i++) {
                String s = token[i].trim().toLowerCase();
                if (!s.isEmpty()) {
                    list.add(s);
                }
            }
            this.token = list.toArray(new String[list.size()]);
        }

        @Override
        public float score(PatchMetadata data) {
            if (token.length == 0) {
                return 1;
            }
            
            float score = 0;
            for (Tag t: data.getTags()) {
                float tscore = 0;
                String s = t.normalizedValue();
                for (int i=0;i<token.length && tscore!=1f;i++) {
                    int index = s.indexOf(token[i]);
                    if (index>=0) {
                        tscore = Math.max(tscore, (s.length()-index-1)/(float)(s.length()-1));
                    }
                }
                score += tscore;
            }
            return score/(float)token.length;
        }
    }
    /*
    public static abstract class TextSearchScorer extends Scorer {
        private String[] token;
        public TextSearchScorer(String[] token) {
            Set<String> list = new HashSet<String>();
            for (int i=0;i<token.length;i++) {
                String s = token[i].trim().toLowerCase();
                if (!s.isEmpty()) {
                    list.add(s);
                }
            }
            this.token = list.toArray(new String[list.size()]);
        }
        @Override
        public float score(PatchMetadata data) {
            if (token.length == 0) {
                return 1f;
            }
            String text = getText(data).toLowerCase();
            int matchcount = 0;
            for (String t: token) {
                if (text.indexOf(t)>=0) {
                    matchcount++;
                }
            }
            return matchcount/(float)token.length;
        }
        protected abstract String getText(PatchMetadata data) ;
    }
*/
    private static final class LastModifiedDateScorerer extends DateScorerer {

        /**
         * 
         */
        private static final long serialVersionUID = 1330587114747991398L;

        public LastModifiedDateScorerer(Date date, int type) {
            super(date, type);
        }

        @Override
        protected Date getDate(PatchMetadata data) {
            return data.getLastModifiedDate();
        }
        
    }
    
    private static final class AuthorIs extends Scorer {
        /**
         * 
         */
        private static final long serialVersionUID = 4443971360038070957L;
        private String name;
        public AuthorIs(String name) {
            this.name = name;
        }
        @Override
        public float score(PatchMetadata data) {
            return name.equalsIgnoreCase(data.getAuthor()) ? 1f : 0f;
        }
    }
/*
    private static final class CommentSearch extends TextSearchScorer {
        public CommentSearch(String[] token) {
            super(token);
        }
        @Override
        protected String getText(PatchMetadata data) {
            return data.getComment();
        }
    }

    private static final class TitleSearch extends TextSearchScorer {
        public TitleSearch(String[] token) {
            super(token);
        }
        @Override
        protected String getText(PatchMetadata data) {
            return data.getTitle();
        }
    }
  */
    
    
}
