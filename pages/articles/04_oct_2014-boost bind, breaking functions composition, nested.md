# boost::bind, breaking functions composition, nested #
## Intro ##

boost::bind allows to use functions composition to achieve effect f(g(x)) easily.  
But what to do when you actually need to bind something like f(x, a(y)).  
Where function f accepts 2 arguments and the second one is a function.  

## Issue ##

We want this piece of code to work  

	#include <boost/bind.hpp>
	#include <boost/function.hpp>
	#include <boost/assign.hpp>
	#include <algorithm>
	#include <vector>
	
	void for_each_in_vector(std::vector<int> v, boost::function<void (int)> fx) {
	  std::for_each(v.begin(), v.end(), fx);
	}
	
	void print_int(int i) {
	  std::cout << i;
	}
	
	int main() {
	  std::vector<std::vector<int> > vecs;
	  vecs.push_back(boost::assign::list_of<int>(1)(2)(3));
	  vecs.push_back(boost::assign::list_of<int>(4)(5)(6));
	  vecs.push_back(boost::assign::list_of<int>(7)(8)(9));
	
	  std::for_each(vecs.begin(), vecs.end(),
	                boost::bind(&for_each_in_vector, _1, 
	                            boost::bind(&print_int, _1)));
	}


So we actually are trying to call for\_each\_in\_vector for every vector in vecs.  
And that for\_each\_in\_vector should call print_int for every int in that every vector.  
In the 'unwrapped' code this will look like this  

	BOOST_FOREACH (std::vector<int> vec, vecs) {
	  for_each_in_vector(vec, boost::bind(&print_int, _1));
    }

And now take a look at that _1 placeholder. Due to the fact that boost::bind will try to make composition here  
we will get argument 1 from the upper bind inserted as argument into the nested bind.  
So by the fact this code tries to call something like this  

	BOOST_FOREACH (std::vector<int> vec, vecs) {
	  for_each_in_vector(vec, boost::bind(&print_int, vec));  // note here vec instead of _1
    }

When you will try to compile it you will get a big mess of templates errors and few usefull lines like this  

	./nested_bind.cpp:23:60:   required from here
	/usr/include/boost/bind/bind.hpp:313:34: error: cannot convert ‘std::vector<int>’ to ‘int’ in argument passing
	         unwrapper<F>::unwrap(f, 0)(a[base_type::a1_], a[base_type::a2_]);


## Solution ##
What we actually want is to break that composition. How do boost::bind do it?  
It just recognizes that it have got boost::bind as an argument and 'connectes' that placeholders within binds.  
To make our code work we should fool boost bind and actually pass a function/functor instead of boost::bind as an argument.  

	std::for_each(vecs.begin(), vecs.end(),
	              boost::bind(&for_each_in_vector, _1, 
	                          boost::function<void (int)>(boost::bind(&print_int, _1, 2)))); // note boost::function here

Here we have used boost::function to wrap nested boost::bind.  
So now top boost::bind will not recognize that nested bind and we will get isolated from each other placeholders.  
