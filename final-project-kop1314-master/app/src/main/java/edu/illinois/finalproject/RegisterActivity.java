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
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;

import edu.illinois.finalproject.DataObject.FriendRef;
import edu.illinois.finalproject.DataObject.User;

//The YouTube video where I get the idea of making RegistrationActivity class
//can be found in the following link.
//https://www.youtube.com/watch?v=0NFwF7L-YA8
public class RegisterActivity extends AppCompatActivity implements View.OnClickListener{
    public static final String TAG = RegisterActivity.class.getSimpleName();
    public static final String NO_BlANK_TEXT = "Can't be blank";
    private EditText firstNameET;
    private EditText lastNameET;
    private EditText emailAddress;
    private EditText password;
    private Button registerButton;
    private String firstName;
    private String lastName;
    private String email;
    private String pass;
    private TextView loginText;
    private FirebaseAuth mAuth;
    private DatabaseReference databaseReference;
    private DatabaseReference usersRef;
    private User currentUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.register);

        databaseReference = FirebaseDatabase.getInstance().getReference();
        usersRef = databaseReference.child("Users");

        firstNameET = (EditText)findViewById(R.id.first_name);
        lastNameET = (EditText)findViewById(R.id.last_name);
        emailAddress = (EditText)findViewById(R.id.email);
        password = (EditText)findViewById(R.id.password);
        registerButton = (Button)findViewById(R.id.registerButton);
        loginText = (TextView)findViewById(R.id.login);

        mAuth = FirebaseAuth.getInstance();

        loginText.setOnClickListener(this);
        registerButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view == loginText) {
            startActivity(new Intent(RegisterActivity.this, LoginActivity.class));
        }
        if (view == registerButton) {
            registerUser();
        }
    }

    private void registerUser() {
        firstName = firstNameET.getText().toString().trim();
        lastName = lastNameET.getText().toString().trim();
        email = emailAddress.getText().toString().trim();
        pass = password.getText().toString().trim();

        if (firstName.equals("")) {
            firstNameET.setError(NO_BlANK_TEXT);
        } else if (lastName.equals("")) {
            lastNameET.setError(NO_BlANK_TEXT);
        } else if (email.equals("")){
            emailAddress.setError(NO_BlANK_TEXT);
        } else if (pass.equals("")){
            password.setError(NO_BlANK_TEXT);
        } else {
            mAuth.createUserWithEmailAndPassword(email, pass)
                    .addOnCompleteListener(this, new OnCompleteListener<AuthResult>() {
                        @Override
                        public void onComplete(@NonNull Task<AuthResult> task) {
                            if (task.isSuccessful()) {
                                //create user's data in database
                                String name = firstName + " " + lastName;
                                User user = new User(name,email);
                                usersRef.push().setValue(user);

                                if (mAuth.getCurrentUser() != null) {
                                    getUserData();
                                    Intent chatListIntent = new Intent(RegisterActivity.this,
                                            ChatListActivity.class);
                                    chatListIntent.putExtra("currentUser", currentUser);
                                    startActivity(chatListIntent);
                                }
                                Log.d(TAG, "createUserWithEmail:success");
                            } else {
                                Log.w(TAG, "createUserWithEmail:failure", task.getException());
                                Toast.makeText(RegisterActivity.this, "Authentication failed.",
                                        Toast.LENGTH_SHORT).show();
                            }
                        }
                    });
        }
    }
    private void getUserData() {
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
                    }
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });
    }
}