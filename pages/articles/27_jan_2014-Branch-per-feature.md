# Branch per feature code review approach #
## Intro ##
Here I will try to explain one of code management and code review techniques.

## Approach ##
Approach is very simple - it consists from the next steps:

1. you take a ticket from a bugtracker queue and put it into 'in progress' state.
2. make a branch from your current dev branch with name like '<ticket-number>-<optional-short-description>'
   for instance somethings like ajd-25-implement-persistent-config
3. make a work on ticket and push a branch to a central repository
   if you have a gitlab - you can create a merge request
4. assign ticket on one of your collegues and put it into 'review' state.
5. your collegue reviews a diff between your feature branch and dev branch.
6. your collegue accepts changes and merges them into dev. assigns ticket on qa team and puts it into 'resolved' state. Or your collegue discard changes and returns ticket to you.

## Advantages ##
### All code is reviewed ###
There are no way code come to a dev branch without review.
All we know situations when code is passed QA team but is low quality and should be rewritten.
This approach allows us to avoid such situations.
### Responsibility matters ###
When ticket is assigned to a concrette person - there are no way he/she will ignore code review.
The responsibility for this code is now on both persons - original author and reviewer.
## Disadvantages ##
### Slow code delivery ###
This approach is very similar to pre commit code review - you can't access unreviewed code.
This is main weakness and the main power of this approach.
### You can choose reviewer ###
You can pick a reviewer who is easy to pass low quality code because he/she is busy or just don't want to make you sad about reject.

# Conclusion #
This is great approach, but have the main disadvantage in process speed - code review can slow down development process.
