//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "unit_test_interface.h"


struct ImplementTestComponent
    :public cppcomponents::implement_runtime_class<ImplementTestComponent,TestComponent_t>{

        ImplementTestComponent(){}

        std::string Test(){return "Hello Components";}


};


CPPCOMPONENTS_REGISTER(ImplementTestComponent)

struct ImplementTestComponentWithConstructor
    :public cppcomponents::implement_runtime_class<ImplementTestComponentWithConstructor,TestComponentWithConstructor_t>{

        std::string s_;
        ImplementTestComponentWithConstructor():s_("Hello Components"){}

        ImplementTestComponentWithConstructor(std::string s):s_(std::move(s)){}

        std::string Test(){return s_;}


};

CPPCOMPONENTS_REGISTER(ImplementTestComponentWithConstructor)


struct ImplementTestComponentWithStatic

    :public cppcomponents::implement_runtime_class<ImplementTestComponentWithStatic,TestComponentWithStatic_t>{

       static std::string GetStaticString(){return "Hello from static method";}
       static std::string GetStaticString2(cppcomponents::string_ref s){return "Hello " + s.to_string();}

       static cppcomponents::use<ComponentInterface> GetTestComponent(){return 
           ImplementTestComponentWithConstructor::create("Returned component").QueryInterface<ComponentInterface>();}

        std::string s_;
        ImplementTestComponentWithStatic():s_("Hello Components"){}

        ImplementTestComponentWithStatic(std::string s):s_(std::move(s)){}

        std::string Test(){return s_;}
};


CPPCOMPONENTS_REGISTER(ImplementTestComponentWithStatic)




struct ImplementTestComponentWithMultipleStatic
	:public cppcomponents::implement_runtime_class<ImplementTestComponentWithMultipleStatic, TestComponentWithMultipleStatic_t>{

		static std::string GetStaticString(){ return "Hello from static method"; }
		static std::string GetStaticString2(cppcomponents::string_ref s){ return "Hello " + s.to_string(); }

		static std::string StaticInterface4_GetStaticStringOtherInterface(){ return "Hello from second static interface"; }
       static cppcomponents::use<ComponentInterface> GetTestComponent(){return 
           ImplementTestComponentWithConstructor::create("Returned component").QueryInterface<ComponentInterface>();}

		std::string s_;
		ImplementTestComponentWithMultipleStatic() : s_("Hello Components"){}

		ImplementTestComponentWithMultipleStatic(std::string s) : s_(std::move(s)){}
};
CPPCOMPONENTS_REGISTER(ImplementTestComponentWithMultipleStatic)


struct ImplementTestComponentWithInheritance
	: public cppcomponents::implement_runtime_class<ImplementTestComponentWithInheritance,
	TestComponentWithInheritedInterfaces_t>
{

	std::string HelloFromInherited(){ return "Hello from Inherited"; }

	std::string Test(){ return "Test"; }

	static std::string GetStaticString(){ return "StaticString"; }
	static     std::string GetStaticString2(cppcomponents::string_ref )	{ 
		return "StaticString2";
	}

	static cppcomponents::use<ComponentInterface> GetTestComponent(){ return ImplementTestComponent::create().QueryInterface<ComponentInterface>(); }

	ImplementTestComponentWithInheritance(){}
	
};
CPPCOMPONENTS_REGISTER(ImplementTestComponentWithInheritance)


struct ImplementTestComponentWithMultipleInterfacesHelper {
	std::string ComponentInterface_Test() { return "ComponentInterface_Test"; }

};



struct ImplementPersonHelper {

	std::string Name_;
	int Age_;

	std::string GetName(){ return Name_; }
	void SetName(std::string n){ Name_ = n; }

	int GetAge(){ return Age_; }
	void SetAge(int a){ Age_ = a; }
	ImplementPersonHelper(std::string n, int a) : Name_(n), Age_(a){}


};

struct ImplementPerson
	: public
	ImplementPersonHelper, cppcomponents::implement_runtime_class<ImplementPerson, Person_t>
{

	using ImplementPersonHelper::GetName;
	std::string GetName(){ return Name_; }
	void SetName(std::string n){ Name_ = n; }

	int GetAge(){ return Age_; }
	void SetAge(int a){ Age_ = a; }
	ImplementPerson() : ImplementPersonHelper("John", 21){


	}
};


CPPCOMPONENTS_REGISTER(ImplementPerson)


struct ImplementPersonWithEvent
	: cppcomponents::implement_runtime_class<ImplementPersonWithEvent, PersonWithEvent_t> ,
	public ImplementPersonHelper 
	
