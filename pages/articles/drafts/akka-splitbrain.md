## split brain ##
By default akka does not ship any split brain resolver.

### what is split brain ###
Split brain is scenario when some nodes in the cluster can't reach each other. 
As a result we got a system ("brain") splitted in two or more parts. 
This state can lead to problems in cases when parts take different decisions and can't converge later on into valid state.
E.g. first part deleted user `A`, while second part adds user `A` into friends of user `B`.

### asynchronous vs partly synchronous ###
What can cause one node not being able to reach another? In case if node `A` is unreachable from node `B` it can mean one of next things

- connection between nodes is broken
- node `A` crashed (software crashed or hardware failure)

Unfortunatelly we don't have reliable mechanisms to detect any of those states. 
We have to impose upper limit on message delivery time in order to have somewhat reliable detection that system is unreachable.


It's easy to understand if we think about systems `A` and `B` as humans, where human `B` sends question to human `A` via regular mail and awaits for answer.
Possible scenarios are

- `A` is dead
- `A` is very very busy now, and takes long time to reply
- envelope with question was lost






Well, it can't in truly asynchronous network - with no upper bound on how much message can take to get delivered. We can't built anything for truly asynchronous network.
But we can do for partly synchronous - when we have upper bound on message travel time. So further on we assume that our network is partly synchronous.

### why resolve split brain ###
In case of split brain warranties given to us by akka ClusterSingleton and AkkaSharding are broken.
As we can't say is node that was hosting ClusterSingleton is dead, or simply unreachable - we can't take a decision should we start ClusterSingleton on other node or not.

### auto downing ###
Akka does ship auto-downing feature but it's barely useful - it can't be used as split brain resolver. It's deprecated and should be avoided.

### commercial solutions ###
Akka does have commercial plugin with split brain resolvers. // TBD: link

### implementing your own ###
Stay tuned, I am going to write an article about implementing your own resolver for fixed size cluster.
