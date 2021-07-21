package edu.illinois.finalproject.DataObject;

import android.os.Parcel;
import android.os.Parcelable;

public class FriendRef implements Parcelable {
    private String friend_email;
    private String chat_room_id;

    public FriendRef(){
    }

    public FriendRef(String friend_email){
        this.friend_email = friend_email;
        this.chat_room_id = "";
    }

    public FriendRef(String friend_email, String chat_room_id){
        this.friend_email = friend_email;
        this.chat_room_id = chat_room_id;
    }

    /**
     * get friend's email
     * @return a string stores email of friend
     */
    public String getFriend_email() {
        return friend_email;
    }

    /**
     * get id of chatroom
     * @return a string stores id of a chatroom
     */
    public String getChat_room_id() {
        return chat_room_id;
    }

    /**
     * set email of friend
     * @param friend_email a string stores email of friend
     */
    public void setFriend_email(String friend_email) {
        this.friend_email = friend_email;
    }

    /**
     * set id of chatroom where this friend chats
     * @param chat_room_id a string stores id fo chatroom
     */
    public void setChat_room_id(String chat_room_id) {
        this.chat_room_id = chat_room_id;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(this.friend_email);
        dest.writeString(this.chat_room_id);
    }

    protected FriendRef(Parcel in) {
        this.friend_email = in.readString();
        this.chat_room_id = in.readString();
    }

    public static final Parcelable.Creator<FriendRef> CREATOR = new Parcelable.Creator<FriendRef>() {
        @Override
        public FriendRef createFromParcel(Parcel source) {
            return new FriendRef(source);
        }

        @Override
        public FriendRef[] newArray(int size) {
            return new FriendRef[size];
        }
    };
}
