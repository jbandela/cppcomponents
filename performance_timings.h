#include "jrb_interface/jrb_interface.hpp"
#include "jrb_interface/cr_string.hpp"

using namespace jrb_interface;


template<bool bImp>
struct TestInterface1:public jrb_interface::define_interface<bImp,6>{
	cross_function<TestInterface1,0,int()> f1;
	cross_function<TestInterface1,1,int(int)> f2;
	cross_function<TestInterface1,2,std::string()>f3;
	cross_function<TestInterface1,3,void(jrb_interface::cr_string)>f4;
	cross_function<TestInterface1,4,void(std::vector<std::string>)>f5;
	cross_function<TestInterface1,5,void()>f6;


	template<class T>
	TestInterface1(T t):TestInterface1::base_t(t),f1(t),f2(t),f3(t),f4(t),f5(t),f6(t){
	}


};


struct VirtualInterface:public portable_base{
	virtual int f1() = 0;
	virtual int f2(int) = 0;
	virtual std::string f3() = 0;
	virtual void f4(const std::string&) = 0;
	virtual void f5(const std::vector<std::string>) = 0;
	virtual void f6() = 0;
};