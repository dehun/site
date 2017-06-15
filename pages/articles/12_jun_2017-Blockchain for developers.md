# Blockchain for developer #
## Intro ##
At the time of writing bitcoin got over 3k usd. 
While being interesting asset for investments it also contains quite amazing technology in its core called blockchain.
Huge amount of articles have been written about it, however most of them quite investor centered. Lets take a quick look from developer point of view.

## Double spend ##
Bitcoin has a solution for double spend problem that was long standing problem in creating distributed crypto currency.
Solution is reaching the consensus about transactions log among the network peers. 
Reaching consensus means that we have synchronized log of transactions across all participants. 

## Distributed algorithm problem ##
Blockchain is solution to byzantium consensus problem. It is different from other solutions in a way that one should have some computing power in order to vote.
This is problem about getting nodes to agree on some value (which is similar for consensus problem). But there is a plot twist - some nodes can misbehave/lie.

Blockchain is list of blocks. Every block containing hash of previous block. The most interesting part about it is how block gets appended to the blockchain.
Lets say that network is all participants. Only whole network should be able to append block to the chain. This is ensured through prof of work mechanism. Only network can solve prof of work.

Prof of work should be something that is very hard to produce, but very easy to verify. 
For example sha hash of the block. We put a random value into the block. Hash it and ensure that hash is less than target (has some amount of leading zeros).
This is very hard to produce (you should bruteforce random value and check hash) and very easy to verify (just hash and check against target).

Blockchain solution did not appeared out of thin air - it is based on hashcash and digital timestamping.
This are referenced in the original paper (https://bitcoin.org/bitcoin.pdf).

## Hashcash ##
Hash cash is anti DoS idea. For example you have a website which you want to protect. 
In order to do a request server will ask client to do some work and send a prof of work (hard to produce, easy to verify).
So in order to DoS such a service attacker requires way more computing power than without applying this technique.

## Digital timestamping ##
This is idea of 3rd party for timestamping that does not require any trust. 
In order to timestamp document one hashes it's content and sends to service.
Service appends this documents hash into chains block where every block contains hash of previous block.
In this way you can prove 

- that your document content has not been altered (hash will not match)
- your document appeared after documents in blocks prior its block, and before documents in blocks after

All this in a way that does not require trusted third party and storing of document content.

## Hashcash + digital timestamping ##
So we have a prof of work which only whole network can produce in order to append a block.
And we have digital timestamping that assures that transactions/content happen in correct order.

## Bitcoin prof of work ##
Bitcoin uses double sha hash of the block header should be less than target. 
Block header contains hash of all transactions, network time, previous block hash, and some other information.
Target depends on how fast network produces blocks. Network should produce blocks every 10 minutes. 
In case if network produces blocks to fast - target decreases (increasing difficulty), othervise target increases (decreasing difficulty).
In order to calculate/agree how fast network produces blocks network timestamp is written into block header. 
Network time is average time of peers. Block will be rejected however if its timestamp is 2 hours difference from current node network time.

## Transactions ##
When we have blockchain that allows you to synchronize/make consensus about order of blocks, content of the blocks and ensuring that only the whole network working together can extend that chain, we can introduce transactions.
Properties of blockchain ensure that transactions will appear in order, and will not be deleted from the chain (Prof of work will not allow that, something more powerful than network needed).

Bitcoin operates with inputs and outputs. Transaction is just transfering of inputs into outputs. Outputs that can be used as a new inputs.
When block is generated - it contains a coinbase transaction. A transaction that have outputs without inputs. Every input contains a public key of the owner.
When person wants to transfer that input to some other person - it creates a transaction where output (new public key) is specified and a signature is attached.
Other parties can verify that transaction is valid - by checking that signature matches a public key of the input.

Bitcoin transaction contains many inputs and outputs. In case if you do not want to transfer whole amount of input - you create a new output that contains your public key.
For example you have input with 50 bitcoins. You create a transaction using that input and sending 25 bitcoins to your friend, and 25 bitcoins back to yourself. 
Because inputs should be fully used.

## Transaction fee ##
Bitcoin contains 1 mb limitation on the block size. Which means that not all the transactions will be included in the next block.
Person which creates bitcoin can select which transactions to include. In order to give some transactions a priority and reward the miner (apart from coinbase transaction) - transaction fees have been introduced.
When user creates a transaction it not fully sends an input to output, but leave out some fee for the miner. Miner can (and should) append that fee to his coinbase transaction.

