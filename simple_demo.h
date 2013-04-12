#include "cross_compiler_interface/interface_unknown.hpp"

#if 0


struct KVStore{
	KVStore();
	~KVStore();
	void Put(const std::string& key, const std::string& value);
	bool Get(const std::string& key,std::string* value);
	bool Delete(const std::string& key);
};

#endif


struct KVStore;
typedef KVStore* HKVStore;
using std::int32_t;
typedef std::int32_t error_code;
#define CALLING_CONVENTION __stdcall

extern "C"{
	HKVStore CALLING_CONVENTION Create_KVStore();
	void CALLING_CONVENTION Destroy_KVStore(HKVStore h);
	error_code CALLING_CONVENTION Put (HKVStore h,const char* key, int32_t key_count,const char* value, int32_t value_count);
	error_code CALLING_CONVENTION Get(HKVStore h, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn);
	error_code CALLING_CONVENTION Delete(HKVStore h, const char* key, int32_t key_count,char* breturn);
}

struct IKVStore{
	virtual error_code CALLING_CONVENTION Put (const char* key, int32_t key_count,const char* value, int32_t value_count) = 0;
	virtual error_code CALLING_CONVENTION Get(const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn) = 0;
	virtual error_code CALLING_CONVENTION Delete(const char* key, int32_t key_count,char* breturn)=0;
	virtual void CALLING_CONVENTION Destroy() = 0;
};

struct IKVStore2;

struct IKVStoreVtable{
	error_code (CALLING_CONVENTION * Put) (IKVStore2* ikv, const char* key, int32_t key_count,const char* value, int32_t value_count);
	error_code (CALLING_CONVENTION *Get)(IKVStore2* ikv, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn);
	error_code (CALLING_CONVENTION *Delete)(IKVStore2* ikv, const char* key, int32_t key_count,char* breturn);
	void (CALLING_CONVENTION *Destroy)(IKVStore2* ikv);
};

struct IKVStore2{
	IKVStoreVtable* vtable;
};


struct simple_cross_function1_usage{
	IKVStore2* ikv;

	void operator()(std::string key, std::string value){
		auto ret = ikv->vtable->Put(ikv,key.data(),key.size(),value.data(),value.size());
		if(ret){
			throw std::runtime_error("Error in Put");
		}
	};

	simple_cross_function1_usage(IKVStore2* i):ikv(i){}
};



struct IKVStore2Derived:public IKVStore2{
	void* pput;

};

struct simple_cross_function1_implementation{
	std::function<void(std::string,std::string)> put;

	static error_code CALLING_CONVENTION Put_ (IKVStore2* ikv, const char* key,
		int32_t key_count,const char* value, int32_t value_count){
			try{
				std::string key(key,key_count);
				std::string value(value,value_count);
				auto ikvd = static_cast<IKVStore2Derived*>(ikv);
				auto& f = *static_cast<std::function<void(std::string,
					std::string)>*>(ikvd->pput);
				f(key,value);
				return 0;
			}
			catch(std::exception&){
				return -1;
			}
	}

	template<class F>
	void operator=(F f){
		put = f;
	}

	simple_cross_function1_implementation(IKVStore2Derived* ikvd){
		ikvd->pput = &put;
		ikvd->vtable->Put = &Put_;
	}
};

struct IKV2DerivedImplementationBase:public IKVStore2Derived{
	IKVStoreVtable vt;
	IKV2DerivedImplementationBase(){
		vtable = &vt;
	}
};
struct IKVStore2UsageWrapper{
	simple_cross_function1_usage Put;

	IKVStore2UsageWrapper(IKVStore2* ikv):Put(ikv){}
};
struct IKVStore2DerivedImplementation:public IKV2DerivedImplementationBase{
	simple_cross_function1_implementation Put;

	IKVStore2DerivedImplementation():Put(this){}
};

template<int n>
struct simple_cross_function2_usage{

	typedef error_code (CALLING_CONVENTION *fun_ptr_t)(cross_compiler_interface::portable_base*, const char*,
		int32_t,const char*, int32_t); 
	cross_compiler_interface::portable_base* pb_;
	void operator()(std::string key, std::string value){
		auto ret = reinterpret_cast<fun_ptr_t>(pb_->vfptr[n])(pb_,key.data(),key.size(),value.data(),value.size());
		if(ret){
			throw std::runtime_error("Error in simple cross_function2");
		}
	}

