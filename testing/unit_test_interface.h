#include "../cross_compiler_interface/cross_compiler_interface.hpp"
#include "../cross_compiler_interface/interface_unknown.hpp"
#include "../cross_compiler_interface/cross_compiler_introspection.hpp"


using cross_compiler_interface::cross_function;
using cross_compiler_interface::use_interface;

template<class T> 
struct BaseInterface:public cross_compiler_interface::define_interface<T>{

    cross_function<BaseInterface,0,std::string()> hello_from_base;


    BaseInterface():hello_from_base(this){}
};

template<class T>
struct IGetName:public cross_compiler_interface::define_interface<T>{
    cross_function<IGetName,0,std::string()> get_name;

    IGetName():get_name(this){}

};

CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(IGetName,get_name);


template<class Iface, int Id>
struct cross_function_int_int:public cross_compiler_interface::custom_cross_function<Iface,Id,int(int),cross_compiler_interface::error_code (cross_compiler_interface::portable_base*,int*, int),cross_function_int_int<Iface,Id>>{

    
    int call_vtable_function(int i)const{
        int r = 0;
        auto ret = this->get_vtable_fn()(this->get_portable_base(),&r,i);
        if(ret < 0){
            this->exception_from_error_code(ret);
        }
        return r;
    }


    template<class F>
    static cross_compiler_interface::error_code vtable_function(F f, cross_compiler_interface::portable_base* v,int* r, int i){
        *r = f(i);
        return 0;
    }

    template<class F>
    void operator=(F f){
        this->set_function(f);

    }

    template<class T>
    cross_function_int_int(T t):cross_function_int_int::base_t(t){}


};


// There is at least 2 ways to define the interfaces
// Here is the more verbose way, but it does not require template alias support and non-static data member initializers
#ifdef _MSC_VER

template<class T> struct TestInterface:public cross_compiler_interface::define_interface<T,BaseInterface>{

    cross_function_int_int<TestInterface,0> plus_5;
    cross_function<TestInterface,1,double(double)> times_2point5;
    cross_function<TestInterface,2,void(cross_compiler_interface::out<int>)> double_referenced_int;
    cross_function<TestInterface,3,std::size_t(std::string)> count_characters;
    cross_function<TestInterface,4,std::string(std::string)> say_hello;
    cross_function<TestInterface,5,void(std::string)> use_at_out_of_range;
    cross_function<TestInterface,6,void()> not_implemented;
    cross_function<TestInterface,7,std::vector<std::string>(std::string)> split_into_words;
    cross_function<TestInterface,8,std::string(use_interface<IGetName>)> say_hello2;
    cross_function<TestInterface,9,std::pair<int,std::string> (std::vector<std::string> v,int pos)> get_string_at;
    cross_function<TestInterface,10,use_interface<IGetName>()> get_igetname;
    cross_function<TestInterface,11,std::string()> get_name_from_runtime_parent;
    cross_function_int_int<TestInterface,12> custom_with_runtime_parent;

    cross_function<TestInterface,13,void(cross_compiler_interface::out<std::string>)>
        get_out_string;

    cross_function<TestInterface,14,void(cross_compiler_interface::cr_string)>
        append_string;

    cross_function<TestInterface,15,cross_compiler_interface::cr_string()>
        get_string;

    cross_function<TestInterface,16,std::vector<std::string>(std::vector<std::string>)>
        append_hello_to_vector;

    cross_function<TestInterface,17,std::vector<std::uint32_t>(std::vector<std::uint32_t>)>
        append_5_to_vector;

    // Tests for u16string and u32string
    cross_function<TestInterface,18,std::u16string(std::u16string)>
        test_u16_string;
    cross_function<TestInterface,19,std::u32string(std::u32string)>
        test_u32_string;




    
    TestInterface(): 
        plus_5(this),times_2point5(this),double_referenced_int(this),
        count_characters(this),say_hello(this),use_at_out_of_range(this),not_implemented(this),split_into_words(this),say_hello2(this),
        get_string_at(this),get_igetname(this),get_name_from_runtime_parent(this),custom_with_runtime_parent(this),
        get_out_string(this),append_string(this),get_string(this),append_hello_to_vector(this),
        append_5_to_vector(this),test_u16_string(this),test_u32_string(this){}
};

