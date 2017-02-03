package com.example.bryan.secureandroidclient;

import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;


public class MainActivity extends ActionBarActivity {
    private final String TAG = "SecureAndroidClient";
    private final String address = "wleungtx.no-ip.biz";
    private final int port = 9999;

    private MessagesFragment messagesFragment;
    private EditText chatbox;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Initialize MessagesFragment
        FragmentManager fragmentManager = getFragmentManager();
        messagesFragment = new MessagesFragment();
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.replace(R.id.fragment_container, messagesFragment);
        ft.addToBackStack(null);
        ft.commit();
        fragmentManager.executePendingTransactions();

        //Initialize chatbox
        chatbox = (EditText)findViewById(R.id.chatbox);

        //Initialize send button for the chatbox, which is implemented as android:drawableRight
        //If the send button is pressed, an AsyncTask to send outgoing messages is started and executed
        chatbox.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                final int DRAWABLE_RIGHT = 2;

                if(event.getAction() == MotionEvent.ACTION_UP) {
                    if(event.getRawX() >= (chatbox.getRight() - chatbox.getCompoundDrawables()[DRAWABLE_RIGHT].getBounds().width())) {

                        //Start the AsyncTask to send outgoing messages, and give it the current text on the EditText
                        //If the message is empty, prompt user to type in input
                        String message = chatbox.getText().toString();
                        if(message.equals("")){
                            Log.i(TAG, "showAlertDialog with message: " + message);
                            AlertDialog.Builder builder = new AlertDialog.Builder(v.getContext()); //getActivity().getApplicationContext());
                            builder.setMessage(message);
                            builder.setCancelable(false);
                            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    //do things
                                }
                            });
                            AlertDialog alert = builder.create();
                            alert.show();
                        }
                        else{
                            //Start the AsyncTask to listen for incoming messages
//                            ClientOutgoingAsyncTask client = new ClientOutgoingAsyncTask(address, port, message);
//                            client.response = this;
//                            client.execute();
                        }

                        return true;
                    }
                }
                return false;
            }
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
