# Better than IO - part 2 #
## intro ## 
In the first part we ensured that our little function can produce only desired subset of side effects.
In here we are going to ensure that it's not only can, but it should.

## Should produce a side effect ##
Lets say we want to be even more strict and want always produce a particular side effect from the function `foo`.
First thing that comes to mind is use something like Command pattern.

    case class User(userId:String, nickName:String, age:Int)
    sealed trait DoDbAction[A]
    object DoDbAction {
      case class QueryUser(userId:String) extends DoDbAction[User]
      case class UpdateUserNickname(userId:String, newName:String) extends DoDbAction[Unit]
    }

    def foo(x:Int):DoDbAction[Int]
    
Now we sure that `foo` will return some db action. We will need an executor for those actions.  
However it is not that userful at the moment. What if we want to write a function that first queries the database and then updates the user nickname prepending "Mr. " to it?
Lets introduce a composition `DoDbAction`.

    object DoDbAction {
      case class FlatMap(lhs:DoDbAction[])
      case class Map()
    }

