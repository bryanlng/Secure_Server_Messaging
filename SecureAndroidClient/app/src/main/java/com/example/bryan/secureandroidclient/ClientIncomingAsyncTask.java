package com.example.bryan.secureandroidclient;

import android.os.AsyncTask;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by Bryan on 2/1/2017.
 *
 * ClientIncomingAsyncTask sets up a socket connection to the server, and listens
 * for incoming messages. When a message is received, its fields are extracted
 * and then sent back to the CustomListViewAdapter, which displays the message.
 *
 * ClientIncomingAsyncTask implements 2 of the 4 overridable methods:
 *  1) doInBackground
 *  2) publishProgress
 *
 * 1) doInBackground:
 *      - Steps:
 *          1) Set up the Socket connection
 *          2) Inside a while loop, constantly wait for messages. When a message
 *             arrives, take out the null char at the end and any other irrelevant
 *             characters. Then, extract the timestamp, date_formatted, message,
 *             and sender fields using String.split()
 *          3) Call publishProgress() with the fields to send the message back
 *             to CustomListViewAdapter to be displayed
 * 2) publishProgress
 *      - Steps:
 *          1) Call retrieveResponse(), a method from the AsyncResponse interface.
 *             Since CustomListViewAdapter implements AsyncResponse, the
 *             retrieveResponse() being called is CustomListViewAdapter's overriden
 *             version defined in CustomListViewAdapter.
 *
 *
 * Implementation is primarily based off of this website
 * http://androidsrc.net/android-client-server-using-sockets-client-implementation/
 */
public class ClientIncomingAsyncTask extends AsyncTask<Void, MessageItem, Void> {
    private final String TAG = "SecureAndroidClient";
    private final int MAX_MESSAGE_SIZE = 25600;
    private final String REGULAR_MESSAGE_DELIMITER = ":::::::";

    private String serverAddress;
    private int serverPort;

    public AsyncResponse response = null;   //Used to pass the message from publishProgress() --> adapter's retrieveResponse()
    /*
        Constructor
        Takes in the server's IP address, port #, and the ArrayList<String> messages
        from MessagesFragment. Somehow, Java can magically take the ArrayList<String>
        from MessagesFragment into here. Normally, you would need a reference, but
        somehow it works.
     */
    public ClientIncomingAsyncTask(String address, int port){
       serverAddress = address;
       serverPort = port;
    }

    /*
         Creates a new Socket connection, then listens for messages
         Once a message arrives, write it to a temp buffer, then sends it
         to publishProgress so that it can be added to the official messages ArrayList
     */
    @Override
    protected Void doInBackground(Void... arg0){
        Log.i(TAG, "inside doInBackground");
        Socket socket = null;
        try{

            socket = new Socket(serverAddress, serverPort);

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
                byteArrayOutputStream.write(buffer, 0, bytesRead);
                String rawBuffer = new String(buffer);
                Log.i(TAG, "# of bytes read: " + bytesRead);
                Log.i(TAG, "length of rawBuffer: " + rawBuffer.length());

                //Get the raw message out, taking out the null character attached in the process.
                String rawMessage = rawBuffer.substring(0, bytesRead-1);        //bytesRead-1 b/c we strip out null character

                //Parse the message, extract contents, then add to the ArrayList
                long timestamp;
                String date_formatted;
                String message;
                String sender;

                String[] items = rawMessage.split(REGULAR_MESSAGE_DELIMITER);

                timestamp = Long.parseLong(items[0]);
                date_formatted = items[1];
                message = items[2];
                sender = items[3];

                Log.i(TAG, "after stripping out only the message: " + rawMessage);
                Log.i(TAG, "date_formatted: " + date_formatted);
                Log.i(TAG, "message: " + message);
                Log.i(TAG, "sender: " + sender);
                MessageItem messageItem = new MessageItem(rawMessage, timestamp, date_formatted, message, sender, true);
                publishProgress(messageItem);

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
                    socket.close();

                }
                catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    /*
        Called on the main UI thread,

        This message is called after publishProgress(), which was called
        in doInBackground(). Usually, results are passed in onPostExecute().
        However, since we're stuck in a while loop in doInBackground() reading
        messages, the only way to pass messages is through this method, combined
        with the main UI's overridden retrieveResponse()
     */
    @Override
    protected void onProgressUpdate(MessageItem... newMessage){
        response.retrieveResponse(newMessage[0]);
    }


}
