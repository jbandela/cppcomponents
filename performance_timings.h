#include "jrb_interface/jrb_interface.hpp"
#include "jrb_interface/cr_string.hpp"

using namespace jrb_interface;


template<bool bImp>
struct TestInterface1:public jrb_interface::define_interface<bImp,5>{
	cross_function<TestInterface1,0,void()>f0;
	cross_function<TestInterface1,1,int()> f1;
	cross_function<TestInterface1,2,int(int)> f2;
	cross_function<TestInterface1,3,std::string()>f3;
	cross_function<TestInterface1,4,void(std::string)>f4;


	template<class T>
	TestInterface1(T t):TestInterface1::base_t(t),f0(this),f1(this),f2(this),f3(this),f4(this){
	}


};


struct VirtualInterface:public portable_base{
	virtual void f0() = 0;
	virtual int f1() = 0;
	virtual int f2(int) = 0;
	virtual std::string f3() = 0;
	virtual void f4(std::string) = 0;
};