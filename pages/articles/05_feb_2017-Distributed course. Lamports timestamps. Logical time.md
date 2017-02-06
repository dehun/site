# Distributed course. Lamports timestamps. Logical time. Mutual exclusion #
## Intro ##
In a network of machines we do not have a coordinated time precise enough to tell event ordering (A happened before B) when A and B happens on different machines.
That's where Lamport's timestamps come into help - we can arrange total order on all events using them and node id.
I find that this is a nice pre-requisite for understanding other algorithms/techniques like Raft or vector clocks.

## Basics ##
Lamport's timestamps use an idea of causality. Event in the system is caused by some other event (spoilers: sometimes they are not).
The event A that caused the other event B should happen before B. It is impossible otherwise (time traveling only).
Some events however are not related at all. Lets say we have events A, B, C, D. A caused B. C caused D.
What should be the order in such case? Well we can say for sure that B happened after A, and D happened after C. But what is relation of A to C for example?
They are independent, so there are no order for them (but we will impose one later one using node ids, think about it as 2 events happened precisely at the same time and we need some order).

Using this principles lets assign a number on every event, corresponding to its order. The higher the number - the later event has happened.
So if we have B caused by A. Then we should assign x to A, and y to B in a way that y > x. We are going to use increment by one in such cases - ```y = x + 1```.

So how to use this in a cluster? In a cluster of nodes event on one node can cause an event on other node only through message passing.
So what we do we assign at the node start a variable named ```ltime``` to 0. This will be our timer with which we will mark events to specify their time.
When we send a message out, we increment ```lmtime``` by one and attach it to the message, saying to other node what event logical time was.
On receiving we will assign lmtime to maximal value from current lmtime and lmtime attached to message. 

## Illustrated ##

     A                    B                    C
    ---                  ---                  ---
     |        msg(1)      |                    |
     +-------------------->                    |
     |                    |        msg(2)      |
     |                    +-------------------->
     |                    |        msg(2)      |
     +----------------------------------------->
     |                    |        msg(3)      |
     <-----------------------------------------+
     |      msg(4)        |                    |
     +-------------------->                    |
     |                    |                    |
     
What is interesting here is that message from B to C (msg(2)) and message from A to C (msg(2)) - they are independent and happen at the same logical time.
But message from A to B (msg(1)) and from B to C (msg(2)) are dependent.


## Lamport's Mutual Exclusion ##
Lets imagine a problem of a distributed mutex across set of nodes that guards some resource. We don't want 2 nodes accessing the same resource simultaneously.
I.e. we want a mutex, but distributed one. We are going to do this without a leader (with leader this is too simple, right?).

When node will want to lock a mutex it will broadcast a LockRequest. Nodes will put LockRequest in their queues. If its LockRequest was first - then node holds the mutex and can access the resource. 
When it will send a ReleaseRequest - the LockRequest will be deleted from the queue. Now we need a way to tell which request is first. And here is where we will use Lamport's timestamps.
We will assign an logical timestamp to every Lock or Release event. And then we will be able to make a decisions about requests order.
By request queue here we mean really a list sorted by logical timestamp.

Ok so node which wants to lock the mutex do next:
- increment lmstamp by one 
- broadcast LockRequest with it's node id and its lmstamp

All nodes:
- upon receiving set lmstamp to max of current lmstamp and lmstamp from message 
- put request in the list
- reply with RequestOk
- if first request (with lowest timestamp and node.id) is from this node, then this node obtains the lock

## References ##
[Time, Clocks, and  the Ordering of Events in a  Distributed System](http://amturing.acm.org/p558-lamport.pdf)

## Source code ##
[mutex implementation](https://github.com/dehun/distributed-course/blob/master/src/main/scala/algorithms/LamportsMutexBehavior.scala)




