#define BOOST_TEST_MODULE cross_interface_unit_tests
#include <boost/test/unit_test.hpp>
#include "unit_test_interface.h"

struct MyFixture{
    cross_compiler_interface::module m_;

    cross_compiler_interface::use_interface<TestInterface> iTest;

    cross_compiler_interface::use_interface<TestInterface> iTestMemFn;
    MyFixture():m_("unit_test_dll"),iTest(cross_compiler_interface::create<TestInterface>(m_,"CreateTestInterface")),
        iTestMemFn(cross_compiler_interface::create<TestInterface>(m_,"CreateTestMemFnInterface"))
    {
        
    }

};
BOOST_FIXTURE_TEST_CASE(Test_base,MyFixture)
{
   std::string expected = "Hello from Base";
   BOOST_CHECK_EQUAL(expected, iTest.hello_from_base());
   BOOST_CHECK_EQUAL(expected, iTestMemFn.hello_from_base());
  
}

BOOST_FIXTURE_TEST_CASE(Integer_manipulation,MyFixture)
{
   auto expected = 10;
   BOOST_CHECK_EQUAL(expected, iTest.plus_5(5));
   BOOST_CHECK_EQUAL(expected, iTestMemFn.plus_5(5));
}
BOOST_FIXTURE_TEST_CASE(Double_manipulation,MyFixture)
{
   auto expected = 10.0;
   BOOST_CHECK_EQUAL(expected, iTest.times_2point5(4));
   BOOST_CHECK_EQUAL(expected, iTestMemFn.times_2point5(4));
}

BOOST_FIXTURE_TEST_CASE(Manipulation_of_int_reference,MyFixture)
{
   auto expected = 2;
   int i = 4;
   // Todo FIX
   iTest.double_referenced_int(&i);
  BOOST_CHECK_EQUAL(expected,i);

   i = 4;
   iTestMemFn.double_referenced_int(&i);
  BOOST_CHECK_EQUAL(expected,i);
}

BOOST_FIXTURE_TEST_CASE(string_manipulation_1,MyFixture)
{
    std::string s= "Hello World\n";
   auto expected = s.size();
   BOOST_CHECK_EQUAL(expected, iTest.count_characters(s));
   BOOST_CHECK_EQUAL(expected, iTestMemFn.count_characters(s));
}

BOOST_FIXTURE_TEST_CASE(string_manipulation_2,MyFixture)
{
    std::string s= "John";
   auto expected = "Hello " + s;
   BOOST_CHECK_EQUAL(expected, iTest.say_hello(s));
   BOOST_CHECK_EQUAL(expected, iTestMemFn.say_hello(s));
}

BOOST_FIXTURE_TEST_CASE(Exception_handling_1,MyFixture)
{
    std::string s= "John";
   BOOST_CHECK_THROW(iTest.use_at_out_of_range(s),std::out_of_range);
   BOOST_CHECK_THROW(iTestMemFn.use_at_out_of_range(s),std::out_of_range);
}

BOOST_FIXTURE_TEST_CASE(Exception_handling_2,MyFixture)
{

   BOOST_CHECK_THROW(iTest.not_implemented(),cross_compiler_interface::error_not_implemented);
   BOOST_CHECK_THROW(iTestMemFn.not_implemented(),cross_compiler_interface::error_not_implemented);
}

BOOST_FIXTURE_TEST_CASE(vector_of_strings,MyFixture)
{
    std::vector<std::string> expected;
    expected.push_back("This");
    expected.push_back("is");
    expected.push_back("a");
    expected.push_back("test");


    std::string s= "This is a test";

    BOOST_CHECK(expected == iTest.split_into_words(s));
    BOOST_CHECK(expected == iTestMemFn.split_into_words(s));

}

