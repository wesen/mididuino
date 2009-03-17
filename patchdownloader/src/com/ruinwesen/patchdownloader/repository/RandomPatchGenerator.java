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
package com.ruinwesen.patchdownloader.repository;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Toolkit;
import java.io.File;
import java.io.IOException;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

import name.cs.csutils.CSUtils;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.RedirectAction.RedirectActionMeta;

import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.indexer.IndexReader;
import com.ruinwesen.patchdownloader.indexer.MetadataIndexWriter;
import com.ruinwesen.patchdownloader.patch.PatchFileBuilder;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.Tag;
import com.ruinwesen.patchdownloader.patch.Tagset;
import com.ruinwesen.patchdownloader.swing.PDFrame;

public class RandomPatchGenerator {

    private void generateToken(StringBuilder sb, int minLen, int maxLen) {
        if (minLen>maxLen || minLen <0) {
            throw new IllegalArgumentException("invalid length:min-len="+minLen+",max-len="+maxLen);
        }
        int len = minLen+(int)((maxLen-minLen)*Math.random());
        final String CHARS = "abcDEF012";
        char[] resultChars = new char[len];
        for (int i=0;i<len;i++)
            resultChars[i] = CHARS.charAt((int)((CHARS.length()-1)*Math.random()));
        sb.append(resultChars);
    }
    
    private void generateTokenList(StringBuilder sb, int minListLen, int maxListLen, int minTokenLen, int maxTokenLen) {
        if (minListLen>maxListLen || minListLen <0) {
            throw new IllegalArgumentException("invalid length:min-len="+minListLen+",max-len="+maxListLen);
        }
        int len = minListLen+(int)((maxListLen-minListLen)*Math.random());
        if (len>0) {
            generateToken(sb, minTokenLen, maxTokenLen);
            for (int i=1;i<len;i++) {
                sb.append(' ');
                generateToken(sb, minTokenLen, maxTokenLen);
            }
        }
    }

    private static Tag[] MASTER_SET = Tagset.parseTags("sampler synth generator drums sequence up down").toTagArray();

    private static Tag[] CATEGORY_SET = Tagset.parseTags("category:cat1 category:cat2 category:cat3 category:delay category:generator category:misc category:sequencer").toTagArray();

    private static String[] AUTHORS = new String[]{
        "DR.Who", "Heinz", "Karlheinz", "Hans-August", "Bernd",
        "Claudia", "Gertrude", "Steffi", "Ingrid", "Lydia"
    };
    
    private void randomize(StringBuilder sb, PatchMetadata metadata) {
        sb.setLength(0);
        generateToken(sb, 3, 10);
        metadata.setTitle(sb.toString());

        int aindex = (int)Math.round(Math.random()*(AUTHORS.length-1));
        metadata.setAuthor(AUTHORS[aindex]);
        
        sb.setLength(0);
        generateToken(sb, 3, 10);
        metadata.setName(sb.toString());

        sb.setLength(0);
        generateTokenList(sb, 0, 10, 3, 4);
        

        int maxcount = (int) (MASTER_SET.length*Math.random()); 
        Tagset tagset = new Tagset();
        for (int i=0;i<maxcount;i++) {
            int index = (int)((MASTER_SET.length-1)*Math.random());
            tagset.add(MASTER_SET[index]); // duplicates are removed
        }
        { // one category
            int index = (int)((CATEGORY_SET.length-1)*Math.random());
            tagset.add(CATEGORY_SET[index]); 
        }
        tagset.add("category:minicommand"); 
        
        metadata.setTags(tagset);
        
        String day = ""+(int)(Math.random()*30)+1;
        if (day.length()<=1) day = "0"+day;
        String date = "2009-03-"+day;
        
        metadata.setLastModifiedDate(CSUtils.parseDate(date));
        
        sb.setLength(0);
        generateTokenList(sb, 0, 30, 3, 10);
        metadata.setComment(sb.toString());        
    }
    
    private void buildRandomizedPatch(StringBuilder sb, PatchFileBuilder builder, File dstDir) throws Exception {
        // the sourcecode / midifile properties must be defined in advance
        sb.setLength(0);
        randomize(sb, builder.getMetadata());
        File dstFile;
        while (true) {
            sb.setLength(0);
            generateToken(sb, 5, 17);
            sb.append(".rwp");
            dstFile = new File(dstDir, sb.toString());
            if (!dstFile.exists()) break;
        }
        builder.setOutput(dstFile);
        builder.build();
    }
    
