//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Leak check detection with MSVC
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

namespace{
struct MemLeakCheckInit{
	MemLeakCheckInit(){
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}
};

MemLeakCheckInit mlcinit;
}
#endif
//
#include <gtest/gtest.h>
#include "unit_test_interface.h"
#include <cstring>

struct MyFixture:public ::testing::Test{
    cross_compiler_interface::module m_;

    cross_compiler_interface::use_interface<TestInterface> iTest;

    cross_compiler_interface::use_interface<TestInterface> iTestMemFn;
    MyFixture():m_("unit_test_dll"),iTest(cross_compiler_interface::create<TestInterface>(m_,"CreateTestInterface")),
        iTestMemFn(cross_compiler_interface::create<TestInterface>(m_,"CreateTestMemFnInterface"))
    {
        
    }

};


template<class T, class U>
bool collection_equal(const T& t, const U& u){
	if (t.size() != u.size())return false;
	return std::equal(t.begin(), t.end(), u.begin());
}


// Places as first test to test ObjectCount
TEST(ComponentFirst, test_component_object_count){
	
	std::string module_name = "unit_test_dll";
	cross_compiler_interface::module m(module_name);
	auto get_object_count = m.load_module_function<std::size_t(*)()>("GetObjectCount");

	std::string class_name = "unit_test_dll!Test.Component.3";



	auto s = TestComponentWithStatic::static_interface(module_name, class_name);
	EXPECT_EQ(get_object_count(), 1);


	auto t = TestComponentWithStatic::dynamic_creator(module_name, class_name)();
	EXPECT_EQ(get_object_count(), 2);

	t = nullptr;
	EXPECT_EQ(get_object_count(), 1);

	s = nullptr;
	EXPECT_EQ(get_object_count(), 0);



}

TEST_F(MyFixture,Test_base)
{
   std::string expected = "Hello from Base";
   EXPECT_EQ(expected, iTest.hello_from_base());
   EXPECT_EQ(expected, iTestMemFn.hello_from_base());
  
}

TEST_F(MyFixture,Integer_manipulation)
{
   auto expected = 10;
   EXPECT_EQ(expected, iTest.plus_5(5));
   EXPECT_EQ(expected, iTestMemFn.plus_5(5));
}
TEST_F(MyFixture,Double_manipulation)
{
   auto expected = 10.0;
   EXPECT_EQ(expected, iTest.times_2point5(4));
   EXPECT_EQ(expected, iTestMemFn.times_2point5(4));
}

TEST_F(MyFixture,Manipulation_of_int_reference)
{
   auto expected = 2;
   int i = 4;
   // Todo FIX
   iTest.double_referenced_int(&i);
  EXPECT_EQ(expected,i);

   i = 4;
   iTestMemFn.double_referenced_int(&i);
  EXPECT_EQ(expected,i);
}

TEST_F(MyFixture,string_manipulation_1)
{
    std::string s= "Hello World\n";
   auto expected = s.size();
   EXPECT_EQ(expected, iTest.count_characters(s));
   EXPECT_EQ(expected, iTestMemFn.count_characters(s));
}

TEST_F(MyFixture,string_manipulation_2)
{
    std::string s= "John";
   auto expected = "Hello " + s;
   EXPECT_EQ(expected, iTest.say_hello(s));
   EXPECT_EQ(expected, iTestMemFn.say_hello(s));
}

TEST_F(MyFixture,Exception_handling_1)
{
    std::string s= "John";
   EXPECT_THROW(iTest.use_at_out_of_range(s),std::out_of_range);
   EXPECT_THROW(iTestMemFn.use_at_out_of_range(s),std::out_of_range);
}

TEST_F(MyFixture,Exception_handling_2)
{

   EXPECT_THROW(iTest.not_implemented(),cross_compiler_interface::error_not_implemented);
   EXPECT_THROW(iTestMemFn.not_implemented(),cross_compiler_interface::error_not_implemented);
}

TEST_F(MyFixture,vector_of_strings)
{
    std::vector<std::string> expected;
    expected.push_back("This");
    expected.push_back("is");
    expected.push_back("a");
    expected.push_back("test");


    std::string s= "This is a test";

    EXPECT_TRUE(expected == iTest.split_into_words(s));
    EXPECT_TRUE(expected == iTestMemFn.split_into_words(s));

}

TEST_F(MyFixture,Passed_in_interface)
{
    std::string s= "John";
   auto expected = "Hello " + s;

   cross_compiler_interface::implement_interface<IGetName> ign;
   ign.get_name = [s](){return s;};
   EXPECT_EQ(expected,iTest.say_hello2(ign.get_use_interface()));
   EXPECT_EQ(expected,iTestMemFn.say_hello2(ign.get_use_interface()));
}

TEST_F(MyFixture,std_pair)
{
    std::pair<int,std::string> expected(3,"test");
    std::vector<std::string> v;
    v.push_back("This");
    v.push_back("is");
    v.push_back("a");
    v.push_back("test");

    EXPECT_TRUE(expected == iTest.get_string_at(v,3));
    EXPECT_TRUE(expected == iTestMemFn.get_string_at(v,3));
}


TEST_F(MyFixture,returned_interface)
{
    std::string expected = "Hello from returned interface";

    EXPECT_TRUE(expected == iTest.get_igetname().get_name());
    EXPECT_TRUE(expected == iTestMemFn.get_igetname().get_name());
}


TEST_F(MyFixture,runtime_parent)
{
    std::string expected = "TestImplementation";
    int iexpected = 15;

    EXPECT_EQ(expected , iTest.get_name_from_runtime_parent());
    EXPECT_EQ(expected , iTestMemFn.get_name_from_runtime_parent());
    EXPECT_EQ(iexpected , iTest.custom_with_runtime_parent(5));
    EXPECT_EQ(iexpected , iTestMemFn.custom_with_runtime_parent(5));
}

