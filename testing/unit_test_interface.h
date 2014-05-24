#include "../cppcomponents/cppcomponents.hpp"
 

struct ComponentInterface : public cppcomponents::define_interface < cppcomponents::uuid<0xa4311094, 0xe1cc, 0x4d6a, 0x8c2e, 0xa1652d3843f8>>{


	std::string Test();

	CPPCOMPONENTS_CONSTRUCT(ComponentInterface, Test)

};

inline const char* mycomponentname(){return "Test.Component";}
typedef cppcomponents::runtime_class<mycomponentname,cppcomponents::object_interfaces<ComponentInterface> > TestComponent_t;

typedef cppcomponents::use_runtime_class<TestComponent_t> TestComponent;


struct FactoryInterface2 : public cppcomponents::define_interface < cppcomponents::uuid<0xFC9A22B0, 0x04FE, 0x438A, 0xB4E9, 0xB340D81F4337>>{


    typedef cppcomponents::use<cppcomponents::InterfaceUnknown> Unknown;

    Unknown Create();
    Unknown CreateWithString(std::string);

    CPPCOMPONENTS_CONSTRUCT(FactoryInterface2,Create,CreateWithString)


};


inline const char* componentname2(){return "Test.Component.2";}

typedef cppcomponents::runtime_class<componentname2,cppcomponents::object_interfaces<ComponentInterface>,
	cppcomponents::factory_interface<FactoryInterface2> >
    TestComponentWithConstructor_t;

typedef cppcomponents::use_runtime_class<TestComponentWithConstructor_t> TestComponentWithConstructor;

inline const char* componentname3(){return "unit_test_dll!Test.Component.3";}
struct StaticInterface3 : public cppcomponents::define_interface < cppcomponents::uuid<0xF2CE950A, 0xB826, 0x4354, 0xA07C, 0xD17431323C26>> {


    std::string GetStaticString();
    std::string GetStaticString2(cppcomponents::string_ref s);

   
   cppcomponents::use<ComponentInterface> GetTestComponent();
     

    CPPCOMPONENTS_CONSTRUCT(StaticInterface3,GetStaticString,GetTestComponent,GetStaticString2)


	CPPCOMPONENTS_STATIC_INTERFACE_EXTRAS(StaticInterface3){
		static std::string OtherGetStaticString(){
			return Class::GetStaticString();
		}
	};
};




typedef cppcomponents::runtime_class<componentname3,cppcomponents::object_interfaces<ComponentInterface>,
	cppcomponents::factory_interface<FactoryInterface2>,cppcomponents::static_interfaces<StaticInterface3> >
    TestComponentWithStatic_t;

typedef cppcomponents::use_runtime_class<TestComponentWithStatic_t> TestComponentWithStatic;

struct InterfaceTestComponentWithMultipleStatic: public cppcomponents::define_interface<cppcomponents::uuid<0x93CC8F89, 0x98C3, 0x448C, 0x840A, 0x33A49756912F>>{
		




	CPPCOMPONENTS_CONSTRUCT_NO_METHODS(InterfaceTestComponentWithMultipleStatic)


};

struct StaticInterface4 : public cppcomponents::define_interface < cppcomponents::uuid<0x244D5533, 0x28DD, 0x46C5, 0x9008, 0x44A73E5495A4>>{




	std::string GetStaticStringOtherInterface();

	CPPCOMPONENTS_CONSTRUCT(StaticInterface4,GetStaticStringOtherInterface )


};
inline const char* TestComponentWithMultipleStaticComponentName(){ return "unit_test_dll!TestComponentWithMultipleStatic"; }

typedef cppcomponents::runtime_class<TestComponentWithMultipleStaticComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithMultipleStatic>,
	cppcomponents::factory_interface<FactoryInterface2>, cppcomponents::static_interfaces<StaticInterface3,StaticInterface4> >
	TestComponentWithMultipleStatic_t;
typedef cppcomponents::use_runtime_class<TestComponentWithMultipleStatic_t> TestComponentWithMultipleStatic;

