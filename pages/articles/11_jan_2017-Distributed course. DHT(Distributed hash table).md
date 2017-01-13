# Distributed course. DHT(Distributed hash table) #
## Intro ##
So you have a hash table - an associative table where you can quickly lookup by key some value.
What if we would like to store this table on several nodes? DHT is mechanism to do that.
This finds some very surprising applications to me - bittorrent uses dht mechanism to provide a distributed tracker.

## Basics ##
Lets assume we already have some value type and hash function on it that returns an integer.
Lets call that integer a key. And key space is set of all possible key values (Integer.minValue to Integer.maxValue).
DHT just divides a key space between the nodes and makes every node being responsible for some particular part of key space.
So we need keyspace division function, or function that will tell is this key part of this nodes key subspace.
The simpliest one is

    nodeIndex == key % amountOfNodes


Lets say we have cluster of 3 dht nodes.

- the value with key 0 will be stored at first node
- key 1 will be stored on second node
- key 11 will be stored on third node (11 % 3 == 2)

## Client-Server ##
In our toy implementation we are going to put all the logic on the server, and only small amount of logic into the client.
Client will be able to query cluster for some spewcific value, or store some specific value.
Client can direct it's request to any backend dht node. The node then checks if the requested key in it's key-space and

- if it is then it stores/returns value for the key
- if not, then it calculates which node is responsible and forwards request to it

## DHT extension ##
One does not simply extend dht cluster with 3 nodes to cluster with 4 nodes.
You have to redistribute a key space between nodes.
One of the solutions is to spawn a few virtual nodes on real node, and then when needed just move virtual nodes from real node to other real node.
We do not really cover this in implementation or this article.

## Reliability ##
What if one node goes down? Well, the simply solution will be to store a value under 2 different hashes in the dht.
Then we can query with both hashes and process the first reply. The constraint exists however that those 2 hashes should not fall under single node key subspace.
We can create a subdivision function like this
first node is as usual

    nodeIndex == key % amountOfNodes

second hash is ensures other node

    nodeIndex == (key + 1) % amountOfNodes

## Implementation ##


    object DhtBehaviour extends {
      object Messages {
        case class Store(value:Int) extends Message
        case class Query(value:Int) extends Message
        case class QueryFound(value:Int, node:String) extends Message
        case class QueryNotFound(value:Int) extends Message
      }

      object Behaviours {
        class Client(valuesToStore:Stream[Int], backendNodes:List[Node]) extends NodeBehaviour {
          override def onMessage(sender: Channel, msg: Message, node: Node, time: Int): Unit = {}
          override def tick(time: Int, node: Node): Unit = {
            // feed values to random nodes one by one
            if (valuesToStore.size > 0) {
              val choosenBackend = backendNodes(Random.nextInt(backendNodes.size))
              choosenBackend.input.send(node.input, Messages.Store(valuesToStore.head))
              node.behaviour = new Client(valuesToStore.tail, backendNodes)
            }
          }
        }

        case class QueryResult(value:Int, where:Option[String])

        class QueryingClient(private var valuesToQuery:Stream[Int],
                             backendNodes:List[Node]) extends NodeBehaviour {
          var results:Set[QueryResult] = Set.empty

          override def onMessage(sender: Channel, msg: Message, node: Node, time: Int): Unit = msg match {
            case Messages.QueryFound(value, where) => {
              results += QueryResult(value, Some(where))
            } case Messages.QueryNotFound(value) => {
              results += QueryResult(value, None)
            }
          }

          override def tick(time: Int, node: Node): Unit = {
            if (!valuesToQuery.isEmpty) {
              val value = valuesToQuery.head
              valuesToQuery = valuesToQuery.tail
              val choosenBackend = backendNodes(Random.nextInt(backendNodes.size))
              choosenBackend.input.send(node.input, Messages.Query(value))
            }
          }
        }

        class BackendNode extends NodeBehaviour{
          var backendNodes:Option[List[Node]] = None

          override def init(node: Node): Unit = {
            backendNodes = Some(node.cluster.get.nodes.values.filter(n => n.behaviour.isInstanceOf[BackendNode]) toList)
          }

          override def tick(time: Int, node: Node): Unit = super.tick(time, node)

          def pickBackendNode(value:Int):Node = {
            backendNodes.get(value % backendNodes.get.size)
          }

          override def onMessage(sender: Channel, msg: Message, node: Node, time: Int): Unit = msg match {
            case Messages.Store(value) => {
              val backendNode = pickBackendNode(value)
              if (node.eq(backendNode)) {
                node.storage.put(value)
              } else {
                backendNode.input.send(node.input, Messages.Store(value))
              }
            }

            case query@Messages.Query(value) => {
              val onNode = pickBackendNode(value)
              if (onNode.eq(node)) { // it should be here!
                node.storage.asList.find(_ == value) match {
                  case Some(_) => sender.send(node.input, Messages.QueryFound(value, node.nodeId))
                  case None => sender.send(node.input, Messages.QueryNotFound(value))
                }
              } else {
                // it is on other node, forward query there (note sender, it's forwarding)
                onNode.input.send(sender, query)
              }
            }

          }
        }
      }
    }


## Source code ##
[view source code at github](https://github.com/dehun/distributed-course)
