# Messaging over tcp #
## Intro ##
TCP is streaming protocol. Messages is datagrams.  
UDP is datagram protocol. But it is not reliable and not ordered.  
Very often we need to send a message over tcp because we need reliability and order preservance over messages.  

## Problem ##

client

    sock.send("msg1")
    sock.send("msg2")

server

    msg1 = sock.recv()
    msg2 = sock.recv()

What will server get? It depends on many factors like MTU, Nahl algorithm and so on.  
But usual cases are next:  

    msg1 = "m"
    msg2 = "sg1msg2"

    msg1 = "msg1msg"
    msg2 = "2"

    msg1 = "msg1msg2"
    msg2 = // still blocked

## Solutions ##
The solution is to know where first message ends and second begins.  
There are severall working approaches for this problem.  

### Magic separator ###
You can use some magic number to separate messages.  
For example you have json ascii serialized messages and 0x00("\0") as a separator for your text.  

### Int prefixed ###
Int-prefixed is the best and most universal choice if you need to transfer binary(or text) messages.  
The solution is next format of sended message


    | sizeof(int) bytes | msg_size bytes                          |
    |-------------------+-----------------------------------------|
    | msg_size          | msg_body                                |
    | 0x00000010        | 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xde |
    |                   | 0xad 0xbe 0xaf 0xb0 0x0b 0xb0 0x0b oxff |

this can be done via next pseudocode

    def send_message(buffer, size):
        _sock.send(htons(size), sizeof(size))
        _sock.send(buffer, size)

Note htons - this is Host TO Network Sequence transformation function.  
This is done to make big endiand and little endian to be able to communicate each other.  

The receiving code is a way more tricky.  
You should handle when you got more than one message, situations when you get a half of header,  
header and half of a message and so on. For inspiration you can take a look at next implementations.

- [Twisted](http://twistedmatrix.com/documents/8.2.0/api/twisted.protocols.basic.IntNStringReceiver.html)
- [ZMQ](https://github.com/zeromq/libzmq/blob/master/src/decoder.hpp)

### Use 3rd party library ###
There are a lot of libraries which can solve transport and message dispatching tasks.
Take a look at next projects:

- [Twisted](https://twistedmatrix.com/trac/) - for python
- [ZMQ](http://zeromq.org/) - supports a lot of languages
- [Apache Thrigt](https://thrift.apache.org/) - in case you need RPC like communications
- [RakNet](http://www.jenkinssoftware.com/) - this is udp game oriented library. But it is awesome