BOOST_FIXTURE_TEST_CASE(Passed_in_interface,MyFixture)
{
    std::string s= "John";
   auto expected = "Hello " + s;

   cross_compiler_interface::implement_interface<IGetName> ign;
   ign.get_name = [s](){return s;};
   BOOST_CHECK_EQUAL(expected,iTest.say_hello2(ign.get_use_interface()));
   BOOST_CHECK_EQUAL(expected,iTestMemFn.say_hello2(ign.get_use_interface()));
}

BOOST_FIXTURE_TEST_CASE(std_pair,MyFixture)
{
    std::pair<int,std::string> expected(3,"test");
    std::vector<std::string> v;
    v.push_back("This");
    v.push_back("is");
    v.push_back("a");
    v.push_back("test");

    BOOST_CHECK(expected == iTest.get_string_at(v,3));
    BOOST_CHECK(expected == iTestMemFn.get_string_at(v,3));
}


BOOST_FIXTURE_TEST_CASE(returned_interface,MyFixture)
{
    std::string expected = "Hello from returned interface";

    BOOST_CHECK(expected == iTest.get_igetname().get_name());
    BOOST_CHECK(expected == iTestMemFn.get_igetname().get_name());
}


BOOST_FIXTURE_TEST_CASE(runtime_parent,MyFixture)
{
    std::string expected = "TestImplementation";
    int iexpected = 15;

    BOOST_CHECK_EQUAL(expected , iTest.get_name_from_runtime_parent());
    BOOST_CHECK_EQUAL(expected , iTestMemFn.get_name_from_runtime_parent());
    BOOST_CHECK_EQUAL(iexpected , iTest.custom_with_runtime_parent(5));
    BOOST_CHECK_EQUAL(iexpected , iTestMemFn.custom_with_runtime_parent(5));
}

BOOST_FIXTURE_TEST_CASE(iuknown_tests,MyFixture)
{
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");
    use_interface<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid::get())));

    BOOST_CHECK(derived.get_portable_base()!=nullptr);
    BOOST_CHECK_EQUAL(3,unk.AddRef());
    BOOST_CHECK_EQUAL(2,unk.Release());

    BOOST_CHECK(unk.QueryInterfaceRaw(&cross_compiler_interface::uuid<0,0,0,0,0,0,0,0,0,0,0>::get()) == nullptr);


    BOOST_CHECK_EQUAL(1,unk.Release());



    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";

    BOOST_CHECK_EQUAL(expected , derived.hello_from_iuknown_derived());

    use_interface<IUnknownDerivedInterface2> derived2(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface2>(derived.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface2>::uuid::get())));
    BOOST_CHECK_EQUAL(1,derived.Release());
    BOOST_CHECK(derived2.get_portable_base()!=nullptr);
    BOOST_CHECK_EQUAL(expected2 , derived2.hello_from_iuknown_derived2());

    use_interface<IUnknownDerivedInterface2Derived> derived2derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface2Derived>(
        derived2.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface2Derived>::uuid::get())));
    BOOST_CHECK_EQUAL(1,derived2.Release());

    BOOST_CHECK(derived2derived.get_portable_base()!=nullptr);
    BOOST_CHECK_EQUAL(expected3 , derived2derived.hello_from_derived());

    BOOST_CHECK_EQUAL(0,derived2derived.Release());

}

