#include "jrb_interface/jrb_interface.hpp"
#include "jrb_interface/cr_string.hpp"
#include <chrono>
#include <iostream>
using namespace jrb_interface;


template<bool bImp>
struct TestInterface1:public jrb_interface::define_interface<bImp,5>{
	cross_function<TestInterface1,0,int()> f1;
	cross_function<TestInterface1,1,int(int)> f2;
	cross_function<TestInterface1,2,std::string()>f3;
	cross_function<TestInterface1,3,void(jrb_interface::cr_string)>f4;
	cross_function<TestInterface1,4,void(std::vector<std::string>)>f5;


	template<class T>
	TestInterface1(T t):base_t(t),f1(t),f2(t),f3(t),f4(t),f5(t){
	}


};
struct LocalImp:public implement_interface<TestInterface1>{
	std::string str;
	LocalImp(){
		f1 = [](){return 10;};
		f2 = [](int k){return 10 + k;};
		f3 = [this]()->std::string{return "Hello World from DLL " + str + "\n";};
		f4 = [this](jrb_interface::cr_string s){str +=" "; str = s;};
		f5 = [this](std::vector<std::string> v){
			str = " ";
			for(const auto& a:v){
				str += " ";
				str += a;
			}
		};
	}

};

struct VirtualInterface{
	virtual int f1() = 0;
	virtual int f2(int) = 0;
	virtual std::string f3() = 0;
	virtual void f4(const std::string&) = 0;
	virtual void f5(const std::vector<std::string>) = 0;
};

struct LocalVirtualImp:public VirtualInterface{
	std::string str;
	__declspec(noinline) virtual int f1(){return 10;}
	virtual int f2(int k){return 10 + k;}
	virtual std::string f3(){return "Hello World from DLL " + str + "\n";}
	virtual void f4(const std::string& s){str =" "; str += s;}
	virtual void f5(const std::vector<std::string> v){
		str = " ";
		for(const auto& a:v){
			str += " ";
			str += a;
		}
	};


};


struct LocalFastImp{

	std::string str;
	int f1(){return 10;}
	int f2(int k){return 10 + k;}
	std::string f3(){return "Hello World from DLL " + str + "\n";}
	void f4(jrb_interface::cr_string s){str +=" "; str = s;}
	void f5(const std::vector<std::string> v){
		str = " ";
		for(const auto& a:v){
			str += " ";
			str += a;
		}
	}

	implement_interface<TestInterface1> imp;

	LocalFastImp(){
		imp.f1.set_fast<LocalFastImp,&LocalFastImp::f1>(this);
		imp.f2.set_fast<LocalFastImp,&LocalFastImp::f2>(this);
		imp.f3.set_fast<LocalFastImp,&LocalFastImp::f3>(this);
		imp.f4.set_fast<LocalFastImp,&LocalFastImp::f4>(this);
		imp.f5.set_fast<LocalFastImp,&LocalFastImp::f5>(this);

	}


};


template<class T>
double timingTest(T& t){

	auto begin = std::chrono::steady_clock::now();
	const int iterations = 10000000;
	for(int i = 0; i < iterations;i++){
		t.f1();
		//t.f2(6) ;
		//t.f4(std::string("String1"));
		//std::vector<std::string> v;
		//v.push_back("v1");
		//v.push_back("v2");
		//v.push_back("v3");

		//t.f5(v);

		//t.f3();

	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - begin;
	return std::chrono::duration<double,std::micro>(diff).count()/iterations;
}


double timingTest(VirtualInterface& t){

	auto begin = std::chrono::steady_clock::now();
	const int iterations = 10000000;
	for(int i = 0; i < iterations;i++){
		t.f1();
		//t.f2(6) ;
		//t.f4(std::string("String1"));
		//std::vector<std::string> v;
		//v.push_back("v1");
		//v.push_back("v2");
		//v.push_back("v3");

		//t.f5(v);

		//t.f3();

	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - begin;
	return std::chrono::duration<double,std::micro>(diff).count()/iterations;
}
int main(){
	auto tx =  [](std::string s)->std::vector<std::string>{
			std::vector<std::string> ret;

			auto wbegin = s.begin();
			auto wend = wbegin;
			for(;wbegin!= s.end();wend = std::find(wend,s.end(),' ')){
				if(wbegin==wend)continue;
				ret.push_back(std::string(wbegin,wend));
				wbegin = std::find_if(wend,s.end(),[](char c){return c != ' ';});
				wend = wbegin;

			}
				return ret;

		};

	tx("This is a test");

	using namespace std;

	std::string a = "test";


	// Local virtual imp
	LocalVirtualImp lvi;
	VirtualInterface* p = 0;
	p = &lvi;
	cout << "Local Virtual Timing " << timingTest(*p) << endl;
	
	// Local Imp
	LocalImp li;
	use_interface<TestInterface1> t1 = li;
	cout << "Local Timing " << timingTest(t1) << endl;

	// Local Fast Imp
	LocalFastImp lif;
	use_interface<TestInterface1> t2 = lif.imp;
	cout << "Local Fast Timing " << timingTest(t2) << endl;



}