struct InterfaceTestComponentWithInheritedInterfaces 
	: public cppcomponents::define_interface<cppcomponents::uuid<0x7F66263B, 0x18A4, 0x450D, 0xA4DB, 0x5EEC961BCDB4>
	,ComponentInterface>{


	std::string HelloFromInherited(){ return "Hello from Inherited"; }


	CPPCOMPONENTS_CONSTRUCT(InterfaceTestComponentWithInheritedInterfaces,HelloFromInherited )


};
struct TestComponentWithInheritedInterfacesFactoryInterface
	: public cppcomponents::define_interface<cppcomponents::uuid<0xA9739B2C, 0x3B9B, 0x4DE4, 0xBC8B, 0xC48947C86986>>{



	cppcomponents::use<cppcomponents::InterfaceUnknown> Create();

	CPPCOMPONENTS_CONSTRUCT(TestComponentWithInheritedInterfacesFactoryInterface,Create )


};
struct TestComponentWithInheritedInterfacesStaticInterface
	: public cppcomponents::define_interface<cppcomponents::uuid<0x8C0281B6, 0x4A4D, 0x4700, 0x92E0, 0x6859545DEE90>
	,StaticInterface3>{





	CPPCOMPONENTS_CONSTRUCT_NO_METHODS(TestComponentWithInheritedInterfacesStaticInterface)


};
inline const char* TestComponentWithInheritedInterfacesComponentName(){ return "unit_test_dll!TestComponentWithInheritedInterfaces"; }

typedef cppcomponents::runtime_class<TestComponentWithInheritedInterfacesComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithInheritedInterfaces>,
	cppcomponents::factory_interface<TestComponentWithInheritedInterfacesFactoryInterface>,
	cppcomponents::static_interfaces<TestComponentWithInheritedInterfacesStaticInterface> >
	TestComponentWithInheritedInterfaces_t;
typedef cppcomponents::use_runtime_class<TestComponentWithInheritedInterfaces_t> TestComponentWithInheritedInterfaces;


struct TestComponentWithForcedPrefixInterfacesStaticInterface;
struct InterfaceTestComponentWithForcedPrefixInterfaces;
namespace cppcomponents{

	template<>
	struct allow_interface_to_map_no_prefix<TestComponentWithForcedPrefixInterfacesStaticInterface>{
		enum{ value = false };
	};

	template<>
	struct allow_interface_to_map_no_prefix<InterfaceTestComponentWithForcedPrefixInterfaces>{
		enum{ value = false };
	};
}
struct InterfaceTestComponentWithForcedPrefixInterfaces
	: public cppcomponents::define_interface<cppcomponents::uuid<0x4F183EC6, 0x2EB1, 0x495A, 0x9969, 0x99721D70DDFA>>{

	std::string Test();



	CPPCOMPONENTS_CONSTRUCT(InterfaceTestComponentWithForcedPrefixInterfaces,Test )


};


struct TestComponentWithForcedPrefixInterfacesStaticInterface
	: public cppcomponents::define_interface<cppcomponents::uuid<0xD0A3968E, 0xFD14, 0x4695, 0x8EB2, 0xC165F590430A>>{

	std::string StaticMethod(){ return "StaticMethod"; }



	CPPCOMPONENTS_CONSTRUCT(TestComponentWithForcedPrefixInterfacesStaticInterface,StaticMethod )


};



inline const char* TestComponentWithForcedPrefixInterfacesComponentName(){ return "unit_test_dll!TestComponentWithForcedPrefixInterfaces"; }

typedef cppcomponents::runtime_class<TestComponentWithForcedPrefixInterfacesComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithForcedPrefixInterfaces>,
	cppcomponents::static_interfaces<TestComponentWithForcedPrefixInterfacesStaticInterface> >
	TestComponentWithForcedPrefixInterfaces_t;
typedef cppcomponents::use_runtime_class<TestComponentWithForcedPrefixInterfaces_t> TestComponentWithForcedPrefixInterfaces;


inline const char* TestComponentWithMultipleInterfacesName(){ return "unit_test_dll!TestComponentWithMultipleInterfaces"; }
typedef cppcomponents::runtime_class<TestComponentWithMultipleInterfacesName, 
	cppcomponents::object_interfaces<InterfaceTestComponentWithForcedPrefixInterfaces,ComponentInterface,cppcomponents::InterfaceUnknown>  >
	TestComponentWithMultipleInterfaces_t;

typedef cppcomponents::use_runtime_class<TestComponentWithMultipleInterfaces_t> TestComponentWithMultipleInterfaces;



inline const char* TestComponentWithRuntimeInheritanceName(){ return "unit_test_dll!TestComponentWithRuntimeInheritance"; }
// Same interfaces as TestComponentWithInheritedInterfaces
typedef cppcomponents::runtime_class < TestComponentWithRuntimeInheritanceName, cppcomponents::object_interfaces<InterfaceTestComponentWithInheritedInterfaces> >
	TestComponentWithRuntimeInheritance_t;