    public void showDialog(Component parentComponent) {
        // first ask how many patches we should to generate
        int count = -1;
        String result = JOptionPane.showInputDialog(parentComponent, "How many patches should be generated ?", "10");
        if (result == null) {
            return;
        }
        try {
            count = Integer.parseInt(result);
        } catch (NumberFormatException ex) {
            count = -1;
        }
    
        if (count==0) {
            return;
        } else if (count<0) {
            JOptionPane.showMessageDialog(parentComponent, "Invalid input: "+result+"\n. No patches were generated.", "Warning", JOptionPane.WARNING_MESSAGE);
            return;
        }
        
        ProgressDialog dialog = new ProgressDialog(parentComponent, count);
        dialog.show();
        GeneratorThread gt = new GeneratorThread(dialog, count);
        gt.start();
    }
    
    private final class GeneratorThread extends Thread {
        private ProgressDialog dialog;
        private volatile long generatorIsRunning = 1;
        private int generated;
        private int generateTotal;

        public GeneratorThread(ProgressDialog dialog, int generateCount) {
            this.dialog = dialog;
            this.generated = 0;
            this.generateTotal = generateCount;
        }

        public final void stopGenerator() {
            this.generatorIsRunning = 0;
        }
        
        public final boolean isGeneratorRunning() {
            return generatorIsRunning == 1;
        }

        @Override
        public void run() {
            try {
                PatchDownloader pd = PatchDownloader.getSharedInstance();
                IndexReader ir = pd.getPatchIndex();
                PatchFileBuilder builder = new PatchFileBuilder();
                StringBuilder sb = new StringBuilder();

                File tmpMidiFile;
                try {
                    tmpMidiFile = File.createTempFile("generated-midifile", ".hex");
                } catch (IOException ex) {
                    ex.printStackTrace();
                    return;
                }

                File tmpSrcFile;
                try {
                    tmpSrcFile = File.createTempFile("generated-source", ".c");
                } catch (IOException ex) {
                    ex.printStackTrace();
                    tmpSrcFile = null;
                }

                dialog.setStatusMessage("Generating...");
                try {
                    MetadataIndexWriter writer = ir.createAppendableIndexWriter();
                    
                    final long updateResolution = 150;
                    long nextUpdate = System.currentTimeMillis()+updateResolution;
                    
                    builder.setMidifile(tmpMidiFile);
                    if (tmpSrcFile != null) {
                        builder.setSourceCode(tmpSrcFile);
                    }
                    File dstDir = pd.getRemoteRepositoryBackup().getBaseDir();
                    while (isGeneratorRunning() && !dialog.isAbortRequested()) {
                        if (generated>=generateTotal) { break; }
                        
                        if (System.currentTimeMillis()>nextUpdate) {
                            dialog.setStatusMessage("Generated "+generated+" of "+generateTotal);
                            dialog.setProgress(generated);
                            nextUpdate = System.currentTimeMillis()+updateResolution;
                        }
                        
                        // build randomized patch
                        randomize(sb, builder.getMetadata());
                        try {
                            buildRandomizedPatch(sb, builder, dstDir);
                        } catch (Exception ex) {
                            ex.printStackTrace();
                            break;
                        }
                        
                        
                        int docid = writer.newDocument(builder.getOutput().getName());
                        writer.putMetadata(docid,builder.getMetadata());
                        generated++;
                    }
                    dialog.setProgress(generateTotal);
                    writer.flush();
                    writer.close();
                    ir.update(); // update the current reader
                } catch (IOException ex) {
                    ex.printStackTrace();
                } finally {
                    tmpMidiFile.delete();
                    if (tmpSrcFile != null) {
                        tmpSrcFile.delete();
                    }
                }
            } finally {
                stopGenerator();
                dialog.closeDialog();
            }
            
            SwingUtilities.invokeLater(new Runnable(){
                public void run() {
                    PatchDownloader pd = 
                        PatchDownloader.getSharedInstance();
                    if (pd != null && pd instanceof PDFrame) {
                        ((PDFrame)pd).getSearchController().indexChanged();
                        ((PDFrame)pd).getSearchController().update();
                    }
                }
            });
        }
    }

