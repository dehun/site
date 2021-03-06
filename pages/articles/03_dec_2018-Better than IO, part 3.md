# Better than IO, part 3 #
## intro ##
In the [first part](./29_oct_2018-Better than IO, part 1.html) and [second part](./15_nov_2018-Better%20than%20IO,%20part%202.html) we ensured that our little function can produce only desired subset of side effects.
In here we are going to ensure that it's not only can, but it should.

## Should produce a side effect ##
Lets say we want to be even more strict and want always produce a particular side effect from the function `foo`.
A quick reminder - `foo` looks like this - 

    def foo(x:Int):Int
    
And it performs database and logging side effects. 

First thing that comes to mind is use something like Command pattern.

    case class User(userId:String, nickName:String, age:Int)
    sealed trait DoDbAction
    object DoDbAction {
      case class UpdateUserNickname(userId:String, newName:String) extends DoDbAction
    }

    def foo(x:Int):(DoDbAction, Int)
    
Now we sure that `foo` will return some db action. And we can implement some interpreter for Db actions and run it after `foo`. 

However it is not that userful at the moment. What if we want to write a function that first queries the database and then updates the user nickname prepending "Mr. " to it? How do we compose that?
Lets abstract for a moment from the fact that it's database and try a simple list of strings. 
So we first get a string and then we modify it in that list. 

    List("one", "two", "three").map(s => if(s) == "two" "Mr. two" else s)

So we can do it with `map`. So we need a functor. 

Lets introduce a composition `DoDbAction`.

    sealed trait DoDbAction[A]

    object DoDbAction {
      case class Map[A, B](a:DoDbAction[A], fx:A => B) extends DoDbAction[B]
      case class FindUser(userId:UserId) extends DoDbAction[User]
      case class UpdateUserNickname(userId:UserId, newNickname:String) extends DoDbAction[String]
    }
    
    def foo(x:Int):DoDbAction[User] = DoDbAction.FindUser(UserId.fromInt(x))
    def bar(y:User):DoDbAction[String] = DoDbAction.UpdateUserNickName(y.id, "Mr. " + y.nickname)
    
    val z:DoDbAction[String] = DoDbAction.Map(foo(12), bar)
    
Ok, that works. We can also introduce `FlatMap` and `Pure`, and make our `DoDbAction` not only functor, but almost a monad!
However if we introduce `Pure` then it will invalidate our original requirement that function MUST produce a side effect, as we will be able just to wrap some `int` into `Pure` and lie that side effect was produced.

You have probably noticed that we dropped logging. No worries - we are going to get it back in a moment.
Lets start with defining the same structure for the logger as for db.

    sealed trait DoLogAction[A]
    
    object DoLogAction {
      case class Map[A, B](a:DoLogAction[A], fx:A => B) extends DoLogAction[B]
      case class FlatMap[A, B](a:DoLogAction[A], fx:A => DoLogAction[B]) extends DoLogAction[B]
      case class LogMsg(msg:String) extends DoLogAction[Unit]
    }
    
How do we compose this now? This depends on our original question - we requested function to produce side effect for sure. 
So which side effect do we require? We can go with both or either of log and db.

    def fooEither(x:Int):Either[DoDbAction[User], DoLogAction[Unit]]
    def fooBoth(x:Int):(DoDbAction[User], DoLogAction[Unit])

We have some problems with `fooBoth` - how do we interpret this? First `_._1` and then `_._2` second side effect? Or vice versa? 
What if side effects are somewhat connected (e.g. log is logging into database)?

Lets solve this for either of side effects. Original question was about producing any side effect, not about producing db side effect.

So what if we want to write a simple program like this - 

    // log "hello world"
    // get user(x)
    // log user age
    
How do we do this with `Either[DoDbAction[User], DoLogAction[Unit]]`? 
Our `Map` and `FlatMap` for both db and log are defined accepting appropriately DoDbAction and DoLogAction.
It feels like we will need some better sum type than either with appropriate FlatMap.

    trait DoAction[A]
    sealed trait DoLogAction[A] extends DoAction[A]
    sealed trait DoDbAction[A] extends DoAction[A]
    
    object DoAction {
      case class Map[A, B](a:DoAction[A], fx:A => B) extends DoAction[B]
      case class FlatMap[A, B](a:DoAction[A], fx:A => DoAction[B]) extends DoAction[B]
    }
    