	simple_cross_function2_usage(cross_compiler_interface::portable_base* p):pb_(p){} 
};

template<int n>
struct simple_cross_function2_implementation{
	std::function<void(std::string,std::string)> f_;

	static error_code CALLING_CONVENTION Function_ (cross_compiler_interface::portable_base* pb, const char* key,
		int32_t key_count,const char* value, int32_t value_count){
			try{
				std::string key(key,key_count);
				std::string value(value,value_count);
				auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
				auto& f = *static_cast<std::function<void(std::string,
					std::string)>*>(vnb->pdata[n]);
				f(key,value);
				return 0;
			}
			catch(std::exception&){
				return -1;
			}
	}

	template<class F>
	void operator=(F f){
		f_ = f;
	}

	simple_cross_function2_implementation(cross_compiler_interface::portable_base* pb){
		auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
		vnb->vfptr[n] = reinterpret_cast<cross_compiler_interface::detail::ptr_fun_void_t>(&Function_);
		vnb->pdata[n] = &f_;
	}
};


struct IKVStore2UsageWrapper2{
	simple_cross_function2_usage<0> Put;

	IKVStore2UsageWrapper2(cross_compiler_interface::portable_base* p):Put(p){}
};
struct IKVStore2DerivedImplementation2:public cross_compiler_interface::vtable_n<4>{
	simple_cross_function2_implementation<0> Put;

	IKVStore2DerivedImplementation2():Put(this){}
};

template<template <class> class Iface>
struct use_interface:public Iface<use_interface<Iface>>{ // Usage
	explicit use_interface(cross_compiler_interface::portable_base* p):Iface<use_interface<Iface>>(p){}
};

template<template <class> class Iface>
struct implement_interface:
	private cross_compiler_interface::vtable_n<4>, 
	public Iface<implement_interface<Iface>>
{
	implement_interface():Iface<implement_interface<Iface>>(this->get_portable_base()){}

	using cross_compiler_interface::vtable_n<4>::get_portable_base;
};
template<class T, int n>
struct simple_cross_function3{ // usage

	typedef error_code (CALLING_CONVENTION *fun_ptr_t)(cross_compiler_interface::portable_base*, const char*,
		int32_t,const char*, int32_t); 
	cross_compiler_interface::portable_base* pb_;
	void operator()(std::string key, std::string value){
		auto ret = reinterpret_cast<fun_ptr_t>(pb_->vfptr[n])(pb_,key.data(),key.size(),value.data(),value.size());
		if(ret){
			throw std::runtime_error("Error in simple cross_function2");
		}
	}

	simple_cross_function3(cross_compiler_interface::portable_base* p):pb_(p){} 
};


template<template<class> class Iface,int n>
struct simple_cross_function3<Iface<implement_interface<Iface>>,n>{ // implementation

	std::function<void(std::string,std::string)> f_;

	static error_code CALLING_CONVENTION Function_ (cross_compiler_interface::portable_base* pb, const char* key,
		int32_t key_count,const char* value, int32_t value_count){
			try{
				std::string key(key,key_count);
				std::string value(value,value_count);
				auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
				auto& f = *static_cast<std::function<void(std::string,
					std::string)>*>(vnb->pdata[n]);
				f(key,value);
				return 0;
			}
			catch(std::exception&){
				return -1;
			}
	}

	template<class F>
	void operator=(F f){
		f_ = f;
	}

	simple_cross_function3(cross_compiler_interface::portable_base* pb){
		auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
		vnb->vfptr[n] = reinterpret_cast<cross_compiler_interface::detail::ptr_fun_void_t>(&Function_);
		vnb->pdata[n] = &f_;
	}
};

template<class T>
struct IKV_simple_cross_function3{
	simple_cross_function3<IKV_simple_cross_function3,0> Put;

	IKV_simple_cross_function3(cross_compiler_interface::portable_base* p):Put(p){}

};

template<class T, int n, class F>
struct simple_cross_function4{};

template<class T, int n, class Parm1, class Parm2>
struct simple_cross_function4<T,n,void(Parm1,Parm2)>{ // usage

