package com.example.bryan.secureandroidclient;

import java.net.Socket;

/**
 * Created by Bryan on 5/28/2017.
 *
 * Needed a way for addMessageToListView() to be called in IncomingMessageRunnable.
 * AKA, I needed to find a way to eventually get to this statement:
 *  messagesFragment.addMessageToListView(messageItem);
 *
 * Since SocketService doesn't have a constructor, I had to create a middleman between
 * IncomingMessageRunnable and MainActivity.
 *
 *
 */
public class LinkSocketServiceToMessageFragment {
    private static MessagesFragment messagesFragment;

    public static MessagesFragment getMessageFragment(){return messagesFragment;}
    public static void setMessageFragment(MessagesFragment m){messagesFragment = m;}
}
