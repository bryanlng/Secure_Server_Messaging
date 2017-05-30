package com.example.bryan.secureandroidclient;

import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
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

import java.lang.ref.WeakReference;


public class MainActivity extends ActionBarActivity implements AsyncResponse{
    private final String TAG            = "SecureAndroidClient";
    private final String address        = "wleungtx.no-ip.biz";
    private final int    port           = 9999;

    private MessagesFragment messagesFragment;
    private EditText chatbox;
    private SocketService socketService;
    private boolean mBound = false;

    @Override
    protected void onStart(){
        super.onStart();
        // Bind to SocketService
//        Log.i(TAG, "onStart(), binding to SocketService now");
//        mBound = true;
//        Intent intent = new Intent(this, SocketService.class);
//        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    /*
        Called when the connection with the service is established
        A ServiceConnection is an "Interface for monitoring the state of an application service"
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            Log.i(TAG, "onServiceConnected()");
            // Because we have bound to an explicit service that is running in our own process, we can cast its IBinder to a concrete class and directly access it.
            socketService = ((SocketService.SocketBinder)service).getService();
            mBound = true;
        }

        // Called when the connection with the service disconnects unexpectedly
        public void onServiceDisconnected(ComponentName className) {
            Log.i(TAG, "onServiceDisconnected()");
            mBound = false;
            socketService = null;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.i(TAG, "MainActivity onCreate(), Thread name: " + Thread.currentThread().getName());


        //Initialize MessagesFragment
        FragmentManager fragmentManager = getFragmentManager();
        messagesFragment = new MessagesFragment();
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        ft.replace(R.id.fragment_container, messagesFragment);
        ft.addToBackStack(null);
        ft.commit();
        fragmentManager.executePendingTransactions();

        //Pass this instance of MessageFragment to a static class "middleman", so that it can be
        //referenced later inside IncomingMessageRunnable, for the Socket connection.
        LinkSocketServiceToMessageFragment.setMessageFragment(messagesFragment);

        //Initialize chatbox
        chatbox = (EditText)findViewById(R.id.chatbox);

        //Start the SocketService. This should result in a call to SocketService's onStartCommand()
        Intent intent = new Intent(this, SocketService.class);
        startService(intent);
        doBindService();


        //Initialize send button for the chatbox, which is implemented as android:drawableRight
        //If the send button is pressed, an AsyncTask to send outgoing messages is started and executed
        chatbox.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                final int DRAWABLE_RIGHT = 2;

                if(event.getAction() == MotionEvent.ACTION_UP) {
                    if(event.getRawX() >= (chatbox.getRight() - chatbox.getCompoundDrawables()[DRAWABLE_RIGHT].getBounds().width())) {
                        Log.i(TAG, "Send button was touched!");
                        //Start the AsyncTask to send outgoing messages, and give it the current text on the EditText
                        //If the message is empty, prompt user to type in input
                        String message = chatbox.getText().toString();
                        Log.i(TAG, "message from EditText, to be sent: " + message);
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
                            startOutgoingTask(message);
                        }

                        return true;
                    }
                }
                return false;
            }
        });

    }

    /*
        Calls bindService(), which connects the Activity to our SocketService

        "If a component starts the service by calling startService() (which results in a call to onStartCommand()),
        the service continues to run until it stops itself with stopSelf() or another component stops it by calling stopService()."
     */
    private void doBindService() {
        Log.i(TAG, "doBindService()");
        bindService(new Intent(MainActivity.this, SocketService.class), mConnection, Context.BIND_AUTO_CREATE);
        mBound = true;
    }

    private void doUnbindService() {
        Log.i(TAG, "doUnbindService()");
        if (mBound) {
            // Detach our existing connection.
            unbindService(mConnection);
            mBound = false;
        }
    }


    /*
        Starts a ClientOutgoingAsyncTask, which allows the client to send messages to the server
        Had to be placed in a method outside, or else "client.response = this;" will fail, since
        we need to be inside the context of an Activity, not an onTouchListener
     */
    public void startOutgoingTask(String message){
        ClientOutgoingAsyncTask client = new ClientOutgoingAsyncTask(address, port, message);
        client.response = this;
        client.execute();
    }

    /*
        Overridden method from the AsyncResponse interface
        A copy of AsyncResponseToFragment's retreiveResponse(), but I had to be clever
        in being able to get the message to the ListAdapter.

        Basically, this method will call messagesFragment's addMessageToListView().
        Inside addMessageToListView(), the MessageFragment will get the adapter's arraylist,
        and from there, it's back to the original functionality of AsyncResponseToFragment's retreiveResponse()

        Adds the message to the official message ArrayList. getView() will
        soon be called, which will allow the message to be shown above.

        Call notifyDataSetChanged() to notify ListView to refresh its data,
        by calling getView()
     */
    @Override
    public void retrieveResponse(MessageItem message){
        Log.i(TAG, "MainActivity retrieveResponse(): Adding message: " + message.getRawMessage());
        messagesFragment.addMessageToListView(message);
    }

    @Override
    protected void onDestroy() {
//        mWorkerThread.quit();
        super.onDestroy();
        doUnbindService();
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


