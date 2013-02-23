#define BOOST_TEST_MODULE cross_interface_unit_tests
#include <boost/test/unit_test.hpp>
#include "unit_test_interface.h"

struct MyFixture{
	jrb_interface::use_interface<TestInterface> iTest;

	jrb_interface::use_interface<TestInterface> iTestMemFn;

	MyFixture():iTest(jrb_interface::create<TestInterface>("unit_test_dll","CreateTestInterface")),
		iTestMemFn(jrb_interface::create<TestInterface>("unit_test_dll","CreateTestMemFnInterface"))
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

   BOOST_CHECK_THROW(iTest.not_implemented(),jrb_interface::error_not_implemented);
   BOOST_CHECK_THROW(iTestMemFn.not_implemented(),jrb_interface::error_not_implemented);
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

   jrb_interface::implement_interface<IGetName> ign;
   ign.get_name = [s](){return s;};
   BOOST_CHECK_EQUAL(expected,iTest.say_hello2(ign));
   BOOST_CHECK_EQUAL(expected,iTestMemFn.say_hello2(ign));
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