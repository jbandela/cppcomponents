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
#include <tuple>



#pragma pack(push,1)


#ifdef __GNUC__ 

#define CROSS_COMPILER_INTERFACE_PACK __attribute__((packed))

#else

#define CROSS_COMPILER_INTERFACE_PACK 

#endif

namespace cross_compiler_interface {


	template<class T>
	struct trivial_conversion{
		typedef T converted_type;
		typedef T original_type;
		static converted_type to_converted_type(original_type i){ return i; };
		static original_type to_original_type(converted_type c){ return c; }
	};

	template<class T> struct cross_conversion;


	// A converstion of trivial if converted_type and the original type are the same
	template<class T>
	struct is_trivial_cross_conversion{
		typedef T o_t;
		typedef cross_conversion<T> cc;
		typedef typename cc::converted_type c_t;
		enum{ value = std::is_same<T, c_t>::value };

	};

	namespace detail{

		template<class T>
		struct converted_type_return{
			typedef cross_conversion<T> cc;
			error_code(CROSS_CALL_CALLING_CONVENTION* transfer_)(void* dest, const typename cc::converted_type& c);
			void* dest_;

		};
		// General return
		template<class T, bool trivial>	struct cross_conversion_return_imp;

		template<class T>
		struct cross_conversion_return_imp<T, false>{
			typedef cross_conversion<T> cc;
			typedef typename cc::original_type return_type;
			typedef converted_type_return<T> converted_type;


			static error_code CROSS_CALL_CALLING_CONVENTION do_transfer(void* pdest, const typename cc::converted_type& c){
				try{
					auto& dest = *static_cast<return_type*>(pdest);
					dest = cc::to_original_type(c);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}

			}

			static void initialize_return(return_type& r, converted_type& c){
				c.transfer_ = do_transfer;
				c.dest_ = &r;

			}

			static void do_return(const return_type& r, const converted_type& c){
				auto rc = cc::to_converted_type(r);
				auto ec = c.transfer_(c.dest_, rc);
				if (ec < 0){
					general_error_mapper::exception_from_error_code(ec);
				}

			}
			static void finalize_return(return_type&, converted_type&){
				// do nothing
			}

		};

		// Trivial return
		template<class T>
		struct cross_conversion_return_imp<T, true>{
			typedef cross_conversion<T> cc;
			typedef typename cc::original_type return_type;
			typedef typename cc::converted_type converted_type;

			static void initialize_return(return_type&, converted_type&){
				// do nothing
			}

			static void do_return(const return_type& r, converted_type& c){
				typedef cross_conversion<T> cc;
				c = cc::to_converted_type(r);
			}
			static void finalize_return(return_type& r, converted_type& c){
				r = cc::to_original_type(c);
			}

		};
	}

	template<class T>
	struct cross_conversion_return : public detail::cross_conversion_return_imp<T, is_trivial_cross_conversion<T>::value >{};

	// Macro for defining trivial conversions
#define JRB_TRIVIAL_CONV(a) template<> struct cross_conversion<a>:public trivial_conversion<a>{}; \
	template<> struct cross_conversion<const a>:public trivial_conversion<const a>{}; \
	template<> struct cross_conversion<a*>:public trivial_conversion<a*>{}; \
	template<> struct cross_conversion<const a*>:public trivial_conversion<const a*>{}; \
	template<> struct cross_conversion<a&>:public trivial_conversion<a&>{}; \
	template<> struct cross_conversion<const a&>:public trivial_conversion<const a&>{}

	JRB_TRIVIAL_CONV(char);
	JRB_TRIVIAL_CONV(wchar_t);
	JRB_TRIVIAL_CONV(std::int8_t);
	JRB_TRIVIAL_CONV(std::int16_t);
	JRB_TRIVIAL_CONV(std::int32_t);
	JRB_TRIVIAL_CONV(std::int64_t);

	JRB_TRIVIAL_CONV(std::uint8_t);
	JRB_TRIVIAL_CONV(std::uint16_t);
	JRB_TRIVIAL_CONV(std::uint32_t);
	JRB_TRIVIAL_CONV(std::uint64_t);

	// Now do float and double
	static_assert(std::numeric_limits<float>::is_iec559, "float is not standard");
	JRB_TRIVIAL_CONV(float);
	static_assert(std::numeric_limits<double>::is_iec559, "double is not standard");
	JRB_TRIVIAL_CONV(double);

	JRB_TRIVIAL_CONV(void*);
	JRB_TRIVIAL_CONV(portable_base*);

#ifndef _MSC_VER
	// In MSVC char16_t and char32_t are not real types
	JRB_TRIVIAL_CONV(char16_t);
	JRB_TRIVIAL_CONV(char32_t);
#endif

#undef JRB_TRIVIAL_CONV


