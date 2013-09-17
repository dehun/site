# Game server with erlang. Part 0x0 #
## Intro ##
One day we got offer to develop a game that will be hosted in social networks.
The game design was a classic bottle game with gifts, mail and so on.

## Spoiler ##
The project really was never launched to production due to conflict with customers.
We failed to create a good contract with a scope of work to do.
Instead we created contract like - 'We will write a game for you. You will give us xxxx$ money'.
So this opened unlimited potential for things like "We don't like this feature implemented this way. Please reimplement".
So after many such requests we just decided to broke the contract.
They give us 10% of money forward - we spent this money on game art - so we returned them an art.
All the code we left for our future usage.

So the project failed. But I got some interesting experience and want to share it with you.

## Requirements ##
Game is real time and should be as fast as possible.
So we have to make a persistent connection - HTTP will be too slow for such task.
And on HTTP you are able only to send messages from client to server.
But sending messages from server to client involves some very tricky techniques like HTTP pooling or websockets.
The pure TCP connection is a better choice for such kind of games.
The main challenge - customers requested server to support up to 50k users online.

## Technology stack ##
This project overlapped with my high interest to erlang & OTP - so I chose erlang as primary server-side language.
Database chose fall on Mnesia - this is erlang embedded database with many reach features like clustering, transactions and so on.
Client side was decided to be written on ActionScript3(adobe flash) - a quite classic choice for such kind of tasks.

	def print_one(x, y):
		print 123
		print x, y
		print y[1]

	