typedef cppcomponents::use_runtime_class<TestComponentWithRuntimeInheritance_t> TestComponentWithRuntimeInheritance;



struct IPerson : cppcomponents::define_interface < cppcomponents::uuid<0x716fdc9a, 0xdb9c, 0x4bb3, 0x8a9b,0xd577415da686>>{

	int GetAge();
	void SetAge(int a);

	std::string GetName();
	void SetName(std::string);

	CPPCOMPONENTS_CONSTRUCT(IPerson, GetAge, SetAge, GetName, SetName)

	CPPCOMPONENTS_INTERFACE_EXTRAS(IPerson){

		//CPPCOMPONENTS_R_PROPERTY(GetAge) AgeReadOnly;
		//CPPCOMPONENTS_RW_PROPERTY(GetName,SetName) Name;
		//CPPCOMPONENTS_W_PROPERTY(SetAge) AgeWriteOnly;

		//CPPCOMPONENTS_INITIALIZE_PROPERTIES_EVENTS(AgeReadOnly, Name, AgeWriteOnly)


	};

};

inline const char* PersonId(){ return "unit_test_dll!Person"; }

typedef cppcomponents::runtime_class<PersonId,cppcomponents::object_interfaces<IPerson>> Person_t;
typedef cppcomponents::use_runtime_class<Person_t> Person;

#include "../cppcomponents/function.hpp"




	struct IPersonWithEvent : cppcomponents::define_interface < cppcomponents::uuid<0x8d1dc800, 0x20eb, 0x4b5c, 0xb1bc, 0x645ae035ff33>, IPerson>{

		typedef cppcomponents::delegate< void (std::string)> PersonNameChangeHandler;
		std::int64_t add_PersonNameChanged(cppcomponents::use<PersonNameChangeHandler>);

		void remove_PersonNameChanged(std::int64_t);

		CPPCOMPONENTS_CONSTRUCT(IPersonWithEvent, add_PersonNameChanged, remove_PersonNameChanged)

		CPPCOMPONENTS_INTERFACE_EXTRAS(IPersonWithEvent){
			//CPPCOMPONENTS_EVENT(PersonNameChangeHandler, add_PersonNameChanged, remove_PersonNameChanged) NameChanged;

			//CPPCOMPONENTS_INITIALIZE_PROPERTIES_EVENTS(NameChanged)


		};

	};

	inline const char* PersonWithEventId(){ return "unit_test_dll!PersonWithEvent"; }

	typedef cppcomponents::runtime_class<PersonWithEventId, cppcomponents::object_interfaces<IPersonWithEvent>> PersonWithEvent_t;
	typedef cppcomponents::use_runtime_class<PersonWithEvent_t> PersonWithEvent;



	struct ITestWString : public cppcomponents::define_interface< cppcomponents::uuid<0xd6f1077b, 0x8adb, 0x4051, 0xafb4, 0xcdaf77404d37> >{

		std::wstring Concat(std::wstring a, cppcomponents::wstring_ref b);

		CPPCOMPONENTS_CONSTRUCT(ITestWString, Concat)

	};

	inline const char* TestWStringId(){ return "unit_test_dll!TestWString"; }

	typedef cppcomponents::runtime_class<TestWStringId, cppcomponents::object_interfaces<ITestWString>> TestWString_t;
	typedef cppcomponents::use_runtime_class<TestWString_t> TestWString;


	struct ITestTuple : public cppcomponents::define_interface < cppcomponents::uuid < 0x67ca4820, 0x1623, 0x41b3, 0x866f, 0x5335c9b58fff> >{

		std::tuple<std::string, std::string> Get2();
		std::tuple<int, std::string, double> Get3();
		std::tuple<int, std::string, double, char> Get4();
		std::tuple<int, std::string, double, char, cppcomponents::use<ITestTuple>> Get5();
		std::tuple <std::string> Get1();

		CPPCOMPONENTS_CONSTRUCT(ITestTuple, Get2, Get3,Get4,Get5,Get1)

	};

	inline const char* TestTupleId(){ return "unit_test_dll!TestTuple"; }

	typedef cppcomponents::runtime_class<TestTupleId, cppcomponents::object_interfaces<ITestTuple>> TestTuple_t;
	typedef cppcomponents::use_runtime_class<TestTuple_t> TestTuple;


	inline const char* TestPureStaticId(){ return "unit_test_dll!TestPureStatic"; }
	typedef cppcomponents::runtime_class < TestPureStaticId, cppcomponents::factory_interface<cppcomponents::NoConstructorFactoryInterface>,
		cppcomponents::static_interfaces < StaticInterface4 >> TestPureStatic_t;

	typedef cppcomponents::use_runtime_class<TestPureStatic_t> TestPureStatic;


