#include "cross_compiler_interface/cross_compiler_interface.hpp"
#include "cross_compiler_interface/interface_unknown.hpp"
#include "cross_compiler_interface/cross_compiler_introspection.hpp"


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

#include "cross_compiler_interface/cross_compiler_component.hpp"


struct ComponentInterface{
typedef cppcomponents::uuid<// {A4311094-E1CC-4D6A-8C2E-A1652D3843F8}

0xa4311094, 0xe1cc, 0x4d6a, 0x8c, 0x2e, 0xa1, 0x65, 0x2d, 0x38, 0x43, 0xf8
> uuid;

std::string Test();

CPPCOMPONENTS_CONSTRUCT(ComponentInterface,Test);



};

inline std::string mycomponentname(){return "Test.Component";}
typedef cppcomponents::runtime_class<mycomponentname,ComponentInterface,
    cppcomponents::DefaultFactoryInterface,cppcomponents::DefaultStaticInterface> TestComponent_t;

typedef cppcomponents::use_runtime_class<TestComponent_t> TestComponent;


struct FactoryInterface2{
    	// {FC9A22B0-04FE-438A-B4E9-B340D81F4337}
	typedef cppcomponents::uuid<
	0xFC9A22B0,0x04FE,0x438A,0xB4,0xE9,0xB3,0x40,0xD8,0x1F,0x43,0x37
	> uuid;

    typedef cppcomponents::use<cppcomponents::InterfaceUnknown> Unknown;

    Unknown Create();
    Unknown CreateWithString(std::string);

    CPPCOMPONENTS_CONSTRUCT(FactoryInterface2,Create,CreateWithString);


};


inline std::string componentname2(){return "Test.Component.2";}

typedef cppcomponents::runtime_class<componentname2,ComponentInterface,FactoryInterface2,cppcomponents::DefaultStaticInterface>
    TestComponentWithConstructor_t;

typedef cppcomponents::use_runtime_class<TestComponentWithConstructor_t> TestComponentWithConstructor;

inline std::string componentname3(){return "Test.Component.3";}
struct StaticInterface3{
   	// {F2CE950A-B826-4354-A07C-D17431323C26}
	typedef cppcomponents::uuid<
	0xF2CE950A,0xB826,0x4354,0xA0,0x7C,0xD1,0x74,0x31,0x32,0x3C,0x26
	> uuid;


    std::string GetStaticString();
    std::string GetStaticString2(cross_compiler_interface::cr_string s);

   
   cppcomponents::use<ComponentInterface> GetTestComponent();
     

    CPPCOMPONENTS_CONSTRUCT(StaticInterface3,GetStaticString,GetTestComponent,GetStaticString2);


};




typedef cppcomponents::runtime_class<componentname3,ComponentInterface,FactoryInterface2,StaticInterface3>
    TestComponentWithStatic_t;

typedef cppcomponents::use_runtime_class<TestComponentWithStatic_t> TestComponentWithStatic;