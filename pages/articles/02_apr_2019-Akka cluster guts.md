# Akka cluster, guts #
## intro ##
I want to somehow document my findings about akka cluster and decided to do a series of blog posts about it.
This post skips roles and datacenters. While they play important role, we want to focus on the basics first.

## what is akka cluster ##
akka-cluster enables clustering capabilities for akka. Like

- actor singletons - a single actor in the cluster
- actors sharding - sharding of actors through the cluster with single instance warranty
- distributed pub-sub - multicasting messaging capabilities
- distributed data - CRDTs

## per project overview ##
For akka 2.5.6, 7eeb9b5210b626a673f6c6aa90605c5a911c2af7  we got next packages

- akka-cluster - cluster akka `ActorSystem` extension, cluster members managing capabilities
- akka-cluster-tools - distributed pub-sub and singletons implementations
- akka-sharding - actors sharding implementation
- akka-distributed-data - CRDTs, `ORMap`, `ORSet`, etc

And some `typed` stuff:
- akka-cluster-typed - a typed wrapper over akka-cluster
- akka-sharding-typed - same as akka-sharding, but for typed actors

## foundation ##
Lets start with a foundation - akka-cluster.  All other things are built on top of it.
Akka cluster handles cluster membership with total order by age among it's members, so it can perform leader-election like thingy - picking leader to be the oldest member.
Akka-cluster is built on top of akka-actors and akka-remote.

## cluster membership, highlevel overview  ##
Akka cluster organizes it's members into ordered (by age) ring, in which information about members(cluster state overview) is exchanged via gossip with conflict detection via vector clocks.
Cluster member can only join to cluster member that is in `Up` state. To be `Up` cluster member should be joined somewhere.
Cluster member on startup issues `Join` requests towards pre-configured list of seed nodes. The first node in list of seed nodes should be address of self.
If none of seed-nodes(without first element - self) replies - node joins itself, goes to UP state and therefor forms a new cluster.
Other nodes then can issue `Join` requests towards it and be replied with `Welcome`.
The joining node gets into cluster state with `Joining` status and this state is gossiped towards other nodes.
When all members have seen this gossip - leader can promote node into `Up` state.

## member age ##
Node age is determined by `Member.upNumber`, it's not a clock generated timestamp, but a some variation of logical timestamp.
`Member.upNumber` got assigned to member when leader moves it from `Joining` into `Up` (happens in the `ClusterCoreDaemon.leaderActionsOnConvergence`).

## leader ##
Leader is defined as oldest node among reachable members. It's not classical leader election in a sence that several leaders can exist(and perform actions) at the same time (members have have different `GossipOverview.reachability`). However eventually all nodes will reach convergence upon who leader is. 
Leader can perform status changes for members - which results in new gossips. It waits for gossip convergence for promoting node from intermediate state into next.

## gossip ##
Each gossip contains member states, version(vector clock), reachability and seen.
- members - member info together with state. State in the cluster is represented via next FSM diagram that goes between `Joining`, `WeaklyUp`, `Up`, `Leaving`, `Exiting`, `Down`, `Removed`.
- version - vector clock of this gossip
- seen - set of members who saw this gossip
- reachability - overview of reachability

## convergence ##
Any node can mark any other node as `Joining`, `Leaving`, `Exiting`.
Leader can promote node from this state into `Up`, `Down`, `Removed` when gossip convergence was reached.
Gossip convergence is when
- leader receives a gossip that contains all the nodes in the seen set.
- this gossip indicates that all nodes that are `Up` or `Leaving` can see each other(reachability is full)

This means that every node saw that gossip and acknowledged it as non conflicting one. 

e.g. for `Joining`
- node leader waits for all members to see the member in joining state (when all members will put themselvs into seen of gossip).
- then he promotes joining node from `Joining` into `Up` and issues a new gossip with updated version of joining member.

## node restart ##
Akka cluster uses `UniqueAddress` to identify it's members. This address contains regular network address together with randomly generated pid.
This for example allows cluster to recognize when node issues another `Join` request that it was a restart, and not delayed message - allowing this node to participate in the cluster again.

## reachability ##
Like many distributed systems akka cluster uses heartbeating to detect unreachable nodes, or phi-accural detector to be more precise.
Each node (subject) got heaartbeated periodically by N other nodes(observers). If at least one node considers node as unreachable - the node is considered unreachable in all computations(leader, oldest, etc).
Each gossip contains reachability, which is structure describing can one node (observer) reach other node (subject).
It has somewhat complicated versioning - in order to perform proper merging of 2 different gossips, which includes merging of `Reachability`.
`Reachability` contains `records` - sequence of `Reachability.Record`.
`Reachability` also contains `versions` - a vector clock like thingy - `Map[UniqueAddress, Long]`. Where `UniqueAddress` is address of the observer. And long - is version number.
Each `Reachability.Record` contains version stamp - when it was created/changed (`Reachability.change`).
I have not found where `Reachability.Record.version` is used.  I suspect it is currently used only for debug purposes.

### merging ###
When gossip encounters conflicting gossip - it merges 2 reachabilities through `Reachability.merge`.
That one selects newest version of records for each observer (according to `Reachability.versions`).