	// Support for char** as that is sometimes used by C for argv
	template<> struct cross_conversion<char**>:public trivial_conversion<char**>{}; 


	// Support for bool, bool has an implementation defined size
	// so use uint8_t
	template<>
	struct cross_conversion<bool>{
		typedef bool original_type;
		typedef std::uint8_t converted_type;
		static converted_type to_converted_type(bool b){
			return b;
		}
		static original_type to_original_type(std::uint8_t u){
			return u != 0;
		}
	};
	template<>
	struct cross_conversion<const bool>{
		typedef const bool original_type;
		typedef const std::uint8_t converted_type;
		static converted_type to_converted_type(const bool b){
			return b;
		}
		static original_type to_original_type(const std::uint8_t u){
			return u != 0;
		}
	};



	// Make sure size_t is concordant with void* in terms of size
	static_assert(sizeof(std::size_t) == sizeof(void*) , "size_t is not non-concordant with void*");

	template<class charT>
	struct cross_string{
		const charT* begin;
		const charT* end;
	}CROSS_COMPILER_INTERFACE_PACK;

	template<class T>
	struct cross_vector{
		const void* retvector;
		error_code(CROSS_CALL_CALLING_CONVENTION *get)(const void*, std::size_t, T*);

		std::size_t size;

	}CROSS_COMPILER_INTERFACE_PACK;

	template<class T>
	struct cross_vector_trivial{
		const T* begin_;
		const T* end_;
	}CROSS_COMPILER_INTERFACE_PACK;

	template<class charT, class Allocator>
	struct cross_conversion<std::basic_string<charT, Allocator>>{
		typedef std::basic_string<charT, Allocator> original_type;
		typedef cross_string<charT> converted_type;
		static converted_type to_converted_type(const original_type& s){
			cross_string<charT> ret;
			ret.begin = s.data();
			ret.end = s.data() + s.size();
			return ret;
		}
		static  original_type to_original_type(const converted_type& c){
			return original_type(c.begin, c.end);
		}
	};

	// Make sure that wchar_t is standard
	// On Windows wchar_t is 16 bits
	// On Linux it is 32 bits

#ifdef _WIN32
	static_assert(sizeof(wchar_t) == 2, "Sizeof wchar_t is not 16 bits on Windows");
#else
	static_assert(sizeof(wchar_t)==4,"Sizeof wchar_t is not 32 bits on Linux");
#endif

	template<bool b, class T>
	struct cross_conversion_vector_imp{};

	template<class T>
	struct cross_conversion_vector_imp<false, T>{
		typedef std::vector<T> original_type;
		typedef T original_value_type;
		typedef typename cross_conversion<T>::converted_type converted_value_type;
		typedef cross_vector<converted_value_type> converted_type;
		static error_code CROSS_CALL_CALLING_CONVENTION do_get(const void* vec, std::size_t i, converted_value_type* pt){
			try{
				auto& v = *static_cast<const original_type*>(vec);
				typedef cross_conversion<T> cc;
				*pt = cc::to_converted_type(v[i]);
				return 0;
			}
			catch (std::exception& e){
				return general_error_mapper::error_code_from_exception(e);
			}
		}

		static converted_type to_converted_type(const original_type& s){
			converted_type ret;
			ret.retvector = &s;
			ret.get = &do_get;
			ret.size = s.size();
			return ret;
		}
		static  original_type to_original_type(const converted_type& c){
			original_type ret;
			ret.reserve(c.size);
			typedef cross_conversion<T> cc;
			for (std::size_t i = 0; i < c.size; i++){
				converted_value_type v;

				auto ec = c.get(c.retvector, i, &v);
				if (ec < 0){
					general_error_mapper::exception_from_error_code(ec);
				}
				ret.push_back(cc::to_original_type(v));

			}

			return ret;
		}

	};
	template<class T>
	struct cross_conversion_vector_imp<true, T>{
		typedef std::vector<T> original_type;
		typedef cross_vector_trivial<T> converted_type;
		static converted_type to_converted_type(const original_type& s){
			converted_type ret;
			ret.begin_ = nullptr;
			ret.end_ = nullptr;
			if (s.size()){
				ret.begin_ = &s[0];
				ret.end_ = ret.begin_ + s.size();
			};
			return ret;
		}
		static  original_type to_original_type(const converted_type& c){
			return original_type(c.begin_, c.end_);
		}

	};


	template<class T>
	struct cross_conversion<std::vector<T>>
		:public cross_conversion_vector_imp<
		is_trivial_cross_conversion<T>::value, T>
	{

	};



