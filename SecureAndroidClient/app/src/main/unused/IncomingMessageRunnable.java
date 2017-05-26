package com.example.bryan.secureandroidclient;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;
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
    private final String MESSAGE_ITEM_PARCELABLE_KEY = "mess_item_key";
    private final int MAX_MESSAGE_SIZE = 25600;
    private final String REGULAR_MESSAGE_DELIMITER = ":::::::";
    private String serverAddress;
    private int serverPort;
    private Handler mUiHandler;
    /*
        Constructor
        Takes in a reference to the IncomingMessageT
        ask which created it
        We need this reference, because we need to give it our result
     */
    public IncomingMessageRunnable(Handler h, String address, int port){
        Log.i(TAG, "IncomingMessageRunnable constructor");
        mUiHandler = h;
        serverAddress = address;
        serverPort = port;
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
        while(true){
            Socket socket = null;
            try{
                socket = EstablishedSocketConnection.getSharedSocket();
                if(socket == null){
                    socket = new Socket(serverAddress, serverPort);
                    EstablishedSocketConnection.setSharedSocket(socket);
                }

                ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(1024);
                byte[] buffer = new byte[MAX_MESSAGE_SIZE];
                int bytesRead;
                InputStream inputStream = socket.getInputStream();

			/*
             * While the inputStream from socket has stuff:
             *  1) Write it to our byte array
             *  2) Get the raw message out, taking out the null character attached in the process.
             *  3) Parse the message and extract the contents:
             *      -Timestamp
             *      -Date formatted
             *      -Actual message
             *      -Sender
             *  3) Call publishProgress(), which will send the message back to the adapter
             *     to be displayed
             *  4) Clean the buffer using Arrays.fill
             *
             * notice: inputStream.read() will block if no data return
			 */
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    Message handler_message = new Message();
                    Bundle bundle = new Bundle();

                    byteArrayOutputStream.write(buffer, 0, bytesRead);
                    String rawBuffer = new String(buffer);
//                Log.i(TAG, "# of bytes read: " + bytesRead);
//                Log.i(TAG, "length of rawBuffer: " + rawBuffer.length());

                    //Get the raw message out, taking out the null character attached in the process.
                    String rawMessage = rawBuffer.substring(0, bytesRead-1);        //bytesRead-1 b/c we strip out null character

                    Log.i(TAG, "raw message: " + rawMessage);
                    //Parse the message, extract contents, then add to the ArrayList
                    //Ex: 1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::bryan:::::::
                    Long timestamp;
                    String date_formatted;
                    String message;
                    String sender;

                    String[] items = rawMessage.split(REGULAR_MESSAGE_DELIMITER);

                    timestamp = Long.valueOf((items[0]));
                    date_formatted = items[1];
                    message = items[2];
                    sender = items[3];

//                Log.i(TAG, "after stripping out only the message: " + rawMessage);
//                Log.i(TAG, "date_formatted: " + date_formatted);
//                Log.i(TAG, "timestamp in Long: " + timestamp);
                    Log.i(TAG, "message: " + message);
                    Log.i(TAG, "sender: " + sender);
                    MessageItem messageItem = new MessageItem(rawMessage, timestamp, date_formatted, message, sender, true);
                    bundle.putParcelable(MESSAGE_ITEM_PARCELABLE_KEY, messageItem);

                    //Send the MessageItem to the Handler in the UI Thread
                    handler_message.setData(bundle);
                    mUiHandler.sendMessage(handler_message);
                    Log.i(TAG, "IncomingMessageRunnable run(): sending messageItem: " + messageItem.getRawMessage());
                    //this was previously publishProgress()

                    //Use Java's version of "memset" to clean our buffer
                    int z = 0;
                    byte zero = (byte)z;
                    Arrays.fill(buffer, zero);
                }

                //Should not get past here, unless the connection is severed
                //In that case, close all streams
                byteArrayOutputStream.close();
                inputStream.close();
            }
            catch (UnknownHostException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                Log.i(TAG, "UnknownHostException: " + e.toString());
            }
            catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                Log.i(TAG, "IOException: " + e.toString());
            }
            finally {
                if (socket != null) {
                    try {
                        Thread.sleep(2000);
                    }
                    catch (InterruptedException e2) {
                        // TODO Auto-generated catch block
                        e2.printStackTrace();
                    }
                }
            }
        }


    }
}
