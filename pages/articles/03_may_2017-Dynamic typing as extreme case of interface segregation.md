# Dynamic typing as extreme case of interface segregation #
We have interface segregation principle which states that plenty of small interfaces are better than one big interface. Then we have dynamic typed languages like python or ruby. Lets say we have a function in python. 

    def foo(x):
        x.bar()
        
In here we automatically define interface of x - it should have method bar. 
While in static typed language like Java or c++ we will need to define interface upfront.

    interface X {
        void bar();
    }
    
    object Foo {
        static void foo(X x) {
            x.bar();
        }
    }
    
So for every input parameter in dynamic language we automatically generate an interface that is as small as possible to do the job.
Therefor dynamic typing achieves an maximum level of interface segregation.