TEST_F(MyFixture,iuknown_tests)
{
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");
    use_interface<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid_type::get())));

    EXPECT_TRUE(derived.get_portable_base()!=nullptr);
    EXPECT_EQ(3,unk.AddRef());
    EXPECT_EQ(2,unk.Release());

    EXPECT_TRUE(unk.QueryInterfaceRaw(&cross_compiler_interface::uuid<0,0,0,0,0>::get()) == nullptr);


    EXPECT_EQ(1,unk.Release());



    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";

    EXPECT_EQ(expected , derived.hello_from_iuknown_derived());

    use_interface<IUnknownDerivedInterface2> derived2(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface2>(derived.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface2>::uuid_type::get())));
    EXPECT_EQ(1,derived.Release());
    EXPECT_TRUE(derived2.get_portable_base()!=nullptr);
    EXPECT_EQ(expected2 , derived2.hello_from_iuknown_derived2());

    use_interface<IUnknownDerivedInterface2Derived> derived2derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface2Derived>(
        derived2.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface2Derived>::uuid_type::get())));
    EXPECT_EQ(1,derived2.Release());

    EXPECT_TRUE(derived2derived.get_portable_base()!=nullptr);
    EXPECT_EQ(expected3 , derived2derived.hello_from_derived());

    EXPECT_EQ(0,derived2derived.Release());

}

TEST_F(MyFixture,use_unknown_test)
{
    using cross_compiler_interface::use_unknown;
    
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");

    {
    use_unknown<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid_type::get())),false);

    EXPECT_TRUE(!!derived);

    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";

    EXPECT_EQ(expected , derived.hello_from_iuknown_derived());

    auto derived2 = derived.QueryInterface<IUnknownDerivedInterface2>();
    EXPECT_TRUE(!!derived2);
    EXPECT_EQ(expected2 , derived2.hello_from_iuknown_derived2());

    auto derived2derived = derived2.QueryInterface<IUnknownDerivedInterface2Derived>();

    EXPECT_TRUE(!!derived2derived);
    EXPECT_EQ(expected3 , derived2derived.hello_from_derived());

    // Check self copy
    derived2derived = derived2derived;
    EXPECT_TRUE(!!derived2derived);
    EXPECT_EQ(expected3 , derived2derived.hello_from_derived());


    use_unknown<cross_compiler_interface::InterfaceUnknown> unk2 = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterface");

    auto d = unk2.QueryInterface<IUnknownDerivedInterface2Derived>();
    EXPECT_TRUE(!!d);
    EXPECT_EQ(expected3 , d.hello_from_derived());

    // Check  copy
    derived2derived = d;
    EXPECT_TRUE(!!derived2derived);
    EXPECT_EQ(expected3 , derived2derived.hello_from_derived());

    // Check assignment to nullptr
    d = nullptr;
    EXPECT_TRUE(!d);
    EXPECT_THROW(d.hello_from_iuknown_derived2(),cross_compiler_interface::error_pointer);


    EXPECT_THROW(unk2.QueryInterface<IUnknownDerivedInterfaceUnused>(),cross_compiler_interface::error_no_interface);

    auto du = unk2.QueryInterfaceNoThrow<IUnknownDerivedInterfaceUnused>();
    EXPECT_TRUE(!du);




    }

    // If all our cleanup is ok, releasing should make the reference count 0
    EXPECT_EQ(0,unk.Release());

}


TEST_F(MyFixture,pass_return_use_unknown)
{
    using cross_compiler_interface::use_unknown;
    
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");

    {
    use_unknown<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid_type::get())),false);

    EXPECT_TRUE(!!derived);

    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";


    auto derived2 = derived.QueryInterface<IUnknownDerivedInterface2>();
    EXPECT_TRUE(!!derived2);

    auto d = derived2.get_derived();
    EXPECT_TRUE(!!d);

    EXPECT_EQ(expected , d.hello_from_iuknown_derived());

    
    EXPECT_EQ(expected , derived2.get_string(d));
    EXPECT_EQ(expected2 , derived2.hello_from_iuknown_derived2());






    }

    // If all our cleanup is ok, releasing should make the reference count 0
    EXPECT_EQ(0,unk.Release());

}

TEST_F(MyFixture,check_throws_on_null_use)
{
    using cross_compiler_interface::use_unknown;
    
    auto i = iTest;
    // Check that regular function call and custom function call succeed
    EXPECT_EQ(i.hello_from_base(),   std::string("Hello from Base"));
    EXPECT_EQ(i.plus_5(5),10);

    // Check that regular function call and custom function call fail with nullptr
    i = nullptr;
    EXPECT_THROW(i.hello_from_base(),cross_compiler_interface::error_pointer);
    EXPECT_THROW(i.plus_5(5),cross_compiler_interface::error_pointer);

    // Check that regular function call and custom function call succeed after valid assignment
    i = iTest;
    EXPECT_EQ(i.hello_from_base(),   std::string("Hello from Base"));
    EXPECT_EQ(i.plus_5(5),10);




}

TEST_F(MyFixture,packing)
{


    EXPECT_EQ(sizeof(cross_compiler_interface::cross_pair<char,double>), 9);
    EXPECT_EQ(sizeof(cross_compiler_interface::cross_string<char>), sizeof(char*)*2);

}

TEST_F(MyFixture,check_single_interface_implement_iunknown_interfaces)
{


    auto only = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterfaceOnly").QueryInterface<IUnknownDerivedInterface>();
    std::string expected = "Hello from ImplementIuknownDerivedInterfaceOnly";
    EXPECT_EQ(only.hello_from_iuknown_derived(),expected);

}


// Check that layout is compatible with COM on windows

#ifdef _WIN32

#include <Unknwn.h>

struct ITestLayoutPure:public IUnknown{
    virtual HRESULT __stdcall set_int(std::int32_t) = 0;
    virtual HRESULT __stdcall add_2_5_to_int(double*) = 0;
};

struct ITestLayout2Pure:public IUnknown{
    virtual HRESULT __stdcall get_int(std::int32_t*) = 0;
};

// MingW does not seem to have IID_IUnknown
// so just do this test on MSVC
#ifdef _MSC_VER
TEST(uuid_test, test_equivalence){

	EXPECT_EQ(TRUE,IsEqualGUID(IID_IUnknown, cross_compiler_interface::Unknown_uuid_t::get_windows_guid<GUID>()));
}
#endif

