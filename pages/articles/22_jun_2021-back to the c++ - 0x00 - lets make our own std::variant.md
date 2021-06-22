# back to the c++ - 0x00 - lets make our own std::variant #
## intro ##
I have not touched c++ for some time because I have been quite occupied with scala. But recently I have decided to refresh my knowledge and (re)-learn few tricks.  
After doing primarely c++ for ~8 years at a various jobs I have decided to try something new and switched to Java and later to Scala. Nowadays at my job I am writing Scala daily and I am mostly happy, at least until I look into implementation of collections where one can find a lot of copying. JVM microservices are also sometimes look like a bad joke - 256mb consumed right from the start with amazing spikes for JIT compilation at startup. I do find c++ to be one of the most expressive languages out there. 
C++11 was quite revolutionary for the way I do express my thoughts in code.

In here you will sometimes see some very simple scala code, just for comparison, worry not.

In this article we will try to implement our own `std::variant`.

## coproduct ##
Imagine the situation when you have to express a coproduct. Meaning you want something like `Either[LeftType, RightType]`.  
This can be used to return either an error or a value out of a function. 
What if we want to have choice of more than 2 types provided by our `Either`. Well, in that case we can stack eithers and have something like `Either[A, Either[B, C]]`. This can be used to discriminate between errors or to store state of the FSM. 

## product ##
A sibling constuct to coproduct will be a product. When for coproduct we want to store only one of the possible types, in case of a product we want to store a value for each type. Tuple manifests itself as pair of values or tuple. In c++ you may have encountered product as `std::pair<A, B>` or `std::tuple<A, B, C>`. 
One of the books that has been quite influential on me was "Modern C++ Design: Generic Programming and Design Patterns Applied" by Andrei Alexandrescu. 
I have read it before c++11 was a thing in my daily job. One important difference between c++03 and c++11 is that later has variadic templates.
Which means doing `std::tuple<A, B, C>` is not possible if we have c++03. We can only do `std::pair<A, std::pair<B, C> >`.
At this point we see the pattern - both `Either[A, Either[B, C]]` and `std::pair<A, std::pair<B, C>>` use recursion to represent list of types.

## type lists ##
In a "Modern C++ Design" book author uses c++03 and generalizes the above described recursion idea with a type list. 
So both product and coproduct now can use it - `tuple<TList<A, TList<B, TList<C, TNil> > > >`.  
Most of us are lucky to have c++11 nowadays. And we can even play with c++20 now. We have variadic templates and argument packs. Which means we can avoid doing the `TList` workaround, and use template arguments directly - i.e. product `std::tuple<A, B, C>` or coproduct `std::variant<A, B, C>`.

## std::variant ##
`std::variant` is a coproduct implementation. It can hold value that is one of the types in it's argument list. It's something like a `union`.

    std::variant<A, B, C> v;
    v = B { 1.1, 3};
    std::cout << "holds A: " << std::holds_alternative<A>(v) << std::endl;
    std::cout << "holds B: " << std::holds_alternative<B>(v) << std::endl;
    std::cout << std::get<B>(v).b_d << std::endl;
    std::cout << "holds C: " << std::holds_alternative<C>(v) << std::endl;
    
