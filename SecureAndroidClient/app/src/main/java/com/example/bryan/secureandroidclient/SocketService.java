package com.example.bryan.secureandroidclient;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.widget.Toast;

/**
 * Created by Bryan on 5/25/2017.
 */
public class SocketService extends Service {
    private String address;
    private int    port;
    private MessagesFragment messagesFragment;

    public SocketService(String serverAddr, int serverPort, MessagesFragment mFrag){
        address = serverAddr;
        port = serverPort;
        messagesFragment = mFrag;
    }

    @Override
    public IBinder onBind(Intent intent) {
        System.out.println("SocketService onBind()");
        return myBinder;
    }

    private final IBinder myBinder = new SocketBinder();

    public class SocketBinder extends Binder {
        public SocketService getService() {
            System.out.println("SocketBinder getService()");
            return SocketService.this;
        }
    }


    @Override
    public void onCreate() {
        super.onCreate();
        System.out.println("SocketBinder onCreate()");
    }

    public void IsBoundable(){

    }

    @Override
    public int onStartCommand(Intent intent,int flags, int startId){
        super.onStartCommand(intent, flags, startId);
        System.out.println("I am in on start");
        //  Toast.makeText(this,"Service created ...", Toast.LENGTH_LONG).show();
        Runnable socket_runnable = new IncomingMessageRunnable(address, port, messagesFragment);
        new Thread(socket_runnable).start();
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EstablishedSocketConnection.closeSharedSocket();
    }
}