	template<template<class> class T>
	struct cross_conversion<use_interface<T>>{
		typedef use_interface<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			return s.get_portable_base();
		}
		static  original_type to_original_type(const converted_type c){
			return use_interface<T>(reinterpret_portable_base<T>(c));
		}

	};



	template<class T, class U>
	struct  cross_pair{
		typedef  cross_conversion<T> cct;
		typedef  cross_conversion<U> ccu;

		typedef typename cct::converted_type first_type;
		typedef T original_first_type;
		first_type first;

		typedef typename ccu::converted_type second_type;
		typedef U original_second_type;
		second_type second;

	}CROSS_COMPILER_INTERFACE_PACK;


	template<class T, class U>
	struct cross_conversion<cross_pair<T, U>>:public trivial_conversion<cross_pair<T, U>>{};

	template<class T, class U>
	cross_pair<T, U> make_cross_pair(const T & t, const U & u){
		cross_pair<T, U> ret{ cross_conversion<T>::to_converted_type(t), cross_conversion<U>::to_converted_type(u) };
		return ret;
	};


	template<class T, class U>
	struct cross_conversion<std::pair<T, U>>{
		typedef std::pair<T, U> original_type;
		typedef cross_pair<T, U> converted_type;
		static converted_type to_converted_type(const original_type& s){
			converted_type ret;
			typedef cross_conversion<T> ccT;
			typedef cross_conversion<U> ccU;
			ret.first = ccT::to_converted_type(s.first);
			ret.second = ccU::to_converted_type(s.second);
			return ret;
		}
		static  original_type to_original_type(const converted_type& c){
			original_type ret;
			typedef cross_conversion<T> ccT;
			typedef cross_conversion<U> ccU;
			ret.first = ccT::to_original_type(c.first);
			ret.second = ccU::to_original_type(c.second);
			return ret;
		}

	};


	// out parameters

	template<class T, bool IsTrivial = is_trivial_cross_conversion<T>::value>
	class out{
		void* original_;
		typedef cross_conversion<T> ccT;
		typedef typename ccT::converted_type c_t;

		error_code(CROSS_CALL_CALLING_CONVENTION *assign)(void*,
			c_t);
		template<class U> friend class cross_conversion;
		static error_code CROSS_CALL_CALLING_CONVENTION do_assign(void* v,
			c_t c){
				try{
					T& t = *static_cast<T*>(v);
					t = ccT::to_original_type(c);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
		}
		out() : original_(nullptr){}
	public:
		out(T* t) : original_(t), assign(&do_assign){
			if (!original_){
				error_pointer e;
				throw e;
			}
		}
		void set(const T& t){
			typedef cross_conversion<T> cc;
			auto ec = assign(original_, cc::to_converted_type(t));
			if (ec < 0){ general_error_mapper::exception_from_error_code(ec); }
		}

	};

	template<class T>
	class out<T, true>{
		T* original_;
		out() : original_(nullptr){}
		template<class U> friend class cross_conversion;

	public:
		out(T* t) : original_(t){
			if (!original_){
				error_pointer e;
				throw e;
			}
		}
		void set(const T& t){
			*original_ = t;
		}

	};


	template<class T>
	struct cross_out{
		void* original_;
		typedef cross_conversion<T> ccT;
		typedef typename ccT::converted_type c_t;
		error_code(CROSS_CALL_CALLING_CONVENTION *assign)(void*,
			c_t);
	}CROSS_COMPILER_INTERFACE_PACK;

	template< class T>
	struct cross_conversion<out<T, false>>{
		typedef cross_out<T> converted_type;
		typedef out<T> original_type;

		static cross_out<T> to_converted_type(const out<T>& o){
			cross_out<T> ret;
			ret.assign = o.assign;
			ret.original_ = o.original_;
			return ret;
		}
		static  out<T> to_original_type(const cross_out<T> c){
			out<T> ret;
			ret.assign = c.assign;
			ret.original_ = c.original_;
			return ret;
		}

	};
	template< class T>
	struct cross_conversion<out<T, true>>{
		typedef T* converted_type;
		typedef out<T> original_type;

		static T* to_converted_type(const out<T>& o){
			return o.original_;
		}
		static  out<T> to_original_type(T* t){
			return out<T>(t);
		}

	};
	template< class T, bool b>
	struct cross_conversion_return<out<T, b>>{
		// Cannot return out parameters
	};



	// Support for tuples
	namespace detail {
		template<int...>
		struct sequence{};

		template<int, class s = sequence<>> struct make_sequence;

		template<int num, int... seq> struct make_sequence<num, sequence<seq...>>{
			typedef typename make_sequence<num - 1, sequence<num - 1, seq...>>::type type;

		};

		template<int... seq> struct make_sequence<0, sequence<seq...>>{
			typedef sequence<seq...> type;
		};

		template<class... T>
		struct to_cross_pair_t{};

		template<class First, class Second>
		struct to_cross_pair_t <First, Second>{
			typedef cross_pair<typename std::decay<First>::type,typename std::decay<Second>::type> type;
		};
		template<class First, class Second,class Third, class... Rest>
		struct to_cross_pair_t <First,Second,Third, Rest...>{
			typedef cross_pair<typename std::decay<First>::type, typename to_cross_pair_t<Second,Third,Rest...>::type> type;
		};

		template<class First, class Second>
		 cross_pair<typename std::decay<First>::type,typename std::decay<Second>::type> get_cross_pair(First && f, Second && s) {
			return make_cross_pair(std::forward<First>(f),std::forward<Second>(s));
		}	
		 
		 template<class First,class Second, class Third,class... Rest>
		typename to_cross_pair_t<First, Second, Third, Rest...>::type get_cross_pair(First && f, Second && s, Third && t, Rest && ... r){
			return make_cross_pair(std::forward<First>(f), get_cross_pair(std::forward<Second>(s),std::forward<Third>(t),std::forward<Rest>(r)...));
		}

		template<int sz,int pos, class CP>
		struct cp_getter{
			typedef typename CP::original_second_type second_type;
			typedef cp_getter<sz -1,pos - 1, second_type> getter_t;
			typedef typename getter_t::type type;

			static  type get(const CP& c){
				return getter_t::get(c.second);
			}
		};
		template<int sz, class CP>
		struct cp_getter<sz, 0, CP>{
			typedef typename CP::original_first_type type;


			static  type get(const CP& c){
				return cross_conversion<type>::to_original_type(c.first);
			}
		};
		template<class CP>
		struct cp_getter<1, 0, CP>{
			typedef CP type;

			typedef typename cross_conversion<CP>::converted_type ct_t;


			static  type get(const ct_t& c){
				return cross_conversion<type>::to_original_type(c);
			}
		};

		template<int sz, class C, int... S>
		auto cross_pair_to_tuple(const C& c, sequence<S...>)->decltype(std::make_tuple(cp_getter < sz, S, C>::get(c)...)){
			return std::make_tuple(cp_getter < sz, S, C>::get(c)...);
		}

		template<class... T, int... S>
		auto tuple_to_cross_pair_helper(const std::tuple<T...>& t, sequence<S...>)->typename to_cross_pair_t<T...>::type{
			return get_cross_pair(std::get<S>(t)...);
		}

		template<class Tuple>
		auto tuple_to_cross_pair(const Tuple& t)->decltype(tuple_to_cross_pair_helper(t, typename make_sequence<std::tuple_size<Tuple>::value>::type())){
			return tuple_to_cross_pair_helper(t, typename make_sequence<std::tuple_size<Tuple>::value>::type());
		}



		

	}

	template<class... T>
	struct cross_conversion<std::tuple<T...>>{
		typedef std::tuple<T...> original_type;
		enum{ sz = sizeof...(T) };

		typedef typename detail::to_cross_pair_t<T...>::type converted_type;

		static converted_type to_converted_type(const original_type& o){
			auto p1 = detail::tuple_to_cross_pair(o);
			return p1;
		}
		static original_type to_original_type(const converted_type& c){
			auto ret = detail::cross_pair_to_tuple < sz>(c, typename detail::make_sequence < sz>::type());
			return ret;
		}
	};
	template<class T>
	struct cross_conversion<std::tuple<T>>{
		typedef std::tuple<T> original_type;
		typedef typename cross_conversion<T>::converted_type converted_type;


		static converted_type to_converted_type(const original_type& o){
			return cross_conversion<T>::to_converted_type(std::get<0>(o));
		}
		static original_type to_original_type(const converted_type& c){
			return original_type(cross_conversion<T>::to_original_type(c));
		}
	};
	template<>
	struct cross_conversion<std::tuple<>>{
		typedef std::tuple<> original_type;
		typedef int converted_type;


		static converted_type to_converted_type(const original_type& o){
			return 0;
		}
		static original_type to_original_type(const converted_type& c){
			return std::make_tuple();
		}
	};

}


#include "cr_string.hpp"
namespace cross_compiler_interface{
	template<typename charT, typename traits>
	struct cross_conversion<basic_cr_string<charT, traits>>:public trivial_conversion<basic_cr_string<charT, traits>>{};
}
#pragma pack(pop)

#undef CROSS_COMPILER_INTERFACE_PACK