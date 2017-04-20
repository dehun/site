# Triangulation with ICMP pings #
## Intro ##
Once I saw a game servers browser that was implemented as a website. 
What was missing is a ping to the server - which is crucial in selection of game server.
Implementing this on client is tricky. So I decided to make some kind of server side solution to this problem.
The idea is based on triangulation - we can determine a point position by knowing distances to 3 predefined points.
We can also determine distance between 2 points by knowing distances from this 2 points to other 3 points.
Now we apply this idea to ping. We assume that ping is dependable on distance mainly.
Lets see how far we can go with this assumption.

## Source code ##
Scala implementation: [https://bitbucket.org/dehun/triping](https://bitbucket.org/dehun/triping)

## Design overview  ##
I decided to make microservices using Scala and Akka.
Frontend on Play framework connected to tripingMaster service. TripingMaster is orchestrating slaves.
Master is doing all the calculations job. Slaves just ping requested address.

## Triangulation basics ##
Triangulation allows you to determine point (X) position by knowing distances from this point to other 3 points (A, B, C).
You can check this visually - you know for example distance AX. You draw a circle around A with radius AX - the circle is possible positions of point X.
Then you draw a circle around B with radius BX. The points where circle around A and circle around B intersect are possible locations of point X.
When you have the third distance CX - you can say for sure where point is.

If we have 2 points (X, Y) between which we want to know distance, and we have 3 other points (A, B, C). And we know distances AX, BX, CX, AY, BY, CY.
Then we can determine distance XY. Note that we don't need a position coordinates of any point.
The problem most likely has some generic elegant solution, I have not found it unfortunately - ended up breaking problem into plenty of special cases.

## Ping triangulation ##
So points A, B, C are our slaves. Points X and Y are some servers ping between each we want to know without making direct ping from X to Y.
Now we make a big assumption - we tell that ping between A and B is distance AB and so on.
So we make pings from A, B, C to X. Then from A, B, C to Y. And then pings from A to B, from B to C and from C to A.

## Results ##
I got really controversal results for this experiment.
One setup showed almost perfect matches of direct pings, and triangulated pings. While the second setup was a total disaster.
I have decided to measure ping from one of my servers (not a part of triping) to random quake live server.
I have grabed list of servers(see scripts/ips.txt) and performed both regular ping from that server, and triangulated ping.

### Slaves at San Francisco, London and Singapore ###
This setup showed quite amazing results. 

         ip         | triangulated ping  | regular ping
    ---------------------------------------------------
    45.79.189.87    | 85.6825094974246   | 78.391
    84.200.93.128   | 7.04401341257384   | 8.205
    45.32.153.115   | 22.983236965387672 | 13.052
    164.132.164.200 | 13.800187454107373 | 9.199
    108.61.171.41   | 22.760650265734707 | 11.320
    104.129.31.142  | 96.27570089281221  | 95.844
    108.61.167.96   | 12.678056299731189 | 1.328
    84.200.93.156   | 3.814916637278892  | 10.971
    217.76.183.88   | 89.8472465365841   | 66.672
    108.61.171.41   | 21.014837339593598 | 11.289
    196.41.98.78    | 132.03788794551448 | 148.059
    45.76.23.226    | 105.56056651349373 | 105.327
    92.222.137.157  | 14.804161375087942 | 8.304
    104.219.168.187 | 126.74310689118872 | 123.802

### Slaves at San Francisco, Bangalore and Singapore ###
This setup was terrible - quite huge error was observed.
Same ips as in first result.

         ip         | triangulated ping  | regular ping
    ---------------------------------------------------
    45.79.189.87    | 186.3012657512644  | 78.471
    84.200.93.128   | 230.44613058175452 | 8.271
    45.32.153.115   | 200.69645486556436 | 13.183
    164.132.164.200 | 155.50009856875144 | 9.748
    108.61.171.41   | 199.7872365131821  | 11.270
    104.129.31.142  | 163.87946892954105 | 95.621
    84.200.93.156   | 238.93446078214882 | 10.990
    217.76.183.88   | 256.31104734954954 | 66.085
    108.61.171.41   | 199.63267781095726 | 11.297
    196.41.98.78    | 344.2206355708042  | 148.900
    45.76.23.226    | 189.19244908088072 | 107.558
    92.222.137.157  | 164.2040603513659  | 8.236
    104.219.168.187 | 165.1868107668724  | 125.622

### Conclusions ###
This technique seems to work quite well in case if at least of of the triangulated points is close to any of slave points.

## Testing ##
I decided to write triangulator itself in a TDD fashion. 
First I separated it into Triangulator module that accepts ping resutls. Then I wrote an public function definition that was returning ???.
And then I just draw cases _on paper_ and used a _physical ruler_ to make my tests data. 
I decided to share this with you guys, as it is first time I am doing such a direct mapping from physical world to my tests

## Applications ##
### When no access on targets ###
One of the applications is case when you don't have access on any of servers you want to measure ping between.
For example web browser client and quake live game server. You can't ssh on any of them and perform ping. 
But you can perform triangulated ping from your servers.
### When load balancing ### 
Other application will be for load balancing - fastest server selection.
Lets say you have 1000 servers and you want to connect client to the fastest one.
One of the solution will be to ping from that 1000 servers that poor client.
But lets say that is not an option. Then you have 3 triangulator slave servers. And you cache pings from them to 1000 of you working servers. 
When client arrives you calculate pings from 3 triangulator slaves to that client. And then you calculate estimated pings picking the lowest one.

## Related publications ##
After I have finished the experiment I decided to take a look what other people have done in this area.

[Wikipedia/Triangulation](https://en.wikipedia.org/wiki/Triangulation)
[IBM technology echo-location](http://www.webopedia.com/TERM/P/ping_triangulation.html)
[StackExchange related question](https://electronics.stackexchange.com/questions/68619/triangulate-with-ping)