BOOST_FIXTURE_TEST_CASE(use_unknown_test,MyFixture)
{
    using cross_compiler_interface::use_unknown;
    
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");

    {
    use_unknown<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid::get())),false);

    BOOST_CHECK(!!derived);

    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";

    BOOST_CHECK_EQUAL(expected , derived.hello_from_iuknown_derived());

    auto derived2 = derived.QueryInterface<IUnknownDerivedInterface2>();
    BOOST_CHECK(!!derived2);
    BOOST_CHECK_EQUAL(expected2 , derived2.hello_from_iuknown_derived2());

    auto derived2derived = derived2.QueryInterface<IUnknownDerivedInterface2Derived>();

    BOOST_CHECK(!!derived2derived);
    BOOST_CHECK_EQUAL(expected3 , derived2derived.hello_from_derived());

    // Check self copy
    derived2derived = derived2derived;
    BOOST_CHECK(!!derived2derived);
    BOOST_CHECK_EQUAL(expected3 , derived2derived.hello_from_derived());


    use_unknown<cross_compiler_interface::InterfaceUnknown> unk2 = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterface");

    auto d = unk2.QueryInterface<IUnknownDerivedInterface2Derived>();
    BOOST_CHECK(!!d);
    BOOST_CHECK_EQUAL(expected3 , d.hello_from_derived());

    // Check  copy
    derived2derived = d;
    BOOST_CHECK(!!derived2derived);
    BOOST_CHECK_EQUAL(expected3 , derived2derived.hello_from_derived());

    // Check assignment to nullptr
    d = nullptr;
    BOOST_CHECK(!d);
    BOOST_CHECK_THROW(d.hello_from_iuknown_derived2(),cross_compiler_interface::error_pointer);


    BOOST_CHECK_THROW(unk2.QueryInterface<IUnknownDerivedInterfaceUnused>(),cross_compiler_interface::error_no_interface);

    auto du = unk2.QueryInterfaceNoThrow<IUnknownDerivedInterfaceUnused>();
    BOOST_CHECK(!du);




    }

    // If all our cleanup is ok, releasing should make the reference count 0
    BOOST_CHECK_EQUAL(0,unk.Release());

}


BOOST_FIXTURE_TEST_CASE(pass_return_use_unknown,MyFixture)
{
    using cross_compiler_interface::use_unknown;
    
    use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterface");

    {
    use_unknown<IUnknownDerivedInterface> derived(cross_compiler_interface::reinterpret_portable_base<IUnknownDerivedInterface>(unk.QueryInterfaceRaw(&use_interface<IUnknownDerivedInterface>::uuid::get())),false);

    BOOST_CHECK(!!derived);

    std::string expected = "Hello from IuknownDerivedInterface";
    std::string expected2 = "Hello from IuknownDerivedInterface2";
    std::string expected3 = "Hello from derived";


    auto derived2 = derived.QueryInterface<IUnknownDerivedInterface2>();
    BOOST_CHECK(!!derived2);

    auto d = derived2.get_derived();
    BOOST_CHECK(!!d);

    BOOST_CHECK_EQUAL(expected , d.hello_from_iuknown_derived());

    
    BOOST_CHECK_EQUAL(expected , derived2.get_string(d));
    BOOST_CHECK_EQUAL(expected2 , derived2.hello_from_iuknown_derived2());






    }

    // If all our cleanup is ok, releasing should make the reference count 0
    BOOST_CHECK_EQUAL(0,unk.Release());

}

BOOST_FIXTURE_TEST_CASE(check_throws_on_null_use,MyFixture)
{
    using cross_compiler_interface::use_unknown;
    
    auto i = iTest;
    // Check that regular function call and custom function call succeed
    BOOST_CHECK_EQUAL(i.hello_from_base(),   std::string("Hello from Base"));
    BOOST_CHECK_EQUAL(i.plus_5(5),10);

    // Check that regular function call and custom function call fail with nullptr
    i = nullptr;
    BOOST_CHECK_THROW(i.hello_from_base(),cross_compiler_interface::error_pointer);
    BOOST_CHECK_THROW(i.plus_5(5),cross_compiler_interface::error_pointer);

    // Check that regular function call and custom function call succeed after valid assignment
    i = iTest;
    BOOST_CHECK_EQUAL(i.hello_from_base(),   std::string("Hello from Base"));
    BOOST_CHECK_EQUAL(i.plus_5(5),10);




}