#else

// Here is the better way with C++11 support for initialization of non-static class members and template alias
template<class T> struct TestInterface:public cross_compiler_interface::define_interface<T,BaseInterface>{

    template<int Id, class F>
    using cf = cross_function<TestInterface,Id,F>;


    cross_function_int_int<TestInterface,0> plus_5 = this;


    cf<1,double(double)> times_2point5 = this;
    cf<2,void(cross_compiler_interface::out<int>)> double_referenced_int = this;
    cf<3,std::size_t(std::string)> count_characters = this;
    cf<4,std::string(std::string)> say_hello = this;
    cf<5,void(std::string)> use_at_out_of_range = this;
    cf<6,void()> not_implemented = this;
    cf<7,std::vector<std::string>(std::string)> split_into_words = this;
    cf<8,std::string(use_interface<IGetName>)> say_hello2 = this;
    cf<9,std::pair<int,std::string> (std::vector<std::string> v,int pos)> get_string_at = this;
    cf<10,use_interface<IGetName>()> get_igetname = this;
    cf<11,std::string()> get_name_from_runtime_parent = this;



    cross_function_int_int<TestInterface,12> custom_with_runtime_parent = this;

    cf<13,void(cross_compiler_interface::out<std::string>)>
        get_out_string = this;
    cf<14,void(cross_compiler_interface::cr_string)>
        append_string=this;

    cf<15,cross_compiler_interface::cr_string()>
        get_string=this;

    cf<16,std::vector<std::string>(std::vector<std::string>)>
        append_hello_to_vector=this;

    cf<17,std::vector<std::uint32_t>(std::vector<std::uint32_t>)>
        append_5_to_vector=this;

    // Tests for u16string and u32string
    cf<18,std::u16string(std::u16string)>
        test_u16_string = this;
    cf<19,std::u32string(std::u32string)>
        test_u32_string = this;

    TestInterface(){}
    

    
};




#endif

CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(TestInterface,plus_5,times_2point5,double_referenced_int,
        count_characters,say_hello,use_at_out_of_range,not_implemented,split_into_words,say_hello2,
        get_string_at,get_igetname,get_name_from_runtime_parent,custom_with_runtime_parent,
        get_out_string,append_string,get_string,append_hello_to_vector,
        append_5_to_vector,test_u16_string,test_u32_string);


// {0AEBCA97-B08D-4FCF-8C41-133C1A8ABF03}
typedef cross_compiler_interface::uuid<0xaebca97, 0xb08d, 0x4fcf, 0x8c, 0x41, 0x13, 0x3c, 0x1a, 0x8a, 0xbf, 0x3>
    UnknownDerivedInterface_uuid_t;
template<class T>
struct IUnknownDerivedInterface:public cross_compiler_interface::define_unknown_interface<T,UnknownDerivedInterface_uuid_t>{

    cross_function<IUnknownDerivedInterface,0,std::string()> hello_from_iuknown_derived;
    
    IUnknownDerivedInterface():hello_from_iuknown_derived(this)
    {}


};
// {9A5306A2-5F78-4ECB-BCEB-AADE1F9223AA}
typedef cross_compiler_interface::uuid<0x9a5306a2, 0x5f78, 0x4ecb, 0xbc, 0xeb, 0xaa, 0xde, 0x1f, 0x92, 0x23, 0xaa>
    UnknownDerivedInterface2_uuid_t;
template<class T>
struct IUnknownDerivedInterface2:public cross_compiler_interface::define_unknown_interface<T,UnknownDerivedInterface2_uuid_t>{

