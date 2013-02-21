#include "jrb_interface/jrb_interface.hpp"
#include "jrb_interface/cr_string.hpp"

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

struct VirtualInterface:public portable_base{
	virtual int f1() = 0;
	virtual int f2(int) = 0;
	virtual std::string f3() = 0;
	virtual void f4(const std::string&) = 0;
	virtual void f5(const std::vector<std::string>) = 0;
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