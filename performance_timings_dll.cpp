#include "performance_timings.h"

using namespace jrb_interface;

struct FunctionImp:public implement_interface<TestInterface1>{
	FunctionImp(){
		f0 = [](){};
		f1 = [](){return 10;};
		f2 = [](int k){return 10 + k;};
		f3 = []()->std::string{return "Hello World from DLL ";};
		f4 = [](std::string s){};

	}

};

struct VirtualImp:public VirtualInterface{
	virtual void f0(){}
	virtual int f1(){return 10;}
	virtual int f2(int k){return 10 + k;}
	virtual std::string f3(){return "Hello World from DLL ";}
	virtual void f4(std::string s){}



};


struct MemFnImp{

	void f0(){}
	int f1(){return 10;}
	int f2(int k){return 10 + k;}
	std::string f3(){return "Hello World from DLL ";}
	void f4(std::string s){}
	void f5(const std::vector<std::string> v){}

	implement_interface<TestInterface1> imp;

	MemFnImp(){
		imp.f0.set_mem_fn<MemFnImp,&MemFnImp::f0>(this);
		imp.f1.set_mem_fn<MemFnImp,&MemFnImp::f1>(this);
		imp.f2.set_mem_fn<MemFnImp,&MemFnImp::f2>(this);
		imp.f3.set_mem_fn<MemFnImp,&MemFnImp::f3>(this);
		imp.f4.set_mem_fn<MemFnImp,&MemFnImp::f4>(this);

	}


};


extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateFunctionImpInterface(){
	static FunctionImp d_;
	return d_.get_portable_base();
}
}

extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateMemFnImpInterface(){
	static MemFnImp d_;
	return d_.imp.get_portable_base();
}
}


extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateVirtualInterface(){
	static VirtualImp d_;
	return &d_;
}
}