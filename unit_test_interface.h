#include "jrb_interface\jrb_interface.hpp"
#include "jrb_interface\custom_cross_function.hpp"
#include "jrb_interface\interface_unknown.hpp"

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
struct cross_function_int_int:public jrb_interface::custom_cross_function<Iface,Id,int(int),jrb_interface::error_code (jrb_interface::portable_base*,int*, int),cross_function_int_int<Iface,Id>>{

	typedef jrb_interface::custom_cross_function<Iface,Id,int(int),jrb_interface::error_code (jrb_interface::portable_base*,int*, int),cross_function_int_int<Iface,Id>> base_t;
	typedef typename base_t::helper helper;
	int call_vtable_function(int i)const{
		int r = 0;
		auto ret = this->get_vtable_fn()(this->get_portable_base(),&r,i);
		if(ret){
			this->exception_from_error_code(ret);
		}
		return r;
	}
	template<class C,class MF, MF mf>
	static int CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v,int* r, int i){
		helper h(v);
		try{
			C* f = h.template get_mem_fn_object<C>();
			*r = (f->*mf)(i);
			return 0;
		} catch(std::exception& e){
			return h.error_code_from_exception(e);
		}
	}
	static int CROSS_CALL_CALLING_CONVENTION vtable_func(jrb_interface::portable_base* v,int* r, int i){
		helper h(v);
		try{
			auto& f = h.get_function();
			if(!f){
				return h.forward_to_runtime_parent(r,i);
			}
			*r = f(i);
			return 0;
		} catch(std::exception& e){
			return h.error_code_from_exception(e);
		}
	}

	template<class F>
	void operator=(F f){
		this->set_function(f);

	}

	template<class T>
	cross_function_int_int(T t):base_t(t){}


};


template<bool b> struct TestInterface:public jrb_interface::define_interface<b,13,BaseInterface<b>>{

	//cross_function<TestInterface,0,int(int)> plus_5;
	cross_function_int_int<TestInterface,0> plus_5;
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
	cross_function<TestInterface,11,std::string()> get_name_from_runtime_parent;
	cross_function_int_int<TestInterface,12> custom_with_runtime_parent;


	
	template<class T>
	TestInterface(T t):TestInterface<b>::base_t(t), 
		plus_5(t),times_2point5(t),double_referenced_int(t),
		count_characters(t),say_hello(t),use_at_out_of_range(t),not_implemented(t),split_into_words(t),say_hello2(t),
		get_string_at(t),get_igetname(t),get_name_from_runtime_parent(t),custom_with_runtime_parent(t){}
};

// {0AEBCA97-B08D-4FCF-8C41-133C1A8ABF03}
typedef jrb_interface::uuid<0xaebca97, 0xb08d, 0x4fcf, 0x8c, 0x41, 0x13, 0x3c, 0x1a, 0x8a, 0xbf, 0x3>
	UnknownDerivedInterface_uuid_t;
template<bool b>
struct IUnknownDerivedInterface:public jrb_interface::define_interface_unknown<b,1,UnknownDerivedInterface_uuid_t>{
	cross_function<IUnknownDerivedInterface,0,std::string()> hello_from_iuknown_derived;
	
	template<class T>
	IUnknownDerivedInterface(T t):
		IUnknownDerivedInterface::base_t(t), hello_from_iuknown_derived(t)
	{}


};
// {9A5306A2-5F78-4ECB-BCEB-AADE1F9223AA}
typedef jrb_interface::uuid<0x9a5306a2, 0x5f78, 0x4ecb, 0xbc, 0xeb, 0xaa, 0xde, 0x1f, 0x92, 0x23, 0xaa>
	UnknownDerivedInterface2_uuid_t;
template<bool b>
struct IUnknownDerivedInterface2:public jrb_interface::define_interface_unknown<b,1,UnknownDerivedInterface2_uuid_t>{
	cross_function<IUnknownDerivedInterface2,0,std::string()> hello_from_iuknown_derived2;
	
	template<class T>
	IUnknownDerivedInterface2(T t):
		IUnknownDerivedInterface2::base_t(t), hello_from_iuknown_derived2(t)
	{}


};


// {9338EC17-6775-457F-9721-0E7C3CBF8942}
typedef jrb_interface::uuid<0x9338ec17, 0x6775, 0x457f, 0x97, 0x21, 0xe, 0x7c, 0x3c, 0xbf, 0x89, 0x42>
	UnknownDerivedInterface2Derived_uuid_t;
template<bool b>
struct IUnknownDerivedInterface2Derived:public jrb_interface::define_interface_unknown<b,1,UnknownDerivedInterface2Derived_uuid_t,
		IUnknownDerivedInterface2<b>>{
	cross_function<IUnknownDerivedInterface2Derived,0,std::string()> hello_from_derived;
	
	template<class T>
	IUnknownDerivedInterface2Derived(T t):
		IUnknownDerivedInterface2Derived::base_t(t), hello_from_derived(t)
	{}


};

// {52918617-D0BE-41FF-A641-2EC32AC1B157}
typedef jrb_interface::uuid<0x52918617, 0xd0be, 0x41ff, 0xa6, 0x41, 0x2e, 0xc3, 0x2a, 0xc1, 0xb1, 0x57>
	UnknownDerivedInterfaceUnused_uuid_t;
template<bool b>
struct IUnknownDerivedInterfaceUnused:public jrb_interface::define_interface_unknown<b,1,UnknownDerivedInterfaceUnused_uuid_t,
		IUnknownDerivedInterface2Derived<b>>{
	cross_function<IUnknownDerivedInterfaceUnused,0,std::string()> unused;
	
	template<class T>
	IUnknownDerivedInterfaceUnused(T t):
		IUnknownDerivedInterfaceUnused::base_t(t), unused(t)
	{}


};