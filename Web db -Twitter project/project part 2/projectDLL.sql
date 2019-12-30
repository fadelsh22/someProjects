USE group44;

drop table if exists URL;
drop table if exists Hashtag;
drop table if exists Tweet;
drop table if exists newTweet;
drop table if exists Users;


create table Users(
screen_name varchar(30),
uname varchar(20),
sub_category varchar(20),
category varchar(30),
state varchar(20),
followers_count int(30),
following_count int(30),
primary key(screen_name)
);

create table Tweet(
Id char(18),
tweet_text varchar(200), 
retweet_count int(20),
retweeted tinyint,
created_at datetime,
uscreen_name varchar(30),
primary key(Id),
foreign key(uscreen_name) references Users(screen_name) on delete cascade
);

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

create table URL(
tId char(18),
url varchar(500),
primary key(tId, url),
foreign key(tId) references Tweet(Id) on delete cascade
);

create table Hashtag(
tId char(18),
hashtagname varchar(50),
primary key(tId,hashtagname),
foreign key(tId) references Tweet(Id) on delete cascade
);

drop trigger if exists Sub_Category_Check;

DELIMITER $$
CREATE TRIGGER Sub_Category_Check 
BEFORE INSERT ON Users
FOR EACH ROW
BEGIN 

IF New.sub_category != 'GOP' and New.sub_category != 'Democrat' and New.sub_category != 'na' and New.sub_category != ''
THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'sub_category can only be GOP, Democrat, na or null';

END IF;
END $$
DELIMITER ;

drop trigger if exists Category_Check;

DELIMITER $$
CREATE TRIGGER Category_Check 
BEFORE INSERT ON Users
FOR EACH ROW
BEGIN 

-- fixing this so house_representatives are not inserted
IF New.category != 'senate_group' and New.category != 'presidential_candidate' and New.category != 'reporter' and 
New.category != 'Senator' and New.category != 'general' and New.category != 'House_representative' and New.category !=  null 
THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'category can only be senate group, presidental candidate, reporter, senator, House_representative, general or null';

END IF;
END $$
DELIMITER ;

drop trigger if exists Presidential_Candidate_State_Check;

DELIMITER $$
CREATE TRIGGER Presidential_Candidate_State_Check 
BEFORE INSERT ON Users
FOR EACH ROW
BEGIN 

IF New.category = 'presidential_candidate' and New.state != 'na'
THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'presidental candidate are not associated with any state';

END IF;
END $$
DELIMITER ;
