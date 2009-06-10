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
package com.ruinwesen.patchdownloader.tasks;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.util.Random;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import name.cs.csutils.CSUtils;

import com.ruinwesen.patch.DeviceIDConstants;
import com.ruinwesen.patch.PatchFileBuilder;
import com.ruinwesen.patch.PatchMetadata;
import com.ruinwesen.patch.Tag;
import com.ruinwesen.patch.Tagset;
import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.indexer.Index;
import com.ruinwesen.patchdownloader.swing.SwingPatchdownloader;
import com.ruinwesen.patchdownloader.swing.dialogs.ProgressDialog;

public class RandomPatchGeneratorTask implements DeviceIDConstants {
    
    private static Random random = new Random();
    
    private void generateToken(StringBuilder sb, int minLen, int maxLen) {
        if (minLen>maxLen || minLen <0) {
            throw new IllegalArgumentException("invalid length:min-len="+minLen+",max-len="+maxLen);
        }
        int len = minLen+random.nextInt(maxLen-minLen+1);
        final String CHARS = "abcDEF012";
        char[] resultChars = new char[len];
        
        for (int i=0;i<len;i++)
            resultChars[i] = CHARS.charAt(random.nextInt(CHARS.length()));
        sb.append(resultChars);
    }
    
    private void generateTokenList(StringBuilder sb, int minListLen, int maxListLen, int minTokenLen, int maxTokenLen) {
        if (minListLen>maxListLen || minListLen <0) {
            throw new IllegalArgumentException("invalid length:min-len="+minListLen+",max-len="+maxListLen);
        }
        int len = minListLen+random.nextInt(maxListLen-minListLen+1);
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
        
        metadata.setDeviceId(DEVICE_ID_MINICOMMAND_1_0);

        sb.setLength(0);
        generateTokenList(sb, 0, 10, 3, 4);
        

        int maxcount = random.nextInt(MASTER_SET.length); 
        Tagset tagset = new Tagset();
        for (int i=0;i<maxcount;i++) {
            int index = random.nextInt(MASTER_SET.length);
            tagset.add(MASTER_SET[index]); // duplicates are removed
        }
        { // one category
            int index = random.nextInt(CATEGORY_SET.length);
            tagset.add(CATEGORY_SET[index]); 
        }
        //tagset.add("category:minicommand"); 
        
        metadata.setTags(tagset);
        
        String day = ""+(random.nextInt(30)+1);
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
        
        ProgressDialog dialog = new ProgressDialog(parentComponent, "Generating patches...");
        GeneratorThread gt = new GeneratorThread(dialog, count);
        gt.start();
        dialog.showProgressDialog();
    }
    
    private final class GeneratorThread extends Thread implements ActionListener {
        private ProgressDialog dialog;
        private volatile long generatorIsRunning = 1;
        private int generated;
        private int generateTotal;
        private boolean canceled = false;

        public GeneratorThread(ProgressDialog dialog, int generateCount) {
            this.dialog = dialog;
            dialog.setCancelAction(this);
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
                Index ir = pd.getPatchIndex();
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

                dialog.setStatusText("Generating...");
                try {
                    final long updateResolution = 150;
                    long nextUpdate = System.currentTimeMillis()+updateResolution;
                    
                    builder.setMidifile(tmpMidiFile);
                    if (tmpSrcFile != null) {
                        builder.setSourceCode(tmpSrcFile);
                    }
                    File dstDir = pd.getRemoteRepositoryBackup().getBaseDir();
                    while (isGeneratorRunning() && !isCanceled()) {
                        if (generated>=generateTotal) { break; }
                        
                        if (System.currentTimeMillis()>nextUpdate) {
                            dialog.setStatusText("Generated "+generated+" of "+generateTotal);
                            dialog.setProgressBarProgress(generated/(float)generateTotal);
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
                        generated++;
                    }
                    dialog.setProgressBarProgress(1f);
                    ir.update(); // update the current reader
                } catch (IOException ex) {
                    ex.printStackTrace();
                } finally {
                    if (!tmpMidiFile.delete()) {
                        System.err.println("Could not delete file: "+tmpMidiFile.getAbsolutePath());
                    }
                    if (tmpSrcFile != null && !tmpSrcFile.delete()) {
                        System.err.println("Could not delete file: "+tmpSrcFile.getAbsolutePath());
                    }
                }
            } finally {
                stopGenerator();
                dialog.closeProgressDialog();
            }
            
            SwingUtilities.invokeLater(new Runnable(){
                public void run() {
                    PatchDownloader pd = 
                        PatchDownloader.getSharedInstance();
                    if (pd != null && pd instanceof SwingPatchdownloader) {
                        ((SwingPatchdownloader)pd).getSearchController().indexChanged();
                        ((SwingPatchdownloader)pd).getSearchController().update();
                    }
                }
            });
        }

        private synchronized boolean isCanceled() {
            return canceled;
        }
        
        @Override
        public synchronized void actionPerformed(ActionEvent e) {
            this.canceled = true;
        }
    }

}
