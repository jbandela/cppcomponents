#include "performance_timings.h"
#include <chrono>
#include <iostream>



template<class T>
double timingTest(T& t){

	auto begin = std::chrono::steady_clock::now();
	const int iterations = 10000000;
	std::string s1("String1");
	for(int i = 0; i < iterations;i++){
		//t.f6();
		//t.f1();
		//t.f2(6) ;
		t.f4(s1);
		//std::vector<std::string> v;
		//v.push_back("v1");
		//v.push_back("v2");
		//v.push_back("v3");

		//t.f5(v);

		//t.f3();
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - begin;
	return std::chrono::duration<double,std::nano>(diff).count()/iterations;


}


int main(){

	using namespace std;



	// Local virtual imp
			typedef const portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
		auto f = load_module_function<CFun>("performance_timings_dll","CreateVirtualInterface");

		VirtualInterface* p = (VirtualInterface*) f();

	cout << "Virtual Timing " << timingTest(*p) << endl;
	
	// Imp

	use_interface<TestInterface1> t1(jrb_interface::create<TestInterface1>("performance_timings_dll","CreateLocalImpInterface"));
	cout << "Regular Timing " << timingTest(t1) << endl;

	//  Fast Imp
	use_interface<TestInterface1> t2(jrb_interface::create<TestInterface1>("performance_timings_dll","CreateLocalFastImpInterface"));
	cout << "Fast Timing " << timingTest(t2) << endl;



}