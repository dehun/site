# scala wtf - 1 - implicit default parameters #
## wtf ##

    # amm

    @ def foo()(implicit a:Int=1, b:String="b") = { Console.println(s"a=$a; b=$b")}
    defined function foo

    @ foo()
    a=1; b=b


    @ { { implicit val x:Int=26; foo() } }
    a=26; b=b

    @ { { implicit val x:Int=26; foo()(b="dsa") } }
    a=1; b=dsa

Boom - `a` is 1, wtf?  I have been expecting `26`.

Works other direction too:

    @ { { implicit val x:String="asd"; foo()(a=26) } } 
    a=26; b=b
    
I have been expecting `b` being "asd" in this case.

## when ##
I wanted to use this one to specify one implicit parameter for particular function call explicitly, while grabbing other one implicitly or from default.
As we can see this syntax grabs only default one.

## why ##
Lets refer to scala spec: https://www.scala-lang.org/files/archive/spec/2.12/04-basic-declarations-and-definitions.html
`4.6.1` explains it quite nicely - scala compiler basically generates extra methods.
In our case we got 

    foo$default$1()                            // with a = 1,  b = "b"
    foo$default$2()(implicit xA:Int=1)         // with a = xA, b = "b"
    foo$default$3()(implicit xB:String="b")    // with a = 1,  b = xB

So when we call `foo()(a=1)` it unrolls into `foo$default$2(xA=1)`

## workarounds ##

Provide implicit with very narrow scope and call without explicit specification of implicit params.

    { { implicit b:String="asd"; foo() } }
    
Lets see it in action with defaults

    @ { { implicit val a:Int=26; foo() } } 
    a=26; b=b
    
And now with implicit `b` in scope

    @ implicit val b:String = "foo" 
    b: String = "foo"
    
    @ { { implicit val a:Int=26; foo() } } 
    a=26; b=foo
