package edu.illinois.finalproject;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import edu.illinois.finalproject.DataObject.User;

public class SearchChatAdapter extends RecyclerView.Adapter<SearchChatAdapter.ViewHolder> {
    private List<User> userList = null;
    private ArrayList<User> arraylist;

    public SearchChatAdapter(List<User> userList) {
        this.userList = userList;
        this.arraylist = new ArrayList<User>();
        this.arraylist.addAll(userList);
    }

    @Override
    public int getItemViewType(int position) {
        User user = userList.get(position);
        return R.layout.search_chat_items;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View userItem = LayoutInflater.from(parent.getContext()).
                inflate(viewType, parent, false);

        return new ViewHolder(userItem);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        final User user = userList.get(position);
        //set textview info
        holder.name.setText(user.getName());
        holder.email.setText(user.getEmail());
        //make items are clickable
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Context context = v.getContext();
                Intent chatIntent = new Intent(context, ChatActivity.class);
                chatIntent.putExtra("user", user);
                context.startActivity(chatIntent);
            }
        });
    }

    @Override
    public int getItemCount() {
        return userList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder{
        public TextView name;
        public TextView email;
        public View itemView;

        public ViewHolder(View itemView) {
            super(itemView);
            this.name = (TextView) itemView.findViewById(R.id.search_user_name);
            this.email = (TextView) itemView.findViewById(R.id.search_user_email);
            this.itemView = itemView;
        }

    }

    public void filter(String charText) {
        charText = charText.toLowerCase(Locale.getDefault());
        userList.clear();
        if (charText.length() == 0) {
            userList.addAll(arraylist);
        } else {
            for (User wp : arraylist) {
                if (wp.getName().toLowerCase(Locale.getDefault()).contains(charText)) {
                    userList.add(wp);
                }
            }
        }
        notifyDataSetChanged();
    }
}
