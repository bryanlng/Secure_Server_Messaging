package com.example.bryan.secureandroidclient;

import android.os.Handler;
import android.os.HandlerThread;

/**
 * Created by Bryan on 2/12/2017.
 */
public class IncomingHandlerThread extends HandlerThread {

    private Handler mWorkerHandler;

    public IncomingHandlerThread(String name) {
        super(name);
    }

    public void postTask(Runnable task){
        mWorkerHandler.post(task);
    }

    public void prepareHandler(){
        mWorkerHandler = new Handler(getLooper());
    }
}
