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
package com.ruinwesen.patchdownloader.swing.extra;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.Point;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.net.URL;

import javax.swing.ImageIcon;
import javax.swing.JComponent;

import name.cs.csutils.Platform;

public class RatingBar extends JComponent {

    /**
     * 
     */
    private static final long serialVersionUID = -199582110228234543L;
    
    public static ImageIcon ENABLED_ICON_SMALL = 
        createIcon("/resources/star-small.png");
    public static ImageIcon DISABLED_ICON_SMALL = 
        createIcon("/resources/star-disabled-small.png");

    public static ImageIcon ENABLED_ICON_BIG = 
        createIcon("/resources/star.png");
    public static ImageIcon DISABLED_ICON_BIG = 
        createIcon("/resources/star-disabled.png");
    
    private static ImageIcon createIcon(String resource) {
        URL url = RatingBar.class.getResource(resource);
        return url != null ? new ImageIcon(url) : new ImageIcon(resource);
    }
    
    private ImageIcon enabledIcon = ENABLED_ICON_BIG;
    private ImageIcon disabledIcon = DISABLED_ICON_BIG;

    private int rating = 2;
    private int ratingRange = 5;
    private int separator = 2;
    private int previewRating = -1;
    
    public RatingBar() {
        setOpaque(false);
        updatePreferredSize();
        new EventHandler().install(this);
    }
    
    public void useDefaultSmallIconSet() {
        this.enabledIcon = ENABLED_ICON_SMALL;
        this.disabledIcon = DISABLED_ICON_SMALL;
    }
    
    public void useDefaultBigIconSet() {
        this.enabledIcon = ENABLED_ICON_BIG;
        this.disabledIcon = DISABLED_ICON_BIG;
    }

    protected void setPreviewRating(int previewRating) {
        if (previewRating >= 1) {
            previewRating = Math.max(1,Math.min(ratingRange, previewRating));
        }
        if (this.previewRating != previewRating) {
            this.previewRating = previewRating;
            repaint();
        }
    }

    public int getRating() {
        return rating;
    }
    
    public void setRatingRange(int ratingRange) {
        if (ratingRange <2) {
            throw new IllegalArgumentException("ratingRange<2");
        }
        if (this.ratingRange != ratingRange) {
            this.ratingRange = ratingRange;
            updatePreferredSize();
        }
    }

    public void setRating(int rating) {
        rating = Math.max(1,Math.min(ratingRange, rating));
        if (this.rating != rating) {
            this.rating = rating;
            repaint();
        }
    }

    public void setEnabledIcon(ImageIcon icon) {
        this.enabledIcon = icon;
        updatePreferredSize();
        repaint();
    }

    @Override
    public Dimension getMinimumSize() {
        return getPreferredSize();
    }

    @Override
    public Dimension getMaximumSize() {
        return getPreferredSize();
    }

    @Override
    public Dimension getPreferredSize() {
        int ih = Math.max(enabledIcon.getIconHeight(),2);
        Insets insets = getInsetsAndUseCache();
        Dimension size = new Dimension(
                insets.left+insets.right+
                computeXarea(),
                insets.top+insets.bottom+
                ih);
        return size;
    }
    
    private transient Insets cachedInsets;
    
    private Insets getInsetsAndUseCache() {
        return cachedInsets = getInsets(cachedInsets);
    }
    
    private void updatePreferredSize() {
        setPreferredSize(getPreferredSize());
    }

    public void setDisabledIcon(ImageIcon icon) {
        this.disabledIcon = icon;
        repaint();
    }

    public ImageIcon getEnabledIcon() {
        return this.enabledIcon;
    }

    public ImageIcon getDisabledIcon() {
        return this.disabledIcon;
    }
    
    private int centerY(Insets insets, int height) {
        return Math.max(insets.top,(getHeight()-height)/2); 
    }
    
    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Insets insets = getInsetsAndUseCache();

        int iw = enabledIcon.getIconWidth();
        int ih = enabledIcon.getIconHeight();
        if (iw<=0 || ih<=0) {
            return;
        }
        int y = centerY(insets, ih);
        
        int useRating = previewRating != -1 ? previewRating : rating;

        int x=insets.left, r=1;
        for(;x<getWidth()&&r<=useRating;x+=iw+separator,r++){
            enabledIcon.paintIcon(this, g, x, y);
        }
        for(;x<getWidth()&&r<=ratingRange;x+=iw+separator,r++){
            disabledIcon.paintIcon(this, g, x, y);
        }
        
    }
    
    private int computeXarea() {
        int iw = Math.max(enabledIcon.getIconWidth(),5);
        return ratingRange*iw+(ratingRange-1)*separator;
    }
    
    protected int getRatingForPoint(Point point) {

        Insets insets = getInsetsAndUseCache();

        
        int iw = enabledIcon.getIconWidth();
        int ih = enabledIcon.getIconHeight();
        if (iw<=0 || ih<=0) {
            return -1; // outside
        }

        float miny = centerY(insets, ih);
        if (point.y<miny || point.y>=miny+ih) {
            return -1; // outside
        }

        int xarea = computeXarea();
        if (point.x<insets.left || point.x>=xarea+insets.left || xarea<=0) {
            return -1;
        }

        int x = point.x-insets.left;
        int r=1;
        for (;r<=ratingRange;r++) {
            if (x<iw+separator/2) break;
            x-=iw;
            x-=separator;
        }
        return r;
        /*
        int nosep = separator*(ratingRange-1);
        float factor = ((point.x-insets.left)/(float)(xarea-nosep));
        int rating = (int)Math.round(((ratingRange) * factor));
        return Math.max(1, Math.min(ratingRange, rating));
        */
    }
    
    private class EventHandler extends MouseAdapter implements FocusListener,
        KeyListener {
        public void install(RatingBar bar) {
            bar.addMouseListener(this);
            bar.addMouseMotionListener(this);
            bar.addFocusListener(this);
            bar.setFocusable(true);
            bar.addKeyListener(this);
        }
        @Override
        public void mouseMoved(MouseEvent e) {
            setPreviewRating(getRatingForPoint(e.getPoint())) ;
        }
        @Override
        public void mouseExited(MouseEvent e) {
            setPreviewRating(-1) ;
        }
        @Override
        public void mouseClicked(MouseEvent e) {
            setPreviewRating(-1) ;
            setRating(getRatingForPoint(e.getPoint())) ;
            requestFocus();
        }
        @Override
        public void focusGained(FocusEvent e) {
            // no op
        }
        @Override
        public void focusLost(FocusEvent e) {
            setPreviewRating(-1);
        }
        @Override
        public void keyPressed(KeyEvent e) {
            handleKey(e);
        }
        @Override
        public void keyReleased(KeyEvent e) {
            handleKey(e);
        }
        @Override
        public void keyTyped(KeyEvent e) {
            // no op
        }
        public void handleKey(KeyEvent e) {
            if (Platform.canPerformKeyAction(e)) {
                if (e.getKeyCode() == KeyEvent.VK_PLUS) {
                    setPreviewRating(-1); // on mouse over the changes wouldn't be visible
                    setRating(getRating()+1);   
                } else if (e.getKeyCode() == KeyEvent.VK_MINUS) {
                    setPreviewRating(-1); // on mouse over the changes wouldn't be visible
                    setRating(getRating()-1);
                }
            }
        }
        
    }

}
