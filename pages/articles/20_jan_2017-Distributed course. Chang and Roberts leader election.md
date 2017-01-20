# Distributed course. Chang and Roberts leader election #
## Intro ##
Chang and Roberts algorithm allows you to elect a leader in network of peers organized in circle.
So nodeA connected to nodeB, nodeB connected to nodeC, nodeC connected to node D, node D connected to nodeA. 
To obtain such an organization you can impose total order on node names or addresses. 
Then having your current node name you can get a next node it should be connected to form a list.

## Basics ##
Algorithm requires each node to have a unique value which is totally ordered.
The node with biggest value becomes a leader after algorithm has been executed.
For example each node has a numeric id which is unique (you can also use strings, but for simplicity reasons I use numbers).
When node detects that it does not have a leader (lets say at the start), then it starts an ellection process.
Node sends an elect message with it's id to next node and becames a participant.
When node receives a message it processes it according to following rules:

- if it is already a participant
  - if id in elect message is greater than its own - then it forwards this elect message to next node
  - if id in elect message is smaller than its own - then it does nothing, it has already send a better candidate.
  - if id in elect message is the same as this node id - node becames a leader
- if it is not a participant
  - if id in elect message is smaller than its own - it becames a participant and send an elect message with its own id to next node
  - if id in elect message is greater than its own - it becames a participant and forwards elect message to next node
  
## Illustrated ##
A(4) - means node A with id 4
e4 - elect message with id 4 in it


    A(4) -- e4  ----> B(3) -- e4 ---> C(11) -- e11 --> D(2)
    ^                                                   |
    |-------------------- e11 --------------------------|
    
And then e11 is going to arrive at A, where it will be forwarded to B. It will end up on C - where it will match with node id, and node C will became a leader.
Also note that message with highest id will always travel through whole ring. 

## Propagating knowledge who is the leader ##
It would be cool to attach senderId to the elect message and the highest id node saw will be the leader.
Unfortunately it's not the case because how will node know that election has finished and no more messages will arrive?
So leader has to announce itself at the end of election if other nodes needs to know who is the leader currently.


## Reliability ##
Algorithm requires a good network (or use of some acknowledgment protocol). 
You can also provide node not only with next node, but also with next-next node. In case if next node is down, you just send your message to next-next node.
However it requires the node to be actually down. Because if the next-node id is higher than this message and it's alive than it will be a problem.

## Source code ##
[all](https://github.com/dehun/distributed-course)
[algorithm implementation](https://github.com/dehun/distributed-course/blob/master/src/main/scala/algorithms/ChangAndRobertsBehaviour.scala)

## References ##
[on wikipedia](https://en.wikipedia.org/wiki/Chang_and_Roberts_algorithm)

