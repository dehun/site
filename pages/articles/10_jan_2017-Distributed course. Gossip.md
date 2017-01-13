# Distributed course. Gossip protocol #
## Intro ##
Gossip protocol allows us to discover nodes when only few nodes know each other.
In other words if every node is vertex in the graph, and every connection is edge - then we would say that gossip protocol allows us to build complete(full) graph from spanning tree.

## Basics ##
Every node task in this algorithm to distribute all its knowledge to other nodes.
Every N seconds we will send to every known node the nodes that that node does not know yet.
Node considered to know other node if it explicitly acknowledges that.

So its goes like this:
Node A tells node B that it knows node C. node A will continue telling node B about node C untill it will acknowledge that. Node B sends acknowledgement that it knows node C to node A. And node A marks node C as known on node B. This will consume memory for storing state for every known node, but will save trafic as we will replicate only unknown nodes.

## Ilustrated ##
TBD

## Examples ##
You and your friends decide to build an address book with all addresses.
However you know only address of few of your friends.
All of you agree to use regular mail. Letters can be lost.
So you decide that every day you will send to all known addresses list of addresses which that address does not know yet. You consider address A to know address B only in case when address A sends you explicitly that it knows address B. When you receive an acknowledgement you mark in your journal that ackowledger knows what he acknowledged. Then when it's time to send messages you send only what that node does not know (according to journal), but what you do know. At some moment nodes will discover each other and there are will be no more messages to send. However it is impossible to tell when this happened from single node perspective.

## Implementation ##

    class ReliableGossipBehaviour(val initialNodes:Set[Node.NodeId]) extends NodeBehaviour {
    var knowledge:Map[Node.NodeId, Set[Node.NodeId]] = Map()

    override def init(node: Node): Unit = {
      knowledge = knowledge.updated(node.nodeId, initialNodes)
    }

    // on every tick we are going to replicate to all known nodes the nodes unknown to particular node
    // we are considering nodes to be known to other node only if node send ack to us
    override def onMessage(sender: Channel, msg: Message, node: Node, time: Int): Unit = msg match {
      case Messages.Gossip(nodes, senderId) => {
        knowledge = knowledge.updated(node.nodeId, knowledge.getOrElse(node.nodeId, Set()) ++ nodes + senderId)
        sender.send(node.input, Messages.GossipAck(nodes, node.nodeId))
      }

      case GossipAck(nodes, acker) => {
        knowledge = knowledge.updated(acker, knowledge.getOrElse(acker, Set()) ++ nodes)
      }
    }

    override def tick(time: Int, node: Node): Unit = {
      knowledge(node.nodeId).foreach(otherNodeName => {
        val ourKnowledge = knowledge(node.nodeId)
        val otherKnowledge = knowledge.getOrElse(otherNodeName, Set())
        val knowledgeToSend = ourKnowledge -- otherKnowledge
        if (!knowledgeToSend.isEmpty) {
          node.cluster.get.nodes(otherNodeName).input.send(node.input, Messages.Gossip(knowledgeToSend, node.nodeId))
        }
      })
    }
  }


## Test example ##

  it should "gossiping over unreliable channel requires different approach. spanning tree" in {
    def spawnNode(name:String, knows:Set[String]) = new Node(name,
      new DroppingChannel(new ReliableChannel(),Stream.iterate(Random.nextBoolean())((_:Boolean) => Random.nextBoolean())), // 50% chance of drop
      new ReliableGossipBehaviour(knows), // we are using special gossiping behaviour
      new ReliableStorage[Int]())
    val nodeNames = Set("A", "B", "C", "D", "E", "F", "G", "H")
    //  A           E - H
    //     \      /
    //      C - D
    //     /     \
    //  B         F - G

    val cluster = Cluster.fromNodeList(List(
      spawnNode("A", Set("A", "C")),
      spawnNode("B", Set("B", "C")),
      spawnNode("C", Set("C", "A", "B", "D")),
      spawnNode("D", Set("D", "E", "F", "C")),
      spawnNode("E", Set("E", "D", "H")),
      spawnNode("H", Set("H", "E")),
      spawnNode("F", Set("F", "D", "G")),
      spawnNode("G", Set("G", "F"))))

    (1 to 500).foreach(cluster.tick)
    // all nodes know each other
    assert(cluster.nodes.values.forall(n => n.behaviour.asInstanceOf[ReliableGossipBehaviour].knowledge(n.nodeId) === nodeNames))
    // no more messages needed
    val previousMessagesCount = Messages.Gossip.instantiations
    cluster.tick(501)
    assert (Messages.Gossip.instantiations === previousMessagesCount)
  }


## Source code ##
[source code](https://github.com/dehun/distributed-course)

## References ##
- [wiki](https://en.wikipedia.org/wiki/Gossip_protocol)
