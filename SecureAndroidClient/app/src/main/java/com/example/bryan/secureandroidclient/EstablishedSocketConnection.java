package com.example.bryan.secureandroidclient;

import java.net.Socket;

/**
 * Created by Bryan on 2/15/2017.
 * Class with global Socket variable created so that we don't unnecessarily try to create a
 * new Socket connection when sending/receiving messages.
 * There should only be 1 socket connection for sending/receiving messages
 * ClientOutgoingAsyncTask, IncomingMessageRunnable should first check if an existing Socket connection
 * exists BEFORE trying to create one
 */
public class EstablishedSocketConnection {
    private static Socket sharedSocket;

    public static Socket getSharedSocket(){return sharedSocket;}
    public static void setSharedSocket(Socket s){sharedSocket = s;}
}
