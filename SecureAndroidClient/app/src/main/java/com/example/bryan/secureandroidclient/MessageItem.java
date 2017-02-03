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
    private boolean is_incoming_message;

    public MessageItem(String raw, long ts, String date, String m, String s, boolean b){
        rawMessage = raw;
        timestamp = ts;
        date_formatted = date;
        message = m;
        sender = s;
        is_incoming_message = b;
    }

    public String getRawMessage(){ return rawMessage; }
    public long getTimestamp(){ return timestamp; }
    public String getDateFormatted(){ return date_formatted; }
    public String getMessage(){ return message; }
    public String getSender(){ return sender; }
    public boolean isIncomingMessage(){ return is_incoming_message;}

}
