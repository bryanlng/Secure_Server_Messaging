package com.example.bryan.secureandroidclient;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
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
    private Context context;
    private ArrayList<String> data;
    private static LayoutInflater inflater = null;

    public CustomListViewAdapter(Context context, ArrayList<String> data){
        this.context = context;
        this.data = data;   //bad
        inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return data.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return data.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        View view = convertView;
        if (view == null)
            view = inflater.inflate(R.layout.row_item_incoming, null);

        //Set the fields
        ImageView imageView = (ImageView) view.findViewById(R.id.picture);
        imageView.setImageResource(R.drawable.test);

        TextView message = (TextView) view.findViewById(R.id.message);
        message.setText(data.get(position));

        TextView date = (TextView) view.findViewById(R.id.date);
        date.setText(data.get(position));

        return view;
    }
}
