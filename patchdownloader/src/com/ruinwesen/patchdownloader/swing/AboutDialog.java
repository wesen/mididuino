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
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.Arrays;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;
import name.cs.csutils.RedirectAction;

import com.ruinwesen.patchdownloader.PatchDownloader;

public class AboutDialog {

    private JDialog dialog;
    private JFrame owner; 
    private String title;
    
    public AboutDialog(JFrame owner) {
        init();
        this.owner = owner;
    }
    
    private void init() {
        this.title = I18N.translate("aboutdialog", "About Patchdownloader");
        dialog = new JDialog(owner, title);
        Container pan = dialog.getContentPane();
        pan.setLayout(new BorderLayout());
        
        //HTMLEditorKit kit = new HTMLEditorKit();
        JTextPane textPane = new JTextPane();
        textPane.setContentType("text/html");
        textPane.setEditable(false);
        textPane.setText(generateHTML());
        pan.add(new JScrollPane(textPane), BorderLayout.CENTER);
        JButton btnClose = new JButton(new RedirectAction(this,"closeDialog"));
        btnClose.setDefaultCapable(true);
        dialog.getRootPane().setDefaultButton(btnClose);
        JPanel bottom = new JPanel();
        bottom.setLayout(new BoxLayout(bottom, BoxLayout.X_AXIS));
        bottom.add(Box.createHorizontalGlue());
        bottom.add(btnClose);
        pan.add(bottom, BorderLayout.SOUTH);
        dialog.pack();
        dialog.setModal(true);
    }

    private String generateHTML() {

        String versionLine = I18N.format("aboutdialog.version", "Version: %s", PatchDownloader.Version());
        String bnLine = I18N.format("aboutdialog.build.number", "Build id: %s", PatchDownloader.BuildNumber());
        String btLine = I18N.format("aboutdialog.build.timestamp", "Build time: %s", PatchDownloader.BuildTimestamp());
        String[] contributors = new String[] {
                "Manuel Odendahl",
                "Christian Schneider",
                "Ruin &amp; Wesen"
        };
        Arrays.sort(contributors);
        
        String apache = "This product includes software developed by the "
        +"Apache Software Foundation http://www.apache.org/";
        
        StringBuilder html = new StringBuilder();
        html.append("<html>");
        html.append("<h1>").append(title).append("</h1>");
        html.append("<div>").append(versionLine).append("</div>");
        html.append("<div>").append(bnLine).append("</div>");
        html.append("<div>").append(btLine).append("</div>");
        html.append("<br>");
        html.append("<div>").append("&copy; Patchdownloader contributors 2009. All rights reserved.").append("</div>");
        html.append("<br>");
        html.append("<div>").append(I18N.translate("aboutdialog.contributors.label", "Contributors:")+" ")
            .append(CSUtils.join(", ", contributors))
            .append("</div>");
        html.append("<br>");
        html.append("<div>").append(apache).append("</div>");
        html.append("</html>");
        return html.toString();
    }

    public void closeDialog() {
        dialog.setVisible(false);
    }
    
    public void show() {
        int w = 480;
        int h = 320;
        Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
        int x = (screen.width-w)/2;
        int y = (screen.height-h)/2;
        dialog.setBounds(x, y, w, h);
        dialog.setVisible(true);
    }
    
}
