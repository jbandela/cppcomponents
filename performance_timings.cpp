#include "performance_timings.h"
#include <chrono>
#include <iostream>


struct CallOnlyTests{
	static std::string Description(){return "CallOnlyTests";}

	template<class T>
	static void Run(T& t){
		t.f0();
	}


};
struct IntegerTests{
	static std::string Description(){return "IntegerTests";}

	template<class T>
	static void  Run(T& t){
		t.f1();
		t.f2(6) ;
	}
};

struct StringTests{
	static std::string Description(){return "StringTests";}

	template<class T>
	static void  Run(T& t){
		t.f3();
		t.f4("Hello");
	}
};


template<class Test,class T>
double TimingTest(T& t){

	auto begin = std::chrono::steady_clock::now();
	const int iterations = 10000000;
	for(int i = 0; i < iterations;i++){
		Test::Run(t);
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - begin;
	return std::chrono::duration<double,std::nano>(diff).count()/iterations;


}

template<class Test,class... Tests>
struct Runner{

	template<class T>
	static void Run(std::string type, T&t){
		std::cout << "Timings for " << type << std::endl;
		RunImp(t);
		std::cout << std::endl;

	}


	template<class T>
	static void RunImp(T& t){
		auto d = TimingTest<Test>(t);
		std::cout << "   " << Test::Description() << " " << d << std::endl;
		Runner<Tests...>::RunImp(t);

	}

};

template<class Test>
struct Runner<Test>{
	template<class T>
	static void RunImp(T& t){
		auto d = TimingTest<Test>(t);
		std::cout <<  "   " << Test::Description() << " " << d << std::endl;
	}

};


int main(){

	cross_compiler_interface::module m("performance_timings_dll");

	// Virtual Function Implementation
	typedef const portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
	auto f = m.load_module_function<CFun>("CreateVirtualInterface");
	VirtualInterface* p = (VirtualInterface*) f();

	// std::function implementation
	use_interface<TestInterface1> t1(cross_compiler_interface::create<TestInterface1>(m,"CreateFunctionImpInterface"));

	// Member function implementation
	use_interface<TestInterface1> t2(cross_compiler_interface::create<TestInterface1>(m,"CreateMemFnImpInterface"));

	typedef Runner<CallOnlyTests,IntegerTests,StringTests> TestRunner;
	TestRunner::Run("VirtualInterface",*p);
	TestRunner::Run("FunctionImp",t1);
	TestRunner::Run("MemFnImp",t2);



}