#ifndef JRB_INTERFACE_ERROR_HANDLING_HPP
#define JRB_INTERFACE_ERROR_HANDLING_HPP

#include <stdexcept>
#include <cstdint>


namespace jrb_interface{

	typedef std::int32_t error_code;

	struct jrb_interface_error_base:public std::runtime_error{
		error_code error_code_;
		virtual error_code get_error_code(){return error_code_;}
		jrb_interface_error_base(error_code e)
			:error_code_(e),runtime_error("jrb_interface error"){}
	};

	template<error_code code>
	struct jrb_interface_error:public jrb_interface_error_base{
		enum{ec = code};
		error_code get_error_code(){return code;}
		jrb_interface_error():jrb_interface_error_base(ec){}
	};


	// Use the same as HRESULT
	struct error_unexpected :public jrb_interface_error<(int)0x8000FFFF>{};
	struct error_not_implemented :public jrb_interface_error<(int)0x80004001>{};

	template<>
	struct jrb_interface_error<(int)0x8007000E>:public std::bad_alloc,public jrb_interface_error_base{
		enum{ec = (int)0x8007000E};
		error_code get_error_code(){return (int)0x8007000E;}
		jrb_interface_error():jrb_interface_error_base(ec){}

	};
	struct error_out_of_memory:public jrb_interface_error<(int)0x8007000E> {};

	struct error_invalid_arg:public jrb_interface_error<(int)0x80070057> {};

	struct error_no_interface:public jrb_interface_error<(int)0x80004002> {};

	struct error_pointer:public jrb_interface_error<(int)0x80004003> {};

	struct error_handle:public jrb_interface_error<(int)0x80070006> {};

	struct error_abort:public jrb_interface_error<(int)0x80004004> {};

	struct error_fail:public jrb_interface_error<(int)0x80004005> {};

	struct error_access_denied:public jrb_interface_error<(int)0x80070005> {};

	struct error_pending:public jrb_interface_error<(int)0x8000000A> {};



	template<>
	struct jrb_interface_error<(int)0x80131508>:public jrb_interface_error_base,
		public std::out_of_range{
			enum{ec = (int)0x80131508};
			error_code get_error_code(){return ec;}
			jrb_interface_error():jrb_interface_error_base(ec),std::out_of_range("jrb_interface error"){}

	};

	struct error_out_of_range:public jrb_interface_error<(int)0x80131508> {};


	struct error_shared_function_not_found:public jrb_interface_error<(int)0x8002802F>{};


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
	template<class T>
	struct interface_error_runtime_mapper<T>{
		static void throw_if_match(error_code e){
			if(e==T::ec){
				throw T();
			}else{
				throw jrb_interface_error_base(e);
			}
		}

	};

	typedef interface_error_runtime_mapper<
		error_fail,error_handle,error_invalid_arg,error_no_interface,
		error_not_implemented,error_out_of_memory,error_out_of_range,error_pending,
		error_pointer,error_unexpected,error_abort,error_access_denied
	> mapper;


	struct general_error_mapper{
		static error_code error_code_from_exception(std::exception& e){
			if(auto pe = dynamic_cast<jrb_interface_error_base*>(&e)){
				return pe->get_error_code();
			}else if(auto pe = dynamic_cast<std::bad_alloc*>(&e)){
				return error_out_of_memory::ec;
			}
			else if(auto pe = dynamic_cast<std::out_of_range*>(&e)){
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

	template<template<bool> class Iface>
	struct error_mapper{
		typedef general_error_mapper mapper;
	};

}

#endif

