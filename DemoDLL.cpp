//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "DemoInterface.hpp"


#include <algorithm>


using namespace jrb_interface;

struct DemoInterfaceImplemention: public implement_interface<DemoInterface>{

	DemoInterfaceImplemention(){
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
	}

};


extern "C"{

 const portable_base* CROSS_CALL_CALLING_CONVENTION CreateDemoInterface(){
	static DemoInterfaceImplemention d_;
	return d_.get_portable_base();
}
};

