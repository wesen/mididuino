/**
 * 
 */
package com.ruinwesen.patchdownloader.tasks;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import name.cs.csutils.UpdateTimer;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patchdownloader.repository.UpdateRepositoryCallback;
import com.ruinwesen.patchdownloader.swing.SearchController;
import com.ruinwesen.patchdownloader.swing.SwingPatchdownloader;
import com.ruinwesen.patchdownloader.swing.dialogs.ProgressDialog;

public class RepositoryUpdateTask implements Runnable, 
    UpdateRepositoryCallback, ActionListener {

    private static Log log = LogFactory.getLog(RepositoryUpdateTask.class);
    
    static final int ST_INIT = 1;
    static final int ST_UPDATE_REPOSITORY = 2;
    
    int state = ST_INIT;
    ProgressDialog dialog;
    UpdateTimer progressUpdate = new UpdateTimer();
    private boolean canceled = false;
    
    private synchronized int getState() {
        return state;
    }
    
    public void run() {
        final SwingPatchdownloader pd = SwingPatchdownloader.getSharedInstance();
        if (getState() == ST_INIT) { // event dispatcher thread
            dialog = new ProgressDialog(pd.getFrame(), "Updating...");
            dialog.setProgressBarIndeterminate(false);
            dialog.setCancelAction(this);
            synchronized(this) {
                state++;
            }
            new Thread(this).start();
            dialog.showProgressDialog(); // thread must start before !!!
        } else if (getState() == ST_UPDATE_REPOSITORY) {
            SearchController searchController = pd.getSearchController();
            
            
            try {
                pd.updateRemoteRepositoryBackup(this);
            } catch (final Throwable ex) {
                if (log.isErrorEnabled()) {
                    log.error("Could not update repository.", ex);
                }
                dialog.closeProgressDialog();
                
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                JOptionPane.showMessageDialog(pd.getFrame(), 
                        "Update failed. Reason:\n"+ex.getMessage(),
                        "Update failed.", JOptionPane.ERROR_MESSAGE);
                    }
                });
                
                return;
            }
            dialog.setStatusText("Loading...");
            searchController.indexChanged();
            searchController.update();

            dialog.setProgressBarTaskCompleted();
            dialog.closeProgressDialogAnimated();
        }
    }

    @Override
    public void updateProgress(String text, float progress) {
        if (progressUpdate.needsUpdateAndReset() 
                || progress == 0.0f || progress == 1.0f) {
            if (text != null)
                dialog.setStatusText(text);
            dialog.setProgressBarProgress(progress);
        }
    }

    @Override
    public synchronized void actionPerformed(ActionEvent e) {
        if ("cancel".equals(e.getActionCommand())) {
            canceled = true;
        }
    }

    @Override
    public synchronized boolean isCanceled() {
        return canceled;
    }

}