package com.example.bryan.secureandroidclient;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.ArrayList;


/**
 */
public class MessagesFragment extends Fragment {
    private ListView messagesListView;

    public MessagesFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_messages, container, false);
        setRetainInstance(true);

        messagesListView = (ListView)rootView.findViewById(R.id.list);

        ArrayList<String> values = new ArrayList<>();
        values.add("a");
        values.add("b");
        values.add("ccccccccccccc");
        values.add("ddddddddddddddd");
        values.add("eeeeeeeeeeeeeeeeeeeeeee");
        values.add("fffff");
        values.add("ggggggggg");
        values.add("hhhhhhhhh");
        values.add("iiiiiiiiiii");
        values.add("jjjjjjjjjjjjjjj");
        values.add("kkkkkkkkkk");


        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(), android.R.layout.simple_list_item_1, values);

        messagesListView.setAdapter(adapter);

        messagesListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String item = parent.getItemAtPosition(position).toString();
                Toast.makeText(view.getContext(), "Item selected: " + item, Toast.LENGTH_LONG).show();
            }
        });

        // Inflate the layout for this fragment
        return rootView;
    }

}
