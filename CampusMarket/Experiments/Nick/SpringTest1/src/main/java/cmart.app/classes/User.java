package cmart.app.classes;

import java.util.ArrayList;
import cmart.app.classes.Post;

public class User
{
    /*---- Class Variables ----*/
    private String username;
    private int id;
    private String firstName;
    private String lastName;
    private ArrayList<Post> posts;
    //private ArrayList<Post> watchList; // Eventually will want to add a watchList or whatever lists we need.
    /*---- Constructors ----*/
    public User(String username, int id)
    {
        this.username = username;
        this.id = id;
        this.posts = new ArrayList<Post>();    
    }
    public User(String username, int id, String firstName, String lastName)
    {
        this.username = username;
        this.id = id;
        this.firstName = firstName;
        this.lastName = lastName;
        this.posts = new ArrayList<Post>();
    }

    /*---- Getters and Setters ----*/
    public String getUserName()
    {
        return this.username;
    }

    public void setUserName(String username)
    {
        this.username = username;
    }

    public int getId()
    {
        return this.id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public ArrayList<Post> getPosts()
    {
        return this.posts;
    }

    public void addPost(Post user)
    {
        this.posts.add(user);
    }

    public void removePost(Post user)
    {
        this.posts.remove(user);
    }

    public int numPosts()
    {
        return this.posts.size();
    }
}