    cross_function<IUnknownDerivedInterface2,0,std::string()> hello_from_iuknown_derived2;
    cross_function<IUnknownDerivedInterface2,1,cross_compiler_interface::use_unknown<IUnknownDerivedInterface>()> get_derived;
    cross_function<IUnknownDerivedInterface2,2,std::string(cross_compiler_interface::use_unknown<IUnknownDerivedInterface>)> get_string;
    
    IUnknownDerivedInterface2():hello_from_iuknown_derived2(this),get_derived(this),get_string(this)
    {}


};

template<class T>
struct IUnknownDerivedInterface2Derived
    :public cross_compiler_interface::define_unknown_interface<T,
    // {9A28B8CE-48C6-40DC-9D7A-84E4268F2BA8}
    cross_compiler_interface::uuid<
    0x9A28B8CE,0x48C6,0x40DC,0x9D,0x7A,0x84,0xE4,0x26,0x8F,0x2B,0xA8
    >
    ,IUnknownDerivedInterface2>
{
    cross_function<IUnknownDerivedInterface2Derived,0,std::string()> hello_from_derived;
    
    IUnknownDerivedInterface2Derived():hello_from_derived(this)
    {}
};


// {52918617-D0BE-41FF-A641-2EC32AC1B157}
typedef cross_compiler_interface::uuid<0x52918617, 0xd0be, 0x41ff, 0xa6, 0x41, 0x2e, 0xc3, 0x2a, 0xc1, 0xb1, 0x57>
    UnknownDerivedInterfaceUnused_uuid_t;
template<class T>
struct IUnknownDerivedInterfaceUnused:public cross_compiler_interface::define_unknown_interface<T,UnknownDerivedInterfaceUnused_uuid_t,
        IUnknownDerivedInterface2Derived>{
    cross_function<IUnknownDerivedInterfaceUnused,0,std::string()> unused;
    
    IUnknownDerivedInterfaceUnused():
        unused(this)
    {}


};



template<class T>
struct ITest
    :public cross_compiler_interface::define_unknown_interface<T,
    // {730B1677-17C8-4CEC-999B-5B86BDD32A98}
    cross_compiler_interface::uuid<
    0x730B1677,0x17C8,0x4CEC,0x99,0x9B,0x5B,0x86,0xBD,0xD3,0x2A,0x98
    >
    >
{




    ITest()

    {}
};

template<class T>
struct ITest2
    :public cross_compiler_interface::define_unknown_interface<T,
    // {82D7EB95-13FB-4B9D-ACDB-3583F7A033F7}
    cross_compiler_interface::uuid<
    0x82D7EB95,0x13FB,0x4B9D,0xAC,0xDB,0x35,0x83,0xF7,0xA0,0x33,0xF7
    >
    ,ITest>
{




    ITest2()

    {}
};


template<class T>
struct ITestLayout
    :public cross_compiler_interface::define_unknown_interface<T,
    // {3C8D130E-4BE2-4BDD-B9D7-A12E65A73E11}
    cross_compiler_interface::uuid<
    0x3C8D130E,0x4BE2,0x4BDD,0xB9,0xD7,0xA1,0x2E,0x65,0xA7,0x3E,0x11
    >
    >
{

    cross_function<ITestLayout,0,void(std::uint32_t)> set_int;
    cross_function<ITestLayout,1,double()> add_2_5_to_int;



    ITestLayout()
        :set_int(this),add_2_5_to_int(this)
    {}
};

template<class T>
struct ITestLayout2
    :public cross_compiler_interface::define_unknown_interface<T,
    // {7E4E520A-ACE6-46BC-839F-7B6E15369C21}
    cross_compiler_interface::uuid<
    0x7E4E520A,0xACE6,0x46BC,0x83,0x9F,0x7B,0x6E,0x15,0x36,0x9C,0x21
    >
    >
{
    cross_function<ITestLayout2,0,std::int32_t()> get_int;



    ITestLayout2()
        :get_int(this)
    {}
};

#include "../cppcomponents/cppcomponents.hpp"


