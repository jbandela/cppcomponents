//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "unit_test_interface.h"

extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base*  CROSS_CALL_CALLING_CONVENTION CreateTestInterface();
}


struct ImplementIuknownDerivedInterface:public cross_compiler_interface::implement_unknown_interfaces<ImplementIuknownDerivedInterface,
	IUnknownDerivedInterface,IUnknownDerivedInterface2Derived>
{
	

	std::string say_hello(){
			return "Hello from IuknownDerivedInterface2";
		}


	ImplementIuknownDerivedInterface(){
		auto imp = get_implementation<IUnknownDerivedInterface>();
		imp->hello_from_iuknown_derived = []()->std::string{
			return "Hello from IuknownDerivedInterface";
		};
		auto imp2 = get_implementation<IUnknownDerivedInterface2Derived>();
		imp2->hello_from_iuknown_derived2.set_mem_fn<ImplementIuknownDerivedInterface,&ImplementIuknownDerivedInterface::say_hello>(this);

		imp2->hello_from_derived = []()->std::string{
			return "Hello from derived";
		};

		imp2->get_derived = [this,imp]()->cross_compiler_interface::use_unknown<IUnknownDerivedInterface>{
            auto ret = ImplementIuknownDerivedInterface::create().QueryInterface<IUnknownDerivedInterface>();
            return ret;
		};

		imp2->get_string = [](cross_compiler_interface::use_unknown<IUnknownDerivedInterface> i){
			return i.hello_from_iuknown_derived();
		};

	}

};

struct ImplementIuknownDerivedInterfaceOnly:public cross_compiler_interface::implement_unknown_interfaces<ImplementIuknownDerivedInterfaceOnly,IUnknownDerivedInterface>{

	ImplementIuknownDerivedInterfaceOnly(){
		auto imp = get_implementation<IUnknownDerivedInterface>();
		imp->hello_from_iuknown_derived = [](){
			return std::string("Hello from ImplementIuknownDerivedInterfaceOnly");
		};

	}
};

struct TestImplementation:public cross_compiler_interface::implement_interface<TestInterface>{

	cross_compiler_interface::implement_interface<IGetName> ign_imp;

	std::string str_;

	TestImplementation(){
		auto& t = *this;
		t.double_referenced_int = [](cross_compiler_interface::out<int> i){ i.set(2);};
		t.plus_5 = [](int i){return i+5;};
		t.times_2point5 = [](double d){return d*2.5;};
		t.hello_from_base = []()->std::string{return "Hello from Base";};
		t.say_hello = [](std::string name)->std::string{return "Hello " + name;};
		t.use_at_out_of_range = [](std::string s){s.at(s.size());};
		t.count_characters = [](std::string s){return s.length();};
		t.split_into_words = [](std::string s)->std::vector<std::string>{
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

		t.say_hello2 = [](cross_compiler_interface::use_interface<IGetName> ign)->std::string{
			return "Hello " + ign.get_name();
		};

		t.get_string_at = [](std::vector<std::string> v, int pos)->std::pair<int,std::string>{
			std::pair<int,std::string> ret;
			ret.first = pos;
			ret.second = v.at(pos);
			return ret;
		};

		ign_imp.get_name = []()->std::string{return "Hello from returned interface";};
		t.get_igetname = [this]()->use_interface<IGetName>{return ign_imp.get_use_interface();};

		t.get_name_from_runtime_parent = []()->std::string{return "TestImplementation";};
		t.custom_with_runtime_parent =[](int i){return i+10;};

		t.get_out_string = [](cross_compiler_interface::out<std::string> s){
			s.set("out_string");
		};

		t.append_string = [this](cppcomponents::string_ref s){

			str_.append(s.begin(),s.end());
		};

		t.get_string = [this]()->cppcomponents::string_ref{
			return str_;
		};

		t.append_5_to_vector = [](std::vector<std::uint32_t> v)->std::vector<std::uint32_t>{
			v.push_back(5);
			return v;
		};

		t.append_hello_to_vector = [](std::vector<std::string> v)->std::vector<std::string>{
			v.push_back("hello");
			return v;
		};

        t.test_u16_string = [](std::u16string s)->std::u16string{
           char16_t a('a');
           return s + a;

        };      
        t.test_u32_string = [](std::u32string s)->std::u32string{
           char32_t a('a');
           return s + a;

        };
	}

};


struct TestImplementationMemFn {
	 cross_compiler_interface::implement_interface<TestInterface> t;

