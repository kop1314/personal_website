package edu.illinois.finalproject;

import android.content.Intent;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SearchView;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;

import edu.illinois.finalproject.DataObject.FriendRef;
import edu.illinois.finalproject.DataObject.User;

public class AddFriendActivity extends AppCompatActivity implements SearchView.OnQueryTextListener,
        View.OnClickListener {
    private static final String TAG = AddFriendActivity.class.getSimpleName();
    private Button cancelBTN;
    private SearchView searchView;
    private RecyclerView recyclerView;
    private AddFriendAdapter adapter;
    private ArrayList<User> userList = new ArrayList<User>();
    private ArrayList<String> friendEmailList;
    private FirebaseAuth mAuth;
    private FirebaseUser user;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private String currentUserEmail;
    private User currentUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.add_friend);

        //get transfer current user's data
        final Intent intent = getIntent();
        currentUser = intent.getParcelableExtra("currentUser");

        //get a list of emails of friends of current user
        if (currentUser != null) {
            friendEmailList = new ArrayList<String>();
            getEmailsOfFriends(currentUser.getFriend_list());
        }

        //connect to firebase
        FirebaseDatabase database = FirebaseDatabase.getInstance();

        databaseReference = database.getReference();
        mAuth = FirebaseAuth.getInstance();
        user = mAuth.getCurrentUser();

        currentUserEmail = user.getEmail();
        usersRef = databaseReference.child("Users");

        //set up recyclerView
        recyclerView = (RecyclerView) findViewById(R.id.add_listview);
        LinearLayoutManager linearLayoutManager =
                new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false);
        recyclerView.setLayoutManager(linearLayoutManager);

        usersRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                //set friendEmailList or userList
                showUsers(dataSnapshot);
                Log.d(TAG, "AddFriend: add friend");
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });

        //set recyclerview adapter
        adapter = new AddFriendAdapter(userList);
        recyclerView.setAdapter(adapter);

        searchView = (SearchView) findViewById(R.id.add_searchView);
        searchView.setOnQueryTextListener(this);

        cancelBTN = (Button) findViewById(R.id.add_cancelBTN);
        cancelBTN.setOnClickListener(this);
    }


    @Override
    public boolean onQueryTextSubmit(String query) {
        return false;
    }

    @Override
    public boolean onQueryTextChange(String newText) {
        //detect text change
        String text = newText;
        adapter.filter(text);
        return false;
    }

    @Override
    public void onClick(View view) {
        if (view == cancelBTN) {
            startActivity(new Intent(AddFriendActivity.this, ChatListActivity.class));
        }
    }

    private void showUsers(DataSnapshot dataSnapshot) {
        //set friendEmailList
        if (currentUser == null || friendEmailList == null) {
            for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
                User user = postSnapshot.getValue(User.class);
                if (user.getEmail().equals(currentUserEmail)) {
                    if (user.getFriend_list() != null) {
                        ArrayList<FriendRef> friendRefList =
                                new ArrayList<FriendRef>(user.getFriend_list());
                        getEmailsOfFriends(friendRefList);
                    }
                }
            }
        }

        //set userList
        for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
            User user = postSnapshot.getValue(User.class);
            if (!currentUserEmail.equals(user.getEmail()) && !isFriend(user)) {
                userList.add(user);
            }
        }
    }

    private boolean isFriend(User user) {
        //Checl if user is a friend
        if (friendEmailList != null) {
            for (int i = 0; i < friendEmailList.size(); i++) {
                if (friendEmailList.get(i).equals(user.getEmail())) {
                    return true;
                }
            }
        }
        return false;
    }

    private void getEmailsOfFriends(ArrayList<FriendRef> friendList) {
        //set friendEmailList from a friendRef list
        for (int i = 0; i < friendList.size(); i++) {
            friendEmailList.add(friendList.get(i).getFriend_email());
        }
    }
}
