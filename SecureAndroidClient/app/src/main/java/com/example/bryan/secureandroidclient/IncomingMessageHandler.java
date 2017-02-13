package com.example.bryan.secureandroidclient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

/**
 * Created by Bryan on 2/13/2017.
 */
public class IncomingMessageHandler extends Handler {
    private final String TAG = "SecureAndroidClient";
    private MessagesFragment messagesFragment;

    public IncomingMessageHandler(MessagesFragment m){ messagesFragment = m;  }

    /*
     * handleMessage() defines the operations to perform when the
     * Handler receives a new Message to process.
     */
    @Override
    public void handleMessage(Message inputMessage) {
        Bundle b = inputMessage.getData();
        MessageItem m = b.getParcelable("test2");
        if(b != null){
            Log.i(TAG, "IncomingMessageHandler handleMessage(): Received test message: " + m.getRawMessage());
        }
//        String s = b.getString("test");
//        Log.i(TAG, "IncomingMessageHandler handleMessage(): Received test message: " + s);
    }
}