TEST_F(MyFixture,check_com_layout_compatible)
{


    auto pbase = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateTestLayout").get_portable_base();
    IUnknown* pUnk = reinterpret_cast<IUnknown*>(pbase);
    ITestLayoutPure* pIL = 0;
    EXPECT_EQ(
        pUnk->QueryInterface(cross_compiler_interface::use_unknown<ITestLayout>::uuid_type::get_windows_guid<GUID>(),reinterpret_cast<void**>(&pIL)),
        S_OK);
    EXPECT_TRUE(pIL != nullptr);
    pIL->set_int(5);
    double d = 0;
    EXPECT_EQ(pIL->add_2_5_to_int(&d),S_OK);
    EXPECT_EQ(d,7.5);

    ITestLayout2Pure* pIL2 = 0;
    EXPECT_EQ(
        pUnk->QueryInterface(cross_compiler_interface::use_unknown<ITestLayout2>::uuid_type::get_windows_guid<GUID>(),reinterpret_cast<void**>(&pIL2)),
        S_OK);
    EXPECT_TRUE(pIL2 != nullptr);
    int i = 0;
    EXPECT_EQ(pIL2->get_int(&i),S_OK);
    EXPECT_EQ(i,5);

    pUnk->Release();
    pIL->Release();
    pIL2->Release();


}

TEST_F(MyFixture, check_query_interface_returns_e_no_interface_on_when_interface_not_found)
{


	auto pbase = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_, "CreateTestLayout").get_portable_base();
	IUnknown* pUnk = reinterpret_cast<IUnknown*>(pbase);
	ITestLayoutPure* pIL = 0;
	EXPECT_EQ(
		pUnk->QueryInterface(cross_compiler_interface::use_unknown<IUnknownDerivedInterface>::uuid_type::get_windows_guid<GUID>(), reinterpret_cast<void**>(&pIL)),
		E_NOINTERFACE);
	pUnk->Release();

}

#endif

TEST_F(MyFixture,check_out_parms)
{

    std::string s;
    std::string expected = "out_string";
    iTest.get_out_string(&s);

    EXPECT_EQ(s,expected);
    s.clear();
    
    iTestMemFn.get_out_string(&s);
    EXPECT_EQ(s,expected);

    

}

// cr_string tests copied from boost
//  Should be equal
typedef cross_compiler_interface::cr_string string_ref;

//  Should be equal
void interop ( const std::string &str, string_ref ref ) {
//  EXPECT_TRUE ( str == ref );
    EXPECT_TRUE ( str.size () == ref.size ());
    EXPECT_TRUE ( std::equal ( str.begin (),  str.end (),  ref.begin ()));
    EXPECT_TRUE ( std::equal ( str.rbegin (), str.rend (), ref.rbegin ()));
    }

void null_tests ( const char *p ) {
//  All zero-length string-refs should be equal
    string_ref sr1; // NULL, 0
    string_ref sr2 ( NULL, 0 );
    string_ref sr3 ( p, 0 );
    string_ref sr4 ( p );
    sr4.clear ();
    
    EXPECT_TRUE ( sr1 == sr2 );
    EXPECT_TRUE ( sr1 == sr3 );
    EXPECT_TRUE ( sr2 == sr3 );
    EXPECT_TRUE ( sr1 == sr4 );    
    }

//  make sure that substrings work just like strings
void test_substr ( const std::string &str ) {
    const size_t sz = str.size ();
    string_ref ref ( str );
    
//  Substrings at the end
    for ( size_t i = 0; i <= sz; ++ i )
        interop ( str.substr ( i ), ref.substr ( i ));
        
//  Substrings at the beginning
    for ( size_t i = 0; i <= sz; ++ i )
        interop ( str.substr ( 0, i ), ref.substr ( 0, i ));
        
//  All possible substrings
    for ( size_t i = 0; i < sz; ++i )
        for ( size_t j = i; j < sz; ++j )
            interop ( str.substr ( i, j ), ref.substr ( i, j ));    
    }

//  make sure that removing prefixes and suffixes work just like strings
void test_remove ( const std::string &str ) {
    const size_t sz = str.size ();
    std::string work;
    string_ref ref;
    
    for ( size_t i = 1; i <= sz; ++i ) {
      work = str;
      ref  = str;     
      while ( ref.size () >= i ) {
          interop ( work, ref );
          work.erase ( 0, i );
          ref.remove_prefix (i);
          }
      }
    
    for ( size_t i = 1; i < sz; ++ i ) {
      work = str;
      ref  = str;     
      while ( ref.size () >= i ) {
          interop ( work, ref );
          work.erase ( work.size () - i, i );
          ref.remove_suffix (i);
          }
      }
    }


void ends_with ( const char *arg ) {
    const size_t sz = strlen ( arg );
    string_ref sr ( arg );
    string_ref sr2 ( arg );
    const char *p = arg;

    while ( !*p ) {
        EXPECT_TRUE ( sr.ends_with ( p ));
        ++p;
        }

    while ( !sr2.empty ()) {
        EXPECT_TRUE ( sr.ends_with ( sr2 ));
        sr2.remove_prefix (1);
        }

    sr2 = arg;
    while ( !sr2.empty ()) {
        EXPECT_TRUE ( sr.ends_with ( sr2 ));
        sr2.remove_prefix (1);
        }

    char ch = sz == 0 ? '\0' : arg [ sz - 1 ];
    sr2 = arg;
    if ( sz > 0 )
      EXPECT_TRUE ( sr2.ends_with ( ch ));
    EXPECT_TRUE ( !sr2.ends_with ( ++ch ));
    EXPECT_TRUE ( sr2.ends_with ( string_ref ()));
    }
    
void starts_with ( const char *arg ) {
    const size_t sz = strlen ( arg );
    string_ref sr  ( arg );
    string_ref sr2 ( arg );
    const char *p = arg + std::strlen ( arg ) - 1;
    while ( p >= arg ) {
        std::string foo ( arg, p + 1 );
        EXPECT_TRUE ( sr.starts_with ( foo ));
        --p;
        }

    while ( !sr2.empty ()) {
        EXPECT_TRUE ( sr.starts_with ( sr2 ));
        sr2.remove_suffix (1);
        }

    char ch = *arg;
    sr2 = arg;
  if ( sz > 0 )
    EXPECT_TRUE ( sr2.starts_with ( ch ));
    EXPECT_TRUE ( !sr2.starts_with ( ++ch ));
    EXPECT_TRUE ( sr2.starts_with ( string_ref ()));
    }

