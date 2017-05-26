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
    private final String MESSAGE_ITEM_PARCELABLE_KEY = "mess_item_key";
    private MessagesFragment messagesFragment;

    public IncomingMessageHandler(MessagesFragment m){ messagesFragment = m;  }

    /*
     * handleMessage() defines the operations to perform when the
     * Handler receives a new Message to process.
     */
    @Override
    public void handleMessage(Message inputMessage) {
        Bundle bundle = inputMessage.getData();
        MessageItem messageItem = bundle.getParcelable(MESSAGE_ITEM_PARCELABLE_KEY);
        if(bundle != null){
            Log.i(TAG, "IncomingMessageHandler handleMessage(): Received test message: " + messageItem.getRawMessage());
            messagesFragment.addMessageToListView(messageItem);
        }
//        String s = b.getString("test");
//        Log.i(TAG, "IncomingMessageHandler handleMessage(): Received test message: " + s);
    }
}
