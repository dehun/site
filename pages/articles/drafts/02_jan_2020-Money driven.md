# money driven #
## prelude ##
This article is a funny mental experiment on making software development methodology in order to understand other methodologies better.
It consists a bit less than whole out of fat trolling presented in an almost logical way. 

You have been warned.

## intro ##
Bitcoint works because of human's greed. It works quite well. Greed I mean. It's something that we can relay on. 
It's why companies are built after all.
Can we use it to build effective development methodology? 
Lets do a crazy mental experiment - mix dog training, psychology, efficient market hypothesis and greed to achieve ultimate software development methodology.

## motivation ##
What makes people to do stuff? We have several theories. 
Maslow hierarchy of needs. Operant conditioning.

Work is something we do for more than 8 hours a day. Or for more than 50% of our awake time. 
So it's reasonable to try to satisfy most of the needs at work. Lets look at the maslow hierarchy

- physiological - no money = no food, no sleep
- safety - no money = no home, no gas & no electricity
- social - you see your collegues more often than your friends or even your spouse, you do form some social relations with them
- esteem - people are trying to get a good reputation with what they doing at work, to be the experts or just good 
- self actualization - people need sense of accomplishment, that what they are doing is important and is recognized achievement

So humans expect reasonably a lot from the time they spend at work. 
How about operant conditioning? A quick reminder about operant conditioning - it's a regular action -> reaction/consequence thing.
It's widely used in dog training. It has a bit funky naming - it easier to think about positive as of an introduction of something, and negative as of withdrawal of something.
When dog performs some action it can have next outcomes

- positive reinforcement - result is introduction of a reward. i.e. dog sits and gets a treat
- positive punishment - result is introduction of a punishment. i.e. dog starts to eat shit and gets jerked on a leash
- negative reinfocement - result is withdrawal of a reward. i.e. dog barks while playing - owner stops playing with a dog
- negative punishment - result is withdrawal of a punishment. i.e. dog was clipped on a leash and not allowed to go anywhere, it sits perfectly and looks owner into the eyes - owner unlicps the leash and allows dog to go

Dog training requires very short feedback loop. If you are trying to reward a dog with a treat, and it takes more than ~15 seconds for you to find a treat - dog already forgot what it was doing and why the treat is suddenly in here.
Same goes for the positive punishment actually - you come home and there are a big pile of turd on the carpet that was produced 2-3 hours ago - dog is not going to connect your screaming at it with the act of defecating on the carpet.
Adult humans are able to learn even when challenged with longer feedback loops, but still do it better with shorter feedback loop. 

So we want to shape rules of the game in order to have

- possibility for satisfaction of needs
- as short feedback loop as possible

This should go through the whole organization, and not be applied only to some particular department.
Then learning will be efficient. In case if manager is doing his job poorly - he should know early and have opportunity to try different approaches to improve. 

## reducing it all to money ##
In case if you will need to expose those motivation mechanisms in some kind of a number what will you do? Money of course.
Can we define operant conditioning thingies using money? 

- positive reinforcement with money is quite easy
- positive punishment also works quite well - your fine for speeding is a nice example
- negative reinforcement - take away the opportunity to earn money, breaking the contract for example
- negative punishment - removal of sanctions or lowering the taxes

As for the maslow - satisfying successfully all those needs usually corelating with having good salary.
I.e. 
- physiological - food costs money
- safety - having a good house requires a good salary
- social - right social connections can ensure you a better job and salary
- esteem - being an expert and having reputation ensures having higher salary
- self actualization - accomplishments are rewarded with higher salary

We are going to base our methodology on money and greed. Those can't fail us.

## company in a company ##
When you look at the challenges of a company they typically include

- raising enough money to survive and strive
- outpacing the competition
- increasing user base
- attracting talent
- exploring new opportunities with r&d

This set of challenges does not intersect at all with the challenges of a typical scrum team.
It does however intersect quite well with a set of challenges of a typical team member.

Lets solve this misconduct by making team to be like a company. It's like everything is a file. But now everything is a company.

## team customers ##
If our team is a company then who are our customers? 
More specifically product owners are still our customers. They do order new features and improvements. 
How do they pay our company? For this kind of works it makes sense to pay per task. 
To not transfer money every day lets do it on schedule every 2-3 weeks. Amount depends of course on amount of completed tasks.

## orders and estimations ##
How do we estimate task? We have 2 dimensions now - how long will it take, and how much will it cost. Time and cost for simplicity.
We estimate time in time intervals, e.g. 1 week or 1 day. Cost we estimate in usd/eur/rub. 
As a customer how can we ensure that team will not fuck-up estimates? 
For time aspect - introduce forfeit - in case if team has not completed task in time it has to pay to the customer
For cost aspect - introduce auction/tender among the teams - teams offer their price tag for each task and customer picks fastest and cheapest ones. 

## team employees ##
Team employees people to perform it's work. It's their problem to hire and retain talent. 
As an isolated company, team has full control over how to manage and what to pay to their employees. 
Team leads are CEO like entities. They are pressed by the market to match employees salary to something reasonable. 
As otherwise employees will join other team or even leave the parent company.

## team composition ##
In this model PO should be able to order whole thing from a single team. 
In case if he orders mobile part of the feature from feature team, and backend part of the feature from backend team, then how can he ensure that parts have been delivered properly?
It becomes more like a DIY thingy for PO, like an Ikea furniture where he has to manually order parts from different shops.