{
	typedef cppcomponents::implement_runtime_class<ImplementPersonWithEvent, PersonWithEvent_t> base_t;

	ImplementPersonWithEvent() :  ImplementPersonHelper("John",21){

	}

	std::int64_t add_PersonNameChanged(cppcomponents::use<IPersonWithEvent::PersonNameChangeHandler> d){
		return 0;
	}

	void remove_PersonNameChanged(std::int64_t i){
	}
	void SetName(std::string n){ 
		Name_ = n; 
	}
};

CPPCOMPONENTS_REGISTER(ImplementPersonWithEvent)


struct ImplementTestWString : public cppcomponents::implement_runtime_class<ImplementTestWString, TestWString_t>{

	std::wstring Concat(std::wstring a, cppcomponents::wstring_ref b){
		a.append(b.begin(), b.end());
		return a;
	}


	ImplementTestWString(){}


};
CPPCOMPONENTS_REGISTER(ImplementTestWString)


struct ImplementTupleTest : public cppcomponents::implement_runtime_class<ImplementTupleTest, TestTuple_t>{

	std::tuple<std::string, std::string> Get2(){
		auto t = std::make_tuple(std::string("Hello World"), std::string("Hello World"));
		return t;
	}
	std::tuple<int, std::string, double> Get3(){
		auto t = std::make_tuple(1, std::string("Hello World"), 2.5);
		return t;
	}
	std::tuple<int, std::string, double, char> Get4(){
		auto t = std::make_tuple(1, std::string("Hello World"), 2.5, 'a');
		return t;
	}
	std::tuple<int, std::string, double, char, cppcomponents::use<ITestTuple>> Get5(){
		auto t = std::make_tuple(1, std::string("Hello World"), 2.5, 'a', QueryInterface<ITestTuple>());
		return t;
	}

	std::tuple <std::string> Get1(){
		auto t = std::make_tuple(std::string("Hello World"));
		return t;
	}

	ImplementTupleTest(){}
};

CPPCOMPONENTS_REGISTER(ImplementTupleTest)


struct ImplementTestPureStatic
	: public cppcomponents::implement_runtime_class<ImplementTestPureStatic, TestPureStatic_t>{



		static std::string GetStaticStringOtherInterface(){ return "Hello from second static interface"; }
private:
		ImplementTestPureStatic(){}

};
CPPCOMPONENTS_REGISTER(ImplementTestPureStatic)


struct ImplementTestFuture
	: public cppcomponents::implement_runtime_class<ImplementTestFuture, TestFuture_t>
{


	cppcomponents::use < cppcomponents::IFuture<std::string>> GetFutureString(){

		return cppcomponents::async(executor_, [](){return std::string("Hello Futures"); });

	}
	cppcomponents::use < cppcomponents::IFuture<std::string>> GetFutureWithException(){
		
		return cppcomponents::async(executor_, []()->std::string{
			throw cppcomponents::error_invalid_arg();
		});

	}
	cppcomponents::use< cppcomponents::IFuture<int>> GetImmediateFuture(){
		return cppcomponents::make_ready_future(5);
	}

	cppcomponents::use < cppcomponents::IFuture < cppcomponents::use < cppcomponents::IFuture < int >> >> GetWrappedFuture(){
		auto e = executor_;
		return cppcomponents::async(executor_,[e](){
			return cppcomponents::async(e, [](){
				return 42;
			});
		});
	}

	cppcomponents::use<cppcomponents::IExecutor> executor_;
	ImplementTestFuture():executor_{launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>()}{
		
	}

};
CPPCOMPONENTS_REGISTER(ImplementTestFuture)


struct ImplementTestChrono : cppcomponents::implement_runtime_class<ImplementTestChrono, TestChrono_t>{
	typedef std::chrono::system_clock::time_point time_point;
	typedef std::chrono::system_clock::duration duration;
	time_point AddTime(time_point p, duration d){
		return p + d;
	}

	duration SubtractTime(time_point a, time_point b){
		return a - b;
	}

	ImplementTestChrono(){}

};
CPPCOMPONENTS_REGISTER(ImplementTestChrono)


struct ImplementTestTemplatedConstructor : cppcomponents::implement_runtime_class<ImplementTestTemplatedConstructor, TestTemplatedConstructor_t>
{
	typedef cppcomponents::delegate < int()> IntDelegate;

	cppcomponents::use<IntDelegate> d_;
	ImplementTestTemplatedConstructor(cppcomponents::use<IntDelegate> d):d_{d}{};

	int CallDelegate(){
		return d_();
	}


};
CPPCOMPONENTS_REGISTER(ImplementTestTemplatedConstructor)


struct ITestInternalClassDll :cppcomponents::define_interface<cppcomponents::uuid<0x0ce03a69, 0xdeb9, 0x40fd, 0x94be, 0x2897dcb1fe03>>
{
	std::string Test();