## mvar ##
Lets implement our own `std::variant` in the learning purposes. To avoid confusion lets name it `mvar` - as in `MyVARiant`.
We want to have roughly the same interface as the original. So we have to accept list of types.

    template <typename ...ArgsT>
    class mvar {
    
Now we have to store our value somewhere, we can do it on the heap with something like bunch of `unique_ptr`s, but for fun and for performance reasons lets do it right in the `mvar` itself. So byte array it is -  `char mem[???]`. But how many bytes do we need? If we can fit the biggest type from the type list, then we should be fine. For that we have to iterate over our type list `ArgsT` and find the biggest type. 

    template <typename ...ArgsT>
    class mvar {
    public:
      char mem[biggest_type_size<ArgsT...>::value];
      

And `biggest_type_size` implementation

    template <typename ...ArgsT>
    struct biggest_type_size;

    template <typename HT, typename ...TailT>
    struct biggest_type_size<HT, TailT...> {
      static constexpr size_t value = std::max(sizeof(HT), biggest_type_size<TailT...>::value);
    };

    template <> 
    struct biggest_type_size<> { 
      static constexpr size_t value = 0; 
    };
    
We recursively go via arguments `ArgsT` on each step deconstructing them into the head `HT` and the tail `TailT`. Eventually we hit empty list `<>` and break recursion in there - we do this via template specialization.

To place value in our new byte buffer we are going to use placement the setter that we define inside `mvar`. 

    template <typename ST>
    void operator=(const ST &s) {
      new(mem) ST(s);
    }
    
And to get our value we need a getter inside `mvar` - 

    template <typename ST>
    const ST* get() const {
      return reinterpret_cast<const ST*>(&mem);
    }
    
This code is missing few important things. If we use `operator=` twice? We have to call the destructor for the previously stored value.
Our `get` is not good as well - we have to check that value of requested type is there or at least provide such possibility - something like `std::holds_alternative`.  

## safer mvar ##

We have to store type at runtime. We can use `typeinfo`, but that will be a bit overkill. Lets store index of the type in the type list.

    template <typename ...ArgsT>
    class mvar {
    public:
      char mem[biggest_type_size<ArgsT...>::value];
      int current_type_index = -1;

      template <typename ST>
      void operator=(const ST &s) {
        new(mem) ST(s);
        current_type_index = type_index_of<ST, ArgsT...>::value;
      }

      template <typename ST>
      bool holds_alternative() const {
        return type_index_of<ST, ArgsT...>::value == current_type_index;
      }


      template <typename ST>
      const ST* get() const {
        assert(holds_alternative<ST>());
        return reinterpret_cast<const ST*>(&mem);
      }
    };
    
For `type_index_of` implementation we first need a function that returns length of a type list. I.e. `type_list_len<A, B, C>::value` should be `3`.

    template <typename... Args> struct type_list_len;

    template <typename HT, typename ...TailT> 
    struct type_list_len<HT, TailT...> : public type_list_len<TailT...> {
      static constexpr int value = 1 + type_list_len<TailT...>::value;
    };

    template <> 
    struct type_list_len<> {
      static constexpr int value = 0;
    };
    
Now `type_index_of` should walk via type list until it hits the needed type and then it can return the length of the tail. It will return an index if we traverse type list backwards, but it's good enough for our purposes.

    template <typename ST, typename... Args> struct type_index_of;

    template <typename ST, typename ...TailT> 
    struct type_index_of<ST, ST, TailT...> {
      static constexpr int value = type_list_len<ST, TailT...>::value;
    };

    template <typename ST, typename HT, typename ...TailT> 
    struct type_index_of<ST, HT, TailT...> {
      static constexpr int value = type_index_of<ST, TailT...>::value;
    };

It works like this - 

    type_index_of<A, A, B, C>::value  // is 2
    type_index_of<B, A, B, C>::value  // is 1
    type_index_of<C, A, B, C>::value  // is 0

## mvar with destructor ##
We have to call a destructor in case if we call `operator=` more than once. 
This is an interesting one because we don't know upfront type index - we store it in a variable at runtime.
We have to bring our static type information available at compile time and information available at runtime together.

    template <typename ...SArgsT>
    struct call_dtor_by_index;

    template <typename HT, typename ...TailT>
    struct call_dtor_by_index<HT, TailT...> {
      void operator()(int type_idx, void *mem) const {
        if (type_idx == type_list_len<TailT...>::value) {
          HT *obj = reinterpret_cast<HT *>(mem);
          obj->~HT();
        } else call_dtor_by_index<TailT...>()(type_idx, mem);
      }
    };

    template <>
    struct call_dtor_by_index<> {
      void operator()(int type_idx, void *mem) const {
        assert(false);
      }
    };
    

    template <typename ST>
    void operator=(const ST &s) {
      if (current_type_index > 0) {
        call_dtor_by_index<ArgsT...>()(current_type_index, &mem[0]);
      }

      new(mem) ST(s);
      current_type_index = type_index_of<ST, ArgsT...>::value;
    }
    
## final result ##


    #include <string>
    #include <iostream>
    #include <cassert>

    struct A {
      int a_a;
      int a_b;
      std::string a_c;

      A(int a, int b, const std::string &c)
        : a_a(a), a_b(b), a_c(c) {
        std::cout << "  A::A()" << std::endl;
      }

      A(const A&rhs)
        : a_a(rhs.a_a),
          a_b(rhs.a_b),
          a_c(rhs.a_c) {
        std::cout << "  A::A(const A &)" << std::endl;
      }

      ~A() {
        std::cout << "  ~A::A()" << std::endl;
      }
    };

    struct B {
      double b_d;
      int b_c;

      B(double d, int c)
        : b_d(d), b_c(c) {
        std::cout << "  B::B()" << std::endl;
      }

      ~B() {
        std::cout << "  ~B::B()" << std::endl;
      }

      B(const B& rhs)
        : b_d(rhs.b_d), b_c(rhs.b_c) {
        std::cout << "  B::B(const B &)" << std::endl;
      }
    };


    struct C {
      int c_c;
    };

    template <typename ...ArgsT>
    struct biggest_type_size;

    template <typename HT, typename ...TailT>
    struct biggest_type_size<HT, TailT...> {
      static constexpr size_t value = std::max(sizeof(HT), biggest_type_size<TailT...>::value);
    };

    template <> struct biggest_type_size<> { static constexpr size_t value = 0; };


    template <typename... Args> struct type_list_len;

    template <typename HT, typename ...TailT> 
    struct type_list_len<HT, TailT...> : public type_list_len<TailT...> {
      static constexpr int value = 1 + type_list_len<TailT...>::value;
    };

    template <> 
    struct type_list_len<> {
      static constexpr int value = 0;
    };



    template <typename ST, typename... Args> struct type_index_of;

    template <typename ST, typename ...TailT> 
    struct type_index_of<ST, ST, TailT...> {
      static constexpr int value = type_list_len<TailT...>::value;
    };

    template <typename ST, typename HT, typename ...TailT> 
    struct type_index_of<ST, HT, TailT...> {
      static constexpr int value = type_index_of<ST, TailT...>::value;
    };


    template <typename ...ArgsT>
    class mvar {
      template <typename ...SArgsT>
      struct call_dtor_by_index;

      template <typename HT, typename ...TailT>
      struct call_dtor_by_index<HT, TailT...> {
        void operator()(int type_idx, void *mem) const {
          if (type_idx == type_list_len<TailT...>::value) {
            HT *obj = reinterpret_cast<HT *>(mem);
            obj->~HT();
          } else call_dtor_by_index<TailT...>()(type_idx, mem);
        }
      };

      template <>
      struct call_dtor_by_index<> {
        void operator()(int type_idx, void *mem) const {
          assert(false);
        }
      };

    public:
      char mem[biggest_type_size<ArgsT...>::value];
      int current_type_index = -1;  

      template <typename ST>
      void operator=(const ST &s) {
        if (current_type_index > 0) {
          call_dtor_by_index<ArgsT...>()(current_type_index, &mem[0]);
        }

        new(mem) ST(s);
        current_type_index = type_index_of<ST, ArgsT...>::value;
      }

      template <typename ST>
      bool holds_alternative() const {
        return type_index_of<ST, ArgsT...>::value == current_type_index;
      }


      template <typename ST>
      const ST* get() const {
        assert(holds_alternative<ST>());
        return reinterpret_cast<const ST*>(&mem);
      }
    };


    void check_mvar() {
      std::cout << "type_list_len<A, B, C>::value = " << type_list_len<A, B, C>::value << std::endl;
      std::cout << "type_index_of<A, A, B, C>::value = " << type_index_of<A, A, B, C>::value << std::endl;
      mvar<A, B, C> v;
      std::cout << "store B" << std::endl;
      v = B { 1.1,  3 };
      std::cout << "  current type index: " << v.current_type_index << std::endl;;  
      std::cout << "  holds A: " << v.holds_alternative<A>() << std::endl;
      std::cout << "  holds B: " << v.holds_alternative<B>() << std::endl;;
      std::cout << "  holds C: " << v.holds_alternative<C>() << std::endl;;
      std::cout << v.get<B>()->b_d << std::endl;  



      std::cout << "store A" << std::endl;
      v = A { 1, 2, "abc"};
      std::cout << "  current type index: " << v.current_type_index << std::endl;;    
      std::cout << "  holds A: " << v.holds_alternative<A>() << std::endl;
      std::cout << "  holds B: " << v.holds_alternative<B>() << std::endl;;
      std::cout << "  holds C: " << v.holds_alternative<C>() << std::endl;; 

      std::cout << v.get<A>()->a_c << std::endl;
      //std::cout << v.get<B>()->b_d << std::endl;    
    }


    int main() {
      check_mvar();
      return 0;
    }
