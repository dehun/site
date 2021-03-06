# Better than IO, part 2 #
## intro ##
In [previous part](./29_oct_2018-Better%20than%20IO,%20part%201.html) we have discussed mtl approach with stack of transformers. Lets see how can we improve on that.

## part 1 drawbacks ##
As you remember from [previous part](./29_oct_2018-Better%20than%20IO,%20part%201.html) we used to say that `Log`, `Db` and `Net` are monad transformers.
And then we used MTL style approach to access parts of that transformets stack. E.g. `MonadLog`, `MonadDb` and `MonadNet`.
While that can work, we also learned that it's also generating a lot of boilerplate for each of effect type we want to have.
Mostly because we have to define that giant `AppStack` stack of monad transformers.
This is cumbersome - for any effect we should define an extra monad transformer. 
So in real world application we most likely will end-up with 10 monad transformers stacked together.
And you have to define `map`/`pure`/`flatMap` for all of them. And also define how to access them - either using some automatic dereviation or manually.

Performance wise it also does not come for free - each time we will need to wrap and unwrap our value into 10 layers of objects.
That will introduce runtime performance penalty.

## optimizing that AppStack ##
So far we have huge `AppStack` for every effect that we are handling in our program. And we also have this "accessors" to parts of that stack.
E.g. `LogT` has corresponding `MonadLog`. And that `MonadLog` instance for `AppStack` knows how to produce `LogT` effects in that `AppStack`.
What if we simplify that `AppStack` and give more power to that accessors typeclass instances?
Logging does not need it's own `LogT`, all the logging can be easily done in the `IO`. 
We can reduce our `AppStack` to be simply `IO`. And then define `MonadLog`, `MonadDb` and `MonadNet` instances for `IO`.
Lets see how it will work out.

## subset of side effects, again ##
So we want to declare our function to produce subset of side effects.
We are improving our 

    def foo(x:Int):IO[Int]
    
That actually does access to database and logging.

How about we define accessors to db and logger and just do dependency injection?

    def foo(x:Int)(implicit logger:Logger, db:Db):IO[Int]
    
Better, we can see that function is using logger and db. So most likely it will not produce any other side effect.
But it can absolutely can access some singleton and do some filesystem operations from there because we got that `IO[Int]` at the end. 

Lets get rid of it - we can simply request any monad and combine actions in it.

    def foo[F[_]:Monad](x:Int)(implicit logger:Logger, db:Db):F[Int]

Our logger has method `def log(msg:String):IO[Unit]` and Db also has methods in IO. We can't use them in our function that uses `F[_]`. Lets go further and declare interface for logger and db - LoggerAlg and DbAlg. Where Alg is abbreviation for Algebra.

    trait LoggerAlg[F[_]] {
      def log(msg:String):F[Unit]
    }
    
    trait Db[F[_]] {
      def getUser(userId:UserId):F[User]
    }

    def foo[F[_]:Monad](implicit loggerAlg:LoggerAlg[F], db:DbAlg[F]):F[Int]
    
Now we can implement logic of our method foo. But we can't run it. To run it we should provide typeclass instance for `DbAlg[IO]` and `LoggerAlg[IO]`.
Then we can run it. For example:

    class IOLoggerAlg(logger:Logger) extends LoggerAlg[IO] {
      override def log(msg:String):IO[Unit] = { logger.log(msg) }
    }
    
    object IOLoggerAlg {
      def impl(logger:Logger) = new IOLoggerAlg(logger)
    }
    
    class IODbAlg(db:Db) extends DbAlg[IO] {
      def getUser(userId):IO[User] = {
        db.getUsre(userId)
      }
    }
    
    object IODbAlg {
        def impl(db:Db) = new IODbAlg(db)
    }

    implicit val loggerAlg = LoggerAlg(logger)
    implicit val dbAlg = IODbAlg(db)
    foo[IO](12) 
    
