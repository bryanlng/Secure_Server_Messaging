package com.example.bryan.secureandroidclient;

import android.os.AsyncTask;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by Bryan on 2/3/2017.
 */
public class ClientOutgoingAsyncTask extends AsyncTask<Void, Void, Void> {
    private final String TAG = "SecureAndroidClient";
    private final int MAX_MESSAGE_SIZE = 25600;
    private final String REGULAR_MESSAGE_DELIMITER = ":::::::";

    private String serverAddress;
    private int serverPort;
    private String rawMessage = "";
    private ArrayList<MessageItem> messages;     //this will be replaced with a synchronized ArrayList,
    //Since 2 Threads will be adding to this ArrayList, locking must be implemented.

    public AsyncResponse response = null;   //Used to pass the message from publishProgress() --> adapter's retrieveResponse()

    /*
        Constructor
        Takes in the server's IP address, port #, and the ArrayList<String> messages
        from MessagesFragment. Somehow, Java can magically take the ArrayList<String>
        from MessagesFragment into here. Normally, you would need a reference, but
        somehow it works.
     */
    public ClientOutgoingAsyncTask(String address, int port, ArrayList<MessageItem> m ){
        serverAddress = address;
        serverPort = port;
        messages = m;
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

            OutputStream out = socket.getOutputStream();
            DataOutputStream dos = new DataOutputStream(out);
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

                }
                catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
        return null;
    }


}
