#include"jrb_interface/jrb_interface.hpp"
#include <string>

// Use cross_function without namespace to shorten code
using jrb_interface::cross_function;


// A Base class to show you can inherit an interface


template<bool b> // All interfaces take a bool template parameter

// All interfaces need to derive from define_interface passing in the bool parameter, and the number of functions
struct BaseInterface:public jrb_interface::define_interface<b,1>{

	// To declare a function in the interface use cross_function
	// it takes the BaseInterface,the 0 based id of the function (starting from 0 to the number of functions - 1,
	// and the signature - same as you would use for std::function;
	cross_function<BaseInterface,0,std::string()> hello_from_base;

	// The interface needs a templated constructor that takes t and passes it to each cross_function constructor
	// also needs to pass the t to the define_iterface base, define_interface typedefs base_t to itself
	template<class T>
	BaseInterface(T t):BaseInterface<b>::base_t(b),hello_from_base(t){}
};

// This is an interface that we will pass into a function
template<bool b>
struct IGetName:public jrb_interface::define_interface<b,1>{
	cross_function<IGetName,0,std::string()> get_name;

	template<class T>
	IGetName(T t):IGetName<b>::base_t(t),get_name(t){}

};

template<bool b> struct DemoInterface:public jrb_interface::define_interface<b,9,BaseInterface<b>>{

	// Pass in an int and return an int
	cross_function<DemoInterface,0,int(int)> plus_5;

	// Same as above with double
	cross_function<DemoInterface,1,double(double)> times_2point5;

	// Manipulate a reference
	cross_function<DemoInterface,2,void(int&)> double_referenced_int;

	// Receive a string and get an int
	cross_function<DemoInterface,3,int(std::string)> count_characters;

	// String as parameter and return
	cross_function<DemoInterface,4,std::string(std::string)> say_hello;

	// An exception thrown by STL
	cross_function<DemoInterface,5,void(std::string)> use_at_out_of_range;

	// Another exception thrown by jrb_interface
	cross_function<DemoInterface,6,void()> not_implemented;

	// Return a vector<string>
	cross_function<DemoInterface,7,std::vector<std::string>(std::string)> split_into_words;

	// Use another interface
	cross_function<DemoInterface,8,std::string(jrb_interface::use_interface<IGetName>)> say_hello2;



	// Initialize the base class (base_t) and all the functions
	template<class T>
	DemoInterface(T t):DemoInterface<b>::base_t(t), 
		plus_5(t),times_2point5(t),double_referenced_int(t),
		count_characters(t),say_hello(t),use_at_out_of_range(t),not_implemented(t),split_into_words(t),say_hello2(t){}
};