void reverse ( const char *arg ) {
//  Round trip
    string_ref sr1 ( arg );
    std::string string1 ( sr1.rbegin (), sr1.rend ());
    string_ref sr2 ( string1 );
    std::string string2 ( sr2.rbegin (), sr2.rend ());

    EXPECT_TRUE ( std::equal ( sr2.rbegin (), sr2.rend (), arg ));
    EXPECT_TRUE ( string2 == arg );
    EXPECT_TRUE ( std::equal ( sr1.begin (), sr1.end (), string2.begin ()));
    }

//	This helper function eliminates signed vs. unsigned warnings
string_ref::size_type ptr_diff ( const char *res, const char *base ) {
    EXPECT_TRUE ( res >= base );
    return static_cast<string_ref::size_type> ( res - base );
    }

void find ( const char *arg ) {
    string_ref sr1;
    string_ref sr2;
    const char *p;

//  Look for each character in the string(searching from the start)
  p = arg;
  sr1 = arg;
  while ( *p ) {
    string_ref::size_type pos = sr1.find(*p);
    EXPECT_TRUE ( pos != string_ref::npos && ( pos <= ptr_diff ( p, arg )));
    ++p;
    }
  
//  Look for each character in the string (searching from the end)
    p = arg;
    sr1 = arg;
    while ( *p ) {
    string_ref::size_type pos = sr1.rfind(*p);
    EXPECT_TRUE ( pos != string_ref::npos && pos < sr1.size () && ( pos >= ptr_diff ( p, arg )));
    ++p;
    }

    sr1 = arg;
    p = arg;
//  for all possible chars, see if we find them in the right place.
//  Note that strchr will/might do the _wrong_ thing if we search for NULL
    for ( int ch = 1; ch < 256; ++ch ) {
        string_ref::size_type pos = sr1.find(static_cast<unsigned char>(ch));
        const char *strp = std::strchr ( arg, ch );
        EXPECT_TRUE (( strp == NULL ) == ( pos == string_ref::npos ));
        if ( strp != NULL )
            EXPECT_TRUE ( ptr_diff ( strp, arg ) == pos );
    }

    sr1 = arg;
    p = arg;
//  for all possible chars, see if we find them in the right place.
//  Note that strchr will/might do the _wrong_ thing if we search for NULL
    for ( int ch = 1; ch < 256; ++ch ) {
        string_ref::size_type pos = sr1.rfind(static_cast<unsigned char>(ch));
        const char *strp = std::strrchr ( arg, ch );
        EXPECT_TRUE (( strp == NULL ) == ( pos == string_ref::npos ));
        if ( strp != NULL )
            EXPECT_TRUE ( ptr_diff ( strp, arg ) == pos );
    }


//  Find everything at the start
    p = arg;
    sr1 = arg;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find(*p);
        EXPECT_TRUE ( pos == 0 );
        sr1.remove_prefix (1);
        ++p;
        }

//  Find everything at the end
    sr1  = arg;
    p    = arg + strlen ( arg ) - 1;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.rfind(*p);
        EXPECT_TRUE ( pos == sr1.size () - 1 );
        sr1.remove_suffix (1);
        --p;
        }
        
//  Find everything at the start
    sr1  = arg;
    p    = arg;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find_first_of(*p);
        EXPECT_TRUE ( pos == 0 );
        sr1.remove_prefix (1);
        ++p;
        }


//  Find everything at the end
    sr1  = arg;
    p    = arg + strlen ( arg ) - 1;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find_last_of(*p);
        EXPECT_TRUE ( pos == sr1.size () - 1 );
        sr1.remove_suffix (1);
        --p;
        }
        
//  Basic sanity checking for "find_first_of / find_first_not_of"
    sr1 = arg;
    sr2 = arg;
    while ( !sr1.empty() ) {
        EXPECT_TRUE ( sr1.find_first_of ( sr2 )     == 0 );
        EXPECT_TRUE ( sr1.find_first_not_of ( sr2 ) == string_ref::npos );
        sr1.remove_prefix ( 1 );
        }

    p = arg;
    sr1 = arg;
    while ( *p ) {
        string_ref::size_type pos1 = sr1.find_first_of(*p);
        string_ref::size_type pos2 = sr1.find_first_not_of(*p);
        EXPECT_TRUE ( pos1 != string_ref::npos && pos1 < sr1.size () && pos1 <= ptr_diff ( p, arg ));
        if ( pos2 != string_ref::npos ) {
            for ( size_t i = 0 ; i < pos2; ++i )
                EXPECT_TRUE ( sr1[i] == *p );
            EXPECT_TRUE ( sr1 [ pos2 ] != *p );
            }

        EXPECT_TRUE ( pos2 != pos1 );
        ++p;
        }
        
//  Basic sanity checking for "find_last_of / find_last_not_of"
    sr1 = arg;
    sr2 = arg;
    while ( !sr1.empty() ) {
        EXPECT_TRUE ( sr1.find_last_of ( sr2 )     == ( sr1.size () - 1 ));
        EXPECT_TRUE ( sr1.find_last_not_of ( sr2 ) == string_ref::npos );
        sr1.remove_suffix ( 1 );
        }

    p = arg;
    sr1 = arg;
    while ( *p ) {
        string_ref::size_type pos1 = sr1.find_last_of(*p);
        string_ref::size_type pos2 = sr1.find_last_not_of(*p);
        EXPECT_TRUE ( pos1 != string_ref::npos && pos1 < sr1.size () && pos1 >= ptr_diff ( p, arg ));
        EXPECT_TRUE ( pos2 == string_ref::npos || pos1 < sr1.size ());
        if ( pos2 != string_ref::npos ) {
            for ( size_t i = sr1.size () -1 ; i > pos2; --i )
                EXPECT_TRUE ( sr1[i] == *p );
            EXPECT_TRUE ( sr1 [ pos2 ] != *p );
            }
         
        EXPECT_TRUE ( pos2 != pos1 );
        ++p;
        }

    }
    



    const char *test_strings [] = {
    "",
    "1",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "0123456789",
    NULL
    };

const char *test_strings2 [] = {
    "",
    "0",
    "abc",
    "AAA",  // all the same
    "adsfadadiaef;alkdg;aljt;j agl;sjrl;tjs;lga;lretj;srg[w349u5209dsfadfasdfasdfadsf",
    "abc\0asdfadsfasf",
    NULL
    };

