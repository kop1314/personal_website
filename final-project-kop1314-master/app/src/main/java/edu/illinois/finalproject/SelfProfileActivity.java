package edu.illinois.finalproject;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;

import edu.illinois.finalproject.DataObject.User;

public class SelfProfileActivity extends AppCompatActivity implements View.OnClickListener{
    private String TAG = SelfProfileActivity.class.getSimpleName();
    private TextView firstName;
    private TextView lastName;
    private TextView email;
    private Button signOutBTN;
    private Button chatListBTN;
    private FirebaseAuth mAuth;
    private FirebaseUser user;
    private User currentUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.self_profile);

        final Intent intent = getIntent();
        currentUser = intent.getParcelableExtra("currentUser");

        mAuth = FirebaseAuth.getInstance();
        user = mAuth.getCurrentUser();

        //check if a user is currently sign out
        if (user == null) {
            startActivity(new Intent(SelfProfileActivity.this, LoginActivity.class));
        }

        firstName = (TextView) findViewById(R.id.self_profile_first_name);
        lastName = (TextView) findViewById(R.id.self_profile_last_name);
        email = (TextView) findViewById(R.id.self_profile_email);
        signOutBTN = (Button) findViewById(R.id.signoutBTN);
        chatListBTN = (Button) findViewById(R.id.self_profile_chatroomListBTN);

        System.out.println(currentUser);
        String name = currentUser.getName();
        email.setText(user.getEmail());
        firstName.setText(name.substring(0, name.indexOf(" ")));
        lastName.setText(name.substring(name.indexOf(" ")+1));

        signOutBTN.setOnClickListener(this);
        chatListBTN.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view == signOutBTN) {
            mAuth.signOut();
            startActivity(new Intent(SelfProfileActivity.this, LoginActivity.class));
        }
        if (view == chatListBTN) {
            Intent chatListIntent = new Intent(SelfProfileActivity.this, ChatListActivity.class);
            chatListIntent.putExtra("currentUser", currentUser);
            startActivity(chatListIntent);
        }
    }
}
