#include "cross_compiler_interface/cross_compiler_interface.hpp"
#include "cross_compiler_interface/interface_unknown.hpp"

using cross_compiler_interface::cross_function;
using cross_compiler_interface::use_interface;

template<bool b> 
struct BaseInterface:public cross_compiler_interface::define_interface<b,1>{

	cross_function<BaseInterface,0,std::string()> hello_from_base;


	BaseInterface(cross_compiler_interface::portable_base* p):BaseInterface::base_t(p),hello_from_base(this){}
};

template<bool b>
struct IGetName:public cross_compiler_interface::define_interface<b,1>{
	cross_function<IGetName,0,std::string()> get_name;

	IGetName(cross_compiler_interface::portable_base* p):IGetName::base_t(p),get_name(this){}

};


template<class Iface, int Id>
struct cross_function_int_int:public cross_compiler_interface::custom_cross_function<Iface,Id,int(int),cross_compiler_interface::error_code (cross_compiler_interface::portable_base*,int*, int),cross_function_int_int<Iface,Id>>{

	typedef cross_compiler_interface::custom_cross_function<Iface,Id,int(int),cross_compiler_interface::error_code (cross_compiler_interface::portable_base*,int*, int),cross_function_int_int<Iface,Id>> base_t;
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
	static int CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(cross_compiler_interface::portable_base* v,int* r, int i){
		helper h(v);
		try{
			C* f = h.template get_mem_fn_object<C>();
			*r = (f->*mf)(i);
			return 0;
		} catch(std::exception& e){
			return h.error_code_from_exception(e);
		}
	}
	static int CROSS_CALL_CALLING_CONVENTION vtable_func(cross_compiler_interface::portable_base* v,int* r, int i){
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


// There is at least 2 ways to define the interfaces
// Here is the more verbose way, but it does not require template alias support and non-static data member initializers
#ifdef _MSC_VER

template<bool b> struct TestInterface:public cross_compiler_interface::define_interface<b,13,BaseInterface>{

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


	
	TestInterface(cross_compiler_interface::portable_base* p):TestInterface::base_t(p), 
		plus_5(this),times_2point5(this),double_referenced_int(this),
		count_characters(this),say_hello(this),use_at_out_of_range(this),not_implemented(this),split_into_words(this),say_hello2(this),
		get_string_at(this),get_igetname(this),get_name_from_runtime_parent(this),custom_with_runtime_parent(this){}
};

#else

// Here is the better way with C++11 support for initialization of non-static class members and template alias
template<bool b> struct TestInterface:public cross_compiler_interface::define_interface<b,13,BaseInterface>{

	template<int Id, class F>
	using cf = cross_function<TestInterface,Id,F>;

	cross_function_int_int<TestInterface,0> plus_5 = this;

	cf<1,double(double)> times_2point5 = this;
	cf<2,void(int&)> double_referenced_int = this;
	cf<3,int(std::string)> count_characters = this;
	cf<4,std::string(std::string)> say_hello = this;
	cf<5,void(std::string)> use_at_out_of_range = this;
	cf<6,void()> not_implemented = this;
	cf<7,std::vector<std::string>(std::string)> split_into_words = this;
	cf<8,std::string(use_interface<IGetName>)> say_hello2 = this;
	cf<9,std::pair<int,std::string> (std::vector<std::string> v,int pos)> get_string_at = this;
	cf<10,use_interface<IGetName>()> get_igetname = this;
	cf<11,std::string()> get_name_from_runtime_parent = this;



	cross_function_int_int<TestInterface,12> custom_with_runtime_parent = this;


	
	TestInterface(cross_compiler_interface::portable_base* p):TestInterface::base_t(p) {}
};


#endif

// {0AEBCA97-B08D-4FCF-8C41-133C1A8ABF03}
typedef cross_compiler_interface::uuid<0xaebca97, 0xb08d, 0x4fcf, 0x8c, 0x41, 0x13, 0x3c, 0x1a, 0x8a, 0xbf, 0x3>
	UnknownDerivedInterface_uuid_t;
template<bool b>
struct IUnknownDerivedInterface:public cross_compiler_interface::define_interface_unknown<b,1,UnknownDerivedInterface_uuid_t>{
	cross_function<IUnknownDerivedInterface,0,std::string()> hello_from_iuknown_derived;
	
	IUnknownDerivedInterface(cross_compiler_interface::portable_base* p):
		IUnknownDerivedInterface::base_t(p), hello_from_iuknown_derived(this)
	{}


};
// {9A5306A2-5F78-4ECB-BCEB-AADE1F9223AA}
typedef cross_compiler_interface::uuid<0x9a5306a2, 0x5f78, 0x4ecb, 0xbc, 0xeb, 0xaa, 0xde, 0x1f, 0x92, 0x23, 0xaa>
	UnknownDerivedInterface2_uuid_t;
template<bool b>
struct IUnknownDerivedInterface2:public cross_compiler_interface::define_interface_unknown<b,1,UnknownDerivedInterface2_uuid_t>{
	cross_function<IUnknownDerivedInterface2,0,std::string()> hello_from_iuknown_derived2;
	
	IUnknownDerivedInterface2(cross_compiler_interface::portable_base* p):
		IUnknownDerivedInterface2::base_t(p), hello_from_iuknown_derived2(this)
	{}


};


// {9338EC17-6775-457F-9721-0E7C3CBF8942}
typedef cross_compiler_interface::uuid<0x9338ec17, 0x6775, 0x457f, 0x97, 0x21, 0xe, 0x7c, 0x3c, 0xbf, 0x89, 0x42>
	UnknownDerivedInterface2Derived_uuid_t;
template<bool b>
struct IUnknownDerivedInterface2Derived:public cross_compiler_interface::define_interface_unknown<b,1,UnknownDerivedInterface2Derived_uuid_t,
		IUnknownDerivedInterface2>{
	cross_function<IUnknownDerivedInterface2Derived,0,std::string()> hello_from_derived;
	
	IUnknownDerivedInterface2Derived(cross_compiler_interface::portable_base* p):
		IUnknownDerivedInterface2Derived::base_t(p), hello_from_derived(this)
	{}


};

// {52918617-D0BE-41FF-A641-2EC32AC1B157}
typedef cross_compiler_interface::uuid<0x52918617, 0xd0be, 0x41ff, 0xa6, 0x41, 0x2e, 0xc3, 0x2a, 0xc1, 0xb1, 0x57>
	UnknownDerivedInterfaceUnused_uuid_t;
template<bool b>
struct IUnknownDerivedInterfaceUnused:public cross_compiler_interface::define_interface_unknown<b,1,UnknownDerivedInterfaceUnused_uuid_t,
		IUnknownDerivedInterface2Derived>{
	cross_function<IUnknownDerivedInterfaceUnused,0,std::string()> unused;
	
	IUnknownDerivedInterfaceUnused(cross_compiler_interface::portable_base* p):
		IUnknownDerivedInterfaceUnused::base_t(p), unused(this)
	{}


};