# Better than IO - part 1 #
## intro ##
In a very rough way IO allows us to mark parts of our system that produce a side effects.
E.g `def foo(x:Int):IO[Int]` can produce a side effect. When `def foo(x:Int):Int` should not.
At least conventionally speaking, we know in Scala they both can produce side effects.

Can we do better than that?
Can we make sure function can produce only subset of side effects?
Can we make that `can produce a side effects` into `produces side effects`.
This is part 1 - it partly covers `can produce a side effect` with mtl style.

## Subset of side effects ##
Lets say we have a function that should have an access to database and logger and produces appropriate side effects - read/write into database and outputs some logs.
Function signature is

    def foo(x:Int):IO[Int]

Just from this signature we can't reason about what type of side effects will be performed. Will network be accessed? Maybe it will write to file system? We don't know.

Lets try to explicitly pass appropriate proxy classes.

    def foo(x:Int)(db:Db, log:Log):IO[Int]

Better, now we know at least that it can do something with log and db. But still side effects are not limited to that.

We can try to solve this by introducing monad for each type of side effect.
E.g.

    class Db[A] // extends Monad
    class Log[A] // extends Monad

Now we can write our foo like this -

    def foo(x:Int):Db[Int]

We lost logger along the way because monads do not compose. We can try to use this one as following

    foo(12).runDb(db) // returns IO[Int]

In order to compose Db and Log they should be monad transformers. Here comes (with kind-projector)

    class DbT[F[_], A]
    class LogT[F[_], A]

    type FooStack[A] = DbT[LogT[Id, ?], A]

    def foo(x:Int):FooStack[Int]

    foo(12).runDbT(db).runLogT(log) // returns IO[Int]

This is way better - now we know that `foo` can only read/write from/to db and log. No writing to file nor opening sockets.
All goes good until we need to extend our small system and add another function `bar` that will access network and also log stuff.
We follow our approach and add another monad transformer for network - `NetT`.

    class NetT[F[_], A]
    type BarStack[A] = NetT[LogT[Id, ?], A]

    def bar(x:Int):BarStack[Int]

    bar(13).runNetT(net).runLogT(log) // returns IO[Int]

And it looks good - same as previous example. However what happens if we need to combine those 2 functions? Well, they don't combine because they got 2 different stacks.
That happens because we fixed the stack in our functions `foo` and `bar`. Actually we do not care that much how stack is constructed as long as it allows us to perform effects we want.
So we keep our monad transformers the same and change our functions to the next thing

    def foo[F[_]:MonadLog:MonadDb](x:Int):F[Int]
    def bar[F[_]:MonadLog:MonadNet](x:Int):F[Int]

    type AppStack[A] = NetT[DbT[LogT[Id, ?], ?], A]

    { foo[AppStack](12) >>= bar[AppStack] }.runNetT(net).runDbT(db).runLogT(log).runIO()

Mission accomplished - both `foo` and `bar` can see only needed parts of our AppStack and we(and compiler!) can now reason about side effects produced by each function.
All we need to do now is to implement MonadLog, MonadDb and MonadNet instances for our AppStack.

## Further ideas ##
Now we can easily test foo and bar without unleashing whole stack - all we need for testing of foo is some stack for which implicits for MonadLog and MonadDb are defined.
We can define those implicits to be stubs and easily test our methods.

## Part 2 ##
In part 2 we are going to explore second question - making "can produce side effect" to be "should produce side effect".

## Source code ##
Full source code for this approach can be find at [github:better-than-io](https://github.com/dehun/better-than-io)