Now we can try to  write our program - 

    def foo(x:Int):DoAction[Unit] = {
      DoAction.FlatMap(DoLogAction.LogMsg("hello world"), 
        { _ => DoAction.FlatMap(DoDbAction.FindUser(UserId(x)), {
          user => DoLogAction.LogMsg(s"user age is ${user.age}")})})
    }

In the test we can use different interpreter for database - for example the one that will use `State` monad for storing `User`s.
If we use `State` that effectively we will build from description of computation represented in `DoAction` a actual computation described with `State`.

However defining interpreters is a bit clumsy - you have to define a whole interpreter for the whole `DoAction` every time.
And it would be great to abstract and make reusable `DoAction.Map` and `DoAction.FlatMap`. And make possible to put our algebras into it.

    sealed trait DoAction[S[_], A]
    object DoAction {
        case class Lift[S[_], A](a:S[A]) extends DoAction[S, A]
        case class Map[S[_], A, B](a:DoAction[S, A], f:A => B) extends DoAction[S, B]
        case class FlatMap[S[_], A, B](a:DoAction[S, A], f:A => DoAction[X, B]) extends DoAction[S, B]
    }
    
In here `S[_]` is our algebra. Now we can go back and write Log and Db algebras separately.

    sealed trait DbAlg[A]
    object DbAlg {
        case class FindUser(userId:UserId) extends DbAlg[User]
        case class UpdateUserNickname(userId:UserId, newName:String) extends DbAlg[Unit]
    }
    
    object Db {
        def findUser(userId:UserId):DoAction[DbAlg, User] = 
              DoAction.Lift[DbAlg, User](DbAlg.FindUser(userId))
        def updateUserNickName(userId:UserId, newName:String):DoAction[DbAlg, Unit] = 
              DoAction.Lift[DbAlg, Unit](DbAlg.UpdateUserNickname(userId, newName))
    }
    
    sealed trait LogAlg[A]
    object LogAlg {
        case class LogMsg(msg:String) extends LogAlg[A]
    }
    
    object Log {
        def logMsg(msg:String):DoAction[LogAlg, Unit] = DoAction.Lift[LogAlg, Unit](LogAlg.LogMsg(msg))
    }
    
Can we compose our programs now? Well no, because `DoAction.Map` and `DoAction.FlatMap` are bound to single algebra `S`.
So in case if we want to pull something like this - 

    def foo(x:Int):DoAction[Unit] = {
      DoAction.FlatMap(LogAction.LogMsg("hello world"), 
        { _ => DoAction.FlatMap(DbAction.FindUser(UserId(x)), {
          user => LogAction.LogMsg(s"user age is ${user.age}")})})
    }
    
It won't compile because function we pass to `DoAction.FlatMap[LogAction, Unit, Unit]` actually returns `DoAction.FlatMap[DbAction, Unit, Unit]`. And `LogAction` is not `DbAction`.

As we saw previously we should make a sum type out of `DbAction` and `LogAction`. We can do this with `EitherK`. 

    type Level0[A] = EitherK[LogAction, NetAction, A]
    type AppStack[A] = EitherK[DbAction, Level0, A]
    
Now we should lift everything into `AppStack`.
At this stage this all resembles `Free` monad quite hard. 
Our `DoAction` is almost same thing as a `Free`.

## Free monad ##
So lets use `Free` from `cats`.

    object DbAction {
      case class QueryUser(userId:String) extends DbAction[User]
      case class StoreUser(newUser:User) extends DbAction[Unit]
    }
    
    object Db {
      def queryUser(userId:String) = Free.liftF(DbAction.QueryUser(userId))
      def storeUser(newUser:User) = Free.liftF(DbAction.StoreUser(newUser))
    }

    // same for net and log as above

    def foo(x:Int):Free[AppStack, user] = for {
        user <- Db.queryUser(x.toString).inject[AppStack]
        _ <- Log.logMsg(s"got user ${user}").inject[AppStack]
        _ <- Db.storeUser(user.copy(age=user.age + 1)).inject[AppStack]
    } yield user
    