BOOST_FIXTURE_TEST_CASE(packing,MyFixture)
{


    BOOST_CHECK_EQUAL(sizeof(cross_compiler_interface::cross_pair<char,double>), 9);
    BOOST_CHECK_EQUAL(sizeof(cross_compiler_interface::cross_string<char>), sizeof(char*)*2);

}

BOOST_FIXTURE_TEST_CASE(check_single_interface_implement_iunknown_interfaces,MyFixture)
{


    auto only = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterfaceOnly").QueryInterface<IUnknownDerivedInterface>();
    std::string expected = "Hello from ImplementIuknownDerivedInterfaceOnly";
    BOOST_CHECK_EQUAL(only.hello_from_iuknown_derived(),expected);

}


// Check that layout is compatible with COM on windows

#ifdef _WIN32

#include <Unknwn.h>
#ifdef uuid_t 
#undef uuid_t
#endif
struct ITestLayoutPure:public IUnknown{
    virtual HRESULT __stdcall set_int(std::int32_t) = 0;
    virtual HRESULT __stdcall add_2_5_to_int(double*) = 0;
};

struct ITestLayout2Pure:public IUnknown{
    virtual HRESULT __stdcall get_int(std::int32_t*) = 0;
};


BOOST_FIXTURE_TEST_CASE(check_com_layout_compatible,MyFixture)
{


    auto pbase = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateTestLayout").get_portable_base();
    IUnknown* pUnk = reinterpret_cast<IUnknown*>(pbase);
    ITestLayoutPure* pIL = 0;
    BOOST_CHECK_EQUAL(
        pUnk->QueryInterface(cross_compiler_interface::use_unknown<ITestLayout>::uuid::get_windows_guid<GUID>(),reinterpret_cast<void**>(&pIL)),
        S_OK);
    BOOST_CHECK(pIL != nullptr);
    pIL->set_int(5);
    double d = 0;
    BOOST_CHECK_EQUAL(pIL->add_2_5_to_int(&d),S_OK);
    BOOST_CHECK_EQUAL(d,7.5);

    ITestLayout2Pure* pIL2 = 0;
    BOOST_CHECK_EQUAL(
        pUnk->QueryInterface(cross_compiler_interface::use_unknown<ITestLayout2>::uuid::get_windows_guid<GUID>(),reinterpret_cast<void**>(&pIL2)),
        S_OK);
    BOOST_CHECK(pIL2 != nullptr);
    int i = 0;
    BOOST_CHECK_EQUAL(pIL2->get_int(&i),S_OK);
    BOOST_CHECK_EQUAL(i,5);

    pUnk->Release();
    pIL->Release();
    pIL2->Release();


}
#endif

BOOST_FIXTURE_TEST_CASE(check_out_parms,MyFixture)
{

    std::string s;
    std::string expected = "out_string";
    iTest.get_out_string(&s);

    BOOST_CHECK_EQUAL(s,expected);
    s.clear();
    
    iTestMemFn.get_out_string(&s);
    BOOST_CHECK_EQUAL(s,expected);

    

}

// cr_string tests copied from boost
//  Should be equal
typedef cross_compiler_interface::cr_string string_ref;

//  Should be equal
void interop ( const std::string &str, string_ref ref ) {
//  BOOST_CHECK ( str == ref );
    BOOST_CHECK ( str.size () == ref.size ());
    BOOST_CHECK ( std::equal ( str.begin (),  str.end (),  ref.begin ()));
    BOOST_CHECK ( std::equal ( str.rbegin (), str.rend (), ref.rbegin ()));
    }

