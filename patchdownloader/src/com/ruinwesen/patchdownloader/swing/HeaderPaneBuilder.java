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
package com.ruinwesen.patchdownloader.swing;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.border.Border;

import name.cs.csutils.CSUtils;

public class HeaderPaneBuilder {

    public JPanel headerPane;
    public JPanel container;
    public JLabel headerTitleLabel;
    public Font headerLabelFont;
    
    public HeaderPaneBuilder(String title) {
        super();

        container = new JHeaderContentPane(new BorderLayout());

        Color defaultBackground = container.getBackground();
        //Color backgroundFill = CSUtils.newColor(defaultBackground, 1.1f);
        Color ovalFill = CSUtils.newColor(defaultBackground, 0.85f);
        container.setBackground(ovalFill);


        //Border HEADER_PANE_BORDER = 
        //    BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(ovalFill),
        //    BorderFactory.createEmptyBorder(3, 4, 3, 4)); 
        Border CONTAINER_PANE_BORDER = BorderFactory.createEmptyBorder(4, 10, 4, 10);
        
        container.setBorder(CONTAINER_PANE_BORDER);
        headerTitleLabel = new JLabel(title, SwingConstants.CENTER);
        headerTitleLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        container.setAlignmentX(Component.CENTER_ALIGNMENT);
        headerLabelFont = headerTitleLabel.getFont(); //CSUtils.changeFontSize(headerTitleLabel.getFont(), 0.9f);
        headerLabelFont = CSUtils.changeFontStyle(headerLabelFont, Font.BOLD);
        headerTitleLabel.setFont(headerLabelFont);
        container.add(headerTitleLabel, BorderLayout.CENTER);
        
        headerPane = new JPanel(new BorderLayout());
       // headerPane.setBackground(backgroundFill);
        headerPane.add(container, BorderLayout.CENTER);
       // headerPane.setBorder(HEADER_PANE_BORDER);
        headerPane.setBorder(BorderFactory.createEmptyBorder(2, 2, 0, 2));
    }

    private static class JHeaderContentPane extends JPanel {
        /**
         * 
         */
        private static final long serialVersionUID = 3297244413087848708L;

        public JHeaderContentPane() {
            super();
            setOpaque(false);
        }

        public JHeaderContentPane(BorderLayout borderLayout) {
           super(borderLayout);
           setOpaque(false);
        }

        @Override
        protected void paintComponent(Graphics g) {
            g.setColor(getBackground());
            final int w = getWidth();
            final int h = getHeight();
            if (g instanceof Graphics2D) {
                // draw rectangular area with round borders
                Graphics2D g2 = (Graphics2D) g;
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                int size = Math.min(w, h)-1;
                
                g.fillRect((size)/2, 0, w-size, h-1);
                g2.fillArc(0, 0, size, size, 90, 180);
                g2.fillArc(w-size-1, 0, size, size, 270, 180);
            } else {
                // fallback
                g.fillRect(0, 0, w, h);
            }
        }
        
    }

    
}
