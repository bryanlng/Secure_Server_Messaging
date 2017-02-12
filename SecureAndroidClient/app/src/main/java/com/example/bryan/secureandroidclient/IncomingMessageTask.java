package com.example.bryan.secureandroidclient;

/**
 * Created by Bryan on 2/11/2017.
 */
public class IncomingMessageTask implements IncomingMessageRunnableMethods{
    private MainActivity mainActivityInstance;
    private IncomingMessageRunnable messageRunnable;
    private MessageItem currentMessageItem;

    /**
     * Constructor
     * Creates an IncomingMessageRunnable, then starts it
     */
    public IncomingMessageTask(MainActivity activity) {
        // Create the runnables
        messageRunnable = new IncomingMessageRunnable(this);

        //Get link to MainActivity, the one who created this IncomingMessageTask instance
        mainActivityInstance = activity;
    }

    public MessageItem getMessageItem(){
        return currentMessageItem;
    }

    /*
        Implemented method from IncomingMessageRunnable's internal interface
     */
    @Override
    public void setMessageItem(MessageItem m){
        currentMessageItem = m;
    }


    @Override
    void handleRunnableState(int state){
        int outState;

        // Converts the download state to the overall state
        switch(state) {
            case IncomingMessageRunnable.STATE_COMPLETED:
                outState = mainActivityInstance.TASK_COMPLETE;
                break;
            case IncomingMessageRunnable.STATE_FAILED:
                outState = mainActivityInstance.TASK_FAILED;
                break;
            default:
                outState = mainActivityInstance.TASK_STARTED;
                break;
        }

        // Passes the state to the ThreadPool object.
        handleState(outState);
    }

    // Passes the state to MainActivity
    void handleState(int state) {
        /*
         * Passes a handle to this task and the
         * current state to the class that created
         * the thread pools
         */
        mainActivityInstance.handleState(this, state);
    }



}
