# Projects #
## kissbang ##
[sources (erlang)](https://bitbucket.org/dehun/kissbang/)

This is almost finished erlang social game server. It supports 3 social russian platforms vk.com, odnoklasniki.ru, moimir.ru.
I wrote a set of articles about the architecture of this projects. Check the [articles](articles.html) page.

## protogen ##
[sources (python)](https://bitbucket.org/dehun/protogen/)

This is protocol generator. The idea is that you have a DDL(data definition language),
and you describe with this DDL all the messages you wanna to have in protocol.
Protogen takes this source in DDL and generates the code for serialization/deserialization/handling of this messages.
Currently supported languages are python and erlang. Messages are encoded into json.
The project is kinda abandoned due to time lack.

## xboomx ##
[sources (python)](https://bitbucket.org/dehun/xboomx/)

xboomx is wrapper around the dmenu. It is also a launcher. All the things it done is just sorting commands to launch according to their launch frequency.
In other words - if you launch emacs and lxterminal all the time - they will appear in the list of commands first.

[project wiki](https://bitbucket.org/dehun/xboomx/wiki/Home)

## lj frenzy ##
[sources (perl, sql)](https://bitbucket.org/dehun/ljfrenzy/)

ljfrenzy is a bot for livejournal. The main aim is to make as many mutual friends as possible.
You specify the communities to gather victims from. And then bot adds this people to your friends.
If they don't friend you mutualy during next X days - they got deleted from your friends.
So as result you got something about ~700 mutual friends in month.
And the coolest thing is that you can "filter" your auditory to get the target audience using communities.
For example if you want all people from kiev you need just to add the kiev community to a list.

The project is abandoned now due to my livejournal inactivity. Last tested at the beginning of 2013.

## my homepage ##
[sources (python, html, css)](https://bitbucket.org/dehun/site/)

Yes. This particular site. It is written with python + markdown + html + css.
Maybe the most interesting thing is that fact that it is completly static for performance & simplicity.

## sforward ##
[sources (C)](https://bitbucket.org/dehun/sforward/)

Simple tcp/udp port forwarder. Written just for fun & for traversing through one bad company's traffic shaper.
Tested under linux.
