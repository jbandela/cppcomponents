#include "unit_test_interface.h"


struct TestImplementation:public jrb_interface::implement_interface<TestInterface>{

	TestImplementation(){
		auto& t = *this;
		t.double_referenced_int = [](int& i){ i *= 2;};
		t.plus_5 = [](int i){return i+5;};
		t.times_2point5 = [](double d){return d*2.5;};
		t.hello_from_base = []()->std::string{return "Hello from Base";};
		t.say_hello = [](std::string name)->std::string{return "Hello " + name;};
		t.use_at_out_of_range = [](std::string s){s.at(s.size());};
		t.count_characters = [](std::string s)->int{return s.length();};
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

		t.say_hello2 = [](jrb_interface::use_interface<IGetName> ign)->std::string{
			return "Hello " + ign.get_name();
		};

		t.get_string_at = [](std::vector<std::string> v, int pos)->std::pair<int,std::string>{
			std::pair<int,std::string> ret;
			ret.first = pos;
			ret.second = v.at(pos);
			return ret;
		};
	}

};


struct TestImplementationMemFn {
     jrb_interface::implement_interface<TestInterface> t;

	 void double_referenced_int(int& i){ i *= 2;}
	 int plus_5(int i){return i+5;}
	 double times_2point5(double d){return d*2.5;}
	 std::string hello_from_base(){return "Hello from Base";}
	 std::string say_hello(std::string name){return "Hello " + name;};
	 void use_at_out_of_range(std::string s){s.at(s.size());}
	 int count_characters(std::string s){return s.length();};
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

	 std::string say_hello2(jrb_interface::use_interface<IGetName> ign){
			return "Hello " + ign.get_name();
		}

	 std::pair<int,std::string> get_string_at(std::vector<std::string> v, int pos){
			std::pair<int,std::string> ret;
			ret.first = pos;
			ret.second = v.at(pos);
			return ret;
		}
	TestImplementationMemFn(){
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
	}

};


extern "C"{

const jrb_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestInterface(){
	static TestImplementationMemFn  t_;

	return t_.t.get_portable_base();
}
}

