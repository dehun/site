# Better than IO, part 3 #
## intro ## 
In the first and second part we ensured that our little function can produce only desired subset of side effects.
In here we are going to ensure that it's not only can, but it should.

## Should produce a side effect ##
Lets say we want to be even more strict and want always produce a particular side effect from the function `foo`.
A quick reminder - `foo` looks like this - 

    def foo(x:Int):Int

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
    
Ok, that works. 
    
    