void null_tests ( const char *p ) {
//  All zero-length string-refs should be equal
    string_ref sr1; // NULL, 0
    string_ref sr2 ( NULL, 0 );
    string_ref sr3 ( p, 0 );
    string_ref sr4 ( p );
    sr4.clear ();
    
    BOOST_CHECK ( sr1 == sr2 );
    BOOST_CHECK ( sr1 == sr3 );
    BOOST_CHECK ( sr2 == sr3 );
    BOOST_CHECK ( sr1 == sr4 );    
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
        BOOST_CHECK ( sr.ends_with ( p ));
        ++p;
        }

    while ( !sr2.empty ()) {
        BOOST_CHECK ( sr.ends_with ( sr2 ));
        sr2.remove_prefix (1);
        }

    sr2 = arg;
    while ( !sr2.empty ()) {
        BOOST_CHECK ( sr.ends_with ( sr2 ));
        sr2.remove_prefix (1);
        }

    char ch = sz == 0 ? '\0' : arg [ sz - 1 ];
    sr2 = arg;
    if ( sz > 0 )
      BOOST_CHECK ( sr2.ends_with ( ch ));
    BOOST_CHECK ( !sr2.ends_with ( ++ch ));
    BOOST_CHECK ( sr2.ends_with ( string_ref ()));
    }
    
void starts_with ( const char *arg ) {
    const size_t sz = strlen ( arg );
    string_ref sr  ( arg );
    string_ref sr2 ( arg );
    const char *p = arg + std::strlen ( arg ) - 1;
    while ( p >= arg ) {
        std::string foo ( arg, p + 1 );
        BOOST_CHECK ( sr.starts_with ( foo ));
        --p;
        }

    while ( !sr2.empty ()) {
        BOOST_CHECK ( sr.starts_with ( sr2 ));
        sr2.remove_suffix (1);
        }

    char ch = *arg;
    sr2 = arg;
  if ( sz > 0 )
    BOOST_CHECK ( sr2.starts_with ( ch ));
    BOOST_CHECK ( !sr2.starts_with ( ++ch ));
    BOOST_CHECK ( sr2.starts_with ( string_ref ()));
    }

void reverse ( const char *arg ) {
//  Round trip
    string_ref sr1 ( arg );
    std::string string1 ( sr1.rbegin (), sr1.rend ());
    string_ref sr2 ( string1 );
    std::string string2 ( sr2.rbegin (), sr2.rend ());

    BOOST_CHECK ( std::equal ( sr2.rbegin (), sr2.rend (), arg ));
    BOOST_CHECK ( string2 == arg );
    BOOST_CHECK ( std::equal ( sr1.begin (), sr1.end (), string2.begin ()));
    }

//	This helper function eliminates signed vs. unsigned warnings
string_ref::size_type ptr_diff ( const char *res, const char *base ) {
    BOOST_CHECK ( res >= base );
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
    BOOST_CHECK ( pos != string_ref::npos && ( pos <= ptr_diff ( p, arg )));
    ++p;
    }
  
//  Look for each character in the string (searching from the end)
    p = arg;
    sr1 = arg;
    while ( *p ) {
    string_ref::size_type pos = sr1.rfind(*p);
    BOOST_CHECK ( pos != string_ref::npos && pos < sr1.size () && ( pos >= ptr_diff ( p, arg )));
    ++p;
    }

    sr1 = arg;
    p = arg;
//  for all possible chars, see if we find them in the right place.
//  Note that strchr will/might do the _wrong_ thing if we search for NULL
    for ( int ch = 1; ch < 256; ++ch ) {
        string_ref::size_type pos = sr1.find(ch);
        const char *strp = std::strchr ( arg, ch );
        BOOST_CHECK (( strp == NULL ) == ( pos == string_ref::npos ));
        if ( strp != NULL )
            BOOST_CHECK ( ptr_diff ( strp, arg ) == pos );
    }

    sr1 = arg;
    p = arg;
//  for all possible chars, see if we find them in the right place.
//  Note that strchr will/might do the _wrong_ thing if we search for NULL
    for ( int ch = 1; ch < 256; ++ch ) {
        string_ref::size_type pos = sr1.rfind(ch);
        const char *strp = std::strrchr ( arg, ch );
        BOOST_CHECK (( strp == NULL ) == ( pos == string_ref::npos ));
        if ( strp != NULL )
            BOOST_CHECK ( ptr_diff ( strp, arg ) == pos );
    }


