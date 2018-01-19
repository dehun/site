# Projects #
## Learn Functional Programming with Scala ##
Functional programming course using scala language in learn-by-doing style.  
Covers typeclasses, monoids, functors, applicatives, monads, co-functors, co-monads, free monad.  
Sources are at [http://github.com/dehun/learn-fp](http://github.com/dehun/learn-fp).  
Scala, scalatest

## Moshpit ##
Eventually consistent(AP) P2P REST service discovery. Built on top of Akka and uses vector clocks for synchronization.
Sources are at [http://github.com/dehun/moshpit](http://github.com/dehun/moshpit).
Scala, Akka, Akka-Http, scalatest

## BCE ##
BlockChainExperiment is attempt to grasp how blockchain works by implementing our own.
Sources are at [http://github.com/dehun/bce](http://github.com/dehun/bce). 
At the moment it copies bitcoin mostly, but we have plans to adjust PoW and implement some other interesting features.
Haskell.

## triping experiment ##
Triangulation with pings to obtain ping between 2 servers. 
Sources are at [http://github.com/dehun/triping](http://github.com/dehun/triping)
See my article at [20_apr_2017-Triangulation with ICMP pings.html](http://dehun.space/articles/20_apr_2017-Triangulation%20with%20ICMP%20pings.html)
Scala, Akka, Play framework

## hurtmeplenty.space ##
Quake Live matchmaking and on demand server hosting.
Go try it at [http://hurtmeplenty.space](http://hurtmeplenty.space)
Source code is at [http://github.com/dehun/q3mm](http://github.com/dehun/q3mm)

Scala, Akka, Play framework, jquery, zeromq, react, ...

## split-that-bill.com ##
[http://split-that-bill.com](http://split-that-bill.com)
A small expenses calculator for group of people. Was written for fun and as excercise in ReactJs.

## distribued course ##
[sources](https://github.com/dehun/distributed-course)
Scala. A small distributed system emulator with various algorithms implementations

- _raft_
- double phase commit
- three phase commit
- bully
- circle leader election
- etc ...

## allesgif.com ##
[http://allesgif.com](http://allesgif.con)
Website with gifs collection and search. 
Python, django, jquery, mysql, selenium, ...

## eyes\_on\_me ##
[sources (python)](http://github.com/dehun/eyes_on_me/)

eyes\_on\_me is open source project. It's aim is to save your eyes by auto adjusting level of backlight and
white balance of the screen. The main difference from x.flux and redshift is that it can use two adjustment strategies

* webcam
* time of day

In webcam mode it gets the information about light conditions by measuring light conditions with your webcam(very suitable for laptops).  

## xboomx ##
[sources (python)](http://github.com/dehun/xboomx/)

xboomx is wrapper around the dmenu. It is also a launcher. All the things it done is just sorting commands to launch according to their launch frequency.
In other words - if you launch emacs and lxterminal all the time - they will appear in the list of commands first.

## badtaste ##
[sources (erlang)](http://github.com/dehun/badtaste/)

This is almost finished erlang social game server. It supports 3 social russian platforms vk.com, odnoklasniki.ru, moimir.ru.

## emacs configuration ##
[sources (elisp)](http://github.com/dehun/dotemacs/)

My emacs configuration files. You can get some things from it for your .emacs.

## protogen ##
[sources (python)](http://github.com/dehun/protogen/)

This is protocol generator. The idea is that you have a DDL(data definition language),
and you describe with this DDL all the messages you wanna to have in protocol.
Protogen takes this source in DDL and generates the code for serialization/deserialization/handling of this messages.
Currently supported languages are python and erlang. Messages are encoded into json.
The project is kinda abandoned due to time lack.

## lj frenzy ##
[sources (perl, sql)](http://github.com/dehun/ljfrenzy/)

ljfrenzy is a bot for livejournal. The main aim is to make as many mutual friends as possible.
You specify the communities to gather victims from. And then bot adds this people to your friends.
If they don't friend you mutualy during next X days - they got deleted from your friends.
So as result you got something about ~700 mutual friends in month.
And the coolest thing is that you can "filter" your auditory to get the target audience using communities.
For example if you want all people from kiev you need just to add the kiev community to a list.

The project is abandoned now due to my livejournal inactivity. Last tested at the beginning of 2013.

## my homepage ##
[sources (python, html, css)](http://github.com/dehun/site/)

Yes. This particular site. It is written with python + markdown + html + css.
Maybe the most interesting thing is that fact that it is completly static for performance & simplicity.

## sforward ##
[sources (C)](http://github.com/dehun/sforward/)

Simple tcp/udp port forwarder. Written just for fun & for traversing through one bad company's traffic shaper.
Tested under linux.

## flatline ##
[sources (assembly + c++)](http://github.com/dehun/flatline)
An antivirus I wrote back in school with x86-386 emulator (in order to detect packed with upx viruses for example).


## dehumanizer ##
[sources (assembly)](http://github.com/dehun/dehumanizer)
An PE 32bit(Portable Executable) crypter with uncrypter before OEP.
