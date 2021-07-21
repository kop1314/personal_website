package edu.illinois.finalproject;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.CountDownTimer;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.appcompat.widget.Toolbar;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.Random;

import edu.illinois.finalproject.DataObject.ChatRoom;
import edu.illinois.finalproject.DataObject.FriendRef;
import edu.illinois.finalproject.DataObject.Message;
import edu.illinois.finalproject.DataObject.User;
import hani.momanii.supernova_emoji_library.Actions.EmojIconActions;
import hani.momanii.supernova_emoji_library.Helper.EmojiconEditText;


public class ChatActivity extends AppCompatActivity implements View.OnClickListener{
    private static final String TAG = ChatActivity.class.getSimpleName();
    private View rootView;
    private ImageView sendButton;
    private ImageView emojiButton;
    private EmojiconEditText messageArea;
    private EmojIconActions emojicons;
    private Toolbar toolbar;
    private TextView toobarTitle;
    private FirebaseAuth mAuth;
    private FirebaseUser myself;
    private User friend;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private DatabaseReference chatRoomsRef;
    private String myEmail;
    private String chatRoomId;
    private RecyclerView recyclerView;
    private MessageAdapter adapter;
    private ArrayList<Message> messageList = new ArrayList<Message>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.chat);

        final Intent intent = getIntent();
        friend = intent.getParcelableExtra("user");

        rootView = (RelativeLayout) findViewById(R.id.chat_activity);
        rootView.setBackgroundColor(Color.WHITE);

        mAuth = FirebaseAuth.getInstance();

        //check if a user is currently sign out
        if (mAuth.getCurrentUser() == null) {
            startActivity(new Intent(ChatActivity.this, LoginActivity.class));
        }

        databaseReference = FirebaseDatabase.getInstance().getReference();
        myself = mAuth.getCurrentUser();
        myEmail = myself.getEmail();
        usersRef = databaseReference.child("Users");
        chatRoomsRef = databaseReference.child("Chatrooms");

        if(!hasChatRoomID()) {
            setUpChatRoom();
        }

        getChatRoomId();

        //create chat log
        recyclerView = (RecyclerView) findViewById(R.id.chat_recyclerview);
        LinearLayoutManager linearLayoutManager =
                new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false);
        recyclerView.setLayoutManager(linearLayoutManager);

        chatRoomsRef.child(chatRoomId).child("messages").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                //get historical messages from database
                setMessageList(dataSnapshot);

                //set up timer to change background color of rootview
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

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });

        //set up top tool bar
        toolbar = (Toolbar) findViewById(R.id.chat_toolbar);
        toobarTitle = (TextView) findViewById(R.id.chat_toolbar_title);
        toolbar.setNavigationIcon(R.drawable.ic_arrow_back_white_24dp);
        toobarTitle.setText(friend.getName());
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(getApplicationContext(), ChatListActivity.class));
            }
        });

        //set up message bar buttons
        sendButton = (ImageView) findViewById(R.id.sendButton);
        emojiButton = (ImageView) findViewById(R.id.emojiconButton);
        messageArea = (EmojiconEditText) findViewById(R.id.messageArea);

        //create a board of emojicons
        //The following code for making emojicon feature come from a YouTube video
        //which can be found in the following link
        //https://www.youtube.com/watch?v=_CMkPQnrsF4&t=1s
        emojicons = new EmojIconActions(getApplicationContext(),
                rootView,
                emojiButton,
                messageArea);
        emojicons.ShowEmojicon();

        //logOutButton.setOnClickListener(this);
        sendButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view == sendButton) {
            String messageText = messageArea.getText().toString();
            if (!messageText.equals("") ){
                //store new meesage to database
                final Message message = new Message(myEmail, messageText, "");
                chatRoomsRef.child(chatRoomId).addListenerForSingleValueEvent(new ValueEventListener() {
                    @Override
                    public void onDataChange(DataSnapshot dataSnapshot) {
                        ChatRoom chatRoom = dataSnapshot.getValue(ChatRoom.class);
                        if( chatRoom.getMessages() == null) {
                            //first message is added
                            ArrayList<Message> messages = new ArrayList<Message>();
                            messages.add(message);
                            chatRoom.setMessages(messages);
                            dataSnapshot.getRef().setValue(chatRoom);
                        } else {
                            ArrayList<Message> messages = chatRoom.getMessages();
                            messages.add(message);
                            chatRoom.setMessages(messages);
                            dataSnapshot.getRef().setValue(chatRoom);
                        }
                    }

                    @Override
                    public void onCancelled(DatabaseError databaseError) {

                    }
                });
                messageArea.setText("");
            }
        }
    }

    @Override
    public boolean onSupportNavigateUp() {
        onBackPressed();
        return true;
    }

    private boolean hasChatRoomID(){
        //check if it is first time to chat with the friend
        ArrayList<FriendRef> friendRefList = friend.getFriend_list();
        for (int i = 0; i < friendRefList.size(); i++) {
            FriendRef friendRef = friendRefList.get(i);
            if (friendRef.getFriend_email().equals(myEmail)) {
                if (friendRef.getChat_room_id() != null && !friendRef.getChat_room_id().equals("")){
                    return true;
                }
            }
        }
        return false;
    }

    private void setUpChatRoom() {
        getChatRoomId();

        //add chatroom id to both current user and friend
        addChatRoomId(myEmail, friend.getEmail(), chatRoomId);

        ArrayList<String> memberEmails = new ArrayList<String>();
        memberEmails.add(myEmail);
        memberEmails.add(friend.getEmail());

        ArrayList<Message> messages = new ArrayList<Message>();
        messages.add(new Message("" , "Start Chat", ""));
        ChatRoom chatRoom = new ChatRoom(chatRoomId, memberEmails);
        chatRoomsRef.child(chatRoomId).setValue(chatRoom);
    }

    private void addChatRoomId(final String userEmail, final String friendEmail, final String id) {
        //add chatroom id to current user and friend
        usersRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
                    User user = postSnapshot.getValue(User.class);
                    //add chatroom id to current user
                    if ( user.getEmail().equals(userEmail)){
                        ArrayList<FriendRef> friendRefs = user.getFriend_list();
                        for (int i = 0; i < friendRefs.size(); i++) {
                            FriendRef friendRef = friendRefs.get(i);
                            if (friendRef.getFriend_email().equals(friendEmail)) {
                                friendRef.setChat_room_id(id);
                            }
                            friendRefs.set(i, friendRef);
                        }
                        user.setFriend_list(friendRefs);
                    }
                    postSnapshot.getRef().setValue(user);

                    //add chatroom id to friend
                    if (user.getEmail().equals(friendEmail)) {
                        ArrayList<FriendRef> friendRefs = user.getFriend_list();
                        for (int i = 0; i < friendRefs.size(); i++) {
                            FriendRef friendRef = friendRefs.get(i);
                            if (friendRef.getFriend_email().equals(userEmail)) {
                                friendRef.setChat_room_id(id);
                            }
                            friendRefs.set(i, friendRef);
                        }
                        user.setFriend_list(friendRefs);
                    }
                    postSnapshot.getRef().setValue(user);
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });
    }

    private void setMessageList(DataSnapshot dataSnapshot) {
        messageList = new ArrayList<Message>();

        for (DataSnapshot postSnapshot: dataSnapshot.getChildren()){
            Message message = postSnapshot.getValue(Message.class);
            messageList.add(message);
        }

        adapter = new MessageAdapter(messageList, myEmail);
        recyclerView.setAdapter(adapter);
        recyclerView.scrollToPosition(messageList.size()-1);
        Log.d(TAG, "---------CREATE CHAT LOG----------");
    }

    private void getChatRoomId() {
        //combines current user's email and email of friend who is chated with
        //and generate a id for this chatRoom
        String combineEmails = "";
        if (myEmail.compareTo(friend.getEmail()) < 0){
            combineEmails = myEmail + friend.getEmail();
        } else {
            combineEmails = friend.getEmail() + myEmail;
        }
        byte[] data = Base64.encode(combineEmails.getBytes(), Base64.NO_WRAP);
        chatRoomId = new String(data);
        Log.d(TAG, "Chat Activity: Encoded Chatroom id");
    }
}
