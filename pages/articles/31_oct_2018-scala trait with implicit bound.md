# Scala trait with implicit bound #
## intro ##
Sometimes when we define traits with type parameters, we would like to have some implicit evidence for that type parameters.
However traits do not allow us to have any parameters.

## concrete example ##
Lets say we want to implement a `Monad` instance for something. We know that for something to be a monad, it should be a `Functor` first.
So our natural desire would be to write something like

    trait Monad[F[_]:Functor]

That will be desugared into

    trait Monad[F[_]](implicit functor:Functor[F])

And that will produce a compile error because traits can't have parameters.

## workaround ##

    trait Monad[F[_]] {
        implicit def functor:Functor[F]
    }

    implicit def monadInstance[F[_]](implicit f:Functor[F]) = new Monad { functor = f}