struct ComponentInterface : public cppcomponents::define_interface<0xa4311094, 0xe1cc, 0x4d6a, 0x8c, 0x2e, 0xa1, 0x65, 0x2d, 0x38, 0x43, 0xf8>{


	std::string Test();

	CPPCOMPONENTS_CONSTRUCT(ComponentInterface, Test);

};

inline std::string mycomponentname(){return "Test.Component";}
typedef cppcomponents::runtime_class<mycomponentname,cppcomponents::object_interfaces<ComponentInterface> > TestComponent_t;

typedef cppcomponents::use_runtime_class<TestComponent_t> TestComponent;


struct FactoryInterface2 : public cppcomponents::define_interface<0xFC9A22B0, 0x04FE, 0x438A, 0xB4, 0xE9, 0xB3, 0x40, 0xD8, 0x1F, 0x43, 0x37>{


    typedef cppcomponents::use<cppcomponents::InterfaceUnknown> Unknown;

    Unknown Create();
    Unknown CreateWithString(std::string);

    CPPCOMPONENTS_CONSTRUCT(FactoryInterface2,Create,CreateWithString);


};


inline std::string componentname2(){return "Test.Component.2";}

typedef cppcomponents::runtime_class<componentname2,cppcomponents::object_interfaces<ComponentInterface>,
	cppcomponents::factory_interface<FactoryInterface2> >
    TestComponentWithConstructor_t;

typedef cppcomponents::use_runtime_class<TestComponentWithConstructor_t> TestComponentWithConstructor;

inline std::string componentname3(){return "unit_test_dll!Test.Component.3";}
struct StaticInterface3 : public cppcomponents::define_interface < 0xF2CE950A,0xB826,0x4354,0xA0,0x7C,0xD1,0x74,0x31,0x32,0x3C,0x26> {


    std::string GetStaticString();
    std::string GetStaticString2(cross_compiler_interface::cr_string s);

   
   cppcomponents::use<ComponentInterface> GetTestComponent();
     

    CPPCOMPONENTS_CONSTRUCT(StaticInterface3,GetStaticString,GetTestComponent,GetStaticString2);


};




typedef cppcomponents::runtime_class<componentname3,cppcomponents::object_interfaces<ComponentInterface>,
	cppcomponents::factory_interface<FactoryInterface2>,cppcomponents::static_interfaces<StaticInterface3> >
    TestComponentWithStatic_t;

typedef cppcomponents::use_runtime_class<TestComponentWithStatic_t> TestComponentWithStatic;

struct InterfaceTestComponentWithMultipleStatic: public cppcomponents::define_interface<0x93CC8F89, 0x98C3, 0x448C, 0x84, 0x0A, 0x33, 0xA4, 0x97, 0x56, 0x91, 0x2F>{
		




	CPPCOMPONENTS_CONSTRUCT_NO_METHODS(InterfaceTestComponentWithMultipleStatic);


};

struct StaticInterface4: public cppcomponents::define_interface<0x244D5533, 0x28DD, 0x46C5, 0x90, 0x08, 0x44, 0xA7, 0x3E, 0x54, 0x95, 0xA4>{




	std::string GetStaticStringOtherInterface();

	CPPCOMPONENTS_CONSTRUCT(StaticInterface4,GetStaticStringOtherInterface );


};
inline std::string TestComponentWithMultipleStaticComponentName(){ return "unit_test_dll!TestComponentWithMultipleStatic"; }

typedef cppcomponents::runtime_class<TestComponentWithMultipleStaticComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithMultipleStatic>,
	cppcomponents::factory_interface<FactoryInterface2>, cppcomponents::static_interfaces<StaticInterface3,StaticInterface4> >
	TestComponentWithMultipleStatic_t;
typedef cppcomponents::use_runtime_class<TestComponentWithMultipleStatic_t> TestComponentWithMultipleStatic;

