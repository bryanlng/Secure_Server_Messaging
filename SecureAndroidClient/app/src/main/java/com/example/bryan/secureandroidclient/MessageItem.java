package com.example.bryan.secureandroidclient;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

/**
 * Created by Bryan on 2/2/2017.
 * For onSaveInstanceState, I needed to find a way to save an ArrayList<MessageItem>
 * http://stackoverflow.com/questions/12503836/how-to-save-custom-arraylist-on-android-screen-rotate
 * http://stackoverflow.com/questions/6201311/how-to-read-write-a-boolean-when-implementing-the-parcelable-interface
 */
public class MessageItem implements Parcelable{
    private final String TAG = "SecureAndroidClient";
    private String rawMessage;
    private long timestamp;
    private String date_formatted;
    private String message;
    private String sender;
    private boolean is_incoming_message;

    /*
        Regular constructor
     */
    public MessageItem(String raw, long ts, String date, String m, String s, boolean b){
        rawMessage = raw;
        timestamp = ts;
        date_formatted = date;
        message = m;
        sender = s;
        is_incoming_message = b;
    }

    /*
        Constructor for a Parcel object
     */
    public MessageItem(Parcel in){
        rawMessage = in.readString();
        timestamp = in.readLong();
        date_formatted = in.readString();
        message = in.readString();
        sender = in.readString();
        is_incoming_message = in.readByte() != 0;
    }

    /*Getters*/
    public String getRawMessage(){ return rawMessage; }
    public long getTimestamp(){ return timestamp; }
    public String getDateFormatted(){ return date_formatted; }
    public String getMessage(){ return message; }
    public String getSender(){ return sender; }
    public boolean isIncomingMessage(){ return is_incoming_message;}


    /*Methods required in order to get Parcel working*/
    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel dest, int flags) {
        Log.i(TAG, "entering writeToParcel()");
        dest.writeString(rawMessage);
        dest.writeLong(timestamp);
        dest.writeString(date_formatted);
        dest.writeString(message);
        dest.writeString(sender);
        dest.writeByte((byte) (is_incoming_message ? 1 : 0));
    }

    /*
        Parcelable creator
     */
    public static final Parcelable.Creator<MessageItem> CREATOR = new Parcelable.Creator<MessageItem>() {
        private final String TAG = "SecureAndroidClient";
        public MessageItem createFromParcel(Parcel in) {
            Log.i(TAG, "entering createFromParcel()");
            return new MessageItem(in);
        }

        public MessageItem[] newArray(int size) {
            Log.i(TAG, "entering newArray(), with size: " + size);
            return new MessageItem[size];
        }

    };

}
