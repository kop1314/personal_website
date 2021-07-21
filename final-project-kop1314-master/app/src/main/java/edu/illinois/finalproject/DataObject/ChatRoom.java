package edu.illinois.finalproject.DataObject;

import java.util.ArrayList;

public class ChatRoom {
    private String id;
    private ArrayList<String> member_email_list;
    private ArrayList<Message> messages;

    public ChatRoom() {
    }

    public ChatRoom(String id, ArrayList<String> member_email_list) {
        this.id = id;
        this.member_email_list = member_email_list;
        messages = new ArrayList<Message>();
    }

    /**
     * get chatroom id
     * @return string stores id of chatroom
     */
    public String getId() {
        return id;
    }

    /**
     * get emails of chatroom members
     * @return a arraylist stores emails of chatroom members
     */
    public ArrayList<String> getMember_email_list() {
        return member_email_list;
    }

    /**
     * get messages of chatroom
     * @return a arrayList stores messages
     */
    public ArrayList<Message> getMessages() {
        return messages;
    }

    /**
     * set id of chatroom
     * @param id a string id
     */
    public void setId(String id) {
        this.id = id;
    }

    /**
     * set list of emails of members
     * @param member_email_list a string arraylist stores emails of members
     */
    public void setMember_email_list(ArrayList<String> member_email_list) {
        this.member_email_list = member_email_list;
    }

    /**
     * set messages
     * @param messages a Message arraylist stores messages in this chatroom
     */
    public void setMessages(ArrayList<Message> messages) {
        this.messages = messages;
    }
}
