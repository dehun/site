# Error whoch is of non-class type. Most vexing parse #
## Intro ##
C++ is a weird tricky thing which sometimes make me mad.
One day I write a class which accepts a `const std::list<std::string> &` as first and only one parameter.
And then I instantiated this class like this `A(std::list<std::string>())`. Ang got a compile error like in the subj.

## Problem isolated ##
Lets look at sample:

	#include <list>
	#include <string>

	using namespace std;

	class A {
	public:
	  A(const std::list<std::string> &l) {
	  }

	  int foo() {
		return 10;
	  }
	};


	int main(int argc, char **argv) {
	  A a(std::list<std::string>()); // the root of evil

	  a.foo();  // and here we get an error
	}

The result of compilation of this sample will be the next in gcc:

	~/exp $ g++ one.cpp
	 one.cpp: In function ‘int main(int, char**)’:
	 one.cpp:20:5: error: request for member ‘foo’ in ‘a’, which is of non-class type ‘A(std::list<std::basic_string<char> > (*)())’
	 a.foo();

## 'Debugging' the solution ##
CLang usualy have a way better error messages than a gcc. So I used it on this sample to see what will happen:

    ~/exp $ clang++ one.cpp
    one.cpp:18:6: warning: parentheses were disambiguated as a function declaration [-Wvexing-parse]
      A a(std::list<std::string>());
         ^~~~~~~~~~~~~~~~~~~~~~~~~~
    one.cpp:18:7: note: add a pair of parentheses to declare a variable
      A a(std::list<std::string>());
          ^
          (                       )
    one.cpp:20:4: error: member reference base type 'A (std::list<std::string> (*)())' is not a structure or union
      a.foo();
      ~^~~~
    1 warning and 1 error generated.

Here we see 3 very usefull suggestions:

* That our class instantiation of A was really guessed by a compiler as a functional declaration.
* Insert an additional pair of brackets to solve a problem.
* And name of the issue: vexing-parse.

## Vexing parse ##
This issue is easy to 'google' by a search 'vexing parse c++'.
The problem here is with temporary objects passed to a constructor - such initialization line is got by a compiler as a function declaration.
To disambiguate this we can use 3 approaches.

### Call of ctor through cast ###
The ctor with one param is assumed to be a cast operator from type B which is type of param into class type A.
For example:

    struct A {
        int m_a;
    public:
        A(int a)
            : m_a(a) {
        }
    };
    
    // somewhere in code
    A a = 10;


So to make our sample compile we have to use next transformation:

	  A a = std::list<std::string>(); // the root of evil

To restrict such behaviour of ctor with one parameter you can use explicit keyword.

```c++
struct A {
    int m_a;
public:
    explicit A(int a) // note explicit here
        : m_a(a) {
    }
};

// somewhere in code
A a = 10; // will make a compile error
```

### Additional pair of brackets ###
Also we can disambiguate this declaration by adding additional pair of brackets:

	  A a((std::list<std::string>())); // the root of evil

### static_cast ###
Very simple straightforward solution - no casts can appear in function declaration so we do next:

	  A a(static_cast<std::list<std::string> >(std::list<std::string>())); // the root of evil

### CXX 011 unified initialization ###
You see all this ugly stuff and think that this is language issue. So commitet got a solution to it.
To compile cxx11 programms you have to specify a language standart to your compiler.

    g++ --std=c++11 one.cpp

The code changes is very simple:

    A a{std::list<std::string>{}}; // the root of evil

They extended a list initialization syntax and make it unified for all the operations of initialization.
