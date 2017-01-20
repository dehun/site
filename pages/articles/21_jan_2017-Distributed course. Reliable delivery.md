# Distributed course. Reliable delivery #
## Intro ##
Subj. Network is not reliable - what we send can be lost, reordered or duplicated.
How will we deliver N messages from one node A to node B knowing for sure (at node A) that all messages ended up at node B.
Lets implement simplest acknowledgment protocol possible.
We are not going to care about reordering - we just want to deliver all messages.
This can be useful for example if you are replicating movement of a character in online game - 
if we received message with later position, we just discard all the messages with time prior it that we received later due to reshuffling.

## Basics ##
How are we going to know that message was delivered? Lets just tell node A from node B that it was delivered. 
How much times should we try to deliver message from node A to node B - lets deliver until it will acknowledge that it was delivered.
Lets call that acknowledgment message. When we send a message from node A we are sending 

    ReliableMessage(id:Int, msg:Message).
    
On receiving of this message nodeB immediately sends back 

    Ack(id:Int)
    
What if Ack will get lost - well, we will just redeliver ReliableMessage with the same id and node B will send another Ack to node A upon receive.

## Illustrated ##

Happy path:

     A                     B
    ---                   ---    
     |       RM(1, "a")    |
     +-------------------->|
     |       Ack(1)        |
     <---------------------+
     
Message got lost

     A                     B
    ---                   ---    
     |       RM(1, "a")    |
     +----------------x    |
     |                     |
     * ack time out        |
     |                     |
     |      RM(1, "a")     |
     +--------------------->
     |       Ack(1)        |
     <---------------------+


Ack got lost

     A                     B
    ---                   ---    
     |       RM(1, "a")    |
     +-------------------->|
     |       Ack(1)        |
     |     x---------------+
     * ack time out        |
     |                     |
     |                     |
     |      RM(1, "a")     |
     +--------------------->
     |       Ack(1)        |
     <---------------------+     

## Source code ##
[whole github project](https://github.com/dehun/distributed-course)
[algorithm code](https://github.com/dehun/distributed-course/blob/master/src/main/scala/algorithms/ReliableDeliveryBehaviour.scala)
