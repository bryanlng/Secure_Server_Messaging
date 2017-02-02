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
public class CustomListViewAdapter extends BaseAdapter{
    private final String TAG = "SecureAndroidClient";
//    private final String address = "localhost";
//    private final int port = 9999;
    private final String address = "wleungtx.no-ip.biz";
    private final int port = 79;

    private Context context;
    private ArrayList<String> messages = new ArrayList<String>();
    private static LayoutInflater inflater = null;

    public CustomListViewAdapter(Context context, ArrayList<String> m){
        this.context = context;
        messages = m;
        inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        //Start the AsyncTasks to listen for incoming messages + send outgoing messages
        ClientIncomingAsyncTask client = new ClientIncomingAsyncTask(address, port, messages);
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
        if (view == null){
            if(messages.get(position).equals("a")){
//                Log.i(TAG, "getView: " + messages.get(position) + ", OUTGOING");
                view = inflater.inflate(R.layout.row_item_outgoing, null);
            }
            else{
//                Log.i(TAG, "getView: " + messages.get(position) + ", INCOMING");
                view = inflater.inflate(R.layout.row_item_incoming, null);
            }
        }

        else{
            if(messages.get(position).equals("a")){
//                Log.i(TAG, "getView: " + messages.get(position) + ", OUTGOING");
                view = inflater.inflate(R.layout.row_item_outgoing, null);
            }
            else{
//                Log.i(TAG, "getView: " + messages.get(position) + ", INCOMING");
                view = inflater.inflate(R.layout.row_item_incoming, null);
            }
        }


        //Set the fields
        ImageView imageView = (ImageView) view.findViewById(R.id.picture);
        imageView.setImageResource(R.drawable.test);

        TextView message = (TextView) view.findViewById(R.id.message);
        message.setText(messages.get(position));

        TextView date = (TextView) view.findViewById(R.id.date);
        date.setText(messages.get(position));

        return view;
    }
}
