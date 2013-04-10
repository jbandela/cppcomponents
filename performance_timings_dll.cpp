#include "performance_timings.h"

using namespace cross_compiler_interface;

struct FunctionImp:public implement_interface<TestInterface1>{
	FunctionImp(){
		f0 = [](){};
		f1 = [](){return 10;};
		f2 = [](int k){return 10 + k;};
		f3 = []()->std::string{
			std::string s;
			s = "Hello World";
			s+= " from DLL ";
			return s;
		};
		f4 = [](cross_compiler_interface::cr_string s){};
        f5 = [](std::string){};

	}

};

struct VirtualImp:public VirtualInterface{
	virtual void f0(){}
	virtual int f1(){return 10;}
	virtual int f2(int k){return 10 + k;}
	virtual std::string f3(){
		std::string s;
		s = "Hello World";
		s+= " from DLL ";
		return s;
	}
	virtual void f4(const std::string& s){}
	virtual void f5(std::string s){}



};


struct MemFnImp{

	void f0(){}
	int f1(){return 10;}
	int f2(int k){return 10 + k;}
	std::string f3(){
		std::string s;
		s = "Hello World";
		s+= " from DLL ";
		return s;
	}
	void f4(cross_compiler_interface::cr_string s){}
	void f5(std::string){}

	implement_interface<TestInterface1> imp;

	MemFnImp(){
		imp.f0.set_mem_fn<MemFnImp,&MemFnImp::f0>(this);
		imp.f1.set_mem_fn<MemFnImp,&MemFnImp::f1>(this);
		imp.f2.set_mem_fn<MemFnImp,&MemFnImp::f2>(this);
		imp.f3.set_mem_fn<MemFnImp,&MemFnImp::f3>(this);
		imp.f4.set_mem_fn<MemFnImp,&MemFnImp::f4>(this);
        imp.f5.set_mem_fn<MemFnImp,&MemFnImp::f5>(this);

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