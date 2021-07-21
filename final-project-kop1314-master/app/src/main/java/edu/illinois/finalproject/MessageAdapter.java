package edu.illinois.finalproject;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import edu.illinois.finalproject.DataObject.Message;
import hani.momanii.supernova_emoji_library.Helper.EmojiconTextView;


public class MessageAdapter extends RecyclerView.Adapter<MessageAdapter.ViewHolder>{
    private ArrayList<Message> messageList;
    private String myEmail;

    public MessageAdapter(ArrayList<Message> messageList, String myEmail){
        this.messageList = messageList;
        this.myEmail = myEmail;
    }

    @Override
    public int getItemViewType(int position) {
        Message message = messageList.get(position);
        //user right bubble if it is current user's message
        if (message.getAuthorEmail().equals(myEmail)) {
            return R.layout.right_bubble_message;
        }
        return R.layout.left_bubble_message;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View messageItem = LayoutInflater.from(parent.getContext()).
                inflate(viewType, parent, false);

        return new ViewHolder(messageItem);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        final Message message = messageList.get(position);
        holder.myMessage.setText(message.getText());
    }

    @Override
    public int getItemCount() {
        return messageList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder{
        public TextView myMessage;

        public ViewHolder(View itemView) {
            super(itemView);
            myMessage = (EmojiconTextView) itemView.findViewById(R.id.txt_msg);
        }
    }
}
