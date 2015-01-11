# pythonic c++03 #
## intro ##
A lot of people have noticed that c++ is moving towards 'pythonic' style last years.  
All that new c++11 and c++14 features allows a way different style of programming.  
I will try to show how can you achieve a nice 'pythonic' programming style with c++03 and boost.  

## list comprehensions ##
A nice feature is list comprehensions and slicing.  

    validItems = [i for i in self.getSomeItems()[1:10] if i.isValid()]

Now lets try to recreate this with c++ and boost::range.  

    #include <boost/range/algorithm.hpp>
    #include <boost/range/adaptors.hpp>

    std::vector<boost::shared_ptr<Item> > validItems;
    boost::copy(validItems
                | boost::adaptors::sliced(1, 10)
                | boost::adaptors::filtered(boost::bind(&Item::isValid, _1)),
                std::back_inserter(validItems));

## string formatting ##
We will use boost::format library to recreate a limited subset of python formatting.  
So here goes python

    s = '%s and %s' % ('tom', 'jerry')

Alternative in c++

    #include <boost/format.hpp>
    std::string s = boost::str(boost::format("%1% and %2%") % "tom" % "jerry");

But we still can't do stuff like this in c++:

    s = '%(one)s and %(two)s' % {'one': 'tom', 'two': 'jerry'}


## inplace collections ##
We will solve this one with boost::assign  
Python code 

    l = [1, 2, 3, 4]
    s = set([1, 2, 3, 4])
    m = {'a':1, 'b': 2}

And in c++

    #include <boost/assign.hpp>
    std::list<int> l = boost::assign::list_of(1)(2)(3)(4);
    std::set<int> s = boost::assign::list_of(1)(2)(3)(4);
    std::map<std::string, int> m = boost::assign::map_list_of("a", 1)("b", 2);


## foreach ##
Just use boost::foreach and boost::range::adaptors

    for x in [1, 2, 3, 4]:
        print x
    for x in [normalize(i) for i in [1, 2, 3, 4]]:
        print x
    for x in mydict.values():
        print x


Here goes c++

    #include <boost/foreach.hpp>
    BOOST_FOREACH (int x, boost::assign::list_of(1)(2)(3)(4)) {
      std::cout << x;
    }
    BOOST_FOREACH (int x, boost::assign::list_of(1)(2)(3)(4)
                          | boost::adaptors::transformed(boost::bind(&normalize, _1))) {
      std::cout << x;
    }
    BOOST_FOREACH (int x, mymap | boost::adaptors::map_values) {
      std::cout << x;
    }
    

## lambdas ##
Yes, there are real lambdas starting from c++11. But lets see what can we do in C++03.  
As you know there are a lot of limitations on lambda expressions in python.  
So in python you can do only one liners.

In c++03 and boost you have a lot of options for this.
You can use next libraries: boost::bind, boost::phoenix, boost::lambda.

Lets start from the most obvious one - bind.
### boost::bind ###
python:

    s = filter(lambda x: x.value % 2 == 0, [A(1), A(2), A(3)])

cpp:

    #include <boost/range/algorithm.hpp>
    #include <boost/bind.hpp>
    std::list<int> s;
    boost::copy_if(boost::assign::list_of(A(1))(A(2))(A(3)), std::back_inserter(s),
                   boost::bind(&A::value, _1) % 2 == 0);

### boost::lambda ###
python:

    valid = [x for x in objects if x is not None]

c++:

    #include <boost/lambda/lambda.hpp>
    #include <boost/range/algorithm.hpp>
    std::vector<boost::shared_ptr<Object> > valid;
    boost::copy(objs | boost::range::filtered(boost::lambda::_1),
                std::back_inserter(valid));

### boost::phoenix ###
To be done. I am not active user of this.

## string operations ##
Operations like split and join are very usefull and can be performed very easily in python

    "1, 2, 3, 4".split(",")
    ",".join([str(x) for x in [1, 2, 3, 4]])

And c++ equivalent will use boost::algorithm::string, boost::assign and boost::lexical_cast

    #include <boost/lexical_cast.hpp>
    #include <boost/string/algorithm.hpp>
    std::vector<std::string> splitted;
    boost::split(std::string("1, 2, 3, 4"), std::back_inserter(splitted), boost::is_any_of(","));
    //
    boost::join(boost::assign::list_of(1)(2)(3)(4)
                | boost::adaptors::transformed(boost::bind(&boost::lexical_cast<std::string, int>, _1)), ",");

## tuples ##
Here is all simple. You can use boost::tuple for generic case. And std::pair for cases when you need tuple with 2 elements.  
python version

    [("earth", 10, 20.13),
     ("venus", 13, 12.86),
     ("mars", 15, 0.12)]


And c++

    #include <boost/tuple/tuple.hpp>
    boost::assign::list_of
    (boost::tuple<std::string, int, double>("earth", 10, 20.13))
    (boost::tuple<std::string, int, double>("venus", 13, 12.86))
    (boost::tuple<std::string, int, double>("mars", 15, 0.12))
    
## 
