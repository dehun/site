# Writing testable code #
## Intro ##
I am trying to gather my chaotic thoughts about writing testable code in this document.
I will try to improve it from time to time.

## What is testable code? ##
So what is testable code? Well, it is code which we can cover with tests.
But what is the test? Test is a thing that checks expectations for some other thing.
What is expectation? It is when you expect some particular output while giving some particular input.

## Every program architercture ##
Every programm and every piece of code can be viewed as

    input -> code -> output

Out tests should check that 'code' thing.
So every test is basically just a

    if code(input) == output:
        print 'pass'
    else:
        print 'fail'


## What is ideal test? ##
The ideal test is test where you have absolute control over input and output.

Which leads us to question what is input?
Input is anything that can affect output. This is obviously function arguments.
But it is also a state of the world.

Ok, but what is output?
Output is return value of the function and the state of the world.
In case of some languages it is also an exceptions.

## What is state of the world you are talking about? ##
State of the world is state of all resources your code can reach.
So if you are running a code which is isolated in your local machine - that this is state of the machine.
If we are running code which is isolated from all other processes then it is just a state of the process.
If we are running code which is isolated from everything - then state of the world is nothing.
This is state of the world size which differs a good testable code from bad untestable code.

Sometimes output's state of the world is not completly replaced with a new input's state of the world.
Then our test affects other test. Small state of the world ensures that this will not happen.
So small state of the world is good => small input and output are good.

In most cases it is hard to limit the amount of resources your code will be able to reach,
so just assume that state of the world is state of all resources your code is reading/writing.

## What is input/output ?##
Imagine your input/output to be the values of some type.

What is a type? - it is a set of possible values.
The trick is to keep the types of input and output small.

And when you introduce big state of the world - your types are growing.
And they grow blazingly fast.
It will be the multiplication of all types sizes used in input/output.

For example you have function

    bool isOdd(int i) {
    static uint_32t counter = 0;
      ++counter;
      return (i % 2) == 1;
    }

Now your output's type size is len([true, false]) * 2^32.

## Side effects ##
How can code increase it's output's or input's state of the world?

If your code changes some static global variable for example then before each test you should initialize it with proper value =>
your code state of the world now includes this variable. If you write to files - you should rewind files to proper state before next test.
If you do http request - you should set your http server into proper state before the test.

All this is about side effects. Controlling side effects - this is the challenge.
So side effects are read/write of something outside the code.
They are bad because they are increasing your input and output => increasing efforts to write test and increasing possibility of error.

## Dependency injection ##
A lof of people consider that dependency injection is the main thing in design of testable code.
And the singletons are evil.
And from my experience - they are right most of the times. Lets see why.

Imagine we have a class

    class Calculator {
    public:
        Calculator() {}
        int add(int a, int b) {
            Logger::getInstance()->info("adding %d and %d", a, b);
            Auditor::getInstance()->onAddCalled(a, b);
            return a + b;
        }
    };

So what do we get here - giant state of the world used. We use real logger. And that logger uses a lof of other stuff.
Also we use auditor - it can write stuff to mysql database. And that brings in even more values into input and output types.

If we will rewrite this code to be dependent on interfaces and avoid singletons usage - we will shrink a size of input/output types a lot.

    class Calculator {
        const shared_ptr<ILogger> _logger;
        const shared_ptr<IAuditor> _auditor;
    public:
        Calculator(const shared_ptr<ILogger> &logger
                   , const shared_ptr<IAuditor> &auditor)
            : _logger(logger)
            , _auditor(auditor) {
        }

        int add(int a, int b) {
            _logger->info("adding %d and %d", a, b);
            _auditor->onAddCalled(a, b);
            return a + b;
        }
    };

Now we can drop in out implementations of ILogger and IAuditor interfaces. And our implementations - mocks - they will not access any resources.
In this way we will get very small input and output.
In this case input consists of two integers. And output - 2 function calls and one integer.

BTW from theoretical perspective string type size is infinity. So our call to logger makes output size to be infinity.
But we will check only that call to logger was performed at all.

## That '==' sign ##
Lets say you have achieved a small input type and a small output type.
And now you are doing

    code(input) == output

check. Hovewer that '==' can be tricky.
Some types are not so easy to compare. Imagine you have speaking clock function. Your pass a time_point in and get a string out.
It will return 'Half past six'. Or 'Six thirty'. And now imagine it returns different strings for different compilers for some reason.

Or imagine your code draws a captcha. In this case it is impossible to get ideal output to compare against.
Or lets pretend your function transforms array of integers into an unbalanced binary tree. This also can produce trees that are completly different.
So that '==' sign can be very tricky.

### Non-deterministic input and output ###
Non-deterministic input/output is case when your type have 'aliases' for the same value.

Imagine your code return's a set of numbers. But for some reason you use list type for this.
In this case problem arise - [0, 1] and [1, 0] are both correct outputs. But against which of them we will match?
This example is simple - just replace list with set.

But what if you are writing a list of databases with credentials into configuration files and you want to match against this output?
In this case you should make your output deterministic - sort the databases by name or id for example.

So if you have non-deterministic input/output it actually makes your input/output types bigger => harder to handle.
Try to use smaller types to represent values.

## Performance of code, input and output ##
In some extreme cases performance is so low that it makes your tests to run for a long periods of time.
For example testing bruteforce algorithm for SHA-1 cracking - you got small input, small output.
But performance of code is so low that it makes code untestable.

Other extreme case - sometimes you have a very slow input or output generator.

Running time of a test should be small => running time of a code should be small.

## Check Questions ##

- How big are input and output?
- Is your output deterministic and comparable?
- Is running time reasonable?

## Keep it small ##
As a conclusion

- keep your input and output types small
- keep your running time small

The most testable code is an empty code or a code that have been never written.
If it solves your business case - use it.