//Boost tests from string_ref_test1.cpp
TEST_F(MyFixture,test_cr_string1)
{
    using cross_compiler_interface::cr_string;

    const char **p = &test_strings[0];
    
    while ( *p != NULL ) {
        interop ( *p, *p );
        test_substr ( *p );
        test_remove ( *p );
        null_tests ( *p );
    
        p++;
        }


    

}

// Boost tests from string_ref_test2.cpp
TEST_F(MyFixture,test_cr_string2)
{
    using cross_compiler_interface::cr_string;

    const char **p = &test_strings2[0];
    
    while ( *p != NULL ) {
        starts_with ( *p );
        ends_with ( *p );
        reverse ( *p );
        find ( *p );

        p++;
        }



    

}

// Check pass and return
TEST_F(MyFixture,test_cr_string3)
{
    using cross_compiler_interface::cr_string;

    std::string s = "Hello";

    iTest.append_string(s);
    iTest.append_string(" World");

    auto crs = iTest.get_string();

    EXPECT_EQ(crs,cr_string("Hello World"));



    

}

TEST_F(MyFixture,test_vector2)
{
    std::vector<std::string> svec;
    svec.push_back("Name");
    svec.push_back("is");
    svec.push_back(std::string());



    auto svecret = iTest.append_hello_to_vector(svec);

    auto svecexpected = svec;
    svecexpected.push_back("hello");
    EXPECT_TRUE(collection_equal(svecret,svecexpected));


    std::vector<std::uint32_t> ivec;
    ivec.push_back(100);
    ivec.push_back(0);
    ivec.push_back(70000);

    auto ivecret = iTest.append_5_to_vector(ivec);

    auto ivecexpected = ivec;
    ivecexpected.push_back(5);
	EXPECT_TRUE(collection_equal(ivecret,ivecexpected));



    

}

TEST_F(MyFixture,object_count_tests)
{
    auto get_object_count = m_.load_module_function<std::size_t(*)()>("GetObjectCount");
    {
    auto unk = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterface");
    
    EXPECT_EQ(get_object_count(),1);
    use_interface<cross_compiler_interface::InterfaceUnknown> unk2 = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterfaceOnly");
    EXPECT_EQ(get_object_count(),2);
    unk2.Release();
    EXPECT_EQ(get_object_count(),1);


    }

    EXPECT_EQ(get_object_count(),0);




}

TEST_F(MyFixture,u16_32_string){

    char32_t a32('a');
    char16_t a16('a');

    auto r32 = iTest.test_u32_string(std::u32string(1,a32));
    EXPECT_TRUE(r32==std::u32string(2,a32));

    auto r16 = iTest.test_u16_string(std::u16string(1,a16));
    EXPECT_TRUE(r16==std::u16string(2,a16));



}


template<class T>
struct Introspected:cross_compiler_interface::define_unknown_interface<T,
    	// {83BEA17A-68C3-40A7-8504-F67CF0A31C1A}
	cross_compiler_interface::uuid<
	0x83BEA17A,0x68C3,0x40A7,0x8504,0xF67CF0A31C1A
    >>
{
    cross_compiler_interface::cross_function<Introspected,0,int(int)> f1;
    cross_compiler_interface::cross_function<Introspected,1,void()> f2;

Introspected():f1(this),f2(this){}
};

CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(Introspected,
                                                      f1,f2)

TEST(Introspection,test_introspection1){


    auto info = cross_compiler_interface::get_interface_information<Introspected>();



    auto unknowninfo = cross_compiler_interface::get_interface_information<cross_compiler_interface::InterfaceUnknown>();


    EXPECT_EQ(unknowninfo.get_function(0).name,"QueryInterfaceRaw");

    struct ImpIntrospected:public cross_compiler_interface::implement_unknown_interfaces<ImpIntrospected,Introspected>{

        ImpIntrospected(){
            auto imp = get_implementation<Introspected>();


            imp->f1 = [](int i){
                return 2*i;
            };
        }
        

    };
    auto testinfo = cross_compiler_interface::get_interface_information<TestInterface>();
    auto &mptrs = cross_compiler_interface::type_information<cross_compiler_interface::use_interface<TestInterface>>::get_ptrs_to_members();
	(void)mptrs;
    auto pimp = ImpIntrospected::create();
    std::vector<cross_compiler_interface::any> vany;
    vany.push_back(cross_compiler_interface::any(int(5)));
    auto& c = cross_compiler_interface::type_information<cross_compiler_interface::use_unknown<Introspected>>::names();
	(void)c;
    auto iany = info.get_function(0).call(pimp,vany);
    auto i = cross_compiler_interface::any_cast<int>(iany);

    EXPECT_EQ(i,10);




}




struct InitializeComponentMap{

    InitializeComponentMap(){
        cppcomponents::factory::add_mapping("Test.Component","unit_test_dll");

    }

};

InitializeComponentMap icm_;

TEST(Component,test_component1){

    TestComponent t;
   auto s = t.Test();
   EXPECT_EQ(s,"Hello Components");


}


TEST(Component,test_component_with_constructor){

    TestComponentWithConstructor t("This is a test");

   auto s = t.Test();
   EXPECT_EQ(s,"This is a test");


}


TEST(Component,test_component_with_static){

    

   auto s = TestComponentWithStatic::static_interface().GetStaticString();
   EXPECT_EQ(s,"Hello from static method");

   TestComponentWithStatic t;
   s = t.Test();
   EXPECT_EQ(s,"Hello Components");


   TestComponentWithStatic t2 = TestComponentWithStatic::from_interface(TestComponentWithStatic::static_interface().GetTestComponent());
   s = t2.Test();

   EXPECT_EQ(s,"Returned component");


}
TEST(Component,test_component_with_static_static_notation){

    

   auto s = TestComponentWithStatic::GetStaticString();
   EXPECT_EQ(s,"Hello from static method");

   s = TestComponentWithStatic::GetStaticString2("John");
   EXPECT_EQ(s,"Hello John");

   TestComponentWithStatic t;
   s = t.Test();
   EXPECT_EQ(s,"Hello Components");


   TestComponentWithStatic t2 = TestComponentWithStatic::from_interface(TestComponentWithStatic::GetTestComponent());
   s = t2.Test();

   EXPECT_EQ(s,"Returned component");


}


