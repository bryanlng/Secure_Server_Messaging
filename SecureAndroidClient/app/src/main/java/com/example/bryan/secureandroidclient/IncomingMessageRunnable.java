package com.example.bryan.secureandroidclient;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import java.util.concurrent.TimeUnit;

/**
 * Created by Bryan on 2/11/2017.
 * Class that represents the bulk of the work
 * I basically moved the functionality from ClientIncomingAsyncTask --> this class, since I'm trying to move away from the
 * structure of an AsyncTask --> Handler and Runnable structure
 *
 */
public class IncomingMessageRunnable implements Runnable {
    private final String TAG = "SecureAndroidClient";
    private Handler mUiHandler;
    /*
        Constructor
        Takes in a reference to the IncomingMessageT
        ask which created it
        We need this reference, because we need to give it our result
     */
    public IncomingMessageRunnable(Handler h){
        Log.i(TAG, "IncomingMessageRunnable constructor");
        mUiHandler = h;
    }

    /*
           Creates a new Socket connection, then listens for messages
           Once a message arrives, write it to a temp buffer, then sends it
           to publishProgress so that it can be added to the official messages ArrayList

           Basically, what doInBackGround() from ClientIncomingAsyncTask did
     */
    @Override
    public void run(){
        Log.i(TAG, "IncomingMessageRunnable run()");
//        Log.i(TAG, "Inside IncomingMessageRunnable: Are we on the main thread?: " + (Looper.myLooper() == Looper.getMainLooper()));
        for (int i = 0; i < 4; i++) {
            try {
                TimeUnit.SECONDS.sleep(2);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (i == 2) {
                Message a_message = Message.obtain();
                Bundle bundle = new Bundle();
//                b.putString("test", "ggg");
                //dummy values for a MessageItem
//        -Ex:
//        1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::bryan:::::::
                String delimiter = ":::::::";
                StringBuilder b = new StringBuilder("");

                Long ts = new Long(1485328997);
                b.append(ts);
                b.append(delimiter);

                String date = "Wed Jan 25 00:23:17 2017";
                b.append(date);
                b.append(delimiter);

                String m = "kkkk";
                b.append(m);
                b.append(delimiter);

                String s = "bryan";
                b.append(s);
                b.append(delimiter);

                MessageItem messageItem = new MessageItem(b.toString(), ts, date, m, s, true);
                bundle.putParcelable("test2", messageItem);

                Log.i(TAG, "IncomingMessageRunnable run(): sending test messageItem");
                a_message.setData(bundle);
                mUiHandler.sendMessageAtFrontOfQueue(a_message);
                Log.i(TAG, "IncomingMessageRunnable run(): sending test messageItem: " + messageItem.getRawMessage());
//                m.setData(b);
//                Log.i(TAG, "IncomingMessageRunnable run(): sending test message: ggg");
//                mUiHandler.sendMessageAtFrontOfQueue(m);


//                mUiHandler.post(new Runnable() {
//                    @Override
//                    public void run() {
//                        Log.i(TAG,"I am at the middle of background task, posted because i == 2");
//
//                    }
//                });
            }
        }
    }
}
