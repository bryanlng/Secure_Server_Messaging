package com.example.bryan.secureandroidclient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;

/**
 * Created by Bryan on 5/29/2017.
 * Taken from:
 *  https://stackoverflow.com/questions/28915610/android-how-to-communicate-from-worker-thread-to-a-service
 *  https://stackoverflow.com/questions/14985678/how-to-keep-the-android-client-connected-to-the-server-even-on-activity-changes
 *
 * Originally, all this was in IncomingMessageRunnable, but I moved it all into here.
 *
 * Functionality:
 * Separate Thread that does the majority of the network stuff.
 * Sets up a Socket connection to the pi, and constantly waits for a message to come in.
 * When a message does come in, it extracts all the fields, puts all the fields into a MessageItem,
 *  puts that MessageItem into a Bundle, then puts that Bundle into a Message.
 *  Then, sends the Message to the UI thread using the Handler's call to sendMessage()
 */
public class SocketWorkerThread extends Thread {
    private final String TAG                            = "SecureAndroidClient";
    private final String MESSAGE_ITEM_PARCELABLE_KEY    = "mess_item_key";
    private final int MAX_MESSAGE_SIZE                  = 25600;
    private final String REGULAR_MESSAGE_DELIMITER      = ":::::::";

    private String serverAddress;
    private int serverPort;
    private MessagesFragment messagesFragment;
    protected Handler serviceHandler;          // Reference to service's handler.


    public SocketWorkerThread(String address, int port, Handler h) {
        super(SocketWorkerThread.class.getName());

        // Saving injected reference.
        Log.i(TAG, "IncomingMessageRunnable constructor");
        serverAddress = address;
        serverPort = port;
        messagesFragment = LinkSocketServiceToMessageFragment.getMessageFragment();
        serviceHandler = h;
    }

    /*
        Functionality from IncomingMessageRunnable moved into here
     */
    @Override
    public void run() {
        super.run();
        Log.i(TAG,"Thread started: " + this.getClass().getName());

        while (true) {
            // Terminate this method when the thread is interrupted.
            if (this.isInterrupted()) {
                return;
            }

            //Try to establish a connection
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
             *  3) Puts all the fields into a MessageItem
             *      ==> then put the MessageItem into a Bundle
             *      ==> then puts that Bundle into a Message.
             *      ==> then, send the Message to the UI thread using the Handler's call to sendMessage()
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

                    //Send the MessageItem to the LocalHandler of the SocketService, which is running in the UI Thread
                    handler_message.setData(bundle);
                    serviceHandler.sendMessage(handler_message);
                    Log.i(TAG, "IncomingMessageRunnable run(): sending messageItem: " + messageItem.getRawMessage());

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
            // Sending a message back to the service via handler.

        }
    }
}
