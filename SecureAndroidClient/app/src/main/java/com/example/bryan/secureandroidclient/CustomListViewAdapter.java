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


/**
 */
public class CustomListViewAdapter extends BaseAdapter implements AsyncResponse {
    private final String TAG = "SecureAndroidClient";
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
        ClientIncomingAsyncTask client = new ClientIncomingAsyncTask(address, port, messages);
        client.response = this;
        client.execute();

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

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
//        Log.i(TAG, "getView, position: " + position + ", which is equal to: " + messages.get(position));
        View view = convertView;
        if(messages.get(position).equals("a")){
//                Log.i(TAG, "getView: " + messages.get(position) + ", OUTGOING");
            view = inflater.inflate(R.layout.row_item_outgoing, null);
        }
        else{
//                Log.i(TAG, "getView: " + messages.get(position) + ", INCOMING");
            view = inflater.inflate(R.layout.row_item_incoming, null);
        }


        //Set the fields
        ImageView imageView = (ImageView) view.findViewById(R.id.picture);
        imageView.setImageResource(R.drawable.test);

        TextView message = (TextView) view.findViewById(R.id.message);
        message.setText(messages.get(position).getMessage());

        TextView date = (TextView) view.findViewById(R.id.date);
        date.setText(messages.get(position).getDateFormatted());

        return view;
    }

    /*
        Overridden method from the AsyncResponse interface
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

}
