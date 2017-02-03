package com.example.bryan.secureandroidclient;

/**
 * Created by Bryan on 2/2/2017.
 */
public class MessageItem {
    private String rawMessage;
    private long timestamp;
    private String date_formatted;
    private String message;
    private String sender;

    public MessageItem(String raw, long ts, String date, String m, String s){
        rawMessage = raw;
        timestamp = ts;
        date_formatted = date;
        message = m;
        sender = s;
    }

    public long getTimestamp(){ return timestamp; }
    public String getDateFormatted(){ return date_formatted; }
    public String getMessage(){ return message; }
    public String getSender(){ return sender; }

}