struct InterfaceTestComponentWithInheritedInterfaces: public cppcomponents::define_interface<0x7F66263B, 0x18A4, 0x450D, 0xA4, 0xDB, 0x5E, 0xEC, 0x96, 0x1B, 0xCD, 0xB4
	,ComponentInterface>{


	std::string HelloFromInherited(){ return "Hello from Inherited"; }


	CPPCOMPONENTS_CONSTRUCT(InterfaceTestComponentWithInheritedInterfaces,HelloFromInherited );


};
struct TestComponentWithInheritedInterfacesFactoryInterface: public cppcomponents::define_interface<0xA9739B2C, 0x3B9B, 0x4DE4, 0xBC, 0x8B, 0xC4, 0x89, 0x47, 0xC8, 0x69, 0x86>{



	cppcomponents::use<cppcomponents::InterfaceUnknown> Create();

	CPPCOMPONENTS_CONSTRUCT(TestComponentWithInheritedInterfacesFactoryInterface,Create );


};
struct TestComponentWithInheritedInterfacesStaticInterface: public cppcomponents::define_interface<0x8C0281B6, 0x4A4D, 0x4700, 0x92, 0xE0, 0x68, 0x59, 0x54, 0x5D, 0xEE, 0x90
	,StaticInterface3>{





	CPPCOMPONENTS_CONSTRUCT_NO_METHODS(TestComponentWithInheritedInterfacesStaticInterface);


};
inline std::string TestComponentWithInheritedInterfacesComponentName(){ return "unit_test_dll!TestComponentWithInheritedInterfaces"; }

typedef cppcomponents::runtime_class<TestComponentWithInheritedInterfacesComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithInheritedInterfaces>,
	cppcomponents::factory_interface<TestComponentWithInheritedInterfacesFactoryInterface>,
	cppcomponents::static_interfaces<TestComponentWithInheritedInterfacesStaticInterface> >
	TestComponentWithInheritedInterfaces_t;
typedef cppcomponents::use_runtime_class<TestComponentWithInheritedInterfaces_t> TestComponentWithInheritedInterfaces;


struct TestComponentWithForcedPrefixInterfacesStaticInterface;
struct InterfaceTestComponentWithForcedPrefixInterfaces;
namespace cross_compiler_interface{

	template<>
	struct allow_interface_to_map_no_prefix<TestComponentWithForcedPrefixInterfacesStaticInterface>{
		enum{ value = false };
	};

	template<>
	struct allow_interface_to_map_no_prefix<InterfaceTestComponentWithForcedPrefixInterfaces>{
		enum{ value = false };
	};
};
struct InterfaceTestComponentWithForcedPrefixInterfaces: public cppcomponents::define_interface<0x4F183EC6, 0x2EB1, 0x495A, 0x99, 0x69, 0x99, 0x72, 0x1D, 0x70, 0xDD, 0xFA>{

	std::string Test();



	CPPCOMPONENTS_CONSTRUCT(InterfaceTestComponentWithForcedPrefixInterfaces,Test );


};


struct TestComponentWithForcedPrefixInterfacesStaticInterface: public cppcomponents::define_interface<0xD0A3968E, 0xFD14, 0x4695, 0x8E, 0xB2, 0xC1, 0x65, 0xF5, 0x90, 0x43, 0x0A>{

	std::string StaticMethod(){ return "StaticMethod"; }



	CPPCOMPONENTS_CONSTRUCT(TestComponentWithForcedPrefixInterfacesStaticInterface,StaticMethod );


};



inline std::string TestComponentWithForcedPrefixInterfacesComponentName(){ return "unit_test_dll!TestComponentWithForcedPrefixInterfaces"; }

typedef cppcomponents::runtime_class<TestComponentWithForcedPrefixInterfacesComponentName, cppcomponents::object_interfaces<InterfaceTestComponentWithForcedPrefixInterfaces>,
	cppcomponents::static_interfaces<TestComponentWithForcedPrefixInterfacesStaticInterface> >
	TestComponentWithForcedPrefixInterfaces_t;
typedef cppcomponents::use_runtime_class<TestComponentWithForcedPrefixInterfaces_t> TestComponentWithForcedPrefixInterfaces;


