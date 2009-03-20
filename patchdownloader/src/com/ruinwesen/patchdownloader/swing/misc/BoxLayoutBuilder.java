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
package com.ruinwesen.patchdownloader.swing.misc;

import java.awt.Component;
import java.awt.Container;
import java.awt.LayoutManager;
import java.util.LinkedList;

import javax.swing.Box;
import javax.swing.BoxLayout;

/**
 * Helper class to build BoxLayouts.
 * @author chresan
 */
public class BoxLayoutBuilder {

    private Container rootContainer;
    private LinkedList<ContainerInfo> stack = new LinkedList<ContainerInfo>();
    
    public BoxLayoutBuilder(Container container) {
        LayoutManager lm = container.getLayout();
        if (!(lm instanceof BoxLayout)) {
            throw new IllegalArgumentException("container must use BoxLayout");
        }
        BoxLayout bl = (BoxLayout) lm;

        boolean horizontalLayout = (bl.getAxis() == BoxLayout.LINE_AXIS
                || bl.getAxis() == BoxLayout.X_AXIS);
        init(container, horizontalLayout);
    }

    public BoxLayoutBuilder(Container container, boolean horizontalLayout) {
        if (container == null) {
            throw new IllegalArgumentException("container == null");
        }
        init(container, horizontalLayout);
    }

    private void init(Container container, boolean horizontalLayout) {
        this.rootContainer = container;
        stack.add(new ContainerInfo(container, horizontalLayout));
    }

    public Container getRootContainer() {
        return rootContainer;
    }
    
    public void add(Component c) {
        stack.getLast().c.add(c);
    }
    
    public void addGlue() {
        add(isHorizontalLayout()
                ? Box.createHorizontalGlue()
                : Box.createVerticalGlue());
    }

    public void addStrut(int size) {
        add(isHorizontalLayout()
                ? Box.createHorizontalStrut(size)
                : Box.createVerticalStrut(size));
    }
    
    private boolean isHorizontalLayout() {
        return stack.getLast().horizontal;
    }

    public void beginHorizontalBox() {
        beginContainer(Box.createHorizontalBox(), true);
    }
    
    public void beginVerticalBox() {
        beginContainer(Box.createVerticalBox(), false);
    }
    
    public void beginContainer(Container c, boolean horizontal) {
        add(c);
        stack.add(new ContainerInfo(c, horizontal));
    }
 
    public void endContainer() {
        if (stack.size()<=1) {
            throw new IllegalStateException("stack is empty");
        }
        stack.removeLast();
    }
    
    private static class ContainerInfo {
        private Container c;
        private boolean horizontal;
        public ContainerInfo(Container c, boolean horizontal) {
            this.c=c;
            this.horizontal = horizontal;
        }
    }
    
}
