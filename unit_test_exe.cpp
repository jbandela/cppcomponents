#define BOOST_TEST_MODULE cross_interface_unit_tests
#include <boost/test/unit_test.hpp>
#include "unit_test_interface.h"

struct MyFixture{
	cross_compiler_interface::use_interface<TestInterface> iTest;

	cross_compiler_interface::use_interface<TestInterface> iTestMemFn;

	MyFixture():iTest(cross_compiler_interface::create<TestInterface>("unit_test_dll","CreateTestInterface")),
		iTestMemFn(cross_compiler_interface::create<TestInterface>("unit_test_dll","CreateTestMemFnInterface"))
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
   auto expected = 8;
   int i = 4;
   iTest.double_referenced_int(i);
  BOOST_CHECK_EQUAL(expected,i);

   i = 4;
   iTestMemFn.double_referenced_int(i);
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
	use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>("unit_test_dll","CreateIunknownDerivedInterface");
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
	
	use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>("unit_test_dll","CreateIunknownDerivedInterface");

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


	use_unknown<cross_compiler_interface::InterfaceUnknown> unk2 = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>("unit_test_dll","CreateIunknownDerivedInterface");

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
	
	use_interface<cross_compiler_interface::InterfaceUnknown> unk = cross_compiler_interface::create<cross_compiler_interface::InterfaceUnknown>("unit_test_dll","CreateIunknownDerivedInterface");

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