    public final static class ProgressDialog {
        private Component parentComponent;
        private JDialog dialog;
        private JButton btnClose;
        private JLabel labelStatusMessage;
        private JProgressBar progressBar;
        private volatile long statusMessageSetCounter = 0;
        private volatile long abort = 0;

        public ProgressDialog(Component parentComponent, int maximum) {
            this.parentComponent = parentComponent;
            init();
            progressBar.setMaximum(maximum);
        }
        
        public void setProgress(final int value) {
            if (SwingUtilities.isEventDispatchThread()) {
                progressBar.setValue(value);
            } else {
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        setProgress(value);
                    }
                });
            }
        }
        
        public void closeDialog() {
            if (SwingUtilities.isEventDispatchThread()) {
                dialog.setVisible(false);
                dialog.dispose();
            } else {
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        closeDialog();
                    }
                });
            }
        }
        /**
         * This operation is thread save.
         * @param text
         */
        public final void setStatusMessage(final String text) {
            final long currentCounter = ++statusMessageSetCounter;
            if (SwingUtilities.isEventDispatchThread()) {
                labelStatusMessage.setText(text);
            } else {
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        if (currentCounter == statusMessageSetCounter) {
                            labelStatusMessage.setText(text);
                        } // otherwise ignore the operation
                    }
                });
            }
        }
        public void init() {
            if (parentComponent != null) {
                dialog = new JDialog(SwingUtilities.getWindowAncestor(parentComponent));
            } else {
                dialog = new JDialog();
            }
            dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            JPanel pan = new JPanel();
            pan.setBorder(BorderFactory.createEmptyBorder(4,4,4,4));
            dialog.setContentPane(pan);
            pan.setLayout(new BoxLayout(pan, BoxLayout.Y_AXIS));
            
            {
                progressBar = new JProgressBar();
                progressBar.setIndeterminate(false);
                progressBar.setMinimum(0);
                progressBar.setMaximum(1);
                progressBar.setValue(0);
                Box box = Box.createHorizontalBox();
                box.add(progressBar);
                pan.add(box);
            }
            createSeparator(pan);
            {
                JLabel statusLabel = new JLabel("Status:");
                labelStatusMessage = new JLabel();
                statusLabel.setLabelFor(labelStatusMessage);

                statusLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
                labelStatusMessage.setAlignmentX(Component.LEFT_ALIGNMENT);
                Box box = Box.createHorizontalBox();
                box.add(statusLabel);
                box.add(Box.createHorizontalStrut(5));
                box.add(labelStatusMessage);
                box.add(Box.createHorizontalGlue());
                pan.add(box);
            }
            createSeparator(pan);
            {
                btnClose = new JButton(new RedirectAction(this, "btnAbortClick"));
                btnClose.setAlignmentX(Component.RIGHT_ALIGNMENT);
                Box box = Box.createHorizontalBox();
                box.add(Box.createHorizontalGlue());
                box.add(btnClose);
                pan.add(box);
            }
        }
        
        private void createSeparator(JPanel pan) {
            pan.add(Box.createVerticalStrut(5));
        }
        public void show() {
            dialog.pack();
            if (dialog.getWidth()<300) {
                dialog.setSize(300,dialog.getHeight());
            }
            
            Dimension parentSize;
            Point parentLocation = new Point(0,0);
            
            if (parentComponent != null) {
                SwingUtilities.convertPointToScreen(parentLocation, parentComponent);
                parentSize = parentComponent.getSize();
            } else {
                parentSize = Toolkit.getDefaultToolkit().getScreenSize();
            }

            int dx = parentLocation.x+(parentSize.width-dialog.getWidth())/2;
            int dy = parentLocation.x+(parentSize.height-dialog.getHeight())/2;
            dialog.setLocation(dx, dy);
            
            dialog.setResizable(false);
            dialog.setVisible(true);
        }
        
        @RedirectActionMeta(title="Abort")
        public void btnAbortClick() {
            abort = 1;
            closeDialog();
        }
        public boolean isAbortRequested() {
            return abort != 0;
        }
    }
    
}
