#include "performance_timings.h"

using namespace jrb_interface;

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

		f6 = [](){};
	}

};

struct LocalVirtualImp:public VirtualInterface{
	std::string str;
	virtual int f1(){return 10;}
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
	virtual void f6(){}


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

	void f6(){}

	implement_interface<TestInterface1> imp;

	LocalFastImp(){
		imp.f1.set_fast<LocalFastImp,&LocalFastImp::f1>(this);
		imp.f2.set_fast<LocalFastImp,&LocalFastImp::f2>(this);
		imp.f3.set_fast<LocalFastImp,&LocalFastImp::f3>(this);
		imp.f4.set_fast<LocalFastImp,&LocalFastImp::f4>(this);
		imp.f5.set_fast<LocalFastImp,&LocalFastImp::f5>(this);
		imp.f6.set_fast<LocalFastImp,&LocalFastImp::f6>(this);

	}


};


extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateLocalImpInterface(){
	static LocalImp d_;
	return d_.get_portable_base();
}
}

extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateLocalFastImpInterface(){
	static LocalFastImp d_;
	return d_.imp.get_portable_base();
}
}


extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateVirtualInterface(){
	static LocalVirtualImp d_;
	return &d_;
}
}