	cross_compiler_interface::implement_interface<IGetName> ign_imp;

	std::string ign_get_name(){return "Hello from returned interface";} 


	 void double_referenced_int(cross_compiler_interface::out<int> i){ i.set(2);}
	 int plus_5(int i){return i+5;}
	 double times_2point5(double d){return d*2.5;}
	 std::string hello_from_base(){return "Hello from Base";}
	 std::string say_hello(std::string name){return "Hello " + name;};
	 void use_at_out_of_range(std::string s){s.at(s.size());}

	 std::size_t count_characters(std::string s){return s.length();}

	 std::vector<std::string> split_into_words(std::string s){
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

		}

	 std::string say_hello2(cross_compiler_interface::use_interface<IGetName> ign){
			return "Hello " + ign.get_name();
		}

	 std::pair<int,std::string> get_string_at(std::vector<std::string> v, int pos){
			std::pair<int,std::string> ret;
			ret.first = pos;
			ret.second = v.at(pos);
			return ret;
		}

	 use_interface<IGetName> get_igetname(){
		 return ign_imp.get_use_interface();

	 }

	 void get_out_string(cross_compiler_interface::out<std::string> s){
		s.set("out_string"); 
	 }
	TestImplementationMemFn(){
		t.set_runtime_parent(cross_compiler_interface::use_interface<TestInterface>(cross_compiler_interface::reinterpret_portable_base<TestInterface>(CreateTestInterface())));

		t.double_referenced_int.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::double_referenced_int>(this);

		t.plus_5.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::plus_5>(this);

		t.times_2point5.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::times_2point5>(this);
		t.hello_from_base.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::hello_from_base>(this);
		t.say_hello.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::say_hello>(this);
		t.use_at_out_of_range.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::use_at_out_of_range>(this);
		t.count_characters.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::count_characters>(this);
		t.split_into_words.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::split_into_words>(this);

		t.say_hello2.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::say_hello2>(this);

		t.get_string_at.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::get_string_at>(this);


		ign_imp.get_name.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::ign_get_name>(this);
		t.get_igetname.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::get_igetname>(this);
		t.get_out_string.set_mem_fn<TestImplementationMemFn,&TestImplementationMemFn::get_out_string>(this);
	}

};


struct TestLayoutImplementation:public cross_compiler_interface::implement_unknown_interfaces<TestLayoutImplementation,
	ITestLayout,ITestLayout2>
{
	int n_;

	TestLayoutImplementation():n_(0){
		auto imp = get_implementation<ITestLayout>();
		imp->set_int = [this](std::int32_t n){
			n_ = n;
		};
		imp->add_2_5_to_int = [this](){
			return 2.5 + double(n_);
		};

		auto imp2 = get_implementation<ITestLayout2>();

		imp2->get_int = [this](){return n_;};
	}

};

extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestInterface(){
	static TestImplementation  t_;

	return t_.get_portable_base();
}
}

extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestMemFnInterface(){
	static TestImplementationMemFn  t_;

	return t_.t.get_portable_base();
}
}
extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateIunknownDerivedInterface(){
	ImplementIuknownDerivedInterface* derived = new ImplementIuknownDerivedInterface;
			auto& imp = *derived->get_implementation<IUnknownDerivedInterface>();

	return imp.get_portable_base();
}
}

extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateIunknownDerivedInterfaceOnly(){
	auto ret_int = ImplementIuknownDerivedInterfaceOnly::create();

	auto ret = ret_int.get_portable_base();

	ret_int.reset_portable_base();


	return ret;
}
}

extern "C"{

	CROSS_CALL_EXPORT_FUNCTION cross_compiler_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestLayout(){
	auto ret_int = TestLayoutImplementation::create();

	auto ret = ret_int.get_portable_base();

	ret_int.reset_portable_base();


	return ret;
}
}
extern "C"{

	CROSS_CALL_EXPORT_FUNCTION std::size_t CROSS_CALL_CALLING_CONVENTION GetObjectCount(){
	return cross_compiler_interface::object_counter::get().get_count();
}
}



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

