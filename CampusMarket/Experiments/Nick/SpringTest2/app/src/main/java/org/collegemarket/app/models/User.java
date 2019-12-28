package org.collegemarket.app.models;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;

@Entity
@Table(name = "users")
public class User
{
    /*--- Class Variables ---*/
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "id")
    private int     Id;
    @Column(name = "user_name")
    private String  userName;
    @Column(name = "password")
    private String password;
    @Column(name = "first_name")
    private String  firstName;
    @Column(name = "last_name")
    private String  lastName;
    
    /*--- Constructors ---*/
    public User() { }
    // Do we need more constructors?

    /*--- Getters/Setters ---*/
    public String getuserName()
    {
        return this.userName;
    }
    public String getPassword()
    {
        return this.password;
    }
    public String getfirstName()
    {
        return this.firstName;
    }
    public String getlastName()
    {
        return this.lastName;
    }
    public int getId()
    {
        return this.Id;
    }
    public void setuserName(String userName)
    {
        this.userName = userName;
    }
    public void setPassword(String password)
    {
        this.password = password;
    }
    public void setfirstName(String firstName)
    {
        this.firstName = firstName;
    }
    public void setlastName(String lastName)
    {
        this.lastName = lastName;
    }
    public void setId(int Id)
    {
        this.Id = Id;
    }
    // No setter for admin.

    /*--- Class Methods ---*/
    @Override
    public String toString()
    {
        String ret = new String();
        // Convert the class info into a string format
        ret = String.format("{Username:%1$s}\n{Id:%2$d}\n{Firstname:%3$s}\n{Lastname:%4$s}",
                            this.userName,
                            this.Id,
                            this.firstName,
                            this.lastName);
        return ret;
    }
}