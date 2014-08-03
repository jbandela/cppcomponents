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



//
//TEST(Component, property_test){
//
//	Person p;
//	auto s = p.Name();
//	EXPECT_EQ(s, "John");
//
//	p.Name = "William";
//	s = p.Name;
//	EXPECT_EQ(s, "William");
//
//	
//
//
//	auto age = p.AgeReadOnly();
//	EXPECT_EQ(age, 21);
//
//	p.AgeWriteOnly = 5;
//	age = p.AgeReadOnly;
//	EXPECT_EQ(age, 5);
//
//	p.AgeWriteOnly(16);
//
//	age = p.AgeReadOnly;
//	EXPECT_EQ(age, 16);
//
//
//
//
//
//
//
//}
//
//TEST(Component, event_test_1){
//
//	PersonWithEvent p;
//
//	bool called = false;
//	p.NameChanged += [&called](std::string n){
//		called = true;
//	};
//
//	p.Name = "William";
//
//	EXPECT_EQ(called, true);
//
//}
//
//TEST(Component, event_test_remove){
//
//	PersonWithEvent p;
//
//	bool called = false;
//	auto tok = p.NameChanged += [&called](std::string n){
//		called = true;
//	};
//
//	p.Name = "William";
//
//	EXPECT_EQ(called, true);
//
//	called = false;
//	bool called2 = false;
//	auto tok2 = p.NameChanged +=[&called2](std::string n){
//		called2 = true;
//	};
//
//	p.Name = "John";
//
//	EXPECT_EQ(called, true);
//	EXPECT_EQ(called2, true);
//
//	called = false;
//	called2 = false;
//
//	p.NameChanged -= tok;
//
//	p.Name = "Sarah";
//
//	EXPECT_EQ(called, false);
//	EXPECT_EQ(called2, true);
//
//	p.NameChanged -= tok2;
//
//	called = false;
//	called2 = false;
//	p.Name = "Jane";
//	EXPECT_EQ(called, false);
//	EXPECT_EQ(called2, false);
//
//
//
//
//}


