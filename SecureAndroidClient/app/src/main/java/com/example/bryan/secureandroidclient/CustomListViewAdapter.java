package com.example.bryan.secureandroidclient;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.TimeZone;


/**
 */
public class CustomListViewAdapter extends BaseAdapter implements AsyncResponseToFragment {
    private final String TAG = "SecureAndroidClient";
    private final long MILLISECONDS_IN_A_DAY = 1000 * 60 * 60 * 24;
    private final long CPP_JAVA_TIME_CORRECTION = 3571677;
    private final String address = "wleungtx.no-ip.biz";
    private final int port = 9999;

    private Context context;
    private ArrayList<MessageItem> messages = new ArrayList<MessageItem>();
    private static LayoutInflater inflater = null;

    public CustomListViewAdapter(Context context, ArrayList<MessageItem> m){
        this.context = context;
        messages = m;
        inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        //Start the AsyncTask to listen for incoming messages
        ClientIncomingAsyncTask client = new ClientIncomingAsyncTask(address, port);
        client.response = this;
        client.execute();

    }

    public ArrayList<MessageItem> getMessageArrayList(){
        return messages;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return messages.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return messages.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    /*
        Overriden version of getView
        Checks if the MessageItem from messages.get(position) is an Incoming message
        or not.
        If it is, inflate the layout for an incoming message
        Else, inflate the layout for an ougoing message

        Regardless of what type the message is, display its contents
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
//        Log.i(TAG, "getView, position: " + position + ", which is equal to: " + messages.get(position));
        View view = convertView;


        if(messages.get(position).isIncomingMessage()){
//                Log.i(TAG, "getView: " + messages.get(position) + ", INCOMING");
            view = inflater.inflate(R.layout.row_item_incoming, null);

        }
        else{
//                Log.i(TAG, "getView: " + messages.get(position) + ", OUTGOING");
            view = inflater.inflate(R.layout.row_item_outgoing, null);
        }

        //Set the fields
        //Image
        ImageView imageView = (ImageView) view.findViewById(R.id.picture);
        imageView.setImageResource(R.drawable.test);

        //Message
        TextView message = (TextView) view.findViewById(R.id.message);
        message.setText(messages.get(position).getMessage());

        //Date. Does special formatting, depending on how far the date is
        //from the current date
        TextView date = (TextView) view.findViewById(R.id.date);
        long timestamp = messages.get(position).getTimestamp();
        String unformatted_date = messages.get(position).getDateFormatted();
        String formatted_date = formatDate(unformatted_date, timestamp);
        date.setText(formatted_date);
//        String unformatted_date = messages.get(position).getDateFormatted();
//        date.setText(unformatted_date);

        return view;
    }

    /*
        Overridden method from the AsyncResponseToFragment interface
        Adds the message to the official message ArrayList. getView() will
        soon be called, which will allow the message to be shown above.

        Call notifyDataSetChanged() to notify ListView to refresh its data,
        by calling getView()
     */
    @Override
    public void retrieveResponse(MessageItem message){
        Log.i(TAG, "retrieveResponse(): Adding message: " + message);
        messages.add(message);
        notifyDataSetChanged();
    }

    /*
        Formats the date into the correct format
        4 cases
        Case 1: If Time < 1 day, and it's the same day
            - HH:DD AM/PM

        Case 2: If Time < 1 day, but it's not the same day
            -(Month) (Day)

        Case 3:  If Time > 1 day
            -(Month) (Day)

        Case 4: If time is in previous year:
            -(Month) (Day), (Year)

        Ex of date_formatted:
            Thu Feb  2 23:56:31 2017
            FOr som
     */
    public String formatDate(String date, long timestamp){
        Log.i(TAG, "formatDate(), with raw date: " + date);

        //1. Extract all fields
        String[] items = date.split(" ");

        String day_of_week = items[0];
        String month = items[1];
        int day_of_month = Integer.parseInt(items[3]);

        String[] hms = items[4].split(":");
        int hour = Integer.parseInt(hms[0]);
        int min = Integer.parseInt(hms[1]);
        int year = Integer.parseInt(items[5]);

        //2. Go through the 4 cases
        //Find how far the time is from the current time
        long currentTime = System.currentTimeMillis();
        long diff = currentTime - timestamp;
        diff += CPP_JAVA_TIME_CORRECTION;   //ideally, Java and c++'s methods should return the # milliseconds
                                            //from the epoch should return the same value. However, they don't.
                                            //c++'s time function consistently gives a value that is around 3571713 off of
                                            //Java's time function. In order to correctly format the date, we need Java and c++'s time values
                                            //to be as close as possible. I averaged the difference from multiple runs
                                            //to calculate the value. This value will be added onto diff, in order to
                                            //more accurately display the difference

        long diffInDays = diff / MILLISECONDS_IN_A_DAY;

        //Create a GregorianCalendar with CST Timezone, so that we can
        //get the current day, year
        String[] ids = TimeZone.getAvailableIDs();
        TimeZone tz = TimeZone.getTimeZone("GMT-6");
        Calendar calendar = new GregorianCalendar(tz);
        int curr_year = calendar.get(Calendar.YEAR);
        int curr_day = calendar.get(Calendar.DATE);

//        Log.i(TAG, "timestamp: " + timestamp);
//        Log.i(TAG, "currentTimeMillis: " + currentTime);
//        Log.i(TAG, "calendar millis: " + calendar.getTimeInMillis());
        Log.i(TAG, "diff: " + diff);
//        Log.i(TAG, "diffInDays: " + diffInDays);
//        Log.i(TAG, "hour: " + hour);
//        Log.i(TAG, "min: " + min);
//        Log.i(TAG, "curr_day: " + curr_day);
//        Log.i(TAG, "day_of_month: " + day_of_month);

        if(diffInDays < 1) {
            //Case 1: If Time < 1 day, and it's the same day:     HH:MM AM/PM
            if(curr_day == day_of_month){
                if(hour >= 12){
                    if(min < 10){
                        return "" + (hour-12) + ":" + "0" + min + " PM";
                    }
                    else{
                        return "" + (hour-12) + ":" + min + " PM";
                    }

                }

                else{
                    if(min < 10){
                        return "" + (hour-12) + ":" + "0" + min + " AM";
                    }
                    else{
                        return "" + (hour-12) + ":" + min + " AM";
                    }
                }
            }

            //Case 2: If Time < 1 day, but it's not the same day: (Month) (Day)
            else{
                return month + " " + day_of_month;
            }
        }

        else{
            //Case 4: If time is in previous year:               (Month) (Day), (Year)
            if(curr_year > year){
                return month + " " + day_of_month + ", " + year;
            }

            //Case 3:  If Time > 1 day, but in the same year:   (Month) (Day)
            else {
                return month + " " + day_of_month;
            }

        }
    }

}
