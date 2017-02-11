package com.example.bryan.secureandroidclient;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
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
    private final String TAG = "SecureAndroidClient";
    private final String SAVED_INS_STATE_KEY = "messages";
    private ListView messagesListView;
    private CustomListViewAdapter adapter;
    private ArrayList<MessageItem> messages;

    public MessagesFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_messages, container, false);
        setRetainInstance(true);

        //Check if we just came out of a configuration change
        if(savedInstanceState != null){
            Log.i(TAG, "just came out of a configuration change");
            messages = savedInstanceState.getParcelableArrayList(SAVED_INS_STATE_KEY);
        }
        else{
            Log.i(TAG, "first time, no configuration change");
            messages = new ArrayList<MessageItem>();
        }

        //Instantiate the ListView, create a CustomListViewAdapter, and set it to the ListView
        messagesListView = (ListView)rootView.findViewById(R.id.listview);
        adapter = new CustomListViewAdapter(getActivity(), messages);
        messagesListView.setAdapter(adapter);

        // Inflate the layout for this fragment
        return rootView;
    }

    public void addMessageToListView(MessageItem mItem){
        //I'm making the assumption that our ListView is going to be initialized before
        //this message is called

        if(adapter != null){
            Log.i(TAG, "MessageFragment addMessageToListView(), adding message: " + mItem.getRawMessage());
            ArrayList<MessageItem> messages = adapter.getMessageArrayList();
            messages.add(mItem);
            adapter.notifyDataSetChanged();
        }
    }

    /*
        Saves the current state of our program, so that on restart,
        we can restore the program to this state.
     */
    @Override
    public void onSaveInstanceState(Bundle savedInstanceState){
        Log.i(TAG, "going into onSaveInstanceState()");
        savedInstanceState.putParcelableArrayList(SAVED_INS_STATE_KEY, messages);
        super.onSaveInstanceState(savedInstanceState);
    }
}
