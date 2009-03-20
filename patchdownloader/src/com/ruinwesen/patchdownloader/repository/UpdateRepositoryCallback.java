package com.ruinwesen.patchdownloader.repository;

public interface UpdateRepositoryCallback {

    public static final float NONE = 0.0f;
    public static final float COMPLETE = 1.0f;
    
    void updateProgress(String text, float progress);
    
    boolean isCanceled();
    
}