	typedef error_code (CALLING_CONVENTION *fun_ptr_t)(cross_compiler_interface::portable_base*, 
		typename cross_compiler_interface::cross_conversion<Parm1>::converted_type,
		typename cross_compiler_interface::cross_conversion<Parm2>::converted_type); 
	cross_compiler_interface::portable_base* pb_;
	void operator()(Parm1 p1, Parm2 p2){
		auto ret = reinterpret_cast<fun_ptr_t>(pb_->vfptr[n])(pb_,
			cross_compiler_interface::cross_conversion<Parm1>::to_converted_type(p1),
			cross_compiler_interface::cross_conversion<Parm2>::to_converted_type(p2));
		if(ret){
			throw std::runtime_error("Error in simple cross_function2");
		}
	}

	simple_cross_function4(cross_compiler_interface::portable_base* p):pb_(p){} 
};


template<template<class> class Iface,int n,class Parm1, class Parm2>
struct simple_cross_function4<Iface<implement_interface<Iface>>,n,void(Parm1,Parm2)>{ // implementation

	std::function<void(Parm1, Parm2)> f_;
	// Without these msvc has compiler error
	typedef cross_compiler_interface::cross_conversion<Parm1> cc1;
	typedef cross_compiler_interface::cross_conversion<Parm2> cc2;

	static error_code CALLING_CONVENTION Function_ (cross_compiler_interface::portable_base* pb,
		typename cross_compiler_interface::cross_conversion<Parm1>::converted_type p1,
		typename cross_compiler_interface::cross_conversion<Parm2>::converted_type p2){
			try{
				using namespace std;
				using namespace cross_compiler_interface;
				auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
				auto& f = *static_cast<std::function<void(Parm1, Parm2)>*>(vnb->pdata[n]);
				f(cc1::to_original_type(p1),cc2::to_original_type(p2));
				return 0;
			}
			catch(std::exception&){
				return -1;
			}
	}

	template<class F>
	void operator=(F f){
		f_ = f;
	}

	simple_cross_function4(cross_compiler_interface::portable_base* pb){
		auto vnb = static_cast<cross_compiler_interface::vtable_n_base*>(pb);
		vnb->vfptr[n] = reinterpret_cast<cross_compiler_interface::detail::ptr_fun_void_t>(&Function_);
		vnb->pdata[n] = &f_;
	}
};
template<class T>
struct IKV_simple_cross_function4{
	simple_cross_function4<IKV_simple_cross_function4,0,void(std::string,
		std::string)> Put;

	IKV_simple_cross_function4(cross_compiler_interface::portable_base* p):Put(p){}

};
using cross_compiler_interface::cross_function;

template<class T>
struct InterfaceKVStore
	:public cross_compiler_interface::define_interface<T>
{
	cross_function<InterfaceKVStore,0,void(std::string,std::string)> Put;

	cross_function<InterfaceKVStore,1,
		bool(std::string,cross_compiler_interface::out<std::string>)> Get;

	cross_function<InterfaceKVStore,2,
		bool(std::string)> Delete;
	cross_function<InterfaceKVStore,3,void()> Destroy;

	InterfaceKVStore()
		:Put(this),Get(this),Delete(this),Destroy(this)
	{}
};

template<class T>
struct InterfaceKVStore2
	:public cross_compiler_interface::define_interface_unknown<T,
	// {B781B4FF-995D-4122-842C-E14A4C0348CC}
	cross_compiler_interface::uuid<
	0xB781B4FF,0x995D,0x4122,0x84,0x2C,0xE1,0x4A,0x4C,0x03,0x48,0xCC
	>
	>
{

	typedef cross_compiler_interface::cr_string cr_string;
	cross_function<InterfaceKVStore2,0,void(cr_string,cr_string)> Put;
	cross_function<InterfaceKVStore2,1,bool(cr_string,cross_compiler_interface::out<std::string>)> Get;
	cross_function<InterfaceKVStore2,2,bool(cr_string)> Delete;

	InterfaceKVStore2()
		:Put(this),Get(this),Delete(this)
	{}


};

struct FunctionInterface{
	std::function<std::string()> SayHello;
	std::function<std::string(int)> SayMultipleHellos;
};