# on unreachability, death, reliability, time and waste #
How are __unreachability__, __unreability__, __death__, __time__ and __waste__ connected?

## unreachability ##
What can cause one node not being able to reach another?
In case if node `A` is unreachable from node `B` it can mean one of next things

- connection between nodes is broken
- node `A` crashed (software crashed or hardware failure)
- node `A` is too slow

Unfortunately we don't have reliable mechanisms to detect any of those states.
We have to impose upper limit on message delivery time in order to have at least some reasoning about system being unreachable.
Lets try to understand why.

## mail example ##
It's easier to reason/understand all of those if we think about systems `A` and `B` as humans, where human `B` sends question to human `A` via regular mail and awaits for answer.
Possible scenarios are

- happy path: `B` sends question to `A`, `A` receives question
- `A` is dead
- `A` is very very busy now, and takes long time to reply
- envelope with question was lost

So can we be certain that `A` will (not) answer our question? Not really.
If envelope get lost and `A` is in perfect health we can send another one. When should we do it?
In case if `A` has no upper bound for answer - we have no way to calculate that timeout.

Lets impose upper bound: we  know that `A` typically replies within `1.day` and one-way delivery takes `1.day`.
Now we know that if `B` have not received anything in `3.days` after he has sent out question - he should consider possible failures described above.
And by `consider` we imply increasing risk of certain scenario. In other words - `B` will start to worry.

So `B` retries his question, however mailman looses his envelope again. In `3.days` after retry, or `6.days` after first question was sent - `B` again faces the same question - is `A` reachable at all?

## death ##
After `B` has not got answer for the second time - it increases possibility of `A` being dead.
It can't tell for sure how `A` is doing.

We can also request our friends `C` and `D` to check how our common friend `A` is doing.
If they can't reach him either, we further increase probability of `A` being dead.

However in many cases `A` appears to be ok after several weeks, he was doing something else - he was just too slow to respond to our question.

As humans we do this kind of reasoning automatically when we interact with anything.
- We press a button on smartphone - no reaction - most likely it's glitched or is broken
- Progress bar is not moving - it's dead
- Web page takes some time to start rendering - network is down
- pulse is not appearing - he is dead

We arrive at all this conclusions by comparing time of response to typical response time in such situations.

But does it even matter is system `A` alive or dead?
What we care about is not how well `A` is doing, but about can we relay on `A`?
It's question of __reliability__.

## time ##
In case if `A` and `B` are not friends, but colleagues. And completion of task that `B` is doing, depends upon some cooperation from `A`.
Then it does not matter what actually happened to `A` - the end result will be uncompleted task.
At some point in time `B` should start implementing task in alternative way in which `A` is not needed, or replaced by somebody else.
`B` should do this to meet deadlines on his task. Because somebody is waiting for that task, somebody is dependent upon it - and can proclaim task as uncompleted upon hitting deadline.
However in case if `B` can't communicate with `A`, and `B` receives next task - it would be foolish of `B` to again ask `A` to help.

## reliability ##
Reliability is our probability of handling request in a good and timely manner.
Relaying on unreliable 3rd parties can decrease our own reliability.
Unless we somehow mitigate their possibility to fail. Which, as almost any risk reduction, comes at cost.

We can give the same task to 2 different systems. Even though they both unreliable - probability of them both failing is way less.
I.e. we have 2 systems with reliability of 80%. The possibility of failure is 20% in case if we use only one such system. If we give task to both then we got only 4% of failure.
And in case of using 3 systems we got 0.8% of failure. In other words we come from failing every 5th request, to failing every 25th and then to failing every 100th.
This of course creates __waste__d work - our resource utilization would be better if we use only one system per request.

In some cases our deadline allows us to issue such requests not in parallel, but in sequential order.
If we observe deadline missed by first system - we issue request to second one. This creates less __waste__, but increases __latency__.
However still can lead to __waste__d work if we hit deadline and issue request to second system - at that moment we end-up in the same situation as with parallel requests.

## waste ##
In case if mailman looses only replies, e.g. communication works only one way - we have a wasted work being done.
`B` asked `A` to do something, `A` completed the task and sent the result. However `B` will not be able to use it.

In other case when `A` is not very fast and `B` has imposed very tight deadline (and has not communicated it to `A`) - `A` will be working on something that has become obsolete before completion.
Such deadlines should be known to the ones who will do the work - `A` should be aware of deadline, so it would be able to stop.

## summary ##
__unreachability__ can be reduced to __unreability__ in most cases.
__death__ does not matter that much, __reliability__ is the thing that actually matters.
__reliability__ can only be defined by using __time__.
Increasing __reliability__ comes at cost of __waste__d work.

To be __reliable__ is to be quick or be dead.

## bonus ##
When we increase amount of requests per second, the 3rd parties we depend upon  to execute those requests are starting to require more management. Their unreliability starts to show up more (percent wise the same, but for human perception it feels like more often because of higher number of requests). All the issues we discussed above about `waste` are becoming more apparent. However the system scales. Compare start-up to big company.
