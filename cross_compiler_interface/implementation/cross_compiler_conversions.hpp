//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <cstdint>
#include <limits>
#include <utility>

namespace cross_compiler_interface {

	
	template<class T>
	struct trivial_conversion{
		typedef T converted_type;
		typedef T original_type;
		static const converted_type to_converted_type(original_type i){return i;};
		static const original_type to_original_type(converted_type c){return c;}
	};

	template<class T> struct cross_conversion;


	// Macro for defining trivial conversions
#define JRB_TRIVIAL_CONV(a) template<> struct cross_conversion<a>:public trivial_conversion<a>{}; \
	template<> struct cross_conversion<a*>:public trivial_conversion<a*>{}; \
	template<> struct cross_conversion<const a*>:public trivial_conversion<const a*>{}; \
	template<> struct cross_conversion<a&>:public trivial_conversion<a&>{}; \
	template<> struct cross_conversion<const a&>:public trivial_conversion<const a&>{}

	JRB_TRIVIAL_CONV(char);
	JRB_TRIVIAL_CONV(std::int8_t);
	JRB_TRIVIAL_CONV(std::int16_t);
	JRB_TRIVIAL_CONV(std::int32_t);
	JRB_TRIVIAL_CONV(std::int64_t);

	JRB_TRIVIAL_CONV(std::uint8_t);
	JRB_TRIVIAL_CONV(std::uint16_t);
	JRB_TRIVIAL_CONV(std::uint32_t);
	JRB_TRIVIAL_CONV(std::uint64_t);

	// Now do float and double
	static_assert(std::numeric_limits<float>::is_iec559,"float is not standard");
	JRB_TRIVIAL_CONV(float);
	static_assert(std::numeric_limits<double>::is_iec559,"double is not standard");
	JRB_TRIVIAL_CONV(double);

#undef JRB_TRIVIAL_CONV



	struct cross_string{
		const char* begin;
		const char* end;
	};
	static_assert(sizeof(cross_string)==2*sizeof(char*),"Padding in cross_string");

	struct cross_string_return{
		void* retstr;
		void (CROSS_CALL_CALLING_CONVENTION *transfer_string)(void*,const char*, const char*);
	};
	static_assert(sizeof(cross_string)==2*sizeof(char*),"Padding in cross_string");

	template<class T>
	struct cross_vector{
		typedef T* p_t;
		p_t begin_end[2];
	};

	template<class T>
	struct cross_vector_return{
		void* retvector;
		void (CROSS_CALL_CALLING_CONVENTION *push_back)(void*, T);

		// Note do not support vector more than 2^32
		void (CROSS_CALL_CALLING_CONVENTION *reserve_vector)(void*,std::uint32_t sz);
	};

	template<>
	struct cross_conversion<std::string>{
		typedef std::string original_type;
		typedef cross_string converted_type;
		static converted_type to_converted_type(const original_type& s){
			cross_string ret;
			ret.begin = &s[0];
			ret.end = &s[0] + s.size();
			return ret;
		}
		static  std::string to_original_type(converted_type& c){
			std::string ret;
			ret.assign(c.begin,c.end);
			return ret;
		}
	};

	template<>
	struct cross_conversion_return<std::string>{
		typedef std::string return_type;
		typedef cross_string_return converted_type;

		 static void CROSS_CALL_CALLING_CONVENTION do_transfer_string(void* str,const char* begin, const char* end){
				auto& s = *static_cast<std::string*>(str);
				s.assign(begin,end);

			};
		static void initialize_return(return_type& r, converted_type& c){
			c.retstr = &r;
			c.transfer_string = &do_transfer_string;
		}

		static void do_return(const return_type& r,converted_type& c){
			 c.transfer_string(c.retstr,&r[0],&r[0] + r.size());
		}
		static void finalize_return(return_type& r,converted_type& c){
			// do nothing
		}


	};

	template<class T>
	struct cross_conversion<std::vector<T>>{
		typedef std::vector<T> original_type;
		typedef T original_value_type;
		typedef typename cross_conversion<T>::converted_type converted_value_type;
		typedef cross_vector<converted_value_type> converted_type;
		static converted_type to_converted_type(const original_type& s){
			converted_type ret;
			typedef cross_conversion<T> cc;
			ret.begin_end[0] = allocate_array<converted_value_type>(s.size());
			ret.begin_end[1] = ret.begin_end[0] + s.size();
			std::transform(s.begin(),s.end(),ret.begin_end[0],[](const T& t){return cc::to_converted_type(t);});
			return ret;
		}
		static  original_type to_original_type(converted_type& c){
			original_type ret;
			typedef cross_conversion<T> cc;
			std::transform(c.begin_end[0],c.begin_end[1],std::back_inserter(ret),[](converted_value_type& t){return cc::to_original_type(t);});
			shared_free(c.begin_end[0]);
			c.begin_end[0] = 0;
			c.begin_end[1] = 0;
			return ret;
		}

	};


		template<class T>
	struct cross_conversion_return<std::vector<T>>{
		typedef std::vector<T> original_type;
		typedef original_type return_type;
		typedef T original_value_type;
		typedef typename cross_conversion<T>::converted_type converted_value_type;
		typedef cross_vector_return<converted_value_type> converted_type;


		static void CROSS_CALL_CALLING_CONVENTION do_reserve_vector(void* vec, std::uint32_t sz){
				typedef cross_conversion<T> cc;

				auto& v = *static_cast<return_type*>(vec);
				v.reserve(sz);

			}

		static void CROSS_CALL_CALLING_CONVENTION do_push_back(void* vec, converted_value_type t){
				typedef cross_conversion<T> cc;

				auto& v = *static_cast<return_type*>(vec);
				v.push_back(cc::to_original_type(t));
			}

		static void initialize_return(return_type& r, converted_type& c){
			c.retvector = &r;
			c.reserve_vector=do_reserve_vector;
			c.push_back = do_push_back;
		}

		static void do_return(const return_type& r,converted_type& c){
			typedef cross_conversion<T> cc;
			c.reserve_vector(c.retvector,r.size());
			for(auto i = r.begin(); i != r.end(); ++i){
				c.push_back(c.retvector,cc::to_converted_type(*i));
			};
		}
		static void finalize_return(return_type& r,converted_type& c){
			// do nothing
		}


	};



	template<template<bool> class T>
	struct cross_conversion<use_interface<T>>{
		typedef use_interface<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			return s.get_portable_base();
		}
		static  original_type to_original_type(converted_type c){
			return use_interface<T>(c);
		}

	};

	template<class T, class U>
	struct cross_pair{
		typedef  cross_conversion<T> cct;
		typedef  cross_conversion<U> ccu;
		typename cct::converted_type first;
		typename ccu::converted_type second;
	};



		template<class T,class U>
	struct cross_conversion<std::pair<T,U>>{
		typedef std::pair<T,U> original_type;
		typedef cross_pair<T,U> converted_type;
		static converted_type to_converted_type(const original_type& s){
			converted_type ret;
			typedef cross_conversion<T> ccT;
			typedef cross_conversion<U> ccU;
			ret.first = ccT::to_converted_type(s.first);
			ret.second = ccU::to_converted_type(s.second);
			return ret;
		}
		static  original_type to_original_type(converted_type& c){
			original_type ret;
			typedef cross_conversion<T> ccT;
			typedef cross_conversion<U> ccU;
			ret.first = ccT::to_original_type(c.first);
			ret.second = ccU::to_original_type(c.second);
			return ret;
		}

	};

}



