package cmart.app.classes;

public class Post
{
    private String id;
    public Post()
    {
        // this doesn't matter until we get the DB up and running.
        this.id = null;
    }
    public String getId()
    {
        return this.id;
    }
    public void setId(String id)
    {
        this.id = id;
    }
}