TEST(Component, test_wstring_wstring_ref){
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

void test_loop_executor_1();

TEST(LoopExecutor, loop_executor_1){

	test_loop_executor_1();

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





struct ImpTestInternalClass :cppcomponents::implement_runtime_class<ImpTestInternalClass, TestInternalClass_t>{
	ImpTestInternalClass(){}

	std::string Test(){ return "Hello Internal"; }
};

CPPCOMPONENTS_REGISTER(ImpTestInternalClass)


TEST(Components, test_internal_class){

	TestInternalClassTester c;

	EXPECT_EQ("Hello Internal", c.TestInteralExe());
	EXPECT_EQ("Test Dll", c.TestInteralDll());

}
#include "../cppcomponents/iterator.hpp"
#include <algorithm>
TEST(Components, iterator_random_access){
  std::vector<std::string> vec;
  vec.push_back("d");
  vec.push_back("c");
  vec.push_back("b");
  vec.push_back("a");
  typedef cppcomponents::uuid<0xd5c56d8b, 0x3147, 0x4f2b, 0x847b, 0xdeedde864693> u_t;
  auto ibeg = cppcomponents::iterator::make_iterator<u_t>(vec.begin());
  auto iend = cppcomponents::iterator::make_iterator<u_t>(vec.end());

  auto beg =  cppcomponents::iterator::random_access_iterator_wrapper<std::string>{ ibeg };
  auto end =  cppcomponents::iterator::random_access_iterator_wrapper<std::string>{ iend };
  std::random_shuffle(beg, end);
  std::make_heap(beg, end);
  EXPECT_TRUE(std::is_heap(beg, end));
  std::sort(beg,end);
  EXPECT_TRUE(std::is_sorted(beg, end));
  EXPECT_EQ(std::string("a"), *beg);
  EXPECT_EQ(std::string("a"), beg[0]);
  EXPECT_EQ(std::string("c"), beg[2]);
  EXPECT_EQ(std::string("b"), *(++beg));

  *(beg + 0) = std::string("Hello");
  EXPECT_EQ(std::string("Hello"), beg[0]);


}
TEST(Components, iterator_random_access_const){
  std::vector<std::string> vec;
  vec.push_back("d");
  vec.push_back("c");
  vec.push_back("b");
  vec.push_back("a");
  typedef cppcomponents::uuid<0xd5c56d8b, 0x3147, 0x4f2b, 0x847b, 0xdeedde864693> u_t;
  auto ibeg = cppcomponents::iterator::make_iterator<u_t>(vec.cbegin());
  auto iend = cppcomponents::iterator::make_iterator<u_t>(vec.cend());
  auto beg = cppcomponents::iterator::random_access_iterator_wrapper<const std::string>{ ibeg };
  auto end = cppcomponents::iterator::random_access_iterator_wrapper<const std::string>{ iend };

  std::string val = *beg;
  EXPECT_EQ(std::string("d"), val);

  // This should be a compiler error
  // *beg = std::string("c");


}
#include <list>
TEST(Components, iterator_bidirectional){
  std::list<std::string> list;
  list.push_back("d");
  list.push_back("c");
  list.push_back("b");
  list.push_back("a");
  typedef cppcomponents::uuid<0xd5c56d8b, 0x3147, 0x4f2b, 0x847b, 0xdeedde864693> u_t;
  auto ibeg = cppcomponents::iterator::make_iterator<u_t>(list.begin());
  auto iend = cppcomponents::iterator::make_iterator<u_t>(list.end());

  auto beg = cppcomponents::iterator::bidirectional_iterator_wrapper<std::string>{ ibeg };
  auto end = cppcomponents::iterator::bidirectional_iterator_wrapper<std::string>{ iend };
  std::reverse(beg, end);
  EXPECT_TRUE(std::is_sorted(beg, end));
  EXPECT_EQ(std::string("a"), static_cast<std::string>(*beg));
  auto iter1 = beg;
  std::advance(iter1,2);
  EXPECT_EQ(std::string("c"), *iter1);
  EXPECT_EQ(std::string("b"), *(++beg));

 


}

#include <forward_list>

TEST(Components, iterator_forward){
  std::forward_list<std::string> list;
  list.push_front("a");
  list.push_front("b");
  list.push_front("c");
  list.push_front("d");

  typedef cppcomponents::uuid<0xd5c56d8b, 0x3147, 0x4f2b, 0x847b, 0xdeedde864693> u_t;
  auto ibeg = cppcomponents::iterator::make_iterator<u_t>(list.begin());
  auto iend = cppcomponents::iterator::make_iterator<u_t>(list.end());

  auto beg = cppcomponents::iterator::forward_iterator_wrapper<std::string>{ ibeg };
  auto end = cppcomponents::iterator::forward_iterator_wrapper<std::string>{ iend };

  auto iter = std::find(beg, end, std::string("b"));

  EXPECT_EQ(std::string("b"),*iter);
  EXPECT_EQ(2, std::distance(beg, iter));
}

#include <sstream>
#include <iterator>
TEST(Components, iterator_input_output){
  std::stringstream istr{"This is a test"};
  std::stringstream ostr;

  typedef std::istream_iterator<std::string> istr_iter_t;
  typedef std::ostream_iterator<std::string> ostr_iter_t;

  typedef cppcomponents::uuid<0xd5c56d8b, 0x3147, 0x4f2b, 0x847b, 0xdeedde864693> u_t;

  auto ibeg_in = cppcomponents::iterator::make_iterator<u_t>(istr_iter_t{ istr });
  auto iend_in = cppcomponents::iterator::make_iterator<u_t>(istr_iter_t{});
   
  auto ibeg_out = cppcomponents::iterator::make_output_iterator<u_t,std::string>(ostr_iter_t{ ostr,"" });

  auto beg_in = cppcomponents::iterator::input_iterator_wrapper<std::string>{ ibeg_in };
  auto end_in = cppcomponents::iterator::input_iterator_wrapper<std::string>{ iend_in };

  auto beg_out = cppcomponents::iterator::output_iterator_wrapper<std::string>{ ibeg_out };
  
  std::copy(beg_in, end_in, beg_out);

  auto str = ostr.str();

  EXPECT_EQ("Thisisatest",str);
}



TEST(Component, const_return){
  TestConstReturn t;
  t.Hello();
  t.Pair();
  t.Tuple1();
  t.Tuple2();
  t.Tuple3();
  
}


TEST(Component, get_runtime_class_name){

	EXPECT_EQ("Test.Component", TestComponent_t::get_runtime_class_name());
}

void test_loop_with_function();

TEST(LoopExecutor, loop_with_function){
	test_loop_with_function();
}

TEST(Function, simple_function_test){

	typedef cppcomponents::function<void()> fun_t;
	int i = 0;
	fun_t func = cppcomponents::make_function<fun_t>([&](){i = 1; });
	func();
	EXPECT_EQ(i, 1);
}
// Note the function types are different from the implementation
struct ITestFunction :cppcomponents::define_interface<cppcomponents::uuid<0x0ae1dc9a, 0x78b6, 0x4e82, 0x87d5, 0x5663d4521fa3>>
{
	cppcomponents::function<std::string()> GetStringFunction();
	void SetStringFunction(cppcomponents::function<std::string()>);

	cppcomponents::use<cppcomponents::delegate<std::string(std::string)>> GetStringStringFunction();
	void SetStringStringFunction(cppcomponents::use<cppcomponents::delegate<std::string(std::string)>>);

	void SetStringStringFunctionFromIUnknown(cppcomponents::use<cppcomponents::InterfaceUnknown>);

	CPPCOMPONENTS_CONSTRUCT(ITestFunction, GetStringFunction, SetStringFunction, GetStringStringFunction, SetStringStringFunction, SetStringStringFunctionFromIUnknown)
};

inline const char* TestFunctionId(){ return "unit_test_dll!TestFunctionId"; }
typedef cppcomponents::runtime_class<TestFunctionId, cppcomponents::object_interfaces<ITestFunction>> TestFunction_t;
typedef cppcomponents::use_runtime_class<TestFunction_t> TestFunction;

TEST(Function, type_abi_equivalence){
	TestFunction tf;
	cppcomponents::function<std::string()> f1 = tf.GetStringFunction();
	EXPECT_EQ("Hello World", f1());

	cppcomponents::function<std::string()> f2 = [](){return std::string("Bye"); };
	tf.SetStringFunction(f2);
	f1 = tf.GetStringFunction();
	EXPECT_EQ("Bye", f1());

	cppcomponents::function<std::string(std::string)> f3 = tf.GetStringStringFunction();
	EXPECT_EQ("Hello John", f3("John"));

	cppcomponents::function<std::string(std::string)> f4 = [](std::string name){return "Bye " + name; };

	tf.SetStringStringFunction(f4.get_delegate());
	cppcomponents::use<cppcomponents::delegate<std::string(std::string)>> d = tf.GetStringStringFunction();
	EXPECT_EQ("Bye John", d("John"));

	tf.SetStringStringFunctionFromIUnknown(f4.get_delegate().QueryInterface<cppcomponents::InterfaceUnknown>());
	cppcomponents::use<cppcomponents::delegate<std::string(std::string)>> di = tf.GetStringStringFunction();
	EXPECT_EQ("Bye John", di("John"));
}

TEST(Function, from_iunknown){
	TestFunction tf;

	cppcomponents::function<std::string(std::string)> f4 = [](std::string name){return "Bye " + name; };


	tf.SetStringStringFunctionFromIUnknown(f4.get_delegate().QueryInterface<cppcomponents::InterfaceUnknown>());
	cppcomponents::use<cppcomponents::delegate<std::string(std::string)>> di = tf.GetStringStringFunction();
	EXPECT_EQ("Bye John", di("John"));
}


void test_call_by_name();

TEST(test_call_by_name, test_call_by_name){
	test_call_by_name();
}