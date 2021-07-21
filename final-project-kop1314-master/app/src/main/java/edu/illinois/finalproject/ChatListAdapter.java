package edu.illinois.finalproject;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import edu.illinois.finalproject.DataObject.User;

public class ChatListAdapter extends RecyclerView.Adapter<ChatListAdapter.ViewHolder>{
    private ArrayList<User> friendList;

    public ChatListAdapter(ArrayList<User> friendList){
        this.friendList = friendList;
    }

    @Override
    public int getItemViewType(int position) {
        User user = friendList.get(position);
        return R.layout.chat_list_items;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View userItem = LayoutInflater.from(parent.getContext()).
                inflate(viewType, parent, false);

        return new ViewHolder(userItem);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        final User user = friendList.get(position);
        //set textview info
        holder.name.setText(user.getName());
        holder.email.setText(user.getEmail());
        //make items are clickable
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //start chat
                Context context = v.getContext();
                Intent chatIntent = new Intent(context, ChatActivity.class);
                chatIntent.putExtra("user", user);
                context.startActivity(chatIntent);
            }
        });
    }

    @Override
    public int getItemCount() {
        return friendList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder{
        public View itemView;
        public TextView name;
        public TextView email;

        public ViewHolder(View itemView) {
            super(itemView);
            this.itemView = itemView;
            name = (TextView) itemView.findViewById(R.id.chatlist_user_name);
            email = (TextView) itemView.findViewById(R.id.chatlist_user_email);
        }
    }
}
