use group44;

/* newtweet table splits date into, day, month and year. This enables me to use index on month or year to optimize some queries.*/

/*  creates and populates the newtweet table */
drop table if exists newTweet;

create table newTweet(
Id char(18),
tweet_text varchar(200), 
retweet_count int(20),
retweeted tinyint,
tday int(30),
tmonth int(30),
tyear int(30),
uscreen_name varchar(30),
primary key(Id),
foreign key(uscreen_name) references Users(screen_name) on delete cascade
);
Insert into newTweet (Id, tweet_text, retweet_count, retweeted, tday, tmonth, tyear, uscreen_name)
select Id, tweet_text, retweet_count, retweeted, DAY(created_at), MONTH(created_at), YEAR(created_at), uscreen_name FROM tweet;

/* Creates index on month and on year for the newTweet table*/
Create index month_idx on newtweet(tmonth);
Create index year_idx on newtweet(tyear); 

Create index hashtagname_idx on hashtag(hashtagname); 
Create index sub_category_idx on users(sub_category); 
 
/* Q1 OPTIMIZED, index on month using newtweet table*/
drop procedure if exists Q1;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q1`(IN k int, IN Q1Month int, IN Q1Year int)
BEGIN
	SELECT newtweet.retweet_count, newtweet.tweet_text, newtweet.uscreen_name, users.category, users.sub_category 
	FROM users, newtweet 
	where newtweet.uscreen_name = users.screen_name and newtweet.tmonth = Q1Month and newtweet.tyear = Q1Year
	Order by newtweet.retweet_count DESC
	Limit k; 
END $$
DELIMITER ;

    
 
 /*Q2 optimized, Create index on hashtagname */

 


drop procedure if exists Q2;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q2`(IN k int, IN hashtag varchar(50), IN Q2Month int, IN Q2Year int)
BEGIN
	SELECT users.screen_name, users.category, tweet.tweet_text, tweet.retweet_count 
	FROM users, tweet, hashtag
	where tweet.uscreen_name = users.screen_name AND hashtag.tid = tweet.id AND MONTH(tweet.created_at) = Q2Month AND YEAR(tweet.created_at) = Q2Year AND hashtag.hashtagname = hashtag
	Order by tweet.retweet_count DESC
	Limit k;
END $$
DELIMITER ;




/* Q3 optimized, Index on year using newtweet table */
drop procedure if exists Q3;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q3`(IN k int, IN Q3Year int)
BEGIN
	SELECT count(distinct users.state) as state_count, group_concat(distinct users.state, ' ') as state_concat, hashtag.hashtagname 
	FROM users, newtweet, hashtag
	where newtweet.Id = hashtag.tId AND users.screen_name = newtweet.uscreen_name AND tyear = Q3Year
	Group by hashtag.hashtagname 
	order by state_count desc
	Limit k; 
END $$
DELIMITER ;
 

    	
        
/* Q6 Optimized, Using newtweet Table. DONT RUN with Index on hashtagname */
drop procedure if exists Q6;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q6`(IN k int, IN  hashtag varchar(255))
BEGIN  
	SELECT distinct users.screen_name, users.state 
    FROM users, newtweet, hashtag
    where newtweet.Id = hashtag.tId AND users.screen_name = newtweet.uscreen_name AND FIND_IN_SET(hashtag.hashtagname, hashtag)
    Order by users.followers_count DESC
    Limit k;
END $$
DELIMITER ;




/* Q10, Create index on hastagname to make it faster */

drop procedure if exists Q10;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q10`(IN stateList varchar(255), IN Q10Month int, IN Q10Year int) 
BEGIN
	SELECT hashtag.hashtagname, group_concat(distinct users.state, '') as stateList
    FROM users, tweet, hashtag
    where users.screen_name = tweet.uscreen_name AND tweet.Id = hashtag.tId AND FIND_IN_SET(users.state, stateList)
    AND MONTH(tweet.created_at) = Q10Month AND YEAR(tweet.created_at) = Q10Year
    GROUP BY hashtag.hashtagname;
 END $$
DELIMITER ;
   


  
  
 /* Q15, Create index on sub_category to make it faster  */
 

   
drop procedure if exists Q15; 
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q15`(IN sub_category char(20), IN Q15Month int, IN Q15Year int)
BEGIN   
  	SELECT users.screen_name, users.state, group_concat(URL.url, ' ') as url_concat
    FROM users, tweet, url
	WHERE tweet.Id = url.tId AND users.screen_name = tweet.uscreen_name AND users.sub_category = sub_category AND
	MONTH(tweet.created_at) = Q15Month AND YEAR(tweet.created_at) = Q15Year
    GROUP BY users.screen_name;
END $$
DELIMITER ;    
  
    

  
/* Q23, Create index on sub_category to make it faster*/


drop procedure if exists Q23;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q23`(IN sub_category char(30), IN Q23Month varchar(255), IN Q23Year int, IN k int)
BEGIN  
  	SELECT hashtag.hashtagname, count(hashtag.hashtagname) AS hashtag_count
    FROM users, tweet, hashtag
    where tweet.Id = hashtag.tId AND users.screen_name = tweet.uscreen_name AND users.sub_category = sub_category
    AND  FIND_IN_SET(MONTH(tweet.created_at),Q23Month)
    -- '%{$Q23Month}%'
    AND YEAR(tweet.created_at) = Q23Year 
    GROUP BY hashtag.hashtagname
    Order By count(hashtag.hashtagname) DESC
    LIMIT k; 
 END $$
DELIMITER ;
    
    

/*  Q27 cant be optimized    
drop procedure if exists Q27;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q27`(IN Q27MonthA int, IN Q27MonthB int, IN Q27Year int, IN k int)
BEGIN
	SELECT distinct users.screen_name
    FROM users, tweet
    where users.screen_name = tweet.uscreen_name AND YEAR(tweet.created_at) = Q27Year 
    AND (MONTH(tweet.created_at) = Q27MonthA OR MONTH(tweet.created_at = Q27MonthB))
    ORDER BY tweet.retweet_count DESC
    LIMIT k;
END $$
DELIMITER ;  */
  

    

   
    
	

    
    