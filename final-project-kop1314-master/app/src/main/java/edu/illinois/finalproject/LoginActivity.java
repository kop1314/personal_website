package edu.illinois.finalproject;

import android.content.Intent;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthResult;
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

//The YouTube video where I get the idea of making LoginActivity class
//can be found in the following link.
//https://www.youtube.com/watch?v=tJVBXCNtUuk
public class LoginActivity extends AppCompatActivity implements View.OnClickListener{
    public static final String TAG = LoginActivity.class.getSimpleName();
    public static final String NO_BlANK_TEXT = "Can't be blank";
    private TextView registerUser;
    private EditText emailAddress;
    private EditText password;
    private Button loginButton;
    private String email;
    private String pass;
    private FirebaseAuth mAuth;
    private FirebaseUser user;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private User currentUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);

        registerUser = (TextView)findViewById(R.id.register);
        emailAddress = (EditText)findViewById(R.id.email);
        password = (EditText)findViewById(R.id.password);
        loginButton = (Button)findViewById(R.id.loginButton);

        databaseReference = FirebaseDatabase.getInstance().getReference();
        usersRef = databaseReference.child("Users");
        mAuth = FirebaseAuth.getInstance();
        user = mAuth.getCurrentUser();

        //start chat if a email is currently sign in
        if (user != null) {
            email = user.getEmail();
            getUserData();
            Intent chatListIntent = new Intent(LoginActivity.this, ChatListActivity.class);
            chatListIntent.putExtra("currentUser", currentUser);
            startActivity(chatListIntent);
        }

        registerUser.setOnClickListener(this);
        loginButton.setOnClickListener(this);
    }

    private void loginUser() {
        email = emailAddress.getText().toString().trim();
        pass = password.getText().toString().trim();

        if (email.equals("")){
            emailAddress.setError(NO_BlANK_TEXT);
        } else if (pass.equals("")){
            password.setError(NO_BlANK_TEXT);
        } else {
            mAuth.signInWithEmailAndPassword(email, pass)
                    .addOnCompleteListener(LoginActivity.this,
                            new OnCompleteListener<AuthResult>() {
                                @Override
                                public void onComplete(@NonNull Task<AuthResult> task) {
                                    if (task.isSuccessful()) {
                                        getUserData();

                                        Intent chatListIntent = new Intent(LoginActivity.this,
                                                ChatListActivity.class);
                                        chatListIntent.putExtra("currentUser", currentUser);
                                        startActivity(chatListIntent);
                                        Log.d(TAG, "signInWithEmailAndPassword:success");
                                    } else {
                                        Log.w(TAG, "signInWithEmailAndPassword:failure",
                                                task.getException());
                                        Toast.makeText(LoginActivity.this,
                                                "Failed to log in.",
                                                Toast.LENGTH_LONG)
                                                .show();
                                    }
                                }
                            });
        }
    }

    @Override
    public void onClick(View view) {
        if (view == loginButton) {
            loginUser();
        }
        if (view == registerUser) {
            startActivity(new Intent(LoginActivity.this, RegisterActivity.class));
        }
    }

    private void getUserData() {
        //get current user's data
        usersRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                for (DataSnapshot postSnapshot: dataSnapshot.getChildren()) {
                    User user = postSnapshot.getValue(User.class);
                    if (user.getEmail().equals(email)) {
                        currentUser = new User(user.getName(), email);
                        if (user.getFriend_list()!=null) {
                            ArrayList<FriendRef> friendRefs = new ArrayList<FriendRef>(user.getFriend_list());
                            currentUser.setFriend_list(friendRefs);
                        }
                        Log.d(TAG, "----------------GET USER DATA------------");
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });
    }
}
