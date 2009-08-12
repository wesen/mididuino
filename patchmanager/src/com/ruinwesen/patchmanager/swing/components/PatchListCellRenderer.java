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
package com.ruinwesen.patchmanager.swing.components;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.LinearGradientPaint;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.text.DateFormat;
import java.util.Arrays;
import java.util.Date;

import javax.swing.DefaultListCellRenderer;
import javax.swing.JList;
import javax.swing.SwingUtilities;

import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patchmanager.client.index.IndexedPatch;

import name.cs.csutils.CSUtils;
import name.cs.csutils.i18n.I18N;

/**
 * This class renders {@link Doc} instances. 
 * The {@link PatchMetadata metadata} is obtained through the 
 * {@link MetadataCache}.
 * 
 * Note: This cell-renderer relies on a {@link JList} with fixed 
 * cell-width and fixed cell-height.
 * 
 * @author chresan
 * @see {@link JList#setFixedCellWidth(int)}
 * @see {@link JList#setFixedCellHeight(int)}
 */
public class PatchListCellRenderer extends DefaultListCellRenderer {

    /* Note:
     * We implement the paintComponent(java.awt.Graphics) ourselves and bypass
     * the default label rendering. This was necessary because
     * - better performance than a Panel+labels as list cell renderer
     * - better performance than a JLabel+html (the html is necessary
     *   for multiline labels) 
     * - JLabel+html does not truncate the strings if the renderer 
     *   component size is too small
     * - JLabel+html wraps lines and some text may be outside of the 
     *   renderer component
     * 
     * To sum up the problems either bad performance or bad user experience
     * made it necessary to re-implement the painting method. 
     */
    
    /**
     * 
     */
    private static final long serialVersionUID = 7404401364937683131L;
    private String title = "";
    private String titleAddOn = "";
    private String comment = "";
    private String tags = "";
    private String categories = "";
    private Highlighter highlighter = new Highlighter("");
    private boolean paintSelected = false;
    private Color selectionForeground = Color.BLACK;

    // translations
    private String STRING_TAGS;
    private String STRING_CATEGORY;
    
    /**
     * Creates a new patch cell renderer
     * @param metadataCache used to lookup the metadata of the rendered patch
     * @throws IllegalArgumentException the specified metadataCache object is null 
     */
    public PatchListCellRenderer() {
        // clear the label text property so this will not make any problems
        super.setText("");
        STRING_TAGS = I18N.translate("translation.tags", "Tags");
        STRING_CATEGORY = I18N.translate("translation.category", "Category");
    }

    /**
     * We don't use the labels text property and don't allow to modify it.
     */
    @Override
    public void setText(String text) {
        // no op
    }
    
    @Override
    public Component getListCellRendererComponent(
        JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
        PatchMetadata metadata = null;
        if (value == null) {
            // no op
        } else if (value instanceof IndexedPatch) {
            try {
                metadata = ((IndexedPatch)value).getMetadata();
            } catch (Exception ex) {
                metadata = null;
                value = ex;
            }
        } else if (value instanceof PatchMetadata) {
            metadata = (PatchMetadata) value;
        }
        // we keep selected state and selection foreground color so we can use
        // them in the subsequent call to paintComponent(java.awt.Graphics)
        this.paintSelected = isSelected;
        this.selectionForeground = list.getSelectionForeground();
        // setup this cell renderer using the default implementation
        super.getListCellRendererComponent(list, "", index, isSelected, cellHasFocus);
        // read the metadata and update all fields before they are rendered
        update(metadata);
        return this;
    }
    
    /**
     * Sets the query object. The query object is used to highlight the
     * keywords of the query found in the text.
     * @param query the query
     * @throws IllegalArgumentException the specified query object is <code>null</code> 
     */
    public void setHighlighter(Highlighter hl) {
        if (hl == null) {
            hl = new Highlighter("");
        }
        this.highlighter = hl;
    }

    /**
     * The dateformat used to create the date string.
     */
    private DateFormat dateFormat = DateFormat.getDateInstance(DateFormat.LONG);
    
    /**
     * Returns a formated string containing day,month and year of the specified date.
     * @param date a date
     * @return a formated string containing day,month and year of the specified date
     */
    private String dateToString(Date date) {
        return dateFormat.format(date);
    }