`inject` here is for automatic lifting of algebras into our combined algebra `AppStack`.

We also get a `pure` function. Which deviates from our original target of not being able to fake side effects. 
But I guess in this case road was way more interesting than the end result - we ended up with a new way to combine our programs. 

## Running ##
To run program we need interpreters. We get one for `EitherK[F[_], G[_], A]` for free as long as interpreters for both `F` and `G` are defined.

Lets define one for database.

    import cats.effect.IO
    import cats.~>

    class DbIoNat extends ~>[DbAction, IO] {
      override def apply[A](fa: DbAction[A]): IO[A] = fa match {
        case DbAction.QueryUser(userId) => IO {
          Console.println(s"db: querying user $userId")
          User(userId, "somebody", 32)
        }

        case DbAction.StoreUser(user) => IO {
          Console.println(s"db: storing user $user")
         }
      }
    }
    
Where `~>` is natural transformation from one functor to another.

After we define transformations Db, Log and Net we are good to go - 

    val logIoNat = new LogIoNat()
    val dbIoNat = new DbIoNat()
    val netIoNat = new NetIoNat()

    val level0Nat = logIoNat.or(netIoNat)
    val appStackNat = dbIoNat.or[Level0](level0Nat)

    (foo(12) flatMap bar)
      .foldMap(appStackNat)
      .unsafeRunSync()
    

## Produce subset of side effects? ##

Can I tell what `foo` does by simply looking at the signature? __No__.  
I can't simple because it uses `AppStack` algebra that is set of all algebras combined with `EitherK`.
We are going to solve this by defining needed subsets of algebras (through `EitherK`) and in the end injecting them into `AppStack`.
Injecting into `AppStack` will require additional `InjectK` instance for `LogAndDb`, as they are in different `EitherK`s of AppStack.
For `LogAndNet` we don't need one - it will be derived automatically, as it's same thing as `Level0`.

    type Level0[A] = EitherK[LogAction, NetAction, A]
    type AppStack[A] = EitherK[DbAction, Level0, A]

    type LogAndDb[A] = EitherK[LogAction, DbAction, A]
    def foo(x:Int):Free[LogAndDb, User] = for {
      user <- Db.queryUser(x.toString).inject[LogAndDb]
      _ <- Log.logMsg(s"got user ${user}").inject[LogAndDb]
      _ <- Db.storeUser(user.copy(age=user.age + 1)).inject[LogAndDb]
    } yield user

    type LogAndNet[A] = EitherK[LogAction, NetAction, A]
    def bar(user:User):Free[LogAndNet, Int] = for {
      _ <- Log.logMsg("lets notify user change!").inject[LogAndNet]
      _ <- Net.notifyUserChange(user).inject[LogAndNet]
    } yield user.age

    override def main(args: Array[String]): Unit = {
      lazy val logIoNat = new LogIoNat()
      lazy val dbIoNat = new DbIoNat()
      lazy val netIoNat = new NetIoNat()

      lazy val level0Nat = logIoNat.or(netIoNat)
      lazy val appStackNat = dbIoNat.or[Level0](level0Nat)

      implicit val logAndDbIntoAppStack = new InjectK[LogAndDb, AppStack] {
        override def inj: ~>[LogAndDb, AppStack] = new ~>[LogAndDb, AppStack] {
          override def apply[A](fa: LogAndDb[A]): AppStack[A] = fa match {
            case EitherK(Left(act)) => InjectK[LogAction, AppStack].inj(act)
            case EitherK(Right(act)) => InjectK[DbAction, AppStack].inj(act)
          }
        }

        override def prj: ~>[AppStack, λ[α => Option[LogAndDb[α]]]] = ??? // not used
      }

      val r = (foo(12).inject[AppStack] flatMap (u => bar(u).inject[AppStack]))
        .foldMap(appStackNat)
        .unsafeRunSync()
      Console.println(s"end result: $r")
    }


## Source code ##
You can find source code [here](https://github.com/dehun/better-than-io-free)

## Next ##
In [next part](./17_dec_2018-Better than IO, part 4.html) we are going to lift other effects into `Free` monad.

## Further reading about free monad ##

- [cats free monad tutorial](https://typelevel.org/cats/datatypes/freemonad.html)
    