	CPPCOMPONENTS_CONSTRUCT(ITestInternalClassDll, Test)
};
inline const char* TestInternalClassDllId(){ return "!TestInternalClassDll"; }
typedef cppcomponents::runtime_class<TestInternalClassDllId, cppcomponents::object_interfaces<ITestInternalClassDll>> TestInternalClassDll_t;
typedef cppcomponents::use_runtime_class<TestInternalClassDll_t> TestInternalClassDll;

struct ImplementTestInternalClassDll :cppcomponents::implement_runtime_class<ImplementTestInternalClassDll, TestInternalClassDll_t>
{
	ImplementTestInternalClassDll(){}
	std::string Test(){ return "Test Dll"; }
};
CPPCOMPONENTS_REGISTER(ImplementTestInternalClassDll)

struct ImplementTestInternalClassTester :cppcomponents::implement_runtime_class<ImplementTestInternalClassTester, TestInternalClassTester_t>
{
	ImplementTestInternalClassTester(){}
	std::string TestInteralExe(){
		TestInternalClass t;
		return t.Test();
	}
	std::string TestInteralDll(){
		TestInternalClassDll t;
		return t.Test();
	}
};

CPPCOMPONENTS_REGISTER(ImplementTestInternalClassTester)

struct ImplementTestConstReturn :cppcomponents::implement_runtime_class<ImplementTestConstReturn, TestConstReturn_t>
{
  const std::string Hello(){ return "Hello"; }
  const std::pair<const std::string, const int> Pair(){ return{ { "Hello" }, 1 }; }
  const std::tuple<const std::string> Tuple1(){ return std::make_tuple(std::string("Hello") ); }
  const std::tuple<const std::string, const int> Tuple2(){ return std::make_tuple( std::string("Hello"), 1 ); }
  const std::tuple<const std::string, const int, const double> Tuple3(){ return std::make_tuple(std::string("Hello"), 1, 0.0); }


};

CPPCOMPONENTS_REGISTER(ImplementTestConstReturn)



struct ITestFunction :cppcomponents::define_interface<cppcomponents::uuid<0x0ae1dc9a, 0x78b6, 0x4e82, 0x87d5, 0x5663d4521fa3>>
{
	cppcomponents::use<cppcomponents::delegate<std::string()>> GetStringFunction();
	void SetStringFunction(cppcomponents::use<cppcomponents::delegate<std::string()>>);

	cppcomponents::function<std::string(std::string)> GetStringStringFunction();
	void SetStringStringFunction(cppcomponents::function<std::string(std::string)>);

	void SetStringStringFunctionFromIUnknown(cppcomponents::use<cppcomponents::InterfaceUnknown>);

	CPPCOMPONENTS_CONSTRUCT(ITestFunction, GetStringFunction, SetStringFunction, GetStringStringFunction, SetStringStringFunction,SetStringStringFunctionFromIUnknown)
};

inline const char* TestFunctionId(){ return "unit_test_dll!TestFunctionId"; }
typedef cppcomponents::runtime_class<TestFunctionId, cppcomponents::object_interfaces<ITestFunction>> TestFunction_t;
typedef cppcomponents::use_runtime_class<TestFunction_t> TestFunction;

struct ImplementTestFunction :cppcomponents::implement_runtime_class<ImplementTestFunction, TestFunction_t>{

	cppcomponents::use<cppcomponents::delegate<std::string()>> string_func_;
	cppcomponents::function<std::string(std::string)> string_string_func_;


	ImplementTestFunction(){
		string_func_ = cppcomponents::make_delegate<cppcomponents::delegate<std::string()>>([](){return std::string("Hello World"); });
		string_string_func_ = [](std::string name){return "Hello " + name; };
	}

	cppcomponents::use<cppcomponents::delegate<std::string()>> GetStringFunction(){
		return string_func_;
	}
	void SetStringFunction(cppcomponents::use<cppcomponents::delegate<std::string()>> f){
		string_func_ = f;
	}

	cppcomponents::function<std::string(std::string)> GetStringStringFunction(){
		return string_string_func_;
	}
	void SetStringStringFunction(cppcomponents::function<std::string(std::string)> f){
		string_string_func_ = f;
	}

	void SetStringStringFunctionFromIUnknown(cppcomponents::use<cppcomponents::InterfaceUnknown> i){
		auto f = i.QueryInterface<cppcomponents::delegate<std::string(std::string)>>();
		string_string_func_ = f;
	}
};

CPPCOMPONENTS_REGISTER(ImplementTestFunction)



CPPCOMPONENTS_DEFINE_FACTORY()

