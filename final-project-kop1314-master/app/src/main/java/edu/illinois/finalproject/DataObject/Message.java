package edu.illinois.finalproject.DataObject;

public class Message {
    private String authorEmail;
    private String text;
    private String imageUrl;

    //default constructor
    public Message(){
    }

    //construct a Message with authorEmail, tet, and imageUrl
    public Message(String authorEmail, String text, String imageUrl) {
        this.authorEmail = authorEmail;
        this.text = text;
        this.imageUrl = imageUrl;
    }

    /**
     * get authorEmail of this message
     * @return a String stores authorEmail's name
     */
    public String getAuthorEmail() {
        return authorEmail;
    }

    /**
     * get text of this message
     * @return a String stores a text
     */
    public String getText() {
        return text;
    }

    /**
     * get imageUrl of image that is sent
     * @return a string stores a encoded imageUrl
     */
    public String getImageUrl() {
        return imageUrl;
    }

    /**
     * set email of author of this message
     * @param authorEmail a string stores a email of this author
     */
    public void setAuthorEmail(String authorEmail) {

        this.authorEmail = authorEmail;
    }

    /**
     * set text in this message
     * @param text a string stores text
     */
    public void setText(String text) {
        this.text = text;
    }

    /**
     * set imageUrl
     * @param imageUrl a string stores image url
     */
    public void setImageUrl(String imageUrl) {
        this.imageUrl = imageUrl;
    }
}