//  Find everything at the start
    p = arg;
    sr1 = arg;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find(*p);
        BOOST_CHECK ( pos == 0 );
        sr1.remove_prefix (1);
        ++p;
        }

//  Find everything at the end
    sr1  = arg;
    p    = arg + strlen ( arg ) - 1;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.rfind(*p);
        BOOST_CHECK ( pos == sr1.size () - 1 );
        sr1.remove_suffix (1);
        --p;
        }
        
//  Find everything at the start
    sr1  = arg;
    p    = arg;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find_first_of(*p);
        BOOST_CHECK ( pos == 0 );
        sr1.remove_prefix (1);
        ++p;
        }


//  Find everything at the end
    sr1  = arg;
    p    = arg + strlen ( arg ) - 1;
    while ( !sr1.empty ()) {
        string_ref::size_type pos = sr1.find_last_of(*p);
        BOOST_CHECK ( pos == sr1.size () - 1 );
        sr1.remove_suffix (1);
        --p;
        }
        
//  Basic sanity checking for "find_first_of / find_first_not_of"
    sr1 = arg;
    sr2 = arg;
    while ( !sr1.empty() ) {
        BOOST_CHECK ( sr1.find_first_of ( sr2 )     == 0 );
        BOOST_CHECK ( sr1.find_first_not_of ( sr2 ) == string_ref::npos );
        sr1.remove_prefix ( 1 );
        }

    p = arg;
    sr1 = arg;
    while ( *p ) {
        string_ref::size_type pos1 = sr1.find_first_of(*p);
        string_ref::size_type pos2 = sr1.find_first_not_of(*p);
        BOOST_CHECK ( pos1 != string_ref::npos && pos1 < sr1.size () && pos1 <= ptr_diff ( p, arg ));
        if ( pos2 != string_ref::npos ) {
            for ( size_t i = 0 ; i < pos2; ++i )
                BOOST_CHECK ( sr1[i] == *p );
            BOOST_CHECK ( sr1 [ pos2 ] != *p );
            }

        BOOST_CHECK ( pos2 != pos1 );
        ++p;
        }
        
