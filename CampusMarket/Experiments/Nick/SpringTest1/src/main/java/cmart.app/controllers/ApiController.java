package cmart.app.controllers;

import java.util.Random;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.bind.annotation.RequestParam;

import cmart.app.classes.*;

/**
 * Controller for the RestAPI to communicate with the mobile app
 */
@RestController
@RequestMapping("/api")
public class ApiController
{
    @RequestMapping(value = "", method = RequestMethod.GET)
    public String home()
    {
        return "Mobile API Entry, go to /api/testUser to see sample JSON.";
    }
    @RequestMapping("/testUser")
    public User testUser(@RequestParam(value = "name", defaultValue = "JohnDoe", required = false) String username)
    {
        Random rand = new Random();
        User u = new User(username, rand.nextInt(1024));
        Post p = new Post();
        u.addPost(p);
        return u;
    }
}