Now you wonder will it combine with our function bar, that as you remember from part 1 was doing Network operations and logging.
We apply same principles and get

    def bar[F[_]:Monad:LoggerAlg:NetAlg](x:Int):F[Int]
    
Both bar and foo produce `F[_]` which is `Monad` - so no problems combining them. 
In case if we "run" them - specializing them on the same monad - also no problems composing.

    foo[IO](12) >>= bar[IO]
    
This approach called tagless final. For now no explanation why, we have to understand few more things to understand the name.
But we can perfectly use the approach without understanding it's name.
    
## wtf is algebra ##
Algebra in this case is just set of objects and operations. It's just a way to describe your computation.
In other words it's embedded dsl.

## testing ##
Testing is straightforward. You don't have to use `IO` for testing, `Id` will be just fine.
So for testing our foo method all we need is mock algebras in `Id` monad. 

    implicit val loggerAlgMock:LoggerAlg[Id] = new LoggerAlg[Id] { ... }
    implicit val dbAlgMock:DbAlg[Id] = new DbAlg[Id] { ... }
    foo[Id](12) should ===(42)

## summoners ##
While writing code for `foo` we will have 2 possibilities

    def foo[F[_]:Monad:LoggerAlg:DbAlg](x:Int):F[Int] = for {
      _ <- implicitly[LoggerAlg[F]].log("hello world")
      user <- implicitly[DbAlg[F]].findUser(UserId("my-user-id"))
    } yield user.age
    
and 

    def foo[F[_]:Monad:LoggerAlg:DbAlg](x:Int)(implicit loggerAlg:LoggerAlg[F], dbAlg:DbAlg[F]):F[Int] = for {
      _ <- loggerAlg.log("hello world")
      user <- dbAlg.findUser(UserId("my-user-id"))
    } yield user.age
    
Can we do better? Lets take approach #3 - summoners - we will define specialized implicitly for every algebra. 

    object LoggerAlg {
      def apply[F[_]](implicit loggerAlg:LoggerAlg[F]) = loggerAlg
    }
    
    object DbAlg {
      def apply[F[_]](implicit dbAlg:DbAlg[F]) = dbAlg
    }
    
    def foo[F[_]:Monad:LoggerAlg:DbAlg](x:Int):F[Int] = for {
      _ <- LoggerAlg[F].log("hello world")
      user <- DbAlg[F].findUser(UserId("my-user-id"))
    } yield user.age
    
## using with monad transformers ##
Monad transformers still can be highly useful, especially for cases when we are handling errors.
Lets see how can we do it on example of DbAlg that returns Either.

    trait DbAlg[F[_]] {
        def updateUserAge(userId:UserId, newAge:Int):F[Either[UserNotFound, User]]
    }

    def foo[F[_](x:Int):Monad:LoggerAlg:DbAlg]:F[Either[UserNotFound, Int]] = {
        for {
          _ <- EitherT.liftF(LoggerAlg[F].log("hello world"))
          u <- EitherT(DbAlg[F].updateUserAge(UserId("my-user-id"), x))
        } yield 
    }.value
    
That lifting of `LoggerAlg` is annoying. We can make it slightly better with natural transformations.

    def foo[F[_](x:Int):Monad:LoggerAlg:DbAlg]:F[Either[UserNotFound, Int]] = {
        for {
          _ <- LoggerAlg[F].log("hello world").nat[EitherT[F, String, ?]]
          u <- EitherT(DbAlg[F].updateUserAge(UserId("my-user-id"), x))
        } yield 
    }.value

We still have to specify `EitherT` type parameters. But this one should work way better for optional.

See [github:better-than-io-tf](https://github.com/dehun/better-than-io-tf) for details.

## eco-system ##
Tagless final approach plays nicely with doobie, fs2 and http4s.

## part 3 ##
In [part 3](./03_dec_2018-Better than IO, part 3.html) we are going to answer second question - "must produce side effect".

## source code ##
You can find source code at [github:better-than-io-tf](https://github.com/dehun/better-than-io-tf)