//  Basic sanity checking for "find_last_of / find_last_not_of"
    sr1 = arg;
    sr2 = arg;
    while ( !sr1.empty() ) {
        BOOST_CHECK ( sr1.find_last_of ( sr2 )     == ( sr1.size () - 1 ));
        BOOST_CHECK ( sr1.find_last_not_of ( sr2 ) == string_ref::npos );
        sr1.remove_suffix ( 1 );
        }

    p = arg;
    sr1 = arg;
    while ( *p ) {
        string_ref::size_type pos1 = sr1.find_last_of(*p);
        string_ref::size_type pos2 = sr1.find_last_not_of(*p);
        BOOST_CHECK ( pos1 != string_ref::npos && pos1 < sr1.size () && pos1 >= ptr_diff ( p, arg ));
        BOOST_CHECK ( pos2 == string_ref::npos || pos1 < sr1.size ());
        if ( pos2 != string_ref::npos ) {
            for ( size_t i = sr1.size () -1 ; i > pos2; --i )
                BOOST_CHECK ( sr1[i] == *p );
            BOOST_CHECK ( sr1 [ pos2 ] != *p );
            }
         
        BOOST_CHECK ( pos2 != pos1 );
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
BOOST_FIXTURE_TEST_CASE(test_cr_string1,MyFixture)
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
BOOST_FIXTURE_TEST_CASE(test_cr_string2,MyFixture)
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
BOOST_FIXTURE_TEST_CASE(test_cr_string3,MyFixture)
{
    using cross_compiler_interface::cr_string;

    std::string s = "Hello";

    iTest.append_string(s);
    iTest.append_string(" World");

    auto crs = iTest.get_string();

    BOOST_CHECK_EQUAL(crs,cr_string("Hello World"));



    

}

BOOST_FIXTURE_TEST_CASE(test_vector2,MyFixture)
{
    std::vector<std::string> svec;
    svec.push_back("Name");
    svec.push_back("is");
    svec.push_back(std::string());



    auto svecret = iTest.append_hello_to_vector(svec);

    auto svecexpected = svec;
    svecexpected.push_back("hello");
    BOOST_CHECK_EQUAL_COLLECTIONS(svecret.begin(),svecret.end()
        ,svecexpected.begin(),svecexpected.end());


    std::vector<std::uint32_t> ivec;
    ivec.push_back(100);
    ivec.push_back(0);
    ivec.push_back(70000);

    auto ivecret = iTest.append_5_to_vector(ivec);

    auto ivecexpected = ivec;
    ivecexpected.push_back(5);
    BOOST_CHECK_EQUAL_COLLECTIONS(ivecret.begin(),ivecret.end(),
        ivecexpected.begin(),ivecexpected.end());



    

}

BOOST_FIXTURE_TEST_CASE(object_count_tests,MyFixture)
{
    auto get_object_count = m_.load_module_function<std::size_t(*)()>("GetObjectCount");
    {
    auto unk = cross_compiler_interface::create_unknown(m_,"CreateIunknownDerivedInterface");
    
    BOOST_CHECK_EQUAL(get_object_count(),1);
    use_interface<cross_compiler_interface::InterfaceUnknown> unk2 = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>(m_,"CreateIunknownDerivedInterfaceOnly");
    BOOST_CHECK_EQUAL(get_object_count(),2);
    unk2.Release();
    BOOST_CHECK_EQUAL(get_object_count(),1);


    }

    BOOST_CHECK_EQUAL(get_object_count(),0);




}

BOOST_FIXTURE_TEST_CASE(u16_32_string,MyFixture){

    char32_t a32('a');
    char16_t a16('a');

    auto r32 = iTest.test_u32_string(std::u32string(1,a32));
    BOOST_CHECK(r32==std::u32string(2,a32));

    auto r16 = iTest.test_u16_string(std::u16string(1,a16));
    BOOST_CHECK(r16==std::u16string(2,a16));



}

#include "cross_compiler_interface/cross_compiler_introspection.hpp"

template<class T>
struct Introspected:cross_compiler_interface::define_unknown_interface<T,
    	// {83BEA17A-68C3-40A7-8504-F67CF0A31C1A}
	cross_compiler_interface::uuid<
	0x83BEA17A,0x68C3,0x40A7,0x85,0x04,0xF6,0x7C,0xF0,0xA3,0x1C,0x1A
    >>
{
    cross_compiler_interface::cross_function<Introspected,0,int(int)> f1;
    cross_compiler_interface::cross_function<Introspected,1,void()> f2;

Introspected():f1(this),f2(this){}
};

CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(Introspected,
                                                      f1,f2);

BOOST_FIXTURE_TEST_CASE(test_introspection1,MyFixture){


    auto info = cross_compiler_interface::get_interface_information<Introspected>();



    auto unknowninfo = cross_compiler_interface::get_interface_information<cross_compiler_interface::InterfaceUnknown>();


    BOOST_CHECK_EQUAL(unknowninfo.get_function(0).name,"QueryInterfaceRaw");

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
    auto pimp = ImpIntrospected::create();
    std::vector<cross_compiler_interface::any> vany;
    vany.push_back(cross_compiler_interface::any(int(5)));
    auto& c = cross_compiler_interface::type_information<cross_compiler_interface::use_unknown<Introspected>>::names();
    auto iany = info.get_function(0).call(pimp,vany);
    auto i = cross_compiler_interface::any_cast<int>(iany);

    BOOST_CHECK_EQUAL(i,10);




}
