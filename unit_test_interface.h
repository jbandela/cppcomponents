#include "jrb_interface\jrb_interface.hpp"
#include "jrb_interface\custom_cross_function.hpp"

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


template<class Iface, int Id>
struct cross_function_int_int:public jrb_interface::custom_cross_function<Iface,Id,int(int),int(jrb_interface::portable_base*,int),cross_function_int_int<Iface,Id>>{

	using jrb_interface::custom_cross_function<Iface,Id,int(int),int(jrb_interface::portable_base*,int),cross_function_int_int<Iface,Id>>::helper;
	typedef jrb_interface::custom_cross_function<Iface,Id,int(int),int(jrb_interface::portable_base*,int),cross_function_int_int<Iface,Id>> base_t;
	int call_vtable_function(int i){
		return  this->get_vtable_fn()(this->get_portable_base(),i);
	}
	template<class C,class MF, MF mf>
	static int CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v,int i){
		try{
			helper h(v);
			C* f = h.get_mem_fn_object<C>();
			return (f->*mf)(i);
		} catch(std::exception& e){
			return 0;//typename jrb_interface::error_mapper<jrb_interface::NoBase>::mapper::error_code_from_exception(e);
		}
	}
	static int CROSS_CALL_CALLING_CONVENTION vtable_func(jrb_interface::portable_base* v,int i){
		try{
			helper h(v);
			return h.get_function()(i);
		} catch(std::exception& e){
			return 0;//error_mapper<Iface>::mapper::error_code_from_exception(e);
		}
	}

	template<class F>
	void operator=(F f){
		this->set_function(f);

	}

	template<class T>
	cross_function_int_int(T t):base_t(t){}


};


template<bool b> struct TestInterface:public jrb_interface::define_interface<b,11,BaseInterface<b>>{

	cross_function<TestInterface,0,int(int)> plus_5;
	//cross_function_int_int<TestInterface,0> plus_5;
	cross_function<TestInterface,1,double(double)> times_2point5;
	cross_function<TestInterface,2,void(int&)> double_referenced_int;
	cross_function<TestInterface,3,int(std::string)> count_characters;
	cross_function<TestInterface,4,std::string(std::string)> say_hello;
	cross_function<TestInterface,5,void(std::string)> use_at_out_of_range;
	cross_function<TestInterface,6,void()> not_implemented;
	cross_function<TestInterface,7,std::vector<std::string>(std::string)> split_into_words;
	cross_function<TestInterface,8,std::string(use_interface<IGetName>)> say_hello2;
	cross_function<TestInterface,9,std::pair<int,std::string> (std::vector<std::string> v,int pos)> get_string_at;
	cross_function<TestInterface,10,use_interface<IGetName>()> get_igetname;


	
	template<class T>
	TestInterface(T t):TestInterface<b>::base_t(t), 
		plus_5(t),times_2point5(t),double_referenced_int(t),
		count_characters(t),say_hello(t),use_at_out_of_range(t),not_implemented(t),split_into_words(t),say_hello2(t),
		get_string_at(t),get_igetname(t){}
};