This leaves us with 3 possible compositions 

- mobile team hires backend team as a subcontractor
- backend team hires mobile team as a subcontractor
- each team consists of both backend developers and mobile developers

In order to have auction/tender bids we should go with the 3rd option - cross-functional/feature teams.

## quality ##
How would PO know that whatever was delivered is up to standards and will not cause issues in the future? 
It will be in best financial interest for the team to do as little as possible with minimal acceptable standards.
Standards should be stated formally. Namely definition of done(DoD). So it will be possible to introduce formal inspections from the other teams.
Financially wise team would benefit from other team going down. Hence they will do inspection as good as they can to find issues. Also they should receive bounty for found issues, paid by the inspected team. 

How about bugs? We should penalize each bug, financially of course. So forfeit it is. It's kinda a warranty. 
Bug bounty would be a nice way to encourage bug searching. Who would pay out the bounty? Last team that touched that part of the code.
So far team has next choices for feature delivery
- don't deliver - pay forfeit
- deliver low quality code - pay inspection bounty to competitors
- deliver buggy code - pay bug bounty to the QA

## technical debt ##
How is technical debt going to be handled? In case if your team can sell to PO this task then it's going to be handled as a regular task. 
In case if PO does not want to buy that - it will not be done, as nobody will pay for it. 
PO as any customer wants things as fast as possible at lowest price. 
Team of course wants biggest price possible. However they have to win auction/tender with other teams. So they have to put reasonable price.
Price would include money to cover development costs and possible risks. And team now have quite a lot of real risks - formal inspections, bug bounties and real deadlines.
So final price to touch some shitty piece of codebase would be quite high. PO will have a choice to buy a refactoring - that will make further improvements of the related features way cheaper - as they will be less riskier for the team. 

## buying good stuff ##
Being a PO is not easy. The same as buying stuff is not easy. We want to get best for our money, so we read reviews and check opinions of the experts.
As you can see being a PO is not easy in this scenario. You have to have some basic familarity with software development process. Or have some advisors.
Advisors won't really work well in this scenario, as there are no financial benefit for them.
Nowadays we have people without technical background who are action as PO or scrum masters. Those people are obsolete in this model. 

For the position of PO or buyer we need somebody with good technical knowledge and good knowledge of the domain. He should be motivated to buy good stuff. 
This can be achieved by paying to PO percent from the product profits. Or percent of the received investments.

## but is it webscale? ##
All the organizations want to be agile today. You know - because waterfals are so bad. Nobody saw a real waterfal process, but everybody knows for sure that those are disaster.
Lets check our methodology agains agile manifesto principles to see is it agile enough.

    Our highest priority is to satisfy the customer through early and continuous delivery of valuable software. 
    
PO reward is bound to success of the customers - no sales = no money for PO. No money for PO = no money for the teams. No money for the teams = no work being done. No work being done = product is abandonware at best. 
So PO is going to do everything to ensure that customers are satisfied at the pace that they want. 

    Welcome changing requirements, even late in development. 
    Agile processes harness change for the customer's competitive advantage.
    
Late changing of requirements are pricy. In case if customer want to pay to PO to change the requirements - it will be done.

    Deliver working software frequently, from a couple of weeks to a couple of months, with a preference to the shorter timescale. 
    
This makes it easier for customers to upgrade. Supporting several versions of software in parallel is going to cost extra money. 
In case if this principle will be beneficial for the customers - it will be financially beneficial for the company, then it will be satisfied.

    Business people and developers must work together daily throughout the project. 
    
Teams that have deeper domain knowledge are going to be more successfull. Sharing domain knowledge also ensures lower prices for PO.

    Build projects around motivated individuals. Give them the environment and support they need, and trust them to get the job done. 

The whole methodology is about this basically. All individuals in the process are highly motivated. We also can trust them to get the job done as we understand their motivation very well.

    The most efficient and effective method of conveying information to and within a development team is face-to-face conversation. 
    
If the team is going to find out that this is the most effective way for it to work - they going to adopt it. Otherwise they are going to use proper async communications that are the future anyway.

    Working software is the primary measure of progress.

If it does not work or contains bugs - company losses money. Methodology provides punishment for making not working software - forfeit and bug bounty.

    Agile processes promote sustainable development.
    The sponsors, developers, and users should be able to maintain a constant pace indefinitely.
    
It is as indefinite as a cash flow. As soon as cash stops flowing - it dies out. 

    Continuous attention to technical excellence and good design enhances agility. 

Shitty code is havily penalized with forfeit, formal technical inspections and bug bounties. PO is penalized with hefty prices for pushing features too fast.

    Simplicity--the art of maximizing the amount of work not done--is essential. 
    
Each movement costs real money in this methodology - only the work that has to be done is going to be done.

    The best architectures, requirements, and designs emerge from self-organizing teams.
    
This methodology ensures the level of autonomy of the team that will never be achieved by any scrum team. Team is a company. It can even hire/fire people. 

    At regular intervals, the team reflects on how to become more effective, then tunes and adjusts its behavior accordingly. 
    
Feedback loop is very short with very clear ourcomes. Whole methodology shapes behavior of participants to be the most efficient.
