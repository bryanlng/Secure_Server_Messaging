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
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.SimpleTimeZone;
import java.util.TimeZone;

/**
 * https://www.codeproject.com/Questions/791580/How-to-send-message-from-server-to-client-TCP-andr
 * Created by Bryan on 2/3/2017.
 */
public class ClientOutgoingAsyncTask extends AsyncTask<Void, MessageItem, Void> {
    private final String TAG = "SecureAndroidClient";
    private String name = "bryan";
    private final int MAX_MESSAGE_SIZE = 25600;
    private final String REGULAR_MESSAGE_DELIMITER = ":::::::";
    private final int SIX_HOUR_OFFSET = -6 * 60 * 60 * 1000;
    private int CST_TIMEZONE_ID;

    private String serverAddress;
    private int serverPort;
    private String message;

    public AsyncResponseToMainActivity response = null;   //Used to pass the message from publishProgress() --> adapter's retrieveResponse()

    /*
        Constructor
        Takes in the server's IP address, port #, and the ArrayList<String> messages
        from MessagesFragment. Somehow, Java can magically take the ArrayList<String>
        from MessagesFragment into here. Normally, you would need a reference, but
        somehow it works.
     */
    public ClientOutgoingAsyncTask(String address, int port, String m){
        serverAddress = address;
        serverPort = port;
        message = m;
    }

    /*
	Given a regular message, generates a formatted version with metatdata included
	that can be parsed by the server

    Regular message
		-Simply create the string in the format as specified below
			-Timestamp will be the current time, in milliseconds
			-date_formatted will the current time, formatted in
			-message will be the message
		-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter> sender <delimiter>
		-Ex:
			1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::bryan:::::::

    Returns a MessageItem, since we need to send this to publishProgress --> retrieveResponse
*/
    public MessageItem formatMessage(String message){
        StringBuilder builder = new StringBuilder("");

        //1. Generate timestamp, then concatenate onto our string
        long currentTime = System.currentTimeMillis();
        builder.append(currentTime);
        builder.append(REGULAR_MESSAGE_DELIMITER);

        //2. Generate current date, in the format given above

        //Generate a Gregorian Calendar with the timezone CST (GMT - 6:00)
        // get the supported ids for GMT-06:00 (Central Standard Time)
        String[] ids = TimeZone.getAvailableIDs(SIX_HOUR_OFFSET);
        CST_TIMEZONE_ID = ids.length - 2;
        SimpleTimeZone CST = new SimpleTimeZone(SIX_HOUR_OFFSET, ids[CST_TIMEZONE_ID]);
        Calendar calendar = new GregorianCalendar(CST);
        Date trialTime = new Date();
        calendar.setTime(trialTime);

        //Extract day, month, year, day_of_week, HH:MM:SS add put it all into a long string
        //in the order of:              day_of_week Mon Day HH:MM:SS Year
        StringBuilder dateBuilder = new StringBuilder("");


        String dummy = "Wed Jan 25 00:23:17 2017";
        builder.append(REGULAR_MESSAGE_DELIMITER);

        //3. Add on message
        builder.append(message);
        builder.append(REGULAR_MESSAGE_DELIMITER);

        //4. Add on name
        builder.append(name);
        builder.append(REGULAR_MESSAGE_DELIMITER);

        //5. Add on a null char, since our server is c++ based
        char c = 0;
        builder.append(c);

        return new MessageItem(builder.toString(),currentTime, dummy ,message, name, false);
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
//            MessageItem formattedMessage = formatMessage(message);
//            dos.writeChars(formattedMessage.getRawMessage());
//            publishProgress(formattedMessage);

            String temp = "1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::bryan:::::::";
            dos.writeChars(temp);

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