inline std::string TestComponentWithMultipleInterfacesName(){ return "unit_test_dll!TestComponentWithMultipleInterfaces"; }
typedef cppcomponents::runtime_class<TestComponentWithMultipleInterfacesName, 
	cppcomponents::object_interfaces<InterfaceTestComponentWithForcedPrefixInterfaces,ComponentInterface,cppcomponents::InterfaceUnknown>  >
	TestComponentWithMultipleInterfaces_t;

typedef cppcomponents::use_runtime_class<TestComponentWithMultipleInterfaces_t> TestComponentWithMultipleInterfaces;



inline std::string TestComponentWithRuntimeInheritanceName(){ return "unit_test_dll!TestComponentWithRuntimeInheritance"; }
// Same interfaces as TestComponentWithInheritedInterfaces
typedef cppcomponents::runtime_class < TestComponentWithRuntimeInheritanceName, cppcomponents::object_interfaces<InterfaceTestComponentWithInheritedInterfaces> >
	TestComponentWithRuntimeInheritance_t;

typedef cppcomponents::use_runtime_class<TestComponentWithRuntimeInheritance_t> TestComponentWithRuntimeInheritance;



struct IPerson : cppcomponents::define_interface < 0x716fdc9a, 0xdb9c, 0x4bb3, 0x8a, 0x9b, 0xd5, 0x77, 0x41, 0x5d, 0xa6, 0x86>{

	int GetAge();
	void SetAge(int a);

	std::string GetName();
	void SetName(std::string);

	CPPCOMPONENTS_CONSTRUCT(IPerson, GetAge, SetAge, GetName, SetName);

	CPPCOMPONENTS_INTERFACE_EXTRAS{

		CPPCOMPONENTS_R_PROPERTY(GetAge) AgeReadOnly;
		CPPCOMPONENTS_RW_PROPERTY(GetName,SetName) Name;
		CPPCOMPONENTS_W_PROPERTY(SetAge) AgeWriteOnly;

		CPPCOMPONENTS_INTERFACE_EXTRAS_CONSTRUCTOR(AgeReadOnly,Name,AgeWriteOnly)


	};

};

inline std::string PersonId(){ return "unit_test_dll!Person"; };

typedef cppcomponents::runtime_class<PersonId,cppcomponents::object_interfaces<IPerson>> Person_t;
typedef cppcomponents::use_runtime_class<Person_t> Person;

#include "../cppcomponents/events.hpp"




	struct IPersonWithEvent : cppcomponents::define_interface < 0x8d1dc800, 0x20eb, 0x4b5c, 0xb1, 0xbc, 0x64, 0x5a, 0xe0, 0x35, 0xff, 0x33, IPerson>{

		typedef cppcomponents::event_delegate< void (std::string), 0x351a2745, 0xf88c, 0x420a, 0xa4, 0xc9, 0xf0, 0xd, 0xce, 0x85, 0x7a, 0xb0> PersonNameChangeHandler;
		std::int64_t add_PersonNameChanged(cppcomponents::use<PersonNameChangeHandler>);

		void remove_PersonNameChanged(std::int64_t);

		CPPCOMPONENTS_CONSTRUCT(IPersonWithEvent, add_PersonNameChanged, remove_PersonNameChanged);

		CPPCOMPONENTS_INTERFACE_EXTRAS{
			CPPCOMPONENTS_EVENT(PersonNameChangeHandler, add_PersonNameChanged, remove_PersonNameChanged) NameChanged;

			CPPCOMPONENTS_INTERFACE_EXTRAS_CONSTRUCTOR(NameChanged);


		};

	};

	inline std::string PersonWithEventId(){ return "unit_test_dll!PersonWithEvent"; };

	typedef cppcomponents::runtime_class<PersonWithEventId, cppcomponents::object_interfaces<IPersonWithEvent>> PersonWithEvent_t;
	typedef cppcomponents::use_runtime_class<PersonWithEvent_t> PersonWithEvent;