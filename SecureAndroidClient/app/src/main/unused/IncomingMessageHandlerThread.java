package com.example.bryan.secureandroidclient;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

/**
 * Created by Bryan on 2/12/2017.
 */
public class IncomingMessageHandlerThread extends HandlerThread {
    private final String TAG            = "SecureAndroidClient";
    private Handler mWorkerHandler;

    public IncomingMessageHandlerThread(String name) {
        super(name);
    }

    public void postTask(Runnable task){
        mWorkerHandler.post(task);
    }

    public void prepareHandler(){
        mWorkerHandler = new Handler(getLooper());
    }
}