TEST(Component,test_component_module_string_constructor){
    std::string m("unit_test_dll");
    std::string class_name1 = "Test.Component";
    std::string class_name2 = "Test.Component.2";
    auto t = TestComponent::dynamic_creator(m,class_name1)();
    auto s = t.Test();
    EXPECT_EQ(s,"Hello Components");

    auto t2 = TestComponentWithConstructor::dynamic_creator(m,class_name2)("This is a test");

    s = t2.Test();
    EXPECT_EQ(s,"This is a test");

}

TEST(Component,test_component_with_module_string_static){

    std::string m("unit_test_dll");
    std::string class_name = "unit_test_dll!Test.Component.3";

    

   auto s = TestComponentWithStatic::static_interface(m,class_name).GetStaticString();
   EXPECT_EQ(s,"Hello from static method");

   auto t = TestComponentWithStatic::dynamic_creator(m,class_name)();
   s = t.Test();
   EXPECT_EQ(s,"Hello Components");


   TestComponentWithStatic t2 = TestComponentWithStatic::from_interface(TestComponentWithStatic::static_interface(m,class_name).GetTestComponent());
   s = t2.Test();

   EXPECT_EQ(s,"Returned component");


}


TEST(Component, test_component_with_multiple_static){



	auto s = TestComponentWithMultipleStatic::GetStaticString();
	EXPECT_EQ(s, "Hello from static method");

	s = TestComponentWithStatic::GetStaticString2("John");
	EXPECT_EQ(s, "Hello John");

	s = TestComponentWithMultipleStatic::GetStaticStringOtherInterface();

	EXPECT_EQ(s, "Hello from second static interface");


}

TEST(Component, test_component_with_multiple_static_static_interface_notation){



	auto s = TestComponentWithMultipleStatic::static_interface().GetStaticString();
	EXPECT_EQ(s, "Hello from static method");

	s = TestComponentWithStatic::static_interface().GetStaticString2("John");
	EXPECT_EQ(s, "Hello John");

	s = TestComponentWithMultipleStatic::static_interface().QueryInterface<StaticInterface4>().GetStaticStringOtherInterface();

	EXPECT_EQ(s, "Hello from second static interface");


}

TEST(Component, component_with_inherited_interfaces){

	TestComponentWithInheritedInterfaces t;
	auto s = t.HelloFromInherited();
	EXPECT_EQ(s,"Hello from Inherited");

	s = t.Test();
	EXPECT_EQ(s, "Test");

	s = TestComponentWithInheritedInterfaces::GetStaticString();
	EXPECT_EQ(s, "StaticString");

	auto c = TestComponentWithInheritedInterfaces::GetTestComponent();
	s = c.Test();
	EXPECT_EQ(s, "Hello Components");


}

TEST(Component, component_with_forced_prefix_interface_map){

	TestComponentWithForcedPrefixInterfaces t;
	auto s = t.Test();
	EXPECT_EQ(s, "Test");

	s = TestComponentWithForcedPrefixInterfaces::StaticMethod();
	EXPECT_EQ(s, "StaticMethod");

}

TEST(Component, component_with_multiple_interfaces_with_same_method){

	TestComponentWithMultipleInterfaces t;
	// Correctly gives compiler error for ambiguity
	// auto s = t.Test();

	auto s = t.as<ComponentInterface>().Test();

	EXPECT_EQ(s, "ComponentInterface_Test");

	s = t.as<InterfaceTestComponentWithForcedPrefixInterfaces>().Test();

	EXPECT_EQ(s, "InterfaceTestComponentWithForcedPrefixInterfaces_Test");

}


TEST(Component, component_with_runtime_inheritance){

	TestComponentWithRuntimeInheritance t;
	auto s = t.Test();
	EXPECT_EQ(s, "Test");
	s = t.HelloFromInherited();
	EXPECT_EQ(s, "I overrode this");


}



TEST(Component, property_test){

	Person p;
	auto s = p.Name();
	EXPECT_EQ(s, "John");

	p.Name = "William";
	s = p.Name;
	EXPECT_EQ(s, "William");

	


	auto age = p.AgeReadOnly();
	EXPECT_EQ(age, 21);

	p.AgeWriteOnly = 5;
	age = p.AgeReadOnly;
	EXPECT_EQ(age, 5);

	p.AgeWriteOnly(16);

	age = p.AgeReadOnly;
	EXPECT_EQ(age, 16);







}

TEST(Component, event_test_1){

	PersonWithEvent p;

	bool called = false;
	p.NameChanged += [&called](std::string n){
		called = true;
	};

	p.Name = "William";

	EXPECT_EQ(called, true);

}

TEST(Component, event_test_remove){

	PersonWithEvent p;

	bool called = false;
	auto tok = p.NameChanged += [&called](std::string n){
		called = true;
	};

	p.Name = "William";

	EXPECT_EQ(called, true);

	called = false;
	bool called2 = false;
	auto tok2 = p.NameChanged +=[&called2](std::string n){
		called2 = true;
	};

	p.Name = "John";

	EXPECT_EQ(called, true);
	EXPECT_EQ(called2, true);

	called = false;
	called2 = false;

	p.NameChanged -= tok;

	p.Name = "Sarah";

	EXPECT_EQ(called, false);
	EXPECT_EQ(called2, true);

	p.NameChanged -= tok2;

	called = false;
	called2 = false;
	p.Name = "Jane";
	EXPECT_EQ(called, false);
	EXPECT_EQ(called2, false);




}


TEST(Component, test_wstring_cr_wstring){
	TestWString t;

	auto s = t.Concat(L"Hello ", L"John");

	EXPECT_EQ(s, L"Hello John");

}

