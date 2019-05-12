## split brain ##
By default akka does not ship any split brain resolver.

### what is split brain ###
Split brain is scenario when some nodes in the cluster can't reach each other. Node can't tell is node on the other end is dead, slow or disconnected from network.

### asynchronous vs partly synchronous ###
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
