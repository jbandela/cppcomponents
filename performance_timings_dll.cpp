#include "performance_timings.h"
#include <objbase.h>
using namespace cross_compiler_interface;

static const std::string long_string(1011*4,'a');

struct FunctionImp:public implement_interface<TestInterface1>{
	FunctionImp(){
		f0 = [](){};
		f1 = [](){return 10;};
		f2 = [](int k){return 10 + k;};
		f3 = []()->std::string{
            auto s = long_string;
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
        auto s = long_string;
		return s;
	}
	virtual void f4(const std::string& s){}
	virtual void f5(std::string s){}
    virtual const char* f6(std::size_t* pcount){
        char* ret = (char*) CoTaskMemAlloc(long_string.size() + 1);
        memcpy(ret,long_string.data(),long_string.size());
        ret[long_string.size()] = 0;
        *pcount = long_string.size();
        return ret;

    };
    virtual void f7(const char*,std::size_t){}



};


struct MemFnImp{

	void f0(){}
	int f1(){return 10;}
	int f2(int k){return 10 + k;}
	std::string f3(){
        auto s = long_string;
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