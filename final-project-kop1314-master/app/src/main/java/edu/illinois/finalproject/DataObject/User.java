package edu.illinois.finalproject.DataObject;

import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;

public class User implements Parcelable {
    private String email;
    private String name;
    private ArrayList<FriendRef> friend_list;

    //default constructor
    public User() {
    }

    //construct a User with name and emailAdress
    public User(String name, String email) {
        this.name = name;
        this.email = email;
        friend_list = new ArrayList<FriendRef>();
    }

    /**
     * get name of this user
     * @return a string stores name of this user
     */
    public String getName() {
        return name;
    }

    /**
     * get emailAdress of this user
     * @return a string stores emailAdress address of this user
     */
    public String getEmail() {
        return email;
    }

    /**
     * get a list of friend reference
     * @return a FriendRef arrayList stores a list of friend references
     */
    public ArrayList<FriendRef> getFriend_list() {
        return friend_list;
    }

    /**
     * set name of this user
     * @param name a string stors a name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * set email of this user
     * @param email a string stores a email
     */
    public void setEmail(String email) {
        this.email = email;
    }

    /**
     * set a list of friend references
     * @param friend_list a friendRef arraylist stores a lisr fo friendRef
     */
    public void setFriend_list(ArrayList<FriendRef> friend_list){
        this.friend_list = new ArrayList<FriendRef>(friend_list);
    }

    /**
     * check if this user has a specific user's email
     * @param emailOfUser a string stores email of a user
     * @return true if has specific email of user, false otherwise
     */
    public boolean hasEmailOfUser(String emailOfUser){
        for (int i = 0; i < friend_list.size(); i++) {
            if (friend_list.get(i).getFriend_email().equals(emailOfUser)) {
                return true;
            }
        }
        return false;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(this.email);
        dest.writeString(this.name);
        dest.writeList(this.friend_list);
    }

    protected User(Parcel in) {
        this.email = in.readString();
        this.name = in.readString();
        this.friend_list = new ArrayList<FriendRef>();
        in.readList(this.friend_list, FriendRef.class.getClassLoader());
    }

    public static final Parcelable.Creator<User> CREATOR = new Parcelable.Creator<User>() {
        @Override
        public User createFromParcel(Parcel source) {
            return new User(source);
        }

        @Override
        public User[] newArray(int size) {
            return new User[size];
        }
    };
}
