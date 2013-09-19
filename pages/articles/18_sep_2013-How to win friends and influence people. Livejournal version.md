# How to win friends and influence people. Livejournal version #
## History ##
Almost 4 years ago I have lost my job in Vogster entertainment and decided to write some blog promotion software just for fun and some small profit.
The main idea was to make a blog with target audience.
I used it for a while and then abandon. Few monthes ago I get the project from my archive and launched it again - and it still works :)

## Abstract ##
How can we get audience in livejournal.com - people should friend us. But they should find us first.
We can participate in different 'friend me please' lists. But this will not give us target auditory.
By target auditory I mean for example photographers.

So we have 2 problems:
* Find people which are target auditory
* Make that people to subscribe to our journal(friend us)

## People filtering ##
The solution to the first problem is very simple - there are already formed lists by subject we want.
The name of such lists is communities. You need just to open community profile page and you will see 'friends' of this community.
[sample page of random community](http://afisha_ru.livejournal.com/profile)
So all we need is to make a list of communities we will gather victims from. Then rank them.
We don't need dead souls in friends - so the fresh date of last post is good indicator of potential victim.
Also we don't need people who just post enormous number of links on their livejournal - so rank should be affected by the number of links per post.

## Making people our mutual friends ##
This is the simply part - to make people our mutual friend we need to make them our one way friends first.
The problem here is that nobody want's to add a person with 1000 one way friends and 20 mutual friends.
The solution: unfriend person who don't friend you during 5 days from you friend him.

## Implementation ##
[sources (perl, sql)](https://bitbucket.org/dehun/ljfrenzy/)

At that time only one script language I was using for some time was a Perl.
So I implement it in perl in a shell style - there are set of commands which are operating on one database.
For database backend I chose sqlite. But it also supports postgresql and mysql.

### Commands ###
* harvester.pl - goes through decided communities and writes potential victims to db table fresh_meat. Don't add persons who are in 'zombies' table.
* friender.pl - goes through fresh_meat table and friends N number of victims.
* killer.pl - kills people who don't added us to friends during some time.
* syncer.pl - sync the state of our database with the state of livejournal profile. For example somebody friended us - we adding him to a 'friends' table.
* cron.pl - runs all the commands above in the right order with the right timing

### Notes ###
I parse html with regexpes. Yes this is a bad solution but it works and it is easy to write. Remember I was quite a noob while was writing this code.

SQL part is a mess. I was writing it just for fun & to get it to work as fast as possible - so there are many hacks.

It works really slow. But it should not work fast - livejournal will block us if we will do requests very frequently.

## Retrospective ##
Today for the same project I would chose python + beautifulsoup + sqlalchemy.

* python is just the same as perl. But more easy to read/write in my opinion and have a better future.
* beautifulsoup will eliminate the need in regular expressions for parsing HTML pages.
* sqlalchemy for all the sql works. It will prevent all the errors I got with that raw SQL statements.


## Conclusion ##
I am happy with this project. It was a good for its time.
We promoted some livejournals to ~700 active mutual friends in 1-2 months.
