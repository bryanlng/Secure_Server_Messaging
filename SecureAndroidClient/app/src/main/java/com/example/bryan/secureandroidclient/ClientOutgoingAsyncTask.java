package com.example.bryan.secureandroidclient;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
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
    private final long CPP_JAVA_TIME_CORRECTION = 3571677;
    private final int CST_OFFSET = -6 * 60 * 60 * 1000;
    private final int ADJUSTED_CST_OFFSET = (-6 * 60 * 60 * 1000) - 3600000;
    private final int AMERICA_CHICAG0_TIMEZONE_ID = 2;
    private final int DAYLIGHT_SAVINGS_TIME = 2 * 60 * 60 * 1000;


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
    public ClientOutgoingAsyncTask(String address, int port, String mess){
        serverAddress = address;
        serverPort = port;
        message = mess;
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

            Log.i(TAG, "ClientOutgoingAsyncTask: established connection");
            MessageItem formattedMessage = formatMessage(message);
            PrintWriter printWriter = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);
            printWriter.println(formattedMessage.getRawMessage());
            publishProgress(formattedMessage);

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
        Log.i(TAG, "ClientOutgoingAsyncTask onProgressUpdate()");
        response.retrieveResponse(newMessage[0]);
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
        //Have to account for
        Long currentTime = System.currentTimeMillis();
        currentTime -= CPP_JAVA_TIME_CORRECTION;
        builder.append(currentTime);
        builder.append(REGULAR_MESSAGE_DELIMITER);

        //2. Generate current date, in the format given above

        //Generate a Gregorian Calendar with the timezone CST (GMT - 6:00)
        // get the supported ids for GMT-06:00 (Central Standard Time)
        String[] ids = TimeZone.getAvailableIDs(CST_OFFSET);
        // if no ids were returned, something is wrong. get out.
        if (ids.length == 0)
            System.exit(0);

        // create a Pacific Standard Time time zone
        SimpleTimeZone pdt = new SimpleTimeZone(CST_OFFSET, ids[AMERICA_CHICAG0_TIMEZONE_ID]);

        // set up rules for Daylight Saving Time
        pdt.setStartRule(Calendar.APRIL, 1, Calendar.SUNDAY, DAYLIGHT_SAVINGS_TIME);
        pdt.setEndRule(Calendar.OCTOBER, -1, Calendar.SUNDAY, DAYLIGHT_SAVINGS_TIME);

        // create a GregorianCalendar with the Pacific Daylight time zone
        // and the current date and time
        Calendar calendar = new GregorianCalendar(pdt);
        Date trialTime = new Date();
        calendar.setTime(trialTime);

        //Extract day, month, year, day_of_week, HH:MM:SS add put it all into
        //a big string in the format of:    day_of_week Mon Day HH:MM:SS Year
        int year = calendar.get(Calendar.YEAR);
        String month = intToMonth(calendar.get(Calendar.MONTH));   //In android, Jan = 0
        int day_of_month = calendar.get(Calendar.DATE);
        String day_of_week = intToWeek(calendar.get(Calendar.DAY_OF_WEEK));
        String military_hour = formatHMS(calendar.get(Calendar.HOUR_OF_DAY));
        String min = formatHMS(calendar.get(Calendar.MINUTE));
        String sec = formatHMS(calendar.get(Calendar.SECOND));

        //Append onto a StringBuilder
        StringBuilder dateBuilder = new StringBuilder("");
        dateBuilder.append(day_of_week + " ");      //add the space, as it's our delimiter
        dateBuilder.append(month + "  ");           //add the double space, because cpp formatted it really weirdly
        dateBuilder.append(day_of_month + " ");
        dateBuilder.append(military_hour + ":");
        dateBuilder.append(min + ":");
        dateBuilder.append(sec + " ");
        dateBuilder.append(year);
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

        Log.i(TAG, "outgoing task formatMessage() raw message: " + builder.toString());
        return new MessageItem(builder.toString(),currentTime, dateBuilder.toString() ,message, name, false);
    }

    public String intToMonth(int id){
        switch(id){
            case 0: return "Jan";
            case 1: return "Feb";
            case 2: return "Mar";
            case 3: return "Apr";
            case 4: return "May";
            case 5: return "Jun";
            case 6: return "Jul";
            case 7: return "Aug";
            case 8: return "Sept";
            case 9: return "Oct";
            case 10: return "Nov";
            case 11: return "Dec";
            default: return "error";
        }
    }

    public String intToWeek(int id){
        switch(id){
            case 1: return "Sun";
            case 2: return "Mon";
            case 3: return "Tues";
            case 4: return "Wed";
            case 5: return "Thu";
            case 6: return "Fri";
            case 7: return "Sat";
            default: return "error";
        }
    }

    public String formatHMS(int value){
        if (value < 10){
            return "0" + value;
        }

        return "" + value;
    }




}
