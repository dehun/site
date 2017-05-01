# Distributed course. Implementing Raft as a student #
## Intro ##
When I got interested in improving my knowledge about distributed systems my friend gave me a brilliant advice - "just implement something, raft for example".
I got a paper about Raft and was quite confused - I have not understood a problem algorithm was intended to solve.
For me it was necessary to go through some other papers before Raft paper to understand the problem and the solution.
The target was to implement Raft as a student in university will do it - not a production ready solution, but solution good enough to understand the algorithm.
You can see the results here - [https://github.com/dehun/distributed-course/blob/master/src/9main/scala/algorithms/RaftBehaviour.scala](https://github.com/dehun/distributed-course/blob/master/src/main/scala/algorithms/RaftBehaviour.scala). It is implemented in emulator in which I have also implemented some other algorithms described in this articles series.
I have decided to write this article as a small helper for people who will go the same path.
Beware though that this is student understanding of Raft, it will contain plenty of errors and misinterpretations.

## Path ##
The entry point for understanding Raft seems to be the original paper ["In Search of an Understandable Consensus Algorithm"](https://raft.github.io/raft.pdf).
When I attempted to jump straight to that paper without any experience with other papers - I did not understand much.
At that moment I did not understand next things:

- the problem
- solution
- solution limitations
- solution applications

## Consensus problem ##
The original Raft paper requires familiarity with consensus problem. 
I have found that it is described best at ["Paxos made simple"](https://www.microsoft.com/en-us/research/publication/paxos-made-simple/) paper.

### Theoretical overview ###
Consensus problem can be stated like this:

Having a proposers that propose a values and a set of acceptors next statements should be true

- a single value is accepted across all acceptors
- a proposed value is accepted
- any value can be proposed

The problem is unsolvable in asynchronous systems (FLP result - ["Impossibility of Distributed Consensus with One Faulty Process"](https://groups.csail.mit.edu/tds/papers/Lynch/jacm85.pdf)). 
However in systems which are synchronous (have upper limit on message delivery time) we have few solutions

- Paxos
- Viewstamped replication
- ZAB (ZooKeeper)
- Raft

What makes Raft to be quite different from Paxos is its aim at practical application and understandability.
Also Raft is created around distributed log and uses a strong leader.
So proposers send values to the leader, and leader appends them into log. 
Then the log is consistent across majority. 

### In practice ###
Lets see how consensus problem arises in our day-to-day practical tasks.
You can often see leader election required. For example see [this questions about zookeeper and hadoop](http://stackoverflow.com/a/38325270)
Or distributed locks service - see [Chubby paper](https://research.google.com/archive/chubby.html).


## Synchronous systems ##
Synchronous system are set of nodes connected via network.
Messages sent over network can be lost (but arrive in order, and not duplicated).
The most important thing about synchronous systems is upper limit on message delivery.
We can safely state that in case if message has not arrived in X seconds then it was lost or never sent.

## Raft ##
Raft is a solution for consensus problem in synchronous systems.
It utilizes timeouts to make use of upper limit on message delivery. 
Also it uses leader election and logical timestamps.
I will describe Raft only briefly here, focusing more on the obstacles that I have encountered.

### Main parts ###
Raft consists of 2 tightly connected procedures - leader election and log replication.
Majority plays important role in both these aspects 

- item is considered replicated only when majority accepted it
- leader is elected only when majority voted for it

Election and Replication are connected through the fact that leader with non-consistent log (with majority) can not be elected.
Raft operates using only 2 RPCs
- AppendEntries - for log replication and heartbeating of a leader
- Vote - to request vote for leader election

### Node states ###
Node can be in next states

- follower - assumes there are a leader somewhere 
- candidate - starts voting process to become a leader
- leader - sends AppendEntries calls to followers doing heartbeating and log replication

### Leader election ###
Leader election in Raft is timeout based and plays together with leader heartbeating with empty AppendEntries call.
A normal healthy leader every X seconds will send an empty AppendEntries call to other Raft nodes.
If node has not received leader heartbeat withing Y seconds it concludes that leader has gone and starts a new term by starting Voting process.
The node that started voting process is called candidate. 
When it received votes from majority it concludes that it is a leader now and starts heartbeating.

### Term ###
Term is just a logical timestamp of a vote. When candidate starts the vote it increases it's term by 1.
Every PRC call in raft carries a current term to update a node. Node will reject any request with term lower than it's own.
In case if term if bigger - it will update it's own term. When leader or candidate receives an PRC with higher term - it converts back to follower. 
Lamports timestamps are important by themselves. Understanding them also helps a lot to understand Raft terms.
I have found that reading ["Time, Clocks, and the Ordering of Events in a Distributed System"](http://amturing.acm.org/p558-lamport.pdf) made it simpler to understand Raft.

### Log replication ###
In Raft log is always replicated from leader to follower using AppendEntries RPC call.
Empty AppendEntries RPC call carries no log items to replicate and is used to indicate that leader is still alive.
Log item is considered to be committed only when it was successfully appended to majority of the followers.
Every log entry contains 
- index - item position in log
- term - at what term item was added
- value - user payload, is not used by Raft

Raft maintains a very important property - if log entry index and term are the same in 2 logs then these logs are consistent (equal) up to the entry index.
This property is checked during the vote and during log replication - so we always elect a leader with consistent (to the majority) log. 
AppendEntries fails if this property does not holds.
There are plenty of indexes used in paper. Lets take a look at them

- commitIndex - the index of last entry that was replicated to the majority of nodes
- matchIndex - at leader per follower - at what index log of follower and log of leader match
- nextIndex - at leader per follower - which entry from leader log to send next to the follower

#### CommitIndex ####
This one indicates the index of last commited entry on the leader. 
When leader replicates an entry it updates a matchIndex. When the majority of matchIndexes get bigger than N then we can set commitIndex to N.
commitIndex also gets updated on the followers - leader sends it via AppendEntries PRC call and if it's bigger than followers commit index - then follower updates its commit index to be the same as leaders commit index. 

#### MatchIndex ####
matchIndex is per follower. It is  index where log of the leader and log of follower matches (term and index are the same).
When leader performs successfull AppendEntries call it updates a matchIndex.

#### NextIndex ####
nextIndex is used by leader only and is index about starting at what log entry should we send log entries to follower.
Leader initializes all nextIndexes to commitIndex and starts replication by issuing AppendEntries calls.
When call returns with failure - leader decrements appropriate nextIndex and retries again. 
When it succeeds then we can setup a matchIndex - it will be the same as nextIndex + length (entries) where PRC call succeeded. 

## Raft and CAP ##
Original paper talks about log consensus for replicated state machines. CAP however talks about client request handling.
Lets say we use that state machines to store key/value pairs. And log contains operations like 

    set x 5
    set y 10
        
If client outside the Raft wants to modify some key/value pair it should push a new log entry to the Raft leader.
Leader will then propagate this entry to the followers.
Leader can answer client with success only when it has committed this log entry (replicated to majority).
So our set operation in this scenario is CP. 

Get request from client for some key/value pair can not be served by leader only if we want data to be the newest.
The get request should also go into the log and be replicated into followers. Because it can be that leader already not a real leader, and just has not observed a new term yet.
This is how sync read in ZooKeeper works for example - sync read causes a write with no data to be performed.

## Raft and FLP ##
Raft sacrifices liveleness in case of asynchronous model. 
We set leader election timeout with certain assumptions about upper limit of message delivery time.
Raft can theoretically block forever in case if half of the nodes takes time longer to respond than configured timeout.
Which means terms will just start without a leader - Raft will block.

## Outro ##
Raft was quite an interesting challenge that required plenty of reading apart from the Raft paper itself.
My understanding of Raft and distributed systems improved a lot during this exercise. However there are still plenty of things I have missed or deliberately omitted.
I hope one day I will implement something like this in production system gaining more experience/understanding to share.

## References ##
- [Raft visualization](http://thesecretlivesofdata.com/raft/)
- [Raftoscope visualization](https://raft.github.io/raftscope/index.html)