TEST(Component, test_tuple2){
	TestTuple test;

	auto t1 = test.Get2();

	auto t = std::make_tuple(std::string("Hello World"), std::string("Hello World"));

	EXPECT_EQ(t, t1);

}
TEST(Component, test_tuple3){
	TestTuple test;

	auto t1 = test.Get3();

	auto t = std::make_tuple(1, std::string("Hello World"), 2.5);
	EXPECT_EQ(t, t1);

}
TEST(Component, test_tuple4){
	TestTuple test;

	auto t1 = test.Get4();

	auto t = std::make_tuple(1, std::string("Hello World"), 2.5,'a');
	EXPECT_EQ(t, t1);

}
TEST(Component, test_tuple5){
	TestTuple test;

	auto t1 = test.Get5();

	auto itest = std::get<4>(t1);

	auto t2 = itest.Get4();

	auto t = std::make_tuple(1, std::string("Hello World"), 2.5,'a');
	EXPECT_EQ(t, t2);



}
TEST(Component, test_tuple1){
	TestTuple test;

	auto t1 = test.Get1();

	auto t = std::make_tuple(std::string("Hello World"));
	EXPECT_EQ(t, t1);



}

TEST(Component, test_use_equality){

	TestComponent t;
	auto ic = t.QueryInterface<ComponentInterface>();

	auto iunk = t.QueryInterface<cppcomponents::InterfaceUnknown>();

	EXPECT_EQ(ic, iunk);

	cppcomponents::use<ComponentInterface> ic2;
	EXPECT_NE(ic, ic2);

	TestComponent t2;
	auto ic3 = t2.QueryInterface<ComponentInterface>();
	EXPECT_NE(ic,ic3);


}

#include <set>

TEST(Component, test_less){

	std::set<cppcomponents::use<ComponentInterface>> s;
	TestComponent t;
	auto ic = t.QueryInterface<ComponentInterface>();
	auto iunk = t.QueryInterface<cppcomponents::InterfaceUnknown>();


	cppcomponents::use<ComponentInterface> ic2;

	TestComponent t2;
	auto ic3 = t2.QueryInterface<ComponentInterface>();

	s.insert(ic);
	s.insert(ic2);
	s.insert(ic3);
	s.insert(iunk.QueryInterface<ComponentInterface>());

	EXPECT_EQ(s.size(), 3);
	

}
TEST(Component, test_greater){

	std::set<cppcomponents::use<ComponentInterface>,std::greater<cppcomponents::use<ComponentInterface>>> s;
	TestComponent t;
	auto ic = t.QueryInterface<ComponentInterface>();
	auto iunk = t.QueryInterface<cppcomponents::InterfaceUnknown>();


	cppcomponents::use<ComponentInterface> ic2;

	TestComponent t2;
	auto ic3 = t2.QueryInterface<ComponentInterface>();

	s.insert(ic);
	s.insert(ic2);
	s.insert(ic3);
	s.insert(iunk.QueryInterface<ComponentInterface>());

	EXPECT_EQ(s.size(), 3);


}

#include<unordered_set>
	
TEST(Component, test_hash){

	std::unordered_set<cppcomponents::use<ComponentInterface>> s;
	TestComponent t;
	auto ic = t.QueryInterface<ComponentInterface>();
	auto iunk = t.QueryInterface<cppcomponents::InterfaceUnknown>();


	cppcomponents::use<ComponentInterface> ic2;

	TestComponent t2;
	auto ic3 = t2.QueryInterface<ComponentInterface>();

	s.insert(ic);
	s.insert(ic2);
	s.insert(ic3);
	s.insert(iunk.QueryInterface<ComponentInterface>());

	EXPECT_EQ(s.size(), 3);


}

TEST(Component, pure_static_interface){

	auto s = TestPureStatic::GetStaticStringOtherInterface();

	EXPECT_EQ(s, "Hello from second static interface");
}


#include "../cppcomponents/future.hpp"
#include <chrono>


TEST(Future, future1){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t = 0;

	auto f = cppcomponents::async(e,[](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 5;
	});

	f.Then([&done, &t](cppcomponents::use < cppcomponents::IFuture < int >> res){
		t = res.Get();
		done.store(true);
	});

	while (done.load() == false);

	EXPECT_EQ(t, 5);
}


TEST(Future, future_string){
	TestFuture t;

	std::string s;
	std::atomic<bool> done(false);

	auto f = t.GetFutureString();

	f.Then([&s, &done](cppcomponents::use < cppcomponents::IFuture<std::string > > res){
		s = res.Get();
		done.store(true);
	});

	while (done.load() == false);
	std::string expected = "Hello Futures";
	EXPECT_EQ(s,expected);

}

TEST(Future, future_exception){
	TestFuture t;

	std::atomic<bool> done(false);

	auto f = t.GetFutureWithException();

	f.Then([&done](cppcomponents::use < cppcomponents::IFuture<std::string > > res){
		done.store(true);
	});

	while (done.load() == false);

	EXPECT_THROW(f.Get(), cppcomponents::error_invalid_arg);

}
TEST(Future, future_get_throws_if_not_done){
	TestFuture t;

	std::atomic<bool> done(false);


	auto f = cppcomponents::implement_future_promise<int>::create().QueryInterface<cppcomponents::IFuture<int>>();



	EXPECT_THROW(f.Get(), cppcomponents::error_pending);

}


TEST(Future, future_immediate){
	TestFuture t;

	std::atomic<bool> done(false);

	auto f = t.GetImmediateFuture();
	EXPECT_EQ(f.Get(), 5);

}

TEST(Future, future_wrapped){
	TestFuture t;

	int result = 0;
	std::atomic<bool> done(false);

	auto f = t.GetWrappedFuture();

	cppcomponents::use<cppcomponents::IFuture<int>> f2 = f.Unwrap();
	f2.Then([&result, &done](cppcomponents::use < cppcomponents::IFuture<int> > res){
		result = res.Get();
		done.store(true);
	});

	while (done.load() == false);
	EXPECT_EQ(result,42);

}

TEST(Future, future_unwrap_normal_future){
	TestFuture t;

	std::atomic<bool> done(false);

	auto f = t.GetImmediateFuture();
	cppcomponents::use<cppcomponents::IFuture<int>> f2 = f.Unwrap();
	EXPECT_EQ(f2.Get(), 5);

}

