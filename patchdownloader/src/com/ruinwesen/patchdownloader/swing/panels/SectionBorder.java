package com.ruinwesen.patchdownloader.swing.panels;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.LinearGradientPaint;
import java.awt.Paint;
import java.awt.RenderingHints;
import java.awt.geom.Rectangle2D;

import javax.swing.border.Border;

import name.cs.csutils.CSUtils;

public class SectionBorder implements Border {

    private String title;
    private transient Font baseFont;
    private transient Font boldFont;
    private transient FontMetrics cachedBoldFontMetrics;

    private static final int PAD = 3;
    private static final int TB_PAD = 2;

    public SectionBorder(String title) {
        this.title = title;
    }
    
    public void setTitle(String text )  {
        this.title = text;
    }
    
    private Font getBoldFont(Component c) {
        Font font = c.getFont();
        if (baseFont == null || boldFont != null || !baseFont.equals(font)) {
            baseFont = font;
            boldFont = CSUtils.changeFontStyle(font, Font.BOLD);;
            cachedBoldFontMetrics = null;
        }
        return boldFont; 
    }
    
    private FontMetrics getBoldFontMetrics(Component c) {
        Font font = getBoldFont(c);
        if (cachedBoldFontMetrics == null) {
            cachedBoldFontMetrics = c.getFontMetrics(font);
        }
        return cachedBoldFontMetrics;
    }
    

    @Override
    public Insets getBorderInsets(Component c) {
        return getBorderInsets(null, c);
    }

    private Insets getBorderInsets(Insets insets, Component c) {
        FontMetrics fm = getBoldFontMetrics(c);
        int t = height(fm);
        if (insets == null) {
            insets = new Insets(t, 0, 0, 0);
        } else {
            insets.set(t, 0, 0, 0);
        }
        return insets;
    }

    private int height(FontMetrics fm) {
        return fm.getAscent()+fm.getDescent()+fm.getLeading()+2*PAD
        +TB_PAD*2;
    }
    
    @Override
    public boolean isBorderOpaque() {
        return false;
    }

    @Override
    public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
        // init
        Font font = getBoldFont(c);
        FontMetrics fm = getBoldFontMetrics(c);
        
        final int lrpad = 2; 
        
        final int w = c.getWidth()-lrpad*2;
        int h = height(fm);

        Color bg = CSUtils.newColor(c.getBackground(), 0.85f);
        g.setColor(bg);
        if (g instanceof Graphics2D) {
            // draw rectangular area with round borders
            Graphics2D g2 = (Graphics2D) g;
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            int size = Math.min(w, h-TB_PAD*2)-1;

            Color bg_bright = CSUtils.newColor(bg, 1.05f);

            Paint gradientPaint = new LinearGradientPaint(
                    0, 0, 0, h, new float[] {0f,0.6f,1f}, 
                    new Color[] {bg_bright, bg, bg_bright});
            Paint oldpaint = g2.getPaint();
            g2.setPaint(gradientPaint);
            g2.fillRect(lrpad+(size)/2, TB_PAD, w-size, h-TB_PAD*2-1);
            g2.fillArc(lrpad+0, TB_PAD, size, size, 90, 180);
            g2.fillArc(lrpad+w-size-1, TB_PAD, size, size, 270, 180);
            g2.setPaint(oldpaint);
        } else {
            // fallback
            g.fillRect(lrpad, TB_PAD, w, h-TB_PAD*2);
        }
        
        if (title != null) {
            g.setFont(font);
            Rectangle2D titleBounds = fm.getStringBounds(title, g);
            if (titleBounds.getWidth() < w) {
                g.setColor(c.getForeground());
                int sx = (int)((w-titleBounds.getWidth())/2);
                int sy = h/2+fm.getLeading()+fm.getDescent()+fm.getDescent()/2;
                g.drawString(title, lrpad+sx, sy);
            }
        }
        
    }
    
}
