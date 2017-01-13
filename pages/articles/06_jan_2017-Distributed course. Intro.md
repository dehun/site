# Distributed course. Intro, Framework #
## Motivation ##
At the end of 2016 I decided to switch to most popular functional programming language at the moment - scala.
I decided not to just switch my primary programming language, but also change a domain I am working in - to some backend distributed systems. 
This is how I ended up doing this small course. 
It is not a course really, it is planned just as a bunch of fun articles to play with distributed algorithms and sparkle an interest.

## Emulation framework ##
Short feedback loop provides more fun, so instead of writing real network services I decided to write an emulator.
The idea that you can specify the configuration of network and machines, and then run some algorithm on that configuration. 
You don't need any real network setted up, or plenty of hardware/vms.

### Emulation framework overview ###
Framework is written in scala (see motivation) and uses scalatest.
_The idea that we are going to write a tests to demonstrate how different algorithms works_.
In such a way we will be sure that it works, and we will be able to have ultra-fast feedback loop.
Lets describe the main parts.

#### Cluster ####
Cluster consists from list of nodes, you can get a node by name from the cluster. You can tick the whole cluster with some particular time. 

#### Cluster Tick ####
Tick is not a component but idea, when you tick the cluster - it ticks all the nodes.
Node on tick processes messages from input channel and does some other work. 
Every tick has a time(just int for simplicity) that allows nodes to get sense of time (for timeouts implementation for example.)

#### Node ####
Node represents single node (machine), it has behaviour, has input channel and has storage.

#### Channel ####
Channel represents network channel. You can send message to the channel, and you can receive message from the channel. 
Framework contains some interesting channel implementations that emulate channel without reliable delivery, or channel that duplicates messages sometimes.
We also have a regular channel that just deliveres message to recepient.

#### Storage ####
Storage is representation of machine storage (e.g. database running on machine). ReliableStorage for example will survive reboot. 
While unreliable storage will loose all data during restart.

#### Behaviour ####
The most important - this is where all algorithms are implemented - it represents what node will do when it will receive message or when the tick will be invoced.

## Failure modes ##
This is short note on terminology.

- fail-stop - when node stops and never recovers
- fail-recover - when node stops and then recovers
