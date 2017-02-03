package com.example.bryan.secureandroidclient;

/**
 * Created by Bryan on 2/2/2017.
 * Needed a way to communicate the received message from ClientIncomingAsyncTask --> CustomListViewAdapter
 * In the link below, it shows how you can get a result from onPostExecute() --> main activity, but I'm going to try it
 * with publishProgress() instead. Hopefully, that will work.
 * http://stackoverflow.com/questions/12575068/how-to-get-the-result-of-onpostexecute-to-main-activity-because-asynctask-is-a
 */
public interface AsyncResponseToFragment {
    void retrieveResponse(MessageItem message);
}
