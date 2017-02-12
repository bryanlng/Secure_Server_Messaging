package com.example.bryan.secureandroidclient;

import android.graphics.Bitmap;

/**
 * Created by Bryan on 2/11/2017.
 * Class that represents the bulk of the work
 * I basically moved the functionality from ClientIncomingAsyncTask --> this class, since I'm trying to move away from the
 * structure of an AsyncTask --> Handler and Runnable structure
 *
 */
public class IncomingMessageRunnable implements Runnable {
    // Constants for indicating the state of the decode
    public static final int STATE_FAILED = -1;
    public static final int STATE_STARTED = 0;
    public static final int STATE_COMPLETED = 1;

    private IncomingMessageTask parentTask;

    interface IncomingMessageRunnableMethods{
        /**
         * Sets the actions for each state of the IncomingMessageRunnable instance.
         * @param state The state being handled.
         */
        void handleRunnableState(int state);

        /**
         * Sets the MessageItem for the IncomingMessageTask, given to it by the
         * IncomingMessageRunnable
         * @param m The MessageItem being given to the IncomingMessageTask
         */
        void setMessageItem(MessageItem m);
    }



    /*
        Constructor
        Takes in a reference to the IncomingMessageTask which created it
        We need this reference, because we need to give it our result
     */
    public IncomingMessageRunnable(IncomingMessageTask p){
        parentTask = p;
    }

    /*
           Creates a new Socket connection, then listens for messages
           Once a message arrives, write it to a temp buffer, then sends it
           to publishProgress so that it can be added to the official messages ArrayList

           (what doInBackGround() from ClientIncomingAsyncTask did)
     */
    public void run(){
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

        //Set IncomingMessageTask's MessageItem to be the generated MessageItem
        parentTask.setMessageItem(messageItem);

        //Report a status of "completed"


    }
}
