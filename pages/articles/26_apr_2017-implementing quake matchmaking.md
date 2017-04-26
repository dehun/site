# Implementing Quake live matchmaking #
## Intro ##
We used to play warsow at work every day. And while warsow ffa is fun and everything, I still much more prefer quake live duel.
The only problem is absence of any kind of matchmaking or "play now" button. It was present back when quake live was free to play.
But then eventually was removed and got plenty of complains from players that it was just teleporting them to random server.
Quake live community and developers are already made huge effort to make easy all kind of services creation around quake live.
As a kind of exercise I decided to implement that play now button for duels, with good skills balancing.
Using Scala, Play framework and Akka.

## Chicken and egg problem ##
The project was doomed for failure right from the start. 
It is important to know how to make estimations, that's why all that questions on the interview about the amount of gas stations in London.
Lets do simple math 

- we have 800 players online at peek time
- most of them are at ffa/ca/instagib servers
- lets assume whooping 20% of them playing duels, that gives us 160 players target group at peek time
- best kind of scenario is all players have the same level and we can match any two of them to play, lets assume that
- however there different regions with different ping to servers. if we host our servers in Amsterdam only people from Europe can play there.
- lets assume Europe region is one of the biggest making third of all players, we left with 53 players at peek hour
- now lets calculate how much players it takes matchmake to succeed
- lets say typical matchmake wait time will be 10 minutes (which is enormously big, but lets assume that for this exercise)
- so we need player at least every 9:59 minutes - lets round to  10 minutes
- if you visit site at arbitrary time and click matchmake - we need someone else to be there already
- we have `60*24` minutes in day, if we want every round there be at least one player to match - we need `60*24/10=144` visits per day, or 6 visits per hour
- in best hour we need to convince 6.0/53 players to play on our servers, which is 11%

11% is quite huge number of auditory to convince - people already have their preferred way to matchmake - like irc or discord.
This is our best case scenario - in worst case we will get way more people needed to use our service for matchmaking to work.
In case if we want wait only 1 minute instead of 10 - then we need to convince _113%_ of target group.

## Chicken and egg problem solution - on demand server spawn ##
After I have almost implemented matchmaking I decided to make the whole thing usable a bit more. At least for me.
So I have added server on demand spawn. Which works like this - you need a server - you click create server and get a server.
Either public or private server. With public server I have also decided to add some kind of matchmaking - server title displays your rating and name - so people can find you through regular server browsers.

## User identification ##
We need to identify users. The nice way that also goes through other systems is steamId.
Quake live dedicated server will report its events with steamId of user even is related to.
[qlstats.net](http://qlstats.net) have an api to query user statistics as json providing steamId.
Steam openId allows us to login user by redirecting it to the steam website. After login was done we get steamId.
Read more about steam open id at [Steam WEB Api documentation](https://steamcommunity.com/dev).

## Game launching ##
Ok, lets say we have matched 2 players. How can we kick the into the game?
Steam provides [Steam browser protocol](https://developer.valvesoftware.com/wiki/Steam_browser_protocol) which allows us to generate a link like
```
    steam://connect/95.85.4.47:27960/AXCQW
```
Here is 95.85.4.47:27960 - ip and port. And AXCQW is password. When user clicks on a link like these steam launcher automatically launches a game and connects him to the quake server.

## Design ##
I decided to split the whole system into next separate components/microservices

- ui - user interface to the system
- front - serves web site
- queue - matchmaking logic itself
- instanceMaster - orchestrates slaves
- instanceSlave - launches actual quake live dedicated servers

## Matchmaking ##
Matchmaking is done using external service to provide statistics ([qlstats.net](http://qlstats.net)).
The statistics comes in form of Glicko rating. Then we just found 2 users with almost the same statistics.

### Happy path overview ###
Lets take a look at happy path to understand how this parts work together

- _user_ goes to _ui_ which is served by _front_ and logins via _steam openid_
- _front_ handles _steam openid_ callback and sets cookies, also makes request to _qlstats_ to obtain glicko rating
- _user_ clicks on matchmaking, _front_ puts user into the _queue_
- _queue_ search for another user with the almost the same glicko rating
- _queue_ requests _instanceMaster_ to create a server
- _instanceMaster_ routes server creation request to random  _instanceSlave_
- _instanceSlave_ spawns quake dedicated live server and starts watching it, sends back to the _queue_ that server is ready
- _queue_ sends to _front_ a link to server with user id to whom deliver the link
- _front_ sends to _ui_ of appropriate user the link
- _user_ clicks on the received link in _ui_ and steam launcher launches quake live and connects to the quake server behid the link

### ui ###
Simple javascript with some react. 
Displays basic information. Matchmakes via websocket to the _front_. Sends on demand server requests via simple http post.

### front ###
Front was written in scala/akka/play and is intended to process http requests from user and serve static content.
It also makes requests to 3rd-paty websites like [qlstats.net](http://qlstats.net) and steam openid login.
Interesting part is matchmaking that requires long wait - was implemented over websocket.

### queue ###
Every minute goes through list of users and if good match found (glickos are the same +-200) then it asks _instanceMaster_ to create a server for matched users.
When server is created it reports back to _front_ that sends via websocket to _ui_ of matched users that match was found.

### instanceSlave ###
Spawns and monitors _quake live dedicated server_s. Can have more than one _quake live dedicated server_.
Registers within _instanceMaster_ on the start - reporting its capacity (how many quake live dedicated servers it can run).

Monitoring of quake live dedicated servers is done via zmq. I have not found good library for scala that will support zmq authentication, so I have made a small wrapper around jeromq.
We tell that user for whom we created server either on demand request or via matchmaking is an owner. In case of matchmaking there are 2 owners.
If owner who requested server has not joined the server within 10 minutes - we kill the server.

### instanceMaster ###
Registers _instanceSlave_s capacity and current usage. When request for server creation comes it routes it to random _instanceSlave_.

## Various interesting bugs ##
### Operator== type checking absence ###
== accepts Any as its argument. Which mean no compiler error will be generated if you do this 
```
val x:String = "1"
val y:Int = 1
val z:Boolean = x == y
```
Use cats/scalaz === to avoid this problem. Also declare type from time to time - at least on what function returns :)
This problem striked me when I compared 2 sets using ==. One set was containing user ids, the other one was containing userInfo structures.

### pattern matching not type checked  ###
Well, it is actually type checked. But when you do this on Any - it is obviously not.
So when you receive something from other actor and do match {case ... case ...} it will be hard to catch during the refactoring.
The solution is not to send tuples directly, but use separate case classes for messages going between actors.
In this way compiler will at least check that you matching what you sending.

### Actor postStop is not called ###
One of the most time consuming bugs occurred when I decided that making a blocking call in actors receive would be a good idea.
After sending PoisonPill to the actor - it has not been stopped - postStop was not called. All kind of other weird side effects occurred.
So simply do not block actors ExecutionContext forever.

## Lessons learned ##
- do simple math before starting a project
- do not block in receive
- use proxy classes to enforce compiler assisted refactoring and type checking
- Any is fucking annoying
- cats to the rescue
- deploy scripts rocks

## Conclusion ##
Go visit it at [http://hurtmeplenty.space](http://hurtmeplenty.space).
On demand server spawn is feature I am using a lot. Now you can just send a link to a friend who you want to play with.