#include "../cppcomponents/future.hpp"
	struct ITestFuture
		: public cppcomponents::define_interface < cppcomponents::uuid<0x664a8bfc, 0xca01, 0x469e, 0xa569, 0xccde9f5a7056> >
	{
		cppcomponents::use < cppcomponents::IFuture<std::string>> GetFutureString();
		cppcomponents::use < cppcomponents::IFuture<std::string>> GetFutureWithException();
		cppcomponents::use< cppcomponents::IFuture<int>> GetImmediateFuture();
		cppcomponents::use < cppcomponents::IFuture < cppcomponents::use < cppcomponents::IFuture < int >> >> GetWrappedFuture();

		CPPCOMPONENTS_CONSTRUCT(ITestFuture, GetFutureString, GetFutureWithException, GetImmediateFuture, GetWrappedFuture)


	};

	inline const char* TestFutureId(){ return "unit_test_dll!TestFuture"; }
	typedef cppcomponents::runtime_class<TestFutureId, cppcomponents::object_interfaces<ITestFuture>> TestFuture_t;
	typedef cppcomponents::use_runtime_class<TestFuture_t> TestFuture;




		struct ICounter : public cppcomponents::define_interface < cppcomponents::uuid<0x35779857, 0x9090, 0x4668, 0x8eee, 0xa67e359b66ae>>
		{
			std::uint64_t Increment();
			std::uint64_t Decrement();
			std::uint64_t Count();

			CPPCOMPONENTS_CONSTRUCT(ICounter, Increment, Decrement, Count)
		};



	inline const char* launch_on_new_thread_executor_id(){ return "launch_on_new_thread"; }
	typedef cppcomponents::runtime_class < launch_on_new_thread_executor_id, cppcomponents::object_interfaces<cppcomponents::IExecutor, ICounter>,
		cppcomponents::factory_interface < cppcomponents::NoConstructorFactoryInterface >> launch_on_new_thread_executor_t;

	struct launch_on_new_thread_executor : public cppcomponents::implement_runtime_class<launch_on_new_thread_executor, launch_on_new_thread_executor_t>{
		typedef cppcomponents::delegate < void() > ClosureType;
		std::atomic<std::uint64_t> count_;
		launch_on_new_thread_executor() : count_{ 0 }{}

		struct counter_helper{

			cppcomponents::use<ICounter> c_;
			bool success_;

			counter_helper(cppcomponents::use<ICounter> c)
				: c_{ c }, success_{ false }
			{
				c_.Increment();
			}

			void success(){
				success_ = true;
			}

			~counter_helper(){
				if (!success_){
					c_.Decrement();
				}
			}

		};

		void AddDelegate(cppcomponents::use<ClosureType> d){
			auto counter = this->QueryInterface<ICounter>();

			counter_helper helper(counter);

			std::thread t(
				[d, counter](){
					d();
					counter.Decrement();
			});

			helper.success();

			if (t.joinable()){
				t.detach();
			}

		}
		std::size_t NumPendingClosures(){
			return static_cast<std::size_t>(ICounter_Count());
		}

		std::uint64_t ICounter_Increment(){
			return count_.fetch_add(1);
		}
		std::uint64_t ICounter_Decrement(){
			return count_.fetch_sub(1);
		}
		std::uint64_t ICounter_Count(){
			return count_.load();
		}

	};


	inline const char* chronotestid(){ return "unit_test_dll!chrono_test"; }

	struct ITestChrono : cppcomponents::define_interface < cppcomponents::uuid<0x2e36f49d, 0x8a2f, 0x48af, 0x8929, 0xb72c146e10e7>>{

		typedef std::chrono::system_clock::time_point time_point;
		typedef std::chrono::system_clock::duration duration;
		time_point AddTime(time_point, duration);

		duration SubtractTime(time_point, time_point);

		CPPCOMPONENTS_CONSTRUCT(ITestChrono, AddTime, SubtractTime)

	};

	typedef cppcomponents::runtime_class<chronotestid, cppcomponents::object_interfaces<ITestChrono>> TestChrono_t;

	typedef cppcomponents::use_runtime_class<TestChrono_t> TestChrono;


	struct ITestTemplatedConstructor : cppcomponents::define_interface < cppcomponents::uuid<0xaee6a5eb, 0x2a1c, 0x4e66, 0x96d8, 0xdccf4a60cf60>>
	{
		int CallDelegate(); 

		CPPCOMPONENTS_CONSTRUCT(ITestTemplatedConstructor, CallDelegate)
	};

	struct ITestTemplatedConstructorFactory : cppcomponents::define_interface < cppcomponents::uuid<0x9adbd21b, 0x5ef7, 0x4a3c, 0x8c0c, 0xb07d78428939>>
	{
		typedef cppcomponents::delegate < int()> IntDelegate;
		cppcomponents::use<cppcomponents::InterfaceUnknown> Create(cppcomponents::use<IntDelegate>);

		CPPCOMPONENTS_CONSTRUCT(ITestTemplatedConstructorFactory, Create)

		CPPCOMPONENTS_INTERFACE_EXTRAS(ITestTemplatedConstructorFactory){
			template<class F>
			cppcomponents::use<cppcomponents::InterfaceUnknown> TemplatedConstructor(F f){
				return this->get_interface().Create(cppcomponents::make_delegate<IntDelegate>(f));
			}

		};

	};

	inline const char* TestTemplatedConstructorId(){ return "unit_test_dll!TestTemplatedConstructor"; }

	typedef cppcomponents::runtime_class < TestTemplatedConstructorId, cppcomponents::object_interfaces<ITestTemplatedConstructor>,
		cppcomponents::factory_interface < ITestTemplatedConstructorFactory >> TestTemplatedConstructor_t;

	typedef cppcomponents::use_runtime_class<TestTemplatedConstructor_t> TestTemplatedConstructor;

	struct ITestInternalClass :cppcomponents::define_interface<cppcomponents::uuid<0xe09ee78f, 0xd92f, 0x4cd3, 0xa6af, 0xbf4d52e6cf3f>>
	{
		std::string Test();

		CPPCOMPONENTS_CONSTRUCT(ITestInternalClass, Test)
	};
	inline const char* TestInternalClassId(){ return "TestInternalClass"; }
	typedef cppcomponents::runtime_class<TestInternalClassId, cppcomponents::object_interfaces<ITestInternalClass>> TestInternalClass_t;
	typedef cppcomponents::use_runtime_class<TestInternalClass_t> TestInternalClass;


	struct ITestInternalClassTester :cppcomponents::define_interface<cppcomponents::uuid<0xff784dcc, 0x7bb2, 0x40ef, 0xb1b7, 0x91ca61a6b0bf>>
	{
		std::string TestInteralExe();
		std::string TestInteralDll();

		CPPCOMPONENTS_CONSTRUCT(ITestInternalClassTester, TestInteralExe,TestInteralDll)
	};
	inline const char* TestInternalClassTesterId(){ return "unit_test_dll!TestInternalClassTester"; }
	typedef cppcomponents::runtime_class<TestInternalClassTesterId, cppcomponents::object_interfaces<ITestInternalClassTester>> TestInternalClassTester_t;
	typedef cppcomponents::use_runtime_class<TestInternalClassTester_t> TestInternalClassTester;


  struct ITestConstReturn :cppcomponents::define_interface<cppcomponents::uuid<0x2ef070ff, 0xfe90, 0x42ea, 0x93ee, 0xdd2bb3bd109f>>
  {
    const std::string Hello();
    const std::pair<const std::string, const int> Pair();
    const std::tuple<const std::string> Tuple1();
    const std::tuple<const std::string,const int> Tuple2();
    const std::tuple<const std::string, const int,const double> Tuple3();

    CPPCOMPONENTS_CONSTRUCT(ITestConstReturn,Hello,Pair,Tuple1,Tuple2,Tuple3)
  };

  inline const char* TestConstReturnId(){ return "unit_test_dll!TestConstReturn"; }
  typedef cppcomponents::runtime_class<TestConstReturnId, cppcomponents::object_interfaces<ITestConstReturn>> TestConstReturn_t;
  typedef cppcomponents::use_runtime_class<TestConstReturn_t> TestConstReturn;

