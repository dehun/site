# Commit message #
## Abstract ##
This document describes some my favorite techniques for commit message writing.

## Ticket number in message ##
Almost all the time we can't write a meaningfull commit message. We usualy produce something like:

* fixed config loader
* improved logger unit tests`

or in the worst cases:

* fix
* +++

To make such messages more usefull - just append a ticket number to them:
1. ajr-2034: fixed config loader
2. ajr-3053: fix

Here we append all the long documentation to the ticket. So when you in trouble and don't know why some commit was made - you can just search your bugtracker for specified ticket number.

## Don't duplicate information ##
It is useless to make messages like:

1. john doe: fixed logger
2. 27 jan 2014: almost fixed config loader

In the first sample we specify a commiter name - but cvs already know it.
In the second sample - date of commit. But cvs knows it too.
So both messages contains garbage

## Specify subsystem ##
We can make our messages more informative by appending system::subsystem this commit changes.

* [logger::network] added udp broadcaster
* [game::trading] implemented items repair

## Make atomic commits ##
This strategy insists on a small diffs. Try not to make commits which changes 20+ files and have a diff on 1000+ lines. The worst scenario when you fix two unrelated bugs in one commit or change two unrelared features in differents subsystems.
DONT:

* [logger::network, game::trading] added udp broadcaster and implemented items repair
