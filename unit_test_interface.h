#include "jrb_interface\jrb_interface.hpp"

using jrb_interface::cross_function;
using jrb_interface::use_interface;

template<bool b> 
struct BaseInterface:public jrb_interface::define_interface<b,1>{


	cross_function<BaseInterface,0,std::string()> hello_from_base;


	template<class T>
	BaseInterface(T t):BaseInterface<b>::base_t(b),hello_from_base(t){}
};

template<bool b>
struct IGetName:public jrb_interface::define_interface<b,1>{
	cross_function<IGetName,0,std::string()> get_name;

	template<class T>
	IGetName(T t):IGetName<b>::base_t(t),get_name(t){}

};

template<bool b> struct TestInterface:public jrb_interface::define_interface<b,10,BaseInterface<b>>{

	cross_function<TestInterface,0,int(int)> plus_5;
	cross_function<TestInterface,1,double(double)> times_2point5;
	cross_function<TestInterface,2,void(int&)> double_referenced_int;
	cross_function<TestInterface,3,int(std::string)> count_characters;
	cross_function<TestInterface,4,std::string(std::string)> say_hello;
	cross_function<TestInterface,5,void(std::string)> use_at_out_of_range;
	cross_function<TestInterface,6,void()> not_implemented;
	cross_function<TestInterface,7,std::vector<std::string>(std::string)> split_into_words;
	cross_function<TestInterface,8,std::string(use_interface<IGetName>)> say_hello2;
	cross_function<TestInterface,9,std::pair<int,std::string> (std::vector<std::string> v,int pos)> get_string_at;



	
	template<class T>
	TestInterface(T t):TestInterface<b>::base_t(t), 
		plus_5(t),times_2point5(t),double_referenced_int(t),
		count_characters(t),say_hello(t),use_at_out_of_range(t),not_implemented(t),split_into_words(t),say_hello2(t),
		get_string_at(t){}
};