    /**
     * Extracts the data from the specified metadata object and updates the
     * fields of this cell renderer accordingly. 
     * The argument may be <code>null</code>.
     * @param metadata a metadata object
     */
    public void update(PatchMetadata metadata) {
        if (metadata == null) {
            this.title = "[Unavailable]";
            this.comment = "";
            this.titleAddOn = "";
            this.tags = "";
            this.categories = "";
        } else {
            this.comment = "";
            this.titleAddOn = "";
            this.tags = "-";
            this.categories = "-";

            if (metadata.getTitle() != null) {
                title = CSUtils.limitString(metadata.getTitle().trim(),30);
            }
            if (metadata.getAuthor() != null) {
                this.titleAddOn += metadata.getAuthor().trim();
            }
            if (metadata.getLastModifiedDate() != null) {
                if (!titleAddOn.isEmpty()) {
                    this.titleAddOn += ", "; 
                }
                this.titleAddOn += dateToString(metadata.getLastModifiedDate());
            }
            if (!titleAddOn.isEmpty()) {
                titleAddOn = " - "+titleAddOn;
            }
            
            if (metadata.getComment() != null) {
                this.comment = highlighter.highlight(metadata.getComment().trim(), "$","$");
            }
            Tagset tagset = metadata.getTags();
            if (!tagset.isEmpty()) {
                Tagset tagsOnly = tagset.filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, false, true);
                Tagset categoryTags = tagset.filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, true, true);
                if (!tagsOnly.isEmpty())
                    tags = highlighter.highlight(tagsOnly.toSortedString(), "$","$");
                if (!categoryTags.isEmpty())
                    categories = highlighter.highlight(categoryTags.toSortedString(), "$","$");
            }
        }
    }
    
    // reuse Insets instances
    private transient Insets cachedInsets ;
    // cachedFont and cachedFontMetrics are both used to cache FontMetrics
    // objects. for 0<=i<cachedFont.length : cachedFontMetrics[i] is the
    // FontMetrics object for the font in cachedFont[i].
    private transient Font[] cachedFont;
    private transient FontMetrics[] cachedFontMetrics; 
    
    /**
     * Returns the insets of this component. The method caches the insets
     * object and reuses it in subsequent calls to this method.
     * @return the insets of this component
     */
    private Insets getInsetsCached() {
        return cachedInsets = getInsets(cachedInsets);
    }
    
    /**
     * Returns the {@link FontMetrics} of the specified font. 
     * The index-argument specifies a slot (0-3) where a cached FontMetrics
     * object can be found and where it can be stored.
     * @param font a font
     * @param index the cache index (0-3)
     * @return the FontMetrics of the specified font
     * @throws IndexOutOfBoundsException if 0>index || index>=4
     */
    private FontMetrics getFontMetricsCached(Font font, int index) {
        // create the arrays 
        if (cachedFont == null || cachedFontMetrics == null) {
            cachedFont = new Font[4];
            cachedFontMetrics = new FontMetrics[4];
        }
        // try to retrieve FontMetrics from cache
        if (cachedFont[index] != null && cachedFont[index].equals(font) 
                && cachedFontMetrics[index] != null) {
            return cachedFontMetrics[index];
        }
        // get font metrics and write them to the cache
        this.cachedFont[index] = font;
        return cachedFontMetrics[index] = getFontMetrics(font);
    }
    
    final Color lightgray = new Color(0xeeeeee);
    
    @Override
    protected void paintComponent(Graphics g) {
        // width and height of this component for further computations
        final int w = getWidth();
        final int h = getHeight();

        Graphics2D g2 = g instanceof Graphics2D ? (Graphics2D) g : null;
        
        // turn on antialiasing
        if (g2 != null) {
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
                    RenderingHints.VALUE_ANTIALIAS_ON);
        }

        // paint the background, getBackground() returns the 
        // selection background color if this item is selected,
        // otherwise it returns the normal background color.
        if (isOpaque()) {
            Color bg = getBackground();
            Color bg2 = CSUtils.newColor(bg, 1.3f);

            // draw gradient
            if (g2 != null) {
                // the gradient is very 
                Paint gradientPaint = new LinearGradientPaint(
                        0, 0, w, 0, new float[] {0f,1f}, 
                        new Color[] {bg, bg2});
                Paint oldPaint = g2.getPaint();
                try {
                    g2.setPaint(gradientPaint);
                    g.fillRect(0, 0, w, h);
                } finally {
                    g2.setPaint(oldPaint);
                }
            }
            else {
                g.setColor(getBackground());
                g.fillRect(0, 0, w, h);
            }
        }

        // the insets of this component
        final Insets insets = getInsetsCached();
        // inner width/height (without insets)
        int iw = w-insets.left-insets.right;
        //int ih = h-insets.top-insets.bottom;

        g.setColor(lightgray);
        g.drawLine(insets.left, h-1, iw, h-1);
        
        // the default foreground color
        final Color fg = getForeground();
        g.setColor(fg);
        
        // prepare all fonts and their fontmetrics
        final Font standardFont = getFont();
        final FontMetrics standardFontMetrics = getFontMetricsCached(standardFont, 0);
        final Font smallFont = standardFont; // CSUtils.changeFontSize(standardFont, 0.9f);
        final FontMetrics smallFontMetrics = getFontMetricsCached(smallFont, 1);
        final Font boldFont = CSUtils.changeFontStyle(standardFont, Font.BOLD);
        final FontMetrics boldFontMetrics = getFontMetricsCached(boldFont, 2);
        final Font smallboldFont = CSUtils.changeFontStyle(smallFont, Font.BOLD);
        final FontMetrics smallboldFontMetrics = getFontMetricsCached(smallboldFont, 3);

        // a blue color
        Color bluer = (paintSelected ? selectionForeground : CSUtils.change(fg, 0, 0, 0.8f));
        // a pale color compared to the default foreground color
        Color paler = (paintSelected ? selectionForeground : CSUtils.change(fg, .67f));
        
        // maximum line height of the all fonts
        final int lineHeight = Math.max(boldFontMetrics.getHeight(),
                    standardFontMetrics.getHeight());
   
        // line 1:  BoldTitle - title addon
        int x = insets.left; // we move x,y for each line/column so we don't have overlapping text 
        int y = insets.top;
        
        String ltitleAddOn = layout(smallFontMetrics, titleAddOn, iw, h);
        int wdate = paintIconR.width;
        
        // line 1:  BoldTitle
        g.setFont(boldFont);
        g.setColor(paler);
        if (title.isEmpty()) title = " ";
        String ltitle = layout(boldFontMetrics, title, Math.max(1,iw-wdate), lineHeight);
        x = paintTextR.x+insets.left;
        y = paintTextR.y+boldFontMetrics.getAscent()+boldFontMetrics.getLeading()+insets.top;
        g.drawString(ltitle, x, y);
        // line 1:  title addon
        g.setFont(smallFont);
        g.setColor(paler); // paler
        g.drawString(ltitleAddOn, x+paintTextR.width, y);
        g.setColor(fg); // restore
        y+= paintTextR.height;

        // line2: comment
        String lcomment = layout(standardFontMetrics, comment, iw, lineHeight);
        g.setFont(standardFont);
        //g.drawString(lcomment, x, y);
        paintHighlighted(g, fg, paler, fg, lineHeight, iw,
                standardFont, standardFontMetrics,
                boldFont, boldFontMetrics,
                "", lcomment,
                x, y);
        y+= paintTextR.height;
        y+= standardFontMetrics.getHeight()/2; // extra padding below comment
        
        // line4: tags
        paintHighlighted(g, fg, paler, bluer, lineHeight, iw,
                smallFont, smallFontMetrics,
                smallboldFont, smallboldFontMetrics,
                STRING_CATEGORY+": ", categories,
                x, y);
        // line3: categories
        y+=lineHeight;
        paintHighlighted(g, fg, paler, bluer, lineHeight, iw,
                smallFont, smallFontMetrics,
                smallboldFont, smallboldFontMetrics,
                STRING_TAGS+": ", tags,
                x, y);
    }

    private void paintHighlighted(Graphics g, 
            Color fg, Color paler, Color bluer, int lineHeight, int iw,
            Font smallFont, 
            FontMetrics smallFontMetrics,
            Font smallboldFont,
            FontMetrics smallboldFontMetrics,
            String label, String value, int x, int y) {

        g.setColor(paler); // pale comment 
        g.setFont(smallFont);
        g.drawString(label, x, y);
        g.setColor(fg); // restore color
        layout(smallFontMetrics, label, iw, lineHeight);
        x+=paintTextR.x + paintTextR.width; // move behind label
        
        g.setColor(bluer);
        
        int restOfLine = iw-x;
        
        // now the complicated part, we have to parse the value string
        // and paint it highlighted
        // 'this $is$ an $example$ string'
        // $this$ is an example $string$
        int count = parseHighlightedString(value);
        for (int i=0;i<count;i++) {
            String str;
            FontMetrics fm;
            if (highlighted[i] != null) {
                str = highlighted[i];
                g.setFont(smallboldFont);
                fm = smallboldFontMetrics;
            } else {
                str = normal[i];
                g.setFont(smallFont);
                fm = smallFontMetrics;
            }

            String lstr = layout(fm, str, restOfLine, lineHeight);
            if (i>0 && x+paintTextR.width>=iw) break;
            
            
            g.drawString(lstr, x, y);
            x+=paintTextR.width;
            restOfLine-=paintTextR.width;
            if (restOfLine<=0) break;
        }
        
    }

    private String[] highlighted = new String[0];
    private String[] normal = new String[0];
    
    private void ensureCapacity(int count) {
        if (highlighted.length<count) {
            highlighted = Arrays.copyOf(highlighted, count+10);
            normal = Arrays.copyOf(normal, count+10);
        }
    }
    
    private int parseHighlightedString(String str) {
        int count = 0;
        int fromIndex = 0;
        int marker = 0;
        Arrays.fill(highlighted, null);
        Arrays.fill(normal, null);
        while (fromIndex<str.length()) {
            int toIndex = str.indexOf('$', fromIndex);
            // we found a marker
            if (toIndex>=0) {
                if (fromIndex<toIndex) { // if false then the string is empty
                    ensureCapacity(++count); // prepare a slot
                    // (fromIndex,toIndex) is normal
                    if ((marker%2)==0) {
                        normal[count-1] = str.substring(fromIndex, toIndex);
                        highlighted[count-1] = null;
                    }
                    // (fromIndex,toIndex) is highlighted
                    else {
                        normal[count-1] = null;
                        highlighted[count-1] = str.substring(fromIndex, toIndex);
                    }
                }
                marker++;
                fromIndex = toIndex+1;
            } else {
                // remaining string is normal and not empty
                if (fromIndex+1<str.length()) {
                    ensureCapacity(++count);
                    normal[count-1] = str.substring(fromIndex);
                    highlighted[count-1] = null;
                }
                break;
            }
        }
        
        return count;
    }
    
    private Rectangle paintViewR = new Rectangle();
    private Rectangle paintIconR = new Rectangle();
    private Rectangle paintTextR = new Rectangle();
    
    private String layout(FontMetrics fm, String text, int width, int height) {
        paintViewR.x = 0;
        paintViewR.y = 0;
        paintViewR.width = width ;
        paintViewR.height = height;
        paintIconR.x = paintIconR.y = paintIconR.width = paintIconR.height = 0;
        paintTextR.x = paintTextR.y = paintTextR.width = paintTextR.height = 0;
        return layoutCL(fm, text, paintViewR, paintIconR, paintTextR);
    }

    protected String layoutCL(                
        FontMetrics fontMetrics, 
        String text, 
        Rectangle viewR, 
        Rectangle iconR, 
        Rectangle textR)
    {
        return SwingUtilities.layoutCompoundLabel(
            this,
            fontMetrics,
            text,
            null,
            this.getVerticalAlignment(),
            this.getHorizontalAlignment(),
            this.getVerticalTextPosition(),
            this.getHorizontalTextPosition(),
            viewR,
            iconR,
            textR,
            this.getIconTextGap());
    }
    
    public int computeFixedCellHeight() {
        final Font standardFont = getFont();
        final FontMetrics standardFontMetrics = getFontMetricsCached(standardFont, 0);
        
        return standardFontMetrics.getHeight()*5;
        
    }

}
