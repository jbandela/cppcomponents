//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <cstdint>
#include <string>



namespace cross_compiler_interface{

	typedef std::int32_t error_code;

	struct cross_compiler_interface_error_base:public std::runtime_error{
		error_code error_code_;
		virtual error_code get_error_code(){return error_code_;}
		cross_compiler_interface_error_base(error_code e)
			:runtime_error("cross_compiler_interface error"), error_code_(e){}
	};

	template<error_code code>
	struct cross_compiler_interface_error:public cross_compiler_interface_error_base{
		enum{ec = code};
		error_code get_error_code(){return code;}
		cross_compiler_interface_error():cross_compiler_interface_error_base(ec){}
	};


	// Use the same as HRESULT
	struct error_unexpected :public cross_compiler_interface_error<static_cast<error_code>(0x8000FFFF)>{
        const char* what()const throw()  override{return "Unexpected failure";}
    };

	struct error_not_implemented :public cross_compiler_interface_error<static_cast<error_code>(0x80004001)>{
        const char* what()const throw()  override{return "Not Implemented";}
    
    };

	template<>
	struct cross_compiler_interface_error<static_cast<error_code>(0x8007000E)>:public std::bad_alloc,public cross_compiler_interface_error_base{
		enum{ec = static_cast<error_code>(0x8007000E)};
		error_code get_error_code(){return static_cast<error_code>(0x8007000E);}
		cross_compiler_interface_error():cross_compiler_interface_error_base(ec){}

	};
	struct error_out_of_memory:public cross_compiler_interface_error<static_cast<error_code>(0x8007000E)> {
        const char* what()const throw()  override{return "Memory allocation error";}
    };

	struct error_invalid_arg:public cross_compiler_interface_error<static_cast<error_code>(0x80070057)> {
        const char* what()const throw()  override{return "Invalid argument";}
    };

	struct error_no_interface:public cross_compiler_interface_error<static_cast<error_code>(0x80004002)> {
        const char* what()const throw()  override{return "Interface not available";}
    };

	struct error_pointer:public cross_compiler_interface_error<static_cast<error_code>(0x80004003)> {
        const char* what()const throw()  override{return "Invalid pointer";}
    };

	struct error_handle:public cross_compiler_interface_error<static_cast<error_code>(0x80070006)> {
        const char* what()const throw()  override{return "Invalid handle";}
    };

	struct error_abort:public cross_compiler_interface_error<static_cast<error_code>(0x80004004)> {
        const char* what()const throw()  override{return "Call aborted";}
    };

	struct error_fail:public cross_compiler_interface_error<static_cast<error_code>(0x80004005)> {
        const char* what()const throw()  override{return "Call failed";}
    };

	struct error_access_denied:public cross_compiler_interface_error<static_cast<error_code>(0x80070005)> {
        const char* what()const throw()  override{return "Access denied";}
    };

	struct error_pending:public cross_compiler_interface_error<static_cast<error_code>(0x8000000A)> {
        const char* what()const throw()  override{return "Unable to complete, pending";}
    };

	struct error_class_not_available : public cross_compiler_interface_error<static_cast<error_code>(0x80040111)> {
        const char* what()const throw()  override{return "Class is not available";}
    };


	template<>
	struct cross_compiler_interface_error<static_cast<int>(0x80131508)>:public cross_compiler_interface_error_base,
		public std::out_of_range{
			enum{ec = static_cast<error_code>(0x80131508)};
			error_code get_error_code(){return ec;}
			cross_compiler_interface_error():cross_compiler_interface_error_base(ec),std::out_of_range("cross_compiler_interface error"){}

	};

	struct error_out_of_range:public cross_compiler_interface_error<static_cast<error_code>(0x80131508)> {
        const char* what()const throw()  override{return "Index out of range";}
    };


	struct error_shared_function_not_found:public cross_compiler_interface_error<static_cast<error_code>(0x8002802F)>{
        const char* what()const throw()  override{return "Shared function not found";}
    };

	struct error_unable_to_load_library:public cross_compiler_interface_error<static_cast<error_code>(0x80029C4A)>{
        const char* what()const throw()  override{return "Unable to load library";}
    };
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4673 4672)
#endif
	template<class T, class... Rest>
	struct interface_error_runtime_mapper{
		static void throw_if_match(error_code e){
			if(e==T::ec){
				throw T();
			}else{
				typedef interface_error_runtime_mapper<Rest...> m;
				m::throw_if_match(e);
			}
		}
	};
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	template<class T>
	struct interface_error_runtime_mapper<T>{
		static void throw_if_match(error_code e){
			if(e==T::ec){
				throw T();
			}else{
				throw cross_compiler_interface_error_base(e);
			}
		}

	};

	typedef interface_error_runtime_mapper<
		error_fail,error_handle,error_invalid_arg,error_no_interface,
		error_not_implemented, error_class_not_available,error_out_of_memory, error_out_of_range, error_pending,
		error_pointer,error_unexpected,error_abort,error_access_denied,error_shared_function_not_found,
		error_unable_to_load_library
	> mapper;


	struct general_error_mapper{
		static error_code error_code_from_exception(std::exception& e){
			if(auto pe = dynamic_cast<cross_compiler_interface_error_base*>(&e)){
				return pe->get_error_code();
			}else if(dynamic_cast<std::bad_alloc*>(&e)){
				return error_out_of_memory::ec;
			}
			else if(dynamic_cast<std::out_of_range*>(&e)){
				return error_out_of_range::ec;
			}
			else{
				return error_fail::ec;
			}
		}

		static void exception_from_error_code(error_code e){
			mapper::throw_if_match(e);
		}
	};

	template<template<class> class Iface>
	struct error_mapper{
		typedef general_error_mapper mapper;
	};

}


