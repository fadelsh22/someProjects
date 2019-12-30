use group44;

drop procedure if exists Q1;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q1`(IN k int, IN Q1Month int, IN Q1Year int)
BEGIN

	SELECT tweet.retweet_count, tweet.tweet_text, tweet.uscreen_name, users.category, users.sub_category 
    FROM users, tweet 
    where tweet.uscreen_name = users.screen_name and MONTH(tweet.created_at) = Q1Month and YEAR(tweet.created_at) = Q1Year
    Order by tweet.retweet_count DESC
    Limit k;
    
END $$
DELIMITER ;

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


drop procedure if exists Q3;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q3`(IN k int, IN Q3Year int)
BEGIN
	SELECT count(distinct users.state) as state_count, group_concat(distinct users.state, ' ') as state_concat, hashtag.hashtagname 
    FROM users, tweet, hashtag
    where tweet.Id = hashtag.tId AND users.screen_name = tweet.uscreen_name AND YEAR(tweet.created_at) = Q3Year and not (users.state = "na")
    Group by hashtag.hashtagname 
    order by state_count desc
    Limit k;
   
END $$
DELIMITER ;



drop procedure if exists Q6;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q6`(IN k int, IN  hashtag varchar(255))
BEGIN
	SELECT distinct users.screen_name, users.state 
    FROM users, tweet, hashtag
    where tweet.Id = hashtag.tId AND users.screen_name = tweet.uscreen_name AND FIND_IN_SET(hashtag.hashtagname,hashtag)
    Order by users.followers_count DESC
    Limit k;
END $$
DELIMITER ;



drop procedure if exists Q10;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Q10`(IN stateList varchar(255), IN Q10Month int, IN Q10Year int) 
BEGIN
	SELECT hashtag.hashtagname, group_concat(distinct users.state, '') as stateList
    FROM users, tweet, hashtag
    where users.screen_name = tweet.uscreen_name AND tweet.Id = hashtag.tId AND FIND_IN_SET(users.state,stateList)
    AND MONTH(tweet.created_at) = Q10Month AND YEAR(tweet.created_at) = Q10Year
    GROUP BY hashtag.hashtagname;
END $$
DELIMITER ;

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
DELIMITER ;

drop procedure if exists QI;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `QI`(IN QIScreen varchar(30), IN QIUname varchar(20),
											IN QIsubcat varchar(20), IN QIcat varchar(20), 
                                            IN QIState varchar(20), IN QIFollowers int, 
                                            IN QIFollowing int)
BEGIN
IF not exists (select screen_name FROM users where users.screen_name = QIScreen) THEN 
	INSERT INTO users VALUES (QIScreen, QIUname, QIsubcat, QIcat, QIState, QIFollowers, QIFollowing);
END IF;
END $$
DELIMITER ;

drop procedure if exists QD;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `QD`(IN QDScreen varchar(30))
BEGIN
IF exists (select screen_name FROM users where users.screen_name = QDScreen) THEN 
	DELETE FROM users WHERE users.screen_name = QDScreen;
END IF;
END $$
DELIMITER ; 


drop procedure if exists Qlogin;
DELIMITER $$
CREATE DEFINER=`farook`@`%` PROCEDURE `Qlogin`(IN QloginScreen varchar(30), IN QloginPass varchar(255))
BEGIN
	select * FROM login where username = QloginScreen and pass = SHA2(QloginPass, 256);
END $$
DELIMITER ; 
