package edu.illinois.finalproject;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.recyclerview.widget.RecyclerView;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import edu.illinois.finalproject.DataObject.FriendRef;
import edu.illinois.finalproject.DataObject.User;


public class AddFriendAdapter extends RecyclerView.Adapter<AddFriendAdapter.ViewHolder> {
    private static final String TAG = AddFriendAdapter.class.getSimpleName();
    private String currentUserEmail;
    private FirebaseAuth mAuth;
    private FirebaseUser currentUser;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private List<User> userList;
    private ArrayList<User> arraylist;

    public AddFriendAdapter(List<User> userList) {
        this.userList = userList;
        this.arraylist = new ArrayList<User>();
        this.arraylist.addAll(userList);
    }

    @Override
    public int getItemViewType(int position) {
        User user = userList.get(position);
        return R.layout.add_friend_items;
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
        final ViewHolder aHolder = holder;
        holder.name.setText(user.getName());
        holder.email.setText(user.getEmail());
        //make items are clickable
        holder.addFriendBTN.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //access databse and add friendRefs to both current user and new friend
                connectFirebase(user);
                //unable this button after being clicked
                aHolder.addFriendBTN.setEnabled(false);
                aHolder.addFriendBTN.setText("  Added!  ");
                Toast.makeText(v.getContext(), "Clicked", Toast.LENGTH_SHORT).show();
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
        public Button addFriendBTN;

        public ViewHolder(View itemView) {
            super(itemView);
            name = (TextView) itemView.findViewById(R.id.add_user_name);
            email = (TextView) itemView.findViewById(R.id.add_user_email);
            addFriendBTN = (Button) itemView.findViewById(R.id.add_friendBTN);
        }

    }

    /**
     * The following code come from
     * http://abhiandroid.com/ui/searchview
     * filter out the items
     * @param charText a string contains char of name of user who is looked for
     */
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

    private void connectFirebase(User newFriend) {
        final User aNewFriend = newFriend;
        FirebaseDatabase database = FirebaseDatabase.getInstance();

        databaseReference = database.getReference();
        mAuth = FirebaseAuth.getInstance();
        currentUser = mAuth.getCurrentUser();

        currentUserEmail = currentUser.getEmail();
        usersRef = databaseReference.child("Users");

        usersRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                addFriend(aNewFriend, dataSnapshot);
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });
    }

    private void addFriend(User newFriend, DataSnapshot dataSnapshot) {
        for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
            User user = postSnapshot.getValue(User.class);
            //set a friendRef to current user
            if (user.getEmail().equals(currentUserEmail)) {
                if (user.getFriend_list() == null) {
                    ArrayList<FriendRef> newFriendList = new ArrayList<FriendRef>();
                    FriendRef newFriendRef = new FriendRef(newFriend.getEmail());
                    newFriendList.add(newFriendRef);
                    user.setFriend_list(newFriendList);
                    postSnapshot.getRef().setValue(user);
                }
                if (user.getFriend_list() != null && !user.hasEmailOfUser(newFriend.getEmail())) {
                    ArrayList<FriendRef> newFriendList = new ArrayList<FriendRef>(user.getFriend_list());
                    FriendRef newFriendRef = new FriendRef(newFriend.getEmail());
                    newFriendList.add(newFriendRef);
                    user.setFriend_list(newFriendList);
                    postSnapshot.getRef().setValue(user);
                }
                Log.d(TAG, "AddFriendAdapter: modify current user's friendList");
            }

            //set a friendRef to new friend
            if (user.getEmail().equals(newFriend.getEmail())) {
                if (user.getFriend_list() == null) {
                    ArrayList<FriendRef> newFriendList = new ArrayList<FriendRef>();
                    FriendRef newFriendRef = new FriendRef(currentUserEmail);
                    newFriendList.add(newFriendRef);
                    user.setFriend_list(newFriendList);
                    postSnapshot.getRef().setValue(user);
                }
                if (user.getFriend_list() != null && !user.hasEmailOfUser(currentUserEmail)) {
                    ArrayList<FriendRef> newFriendList = new ArrayList<FriendRef>(user.getFriend_list());
                    FriendRef newFriendRef = new FriendRef(currentUserEmail);
                    newFriendList.add(newFriendRef);
                    user.setFriend_list(newFriendList);
                    postSnapshot.getRef().setValue(user);
                }
                Log.d(TAG, "AddFriendAdapter: modify friend's friendList");
            }
        }
    }
}
