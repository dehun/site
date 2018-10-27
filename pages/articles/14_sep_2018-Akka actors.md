# Akka actors and actor model in general #
## intro ##
This is small rant about akka actors and actor model in general.
While it has some amazing properties, it also has equal amount of drawbacks.

## actors are not typed  ##
A lot of rant on this one everywhere - blog articles and speakers on conferences bashing that `PartialFunction[Any, Unit]` receive function.
Some people reasonably point to typed actors. At this moment they are still in experimental state. 
If you use akka actors you just drop away all the type safety warranties - all interaction with actors are not typed at all.

## actors are introducing complexity for non distributed systems ##
Surprisingly little to no rant on this one. 
Actor is just a queue of messages, with some handler for messages and state. 
It's amazing properties come from the fact how we can access that queue. Pushing into the queue(mailbox) can be done through the network.
Actor location is abstracted. Network is abstracted. 
Everyone likes to bash RPCs for abstracting network. In case of actors it is different as they don't hide anything important about network. 
Your local programming model should be same as remote programming model. 
You always act as you are in worst case scenario and actor you are talking to is remote. 
This introduces a lot of complexity - you now how to think about cases when receiver is dead, slow or message is lost.
Akka documentation has separate section describing message delivery for local actors. They smash a big warning there to be careful what we do with this section.
I think it is precisely for the reasons stated above - in case if you design for local it just won't work in case of remote. RPC all over again.
One extra thing to think about - actor mailbox is a queue. Queue is not endless. Sometimes we precisely configure it not to be infinite. 
In such cases you can encounter message loss even in local actors communications. And most likely you don't have tests for that.  

In case if your system is not going to be distributed, or does not require hyerarchical error handling - don't use actors. 
Use akka-streams, IO/Task/Monix, futures. Those options will provide you with better type safety and will not introduce that extra complexity of distributed systems.

## actors are leaking ##
Lets have a simple situation - we want to make a call to db and load user. We have DbActor, MainActor and WorkerActor. MainActor spawns worker to ask DbActor for a user.
A common pattern would be to spawn a worker and then have `context.setInactivityTimeout(30.seconds)` with handling `ReceiveTimeout` as an error.
One small mistake - you forget to do `context stop self` after successfull operation and you got 2 answers - one success, and one fail in 30 seconds.
If you forget to add inactivity timeout all together into your worker and DbActor went down without answering - you just leaked WorkerActor. It will be there forever.

## akka actors are not playing well with blocking calls ##
Akka actors are running with dispatchers. When you integrating with anything that requires blocking you should manually configure separate dispatcher for that actor.
One of such blocking scenarios is asking another actor for information. Instead of just using `ask` (`?`) you now should do a separate state for awaiting reply.
In erlang you can simple do a `gen_server:call` for that purpose without switching actor behavior. This is possible thanks to green threading - runtime is aware of blocking calls.
In akka we have only dispatchers that are basically just thread pools. 

## cqrs and persistence ##
Persistent actors are different in many ways from regular actors. One of the major differences is inability to change behavior for `receiveRecover`. That works only for `receiveCommand`.  
Which means you can not longer use `context.become` to preserve state in case of modeling FSMs. You can hack around by introducing you own variable that will store current behavior and using that everywhere.
You can also use `PersistentFSM`. But this is still annoying.

## when to use actors ##
I would use actors mostly in cases when I need to model distributed computations. 
In case if that is biggest chunk of a system - most likely I would investigate BEAM based solution on erlang/elixir.
In other cases we still have IO, fs2/akka streams and plain old futures.