struct ImplementTestComponentWithForcedPrefixInterfaces 
	: public cppcomponents::implement_runtime_class < ImplementTestComponentWithForcedPrefixInterfaces, TestComponentWithForcedPrefixInterfaces_t>{
		static std::string TestComponentWithForcedPrefixInterfacesStaticInterface_StaticMethod(){ return "StaticMethod"; }

		std::string InterfaceTestComponentWithForcedPrefixInterfaces_Test(){ return "Test"; }

		ImplementTestComponentWithForcedPrefixInterfaces(){}

};
CPPCOMPONENTS_REGISTER(ImplementTestComponentWithForcedPrefixInterfaces)

struct ImplementTestComponentWithMultipleInterfacesHelper {
	std::string ComponentInterface_Test() { return "ComponentInterface_Test"; }

};



struct ImplementTestComponentWithMultipleInterfaces
	: public cppcomponents::implement_runtime_class < ImplementTestComponentWithMultipleInterfaces, TestComponentWithMultipleInterfaces_t>,
	ImplementTestComponentWithMultipleInterfacesHelper
{

	std::string InterfaceTestComponentWithForcedPrefixInterfaces_Test(){ return "InterfaceTestComponentWithForcedPrefixInterfaces_Test"; }
	ImplementTestComponentWithMultipleInterfaces(){}
};

CPPCOMPONENTS_REGISTER(ImplementTestComponentWithMultipleInterfaces)


struct ImplementTestComponentWithRuntimeInheritance : public cppcomponents::implement_runtime_class < ImplementTestComponentWithRuntimeInheritance, TestComponentWithRuntimeInheritance_t>{

	typedef cppcomponents::implement_runtime_class < ImplementTestComponentWithRuntimeInheritance, TestComponentWithRuntimeInheritance_t> base_t;
	ImplementTestComponentWithRuntimeInheritance() : base_t(TestComponentWithInheritedInterfaces{}){

		// Set our function as the function implementing Hello From Inherited
		get_implementation<InterfaceTestComponentWithInheritedInterfaces>()->HelloFromInherited.set_mem_fn <
			ImplementTestComponentWithRuntimeInheritance, &ImplementTestComponentWithRuntimeInheritance::HelloFromInherited>(this);
	}

	std::string HelloFromInherited(){ return "I overrode this"; }

	


};
CPPCOMPONENTS_REGISTER(ImplementTestComponentWithRuntimeInheritance)

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

	ImplementPerson() : ImplementPersonHelper("John", 21){


	}
};


CPPCOMPONENTS_REGISTER(ImplementPerson)


struct ImplementPersonWithEvent
	: cppcomponents::implement_runtime_class<ImplementPersonWithEvent, PersonWithEvent_t> ,
	public ImplementPersonHelper 
	
{
	typedef cppcomponents::implement_runtime_class<ImplementPersonWithEvent, PersonWithEvent_t> base_t;
	cppcomponents::event_implementation<IPersonWithEvent::PersonNameChangeHandler> h_;

	ImplementPersonWithEvent() :  ImplementPersonHelper("John",21){

	}

	std::int64_t add_PersonNameChanged(cppcomponents::use<IPersonWithEvent::PersonNameChangeHandler> d){
		return h_.add(d);
	}

	void remove_PersonNameChanged(std::int64_t i){
		h_.remove(i);
	}
	void SetName(std::string n){ 
		Name_ = n; 
		h_.raise(n);
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
inline std::string TestInternalClassDllId(){ return "!TestInternalClassDll"; }
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

	CPPCOMPONENTS_CONSTRUCT(ITestFunction, GetStringFunction, SetStringFunction, GetStringStringFunction, SetStringStringFunction)
};

inline std::string TestFunctionId(){ return "unit_test_dll!TestFunctionId"; }
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

};

CPPCOMPONENTS_REGISTER(ImplementTestFunction)



CPPCOMPONENTS_DEFINE_FACTORY()

