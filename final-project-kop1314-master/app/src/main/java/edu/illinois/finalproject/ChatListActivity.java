package edu.illinois.finalproject;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.CountDownTimer;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.appcompat.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.Random;

import edu.illinois.finalproject.DataObject.FriendRef;
import edu.illinois.finalproject.DataObject.User;

public class ChatListActivity extends AppCompatActivity implements View.OnClickListener{
    private static final String TAG = ChatListActivity.class.getSimpleName();
    private View rootView;
    private Button chatListBTN;
    private Button selfProfileBTN;
    private Toolbar toolbar;
    private RecyclerView recyclerView;
    private ChatListAdapter adapter;
    private FirebaseAuth mAuth;
    private FirebaseUser user;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private ArrayList<String> emailList;
    private ArrayList<User> friendList;
    private User currentUser;
    private String currentUserEmail;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.chatlist);

        final Intent intent = getIntent();
        currentUser = intent.getParcelableExtra("currentUser");

        rootView = (RelativeLayout) findViewById(R.id.chatlist_rootview);
        rootView.setBackgroundColor(Color.WHITE);

        //connect to firebase
        FirebaseDatabase database = FirebaseDatabase.getInstance();

        databaseReference = database.getReference();
        mAuth = FirebaseAuth.getInstance();
        user = mAuth.getCurrentUser();

        currentUserEmail = user.getEmail();
        usersRef = databaseReference.child("Users");

        //set up recyclerview
        recyclerView = (RecyclerView) findViewById(R.id.chat_list_recyclerview);
        LinearLayoutManager linearLayoutManager =
                new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false);
        recyclerView.setLayoutManager(linearLayoutManager);

        //get friendRef list of current user
        if(currentUser != null) {
            if (currentUser.getFriend_list() != null) {
                getEmailsOfFriends(currentUser.getFriend_list());
            }
        }

        usersRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                showFriends(dataSnapshot);

                adapter = new ChatListAdapter(friendList);
                recyclerView.setAdapter(adapter);

                changeColor();
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });

        //set top tool bar
        toolbar = (Toolbar) findViewById(R.id.chat_list_toolbar);
        setSupportActionBar(toolbar);

        //set bottom buttons
        chatListBTN = (Button) findViewById(R.id.chatlist_chatroomListBTN);
        selfProfileBTN = (Button) findViewById(R.id.chatlist_profileBTN);
        selfProfileBTN.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view == selfProfileBTN) {
            Intent selfProfileIntent = new Intent(ChatListActivity.this, SelfProfileActivity.class);
            selfProfileIntent.putExtra("currentUser", currentUser);
            startActivity(selfProfileIntent);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.chatlist_menu_item,menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.search_chat:
                Intent intentSC = new Intent(ChatListActivity.this, SearchChatActivity.class);
                intentSC.putParcelableArrayListExtra("friendList", friendList);
                startActivity(intentSC);
                return true;
            case R.id.add_friend:
                Intent intentAF = new Intent(ChatListActivity.this, AddFriendActivity.class);
                intentAF.putExtra("currentUser", currentUser);
                startActivity(intentAF);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void showFriends(DataSnapshot dataSnapshot) {
        //set up email list
        if (emailList == null || currentUser == null) {
            for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
                User user  = postSnapshot.getValue(User.class);
                if (user.getEmail().equals(currentUserEmail)) {
                    ArrayList<FriendRef> friendRefList = new ArrayList<FriendRef>();
                    if( user.getFriend_list() != null) {
                        friendRefList = new ArrayList<FriendRef>(user.getFriend_list());
                        getEmailsOfFriends(friendRefList);
                        Log.d(TAG, "ChatList: get a list of friends' emails");
                    }
                    currentUser = new User(user.getName(), currentUserEmail);
                    currentUser.setFriend_list(friendRefList);
                }
            }
        }

        //set friendList
        if (friendList == null) {
            friendList = new ArrayList<User>();
            for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
                User user  = postSnapshot.getValue(User.class);
                if (emailList != null) {
                    if (emailList.contains(user.getEmail())) {
                        friendList.add(user);
                        Log.d(TAG, "ChatList: ADD A FRIEND");
                    }
                }
            }
        }
    }

    private void getEmailsOfFriends(ArrayList<FriendRef> friendList) {
        //get email list from friendRef arraylist
        emailList = new ArrayList<String>();
        for (int i = 0; i < friendList.size(); i++) {
            emailList.add(friendList.get(i).getFriend_email());
        }
    }

    private void changeColor() {
        new CountDownTimer(2000, 100) {
            public void onTick(long millisUntilFinished) {
                //The following code comes from following website
                //https://teamtreehouse.com/community/dynamically
                // -changing-background-color-android-java
                Random rand = new Random();
                rootView.setBackgroundColor(Color.argb(255,
                        rand.nextInt(256),
                        rand.nextInt(256),
                        rand.nextInt(256) ));
            }

            public void onFinish() {
                rootView.setBackgroundColor(Color.WHITE);
            }
        }.start();
    }
}