TEST(Future, future_wrapped_chained){
	TestFuture t;

	int result = 0;
	std::atomic<bool> done(false);

	auto f = t.GetWrappedFuture();

	cppcomponents::use<cppcomponents::IFuture<int>> f2 = f.Unwrap();
	f2.Then([&result, &done](cppcomponents::use < cppcomponents::IFuture<int> > res){
		return res.Get();
		
	}).Then([&result](cppcomponents::use<cppcomponents::IFuture<int>> res){
		result = 2 * res.Get();
	}).Then([&done](cppcomponents::use<cppcomponents::IFuture<void>> res){
		res.Get();
		done.store(true);
	});

	while (done.load() == false);
	EXPECT_EQ(result, 84);

}
TEST(Future, overloaded_async){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t = 0;

	auto f = cppcomponents::async(e,nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 5;
	});

	f.Then([&done, &t](cppcomponents::use < cppcomponents::IFuture < int >> res){
		t = res.Get();
		done.store(true);
	});

	while (done.load() == false);

	EXPECT_EQ(t, 5);
}
TEST(Future, when_all_vector){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t0 = 0;
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;


	auto f1 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 6;
	});	
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 7;
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 8;
	});
	using namespace cppcomponents;
	std::vector < use < IFuture < int >>> vec;
	vec.push_back(f1);
	vec.push_back(f2);
	vec.push_back(f3);
	vec.push_back(f4);

	auto fut = when_all(vec).Then(
		[&](Future<void> )mutable{
		
			auto& v = vec;
			t0 = v.at(0).Get();
			t1 = v.at(1).Get();
			t2 = v.at(2).Get();
			t3 = v.at(3).Get();
			done.store(true);
	});



	while (done.load() == false);

	EXPECT_EQ(t0, 5);
	EXPECT_EQ(t1, 6);
	EXPECT_EQ(t2, 7);
	EXPECT_EQ(t3, 8);
}
void test_when_all_tuple();

TEST(Future, when_all_tuple){
	test_when_all_tuple();
}

void test_when_all_tuple_empty();
TEST(Future, when_all_tuple_empty){
	test_when_all_tuple_empty();
}
void test_when_any_tuple();
TEST(Future, when_any_tuple){
	test_when_any_tuple();
}
void test_when_any_vector();
TEST(Future, when_any_vector){
	test_when_any_vector();
}

void test_when_any_tuple_empty();
TEST(Future, when_any_tuple_empty){
	test_when_any_tuple_empty();
}


TEST(Chrono, test_chrono){

	auto now = std::chrono::system_clock::now();
	auto two_hours = std::chrono::system_clock::duration{ std::chrono::hours{ 2 } };

	auto expected_time = now + two_hours;

	TestChrono t;
	auto time_result = t.AddTime(now, two_hours);

	EXPECT_EQ(expected_time, time_result);

	auto duration_result = t.SubtractTime(time_result, now);
	EXPECT_EQ(two_hours, duration_result);

}

TEST(Component, templated_constructor){

	TestTemplatedConstructor t([](){return 5; });

	auto result = t.CallDelegate();

	EXPECT_EQ(5, result);

}

TEST(Future, test_future_error_code){
	using namespace cppcomponents;

	auto p = make_promise<int>();
	p.SetError(error_access_denied::ec);

	auto f = p.QueryInterface<IFuture<int>>();
	EXPECT_EQ(error_access_denied::ec, f.ErrorCode());

	EXPECT_THROW(f.Get(), error_access_denied);

}
TEST(Future, test_future_error_code_void){
	using namespace cppcomponents;

	auto p =make_promise<void>();
	p.SetError(error_access_denied::ec);

	auto f = p.QueryInterface<IFuture<void>>();
	EXPECT_EQ(error_access_denied::ec, f.ErrorCode());

	EXPECT_THROW(f.Get(), error_access_denied);

}


#include "../cppcomponents/channel.hpp"

void test_channel_1(); 
TEST(Channel, test_channel_1){
	test_channel_1();
}
void test_channel_2();
TEST(Channel, test_channel_2){

	test_channel_2();


}

#include "../cppcomponents/loop_executor.hpp"

TEST(LoopExecutor, loop_executor_1){

	using namespace cppcomponents;
	LoopExecutor executor;

	int i = 0;

	auto f = [&](){
		i++;
		executor.Add([&](){i++; });
	};
	executor.Add(f);
	executor.Add(f);
	executor.Add(f);

	executor.RunQueuedClosures();
	EXPECT_EQ(3, i);

	

	executor.Add([&](){executor.MakeLoopExit(); });

	executor.MakeLoopExit();

	executor.Loop();

	EXPECT_EQ(6, i);

	EXPECT_FALSE(executor.TryOneClosure());



}

#include "../cppcomponents/buffer.hpp"

TEST(Buffer, buffer_test_1){

	auto buf = cppcomponents::Buffer::Create(128);
	auto p = buf.Begin();
	buf.get_portable_base_addref();
	auto buf2 = cppcomponents::Buffer::OwningIBufferFromPointer(p);
	auto buf3 = cppcomponents::Buffer::NonOwningIBufferFromPointer(p);

	EXPECT_EQ(128, buf3.Capacity());

	buf3.SetSize(64);

	EXPECT_EQ(64, buf.Size());


}

TEST(Buffer, buffer_test_2){

	auto buf = cppcomponents::Buffer::Create(128);


	char str [] = "Hello World\n";
	std::memcpy(buf.Begin(), &str[0], sizeof(str)-1);

	buf.SetSize(sizeof(str)-1);

	std::string s2{ buf.Begin(), buf.End() };

	EXPECT_EQ("Hello World\n", s2);
}
void test_channel_on_closed();
TEST(Channel, test_channel_on_closed){
	test_channel_on_closed();

}

TEST(Component, inherited_interfaces){
	cppcomponents::use<ComponentInterface> i = TestComponent();
	cppcomponents::use<cppcomponents::InterfaceUnknown> iu = i;

}


TEST(Component, static_interface_extras){
	auto s = TestComponentWithStatic::OtherGetStaticString();
	EXPECT_EQ(s, "Hello from static method");
	
}



void test_channel_completed1();
TEST(Channel, test_channel_completed1){
	test_channel_completed1();
}



void test_channel_write_after_complete_throws();
TEST(Channel, test_channel_write_after_complete_throws){
	test_channel_write_after_complete_throws();
}
void test_channel_read_pending_writes_then_throw();
TEST(Channel, test_channel_read_pending_writes_then_throw){
	test_channel_read_pending_writes_then_throw();
}
