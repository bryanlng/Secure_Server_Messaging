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
 * Implementation is primarily based off of this website
 * http://androidsrc.net/android-client-server-using-sockets-client-implementation/
 * http://stackoverflow.com/questions/40480/is-java-pass-by-reference-or-pass-by-value
 *
 */
public class ClientIncomingAsyncTask extends AsyncTask<Void, Void, Void> {
    private final String TAG = "SecureAndroidClient";
    private final int MAX_MESSAGE_SIZE = 25600;
    private String serverAddress;
    private int serverPort;
    private String rawMessage = "";
    private ArrayList<String> messages;     //this will be replaced with a synchronized ArrayList,
                                            //Since 2 Threads will be adding to this ArrayList, locking must be implemented.

    /*
        Constructor
        Takes in the server's IP address, port #, and the ArrayList<String> messages
        from MessagesFragment. Somehow, Java can magically take the ArrayList<String>
        from MessagesFragment into here. Normally, you would need a reference, but
        somehow it works.
     */
    public ClientIncomingAsyncTask(String address, int port, ArrayList<String> m ){
       serverAddress = address;
       serverPort = port;
       messages = m;
    }

    /*
         Creates a new Socket connection, then listens for messages
         Once a message arrives, write it to a temp buffer, then add
         it to the official messages ArrayList
     */
    @Override
    protected Void doInBackground(Void... arg0){
        Log.i(TAG, "inside doInBackground");
        Socket socket = null;
        try{

            socket = new Socket(serverAddress, serverPort);

            ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(1024);
            byte[] buffer = new byte[100];  //MAX_MESSAGE_SIZE

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
             *  3) Add the contents to the official messages ArrayList
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
                String date_formatted;
                String message;
                String sender;

                String[] items = rawMessage.split(":::::::");

                date_formatted = items[1];
                message = items[2];
                sender = items[3];

                messages.add(rawMessage);
                Log.i(TAG, "after stripping out only the message: " + rawMessage);

                //Use Java's version of "memset" to clean our buffer
                int z = 0;
                byte zero = (byte)z;
                Arrays.fill(buffer, zero);
            }
        }
        catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            rawMessage = "UnknownHostException: " + e.toString();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            rawMessage = "IOException: " + e.toString();
        }
        finally {
            if (socket != null) {
                try {
                    socket.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
        return null;
    }


}
