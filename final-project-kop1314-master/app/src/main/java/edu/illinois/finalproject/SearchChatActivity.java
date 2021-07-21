package edu.illinois.finalproject;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.SearchView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import edu.illinois.finalproject.DataObject.User;

public class SearchChatActivity extends AppCompatActivity implements SearchView.OnQueryTextListener,
        View.OnClickListener{
    public static final String EMAIL="123@gmail.com";
    private Button cancelBTN;
    private String[] usernames;
    private SearchView searchView;
    private RecyclerView recyclerView;
    private SearchChatAdapter adapter;
    private ArrayList<User> friendList = new ArrayList<User>();
    private User currentUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.search_chat);

        final Intent intent = getIntent();
        friendList = intent.getParcelableArrayListExtra("friendList");

        recyclerView = (RecyclerView) findViewById(R.id.search_listview);
        LinearLayoutManager linearLayoutManager =
                new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false);
        recyclerView.setLayoutManager(linearLayoutManager);

        adapter = new SearchChatAdapter(friendList);
        recyclerView.setAdapter(adapter);

        searchView = (SearchView) findViewById(R.id.search_searchView);
        searchView.setOnQueryTextListener(this);

        cancelBTN = (Button) findViewById(R.id.search_cancelBTN);
        cancelBTN.setOnClickListener(this);
    }


    @Override
    public boolean onQueryTextSubmit(String query) {
        return false;
    }

    @Override
    public boolean onQueryTextChange(String newText) {
        String text = newText;
        adapter.filter(text);
        return false;
    }

    @Override
    public void onClick(View view) {
        if (view == cancelBTN) {
            Intent chatListIntent = new Intent(SearchChatActivity.this, ChatListActivity.class);
            startActivity(chatListIntent);
        }
    }
}
