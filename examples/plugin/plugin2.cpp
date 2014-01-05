#include "plugin.hpp"


inline std::string Command3Id(){return "Command3";}
typedef cppcomponents::runtime_class<Command3Id,cppcomponents::object_interfaces<ICommand>> Command3_t;


struct ImplementCommand3:cppcomponents::implement_runtime_class<ImplementCommand3,Command3_t>{

	std::wstring GetCommandName(){return L"Command3";}
	std::wstring GetCommandGUID(){return L"16e701c0-58fc-49c8-83cc-cd9975e7d343";}
	bool Execute(std::vector<std::wstring> args){

		std::wstring out = L"Command3 in plugin2 called with ";
		for(auto& a:args){
			out  = out + L" " + a;
		}
		Output::OutputWString(out);
		return true;
	}

};
inline std::string Command4Id(){return "Command4";}
typedef cppcomponents::runtime_class<Command4Id,cppcomponents::object_interfaces<ICommand>> Command4_t;



struct ImplementCommand4:cppcomponents::implement_runtime_class<ImplementCommand4,Command4_t>{

	std::wstring GetCommandName(){return L"Command4";}
	std::wstring GetCommandGUID(){return L"16e701c0-58fc-49c8-83cc-cd9975e7d344";}
	bool Execute(std::vector<std::wstring> args){

		std::wstring out = L"Command4 in plugin2 called with ";
		for(auto& a:args){
			out  = out + L" " + a;
		}
		Output::OutputWString(out);
		return true;

	}
	
};

struct ImplementCommandProvider
:cppcomponents::implement_runtime_class<ImplementCommandProvider,CommandProvider_t>
{

	static std::vector<cppcomponents::use<ICommand>> GetCommands(){
		std::vector<cppcomponents::use<ICommand>> ret;
		ret.push_back(ImplementCommand3::create().QueryInterface<ICommand>());
		ret.push_back(ImplementCommand4::create().QueryInterface<ICommand>());

		return ret;
	}


};

CPPCOMPONENTS_REGISTER(ImplementCommandProvider);

CPPCOMPONENTS_DEFINE_FACTORY()
