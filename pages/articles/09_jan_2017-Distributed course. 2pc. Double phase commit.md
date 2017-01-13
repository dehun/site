# Distributed course. Double phase commit (2pc) #
## Intro ##
Double phase commit is one of the most popular distributed commit/consensus agreement algorithms.

## Basics ##
2pc has a coordinator and acceptor role. 
Coordinator initiates the transaction by asking all the acceptors. 
The acceptors then reply with Yes or No. In case if all acceptors voted yes - coordinator sends commit to all acceptors. In case if at least one acceptor send No - coordinator sends Fail to all acceptors.
2pc ensures that all nodes will have the same value after execution.

## Illustrated ##
Accepting proposal
```
  coordinator                     acceptor1           acceptor2
      |          propose(value)      |                    |
      +----------------------------->+                    |
      |                              |   propose(value)   |
      +-------------------------------------------------->+
      |          accept()            |                    |
      +<-----------------------------+                    |
      |          accept()            |                    |
      +<--------------------------------------------------+
      |          commit              |                    |
      +----------------------------->+                    |
      |          commit              |                    |        
      +-------------------------------------------------->+
```

Rejecting proposal
```
  coordinator                     acceptor1           acceptor2
      |          propose(value)      |                    |
      +----------------------------->+                    |
      |                              |   propose(value)   |
      +-------------------------------------------------->+
      |          accept()            |                    |
      +<-----------------------------+                    |
      |          reject()            |                    |
      +<--------------------------------------------------+
      |          cancel              |                    |
      +----------------------------->+                    |
      |          cancel              |                    |
      +-------------------------------------------------->+
```


## Examples ##
### Happy path - accept ###
You want to make tattoo on the forehead with all your friends. You send a letter to all of them with design proposal.
All of them send a letter back with their confirmation of the proposal you sent.
You then send confirmation mail to everyone. Now everyone agreed about tattoo design.

### Happy path - reject ###
You send everyone letter with tattoo design. One of the friends replies with reject.
You send all other friends that the that design is canceled.
Here it is important that nobody did a wrong tattoo.

### Fail path - unreliable network ###
You send everyone letter about tattoo. But one letter got lost.
Now you are waiting for all your friends replies. You get all of them except one.
Now protocol is blocked - you will wait forever for the last one reply.
You can try to fix this issue with a time-out. Lets say letter reaches your friend, but he replies slowly.
You time-out his reply and send everybody a cancellation.

Or all letters reach your friends, but one of the reply letters got lost - you will wait forever for reply.
Or all letters reach your friends, all replies reach you, but your confirmation got lost - they will be blocked waiting for reply forever(and they can't make other tattoo, as place is already reserved).

### Fail path - coordinator failure ###
You send everyone a letter and then suddenly go out of town.
Everybody replies with confirmation, however you are not up there to send confirmation - all your friends will wait indefinitely for you.

Or coordinator can fail during sending confirmation messages - then nodes without confirmation will block.
We can try to introduce watchdog node to avoid this, but then nodes should be remember what they have voted, so watchdog can ask them and finish the transaction.

## Implementation ##
[source code](https://github.com/dehun/distributed-course/blob/master/src/main/scala/algorithms/DoublePhaseCommitBehaviour.scala)

## Links ##

- http://the-paper-trail.org/blog/consensus-protocols-two-phase-commit/
