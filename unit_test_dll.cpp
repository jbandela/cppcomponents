#include "unit_test_interface.h"

extern "C"{

 jrb_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestInterface();
}


struct ImplementIuknownDerivedInterface{
	jrb_interface::implement_interface<IUnknownDerivedInterface> imp;
	jrb_interface::implement_interface<IUnknownDerivedInterface2Derived> imp2;
	jrb_interface::implement_iunknown<ImplementIuknownDerivedInterface,decltype(imp),decltype(imp2)> imp_unknown;

	ImplementIuknownDerivedInterface():imp_unknown(this,imp,imp2){
		imp.hello_from_iuknown_derived = []()->std::string{
			return "Hello from IuknownDerivedInterface";
		};

		imp2.hello_from_iuknown_derived2 = []()->std::string{
			return "Hello from IuknownDerivedInterface2";
		};

		imp2.hello_from_derived = []()->std::string{
			return "Hello from derived";
		};

	}

};

struct TestImplementation:public jrb_interface::implement_interface<TestInterface>{

	jrb_interface::implement_interface<IGetName> ign_imp;

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

		ign_imp.get_name = []()->std::string{return "Hello from returned interface";};
		t.get_igetname = [this]()->use_interface<IGetName>{return ign_imp;};

		t.get_name_from_runtime_parent = []()->std::string{return "TestImplementation";};
		t.custom_with_runtime_parent =[](int i){return i+10;};

	}

};


struct TestImplementationMemFn {
     jrb_interface::implement_interface<TestInterface> t;

	jrb_interface::implement_interface<IGetName> ign_imp;

	std::string ign_get_name(){return "Hello from returned interface";} 


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

	 use_interface<IGetName> get_igetname(){
		 return ign_imp;

	 }
	TestImplementationMemFn(){
		t.set_runtime_parent(CreateTestInterface());

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
	}

};


extern "C"{

 jrb_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestInterface(){
	static TestImplementation  t_;

	return t_.get_portable_base();
}
}

extern "C"{

 jrb_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateTestMemFnInterface(){
	static TestImplementationMemFn  t_;

	return t_.t.get_portable_base();
}
}
extern "C"{

 jrb_interface::portable_base* CROSS_CALL_CALLING_CONVENTION CreateIunknownDerivedInterface(){
	ImplementIuknownDerivedInterface* derived = new ImplementIuknownDerivedInterface;

	return derived->imp.get_portable_base();
}
}