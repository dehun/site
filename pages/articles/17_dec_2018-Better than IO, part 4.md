# Better than IO, part 4, Effects in Free #
## Intro ##
So far we saw the power of [monad transformers](./29_oct_2018-Better than IO, part 1.html), [tagless final](./15_nov_2018-Better than IO, part 2.html) and [free monad](./03_dec_2018-Better than IO, part 3.html). What else can we desire?
We saw that monad transformers are quite cumbersome to use, however they are still quite helpful in a lot of scenarios.

    ReaderT[EitherT[IO, Error, ?], Config, User]
    
Is still a a decent stack to use while accessing remote server for example.
How do we use it with `Free`? 

Can we have `EitherT`, `StateT`, `OptionT` along with ours `NetT`, `LogT` and `DbT` in a single stack using `Free`?  
Lets give it a try.

## Option in Free ##
Can we describe option algebra in `Free`? I will use `Maybe` as alias for `Option` to avoid name clashes.

    sealed trait MaybeAction[A]

    object MaybeAction {
      case class Just[A](value:A) extends MaybeAction[A]
      case class Nothing[A]() extends MaybeAction[A]
    }
    
    object Maybe {
      def just[A](value:A):Free[MaybeAction, A] = Free.liftF(MaybeAction.Just[A](value))
      def nothing[A]:Free[MaybeAction, A]= Free.liftF(MaybeAction.Nothing[A]())
    }
    
Now we need a transformation from `MaybeAction` into `IO[?]`. 
However we have no way to represent absence of value in `IO`. We need some stack where we can express absence of value.
Which brings us to `OptionT[IO, ?]`. Now we can write our transformation

    class MaybeIoNat extends ~>[MaybeAction, OptionT[IO, ?]] {
      override def apply[A](fa: MaybeAction[A]): OptionT[IO, A] = fa match {
        case MaybeAction.Just(value) => OptionT.some[IO](value)
        case MaybeAction.Nothing() => OptionT.none[IO, A]
      }
    }

And we can write programs like this - 

    def foo2(x:Int):Free[MaybeAndAppStack, User] = for {
      user <- Db.queryUser(x.toString).inject[MaybeAndAppStack]
      _ <- Log.logMsg(s"got user ${user}").inject[MaybeAndAppStack]
      _ <- Maybe.nothing[Unit].inject[MaybeAndAppStack]
      _ <- Db.storeUser(user.copy(age=user.age + 1)).inject[MaybeAndAppStack]
    } yield user
    
This will return `None`. Not very useful though. Let's make our queryUser more realistic - user can be absent. And `maybeQueryUser` can return `None`.

    sealed trait DbAction[A]
    object DbAction {
      case class QueryUser(userId:String) extends DbAction[User]
      case class MaybeQueryUser(userId:String) extends DbAction[Option[User]]
      case class StoreUser(newUser:User) extends DbAction[Unit]
    }

    object Db {
      def queryUser(userId:String) = Free.liftF(DbAction.QueryUser(userId))
      def maybeQueryUser(userId:String) = Free.liftF(DbAction.MaybeQueryUser(userId))
      def storeUser(newUser:User) = Free.liftF(DbAction.StoreUser(newUser))
    }
    
We need some way to involve `Maybe` with `DbAction[Option[User]]`. Lets try some kind of a lift.
And lets start from api part - from usage.

    def foo3(x:Int):Free[MaybeAndAppStack, User] = for {
      user <- Maybe.lift(Db.maybeQueryUser(x.toString).inject[MaybeAndAppStack])
      _ <- Log.logMsg(s"got user ${user}").inject[MaybeAndAppStack]
      _ <- Db.storeUser(user.copy(age=user.age + 1)).inject[MaybeAndAppStack]
    } yield user

Lets try to define `Maybe.lift`.

    object MaybeAction {
      case class Lift[A](v:Option[A]) extends MaybeAction[A]
    }

    object Maybe {
      def lift[F[_], A](value:Free[F, Option[A]]):Free[???, A] = value.flatMap {
        case None => nothing[A]
        case Some(v) => just(v)
      }
    }
     
We don't know what that `Maybe.lift` should return. It accepts some `F`. But we want somehow to embed `MaybeAction` into that `F`.
In the rest of our `Free` program we would normally use `injectK` for that. We need implicit injector for that.

    def lift[F[_], A](value:Free[F, Option[A]])(
     implicit maybeInjector:InjectK[MaybeAction, F]):Free[F, A] = value.flatMap {
      case None => Free.liftF(maybeInjector.inj(MaybeAction.Nothing[A]()))
      case Some(v) => Free.liftF(maybeInjector.inj(MaybeAction.Just[A](v)))
    }
    
So this basically says give me an algebra that supports maybe (through injector) and I will lift MaybeAction from option.
And when we use it like this 

    user <- Maybe.lift(Db.maybeQueryUser(x.toString).inject[MaybeAndAppStack])
    
We are first injecting result of `DbAction` into our Maybe supporting `MaybeAndAppStack`. And we have injector for `MaybeAction` into `MaybeAndAppStack`.

## Next ##
In a next part we are going to continue exploring free effects diving into `Eff`.

## Source code ##
You can find source code [here](https://github.com/dehun/better-than-io-option-in-free)
