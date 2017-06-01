package com.example.bryan.secureandroidclient;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by Bryan on 5/25/2017.
 * Taken from:
 *  https://stackoverflow.com/questions/28915610/android-how-to-communicate-from-worker-thread-to-a-service
 *  https://stackoverflow.com/questions/14985678/how-to-keep-the-android-client-connected-to-the-server-even-on-activity-changes
 *
 * Service that creates a seperate Thread called a WorkerThread, which does the actual receiving of incoming messages.
 * The WorkerThread does the actual networking part;
 * The SocketService and its WorkerThread communicate using a Handler, specifically LocalHandler. LocalHandler's handleMessage()
 * relays the MessageItem (sent from WorkerThread) to the MessagesFragment.
 *
 * Rationale:
 * We needed to have another Thread, because since networks are fundamentally unreliable, we can't have the main UI thread
 * trying to connect to the pi, as it would slow down the app. Thus, we would have 2 Threads; the Service runs on the main UI thread,
 * while the WorkerThread is its own separate Thread. Originally, I wasn't going to use a Handler, but I ran into the issue where another Thread
 * couldn't edit the UI; it had to be the UI Thread. Thus, I had to somehow re-integrate Handler back in, but this time using a Service.
 *
 */
public class SocketService extends Service {
    private final String TAG            = "SecureAndroidClient";
    private static final String s_TAG   = "SecureAndroidClient";
    private final String address        = "wleungtx.no-ip.biz";
    private final int    port           = 9999;
    private static final String MESSAGE_ITEM_PARCELABLE_KEY = "mess_item_key";

    private final IBinder myBinder = new SocketBinder();

    public class SocketBinder extends Binder {
        public SocketService getService() {
            Log.i(TAG, "SocketBinder getService()");
            return SocketService.this;
        }
    }

    protected SocketWorkerThread workerThread;

    static class LocalHandler extends Handler {
        /*
         * handleMessage() defines the operations to perform when the Handler receives a new Message to process.
         * When a Message is received (sent from SocketWorkerThread), we extract the Bundle out of the Message,
         * then extract the MessageItem out of the Bundle. Then, we call the message fragment's addMessageToListView()
         * to add the MessageItem to the MessageFragment.
         *
         * This method was originally in IncomingMessageHandler
         */
        @Override
        public void handleMessage(Message inputMessage) {
            Bundle bundle = inputMessage.getData();
            MessageItem messageItem = bundle.getParcelable(MESSAGE_ITEM_PARCELABLE_KEY);
            if(bundle != null){
                Log.i(s_TAG, "IncomingMessageHandler handleMessage(): Received test message: " + messageItem.getRawMessage());

                MessagesFragment messagesFragment =  LinkSocketServiceToMessageFragment.getMessageFragment();
                messagesFragment.addMessageToListView(messageItem);
            }
        }
    }
    protected Handler handler;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "SocketBinder onCreate()");

        // Instantiating our special handler.
        handler = new LocalHandler();

        // Creating a connection worker thread instance.
        workerThread = new SocketWorkerThread(address, port, handler);
    }

    public void IsBoundable(){

    }

    /*
        Method that is called when startService() is called in MainActivity
        Start the workerThread instance, if it hasn't been started already
     */
    @Override
    public int onStartCommand(Intent intent,int flags, int startId){
        super.onStartCommand(intent, flags, startId);

        Log.i(TAG, "SocketService onStartCommand()");

        // Checking if worker thread is already running.
        if (!workerThread.isAlive()) {

            Log.i(TAG, "Starting working thread, name: " + this.getClass().getName());

            // Starting the worker thread.
            workerThread.start();

            Log.i(TAG, "SocketService started, name: " + this.getClass().getName());
        }
        return START_STICKY;
    }

    /*
        Required method that I had to override
     */
    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "SocketService onBind()");
        return myBinder;
    }

    /*
        1. Stop the thread
        2. Close the shared socket
     */
    @Override
    public void onDestroy() {
        Log.i(TAG, "SocketService onDestroy()");

        // Stop the thread.
        workerThread.interrupt();

        super.onDestroy();
        EstablishedSocketConnection.closeSharedSocket();
    }
}
