#ifndef INCLUDE_GUARD_CPPCOMPONENTS_CPPCOMPONENTS_LOW_LEVEL_HPP_
#define INCLUDE_GUARD_CPPCOMPONENTS_CPPCOMPONENTS_LOW_LEVEL_HPP_
#include "../../cross_compiler_interface/implementation/cross_compiler_interface_pp.hpp"
#ifdef _MSC_VER
// Disable some MSVC warnings
#pragma warning(push)
#pragma warning(disable:4996)
#pragma  warning(disable: 4099)
#pragma  warning(disable: 4351)
#pragma  warning(disable:4503 )
#endif

#include "../../cross_compiler_interface/implementation/cross_compiler_error_handling.hpp"

// Include Platform Specific
#ifdef _WIN32
#include "../../cross_compiler_interface/platform/Windows/platform_specific.hpp"
#endif // _WIN32
#ifdef __linux__
#include "../../cross_compiler_interface/platform/Linux/platform_specific.hpp"
#endif // __linux__


namespace cppcomponents{

	namespace detail{
		// Calling convention defined in platform specific header
		typedef  void(CROSS_CALL_CALLING_CONVENTION *ptr_fun_void_t)();
	}
	struct  portable_base{
		detail::ptr_fun_void_t* vfptr;
	};
	typedef std::int32_t error_code;

}
#include "conversions.hpp"

namespace cppcomponents{

	namespace detail{

		// base class for vtable_n
		struct vtable_n_base :public portable_base{
			void** pdata;
			vtable_n_base(detail::ptr_fun_void_t* vp, void** p) :pdata(p){ vfptr = vp; }
		};

		// Our "vtable" definition
		template<int N>
		struct vtable_n :public vtable_n_base
		{
			detail::ptr_fun_void_t table_n[N];
			void* data[N];
			enum { sz = N };
			vtable_n() :vtable_n_base(&table_n[0], &data[0]), table_n(), data(){
				vfptr = &table_n[0];
			}
		};

		template<int N, class F>
		struct vtable_entry_manipulator;


		template<int N, class R, class... P>
		struct vtable_entry_manipulator<N, R(P...)>{
			typedef R return_type;
			typedef std::tuple<P...> parameter_types;
			static R call(const portable_base* v, typename const P&... p){
				typedef cross_conversion_return<R> ccr;
				typedef typename ccr::converted_type cret_t;
				typename ccr::return_type r;
				cret_t cret;
				ccr::initialize_return(r, cret);
				typedef error_code(CROSS_CALL_CALLING_CONVENTION *fun_t)(const portable_base*, typename cross_conversion<P>::converted_type..., cret_t*);
				auto f = reinterpret_cast<fun_t>(v->vfptr[N]);
				auto ret = f(v, cross_conversion<P>::to_converted_type(p)..., &cret);
				if (ret < 0){
					general_error_mapper::exception_from_error_code(ret);
				}
				ccr::finalize_return(r, cret);
				return r;
			}

		private:
			template<class C, class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION member_function_caller(const portable_base* v, typename cross_conversion<P>::converted_type... parms, typename cross_conversion_return<R>::converted_type* r){
				typedef cross_conversion_return<R> ccr;

				try{
					void* pdata = static_cast<vtable_n_base*>(v)->pdata[N];
					C* c = static_cast<C*>(pdata);
					ccr::do_return((c->*mf)(cross_conversion<P>::to_original_type(parms)...), *r);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}

			template<class SF, SF sf>
			static error_code CROSS_CALL_CALLING_CONVENTION static_function_caller(const portable_base* v, typename cross_conversion<P>::converted_type... parms, typename cross_conversion_return<R>::converted_type* r){
				typedef cross_conversion_return<R> ccr;

				try{
					ccr::do_return(sf(cross_conversion<P>::to_original_type(parms)...), *r);
					return 0;
				}
				catch (std::exception& e){
					general_error_mapper::error_code_from_exception(e);
				}
			}
		public:
			template<class C, class MF, MF mf>
			static void set_member_function(portable_base* v,C* c){
				static_cast<vtable_n_base*>(v)->pdata[N] = c;
				v->vfptr[N] = &member_function_caller<C,MF,mf>;
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = &static_function_caller<Sf,sf>;
			}

		};
		template<int N, class... P>
		struct vtable_entry_manipulator<N, void(P...)>{
			typedef void return_type;
			typedef std::tuple<P...> parameter_types;
			static void call(const portable_base* v, typename const P&... p){
				typedef error_code(CROSS_CALL_CALLING_CONVENTION *fun_t)(const portable_base*, typename cross_conversion<P>::converted_type...);
				auto f = reinterpret_cast<fun_t>(v->vfptr[N]);
				f(v, cross_conversion<P>::to_converted_type(p)..., &cret);
				if (ret < 0){
					general_error_mapper::exception_from_error_code(ret);
				}
			}

		private:
			template<class C, class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION member_function_caller(const portable_base* v, typename cross_conversion<P>::converted_type... parms){

				try{
					void* pdata = static_cast<vtable_n_base*>(v)->pdata[N];
					C* c = static_cast<C*>(pdata);
					(c->*mf)(cross_conversion<P>::to_original_type(parms)...);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}

			template<class SF, SF sf>
			static error_code CROSS_CALL_CALLING_CONVENTION static_function_caller(const portable_base* v, typename cross_conversion<P>::converted_type... parms){
				try{
					sf(cross_conversion<P>::to_original_type(parms)...);
					return 0;
				}
				catch (std::exception& e){
					general_error_mapper::error_code_from_exception(e);
				}
			}
		public:
			template<class C, class MF, MF mf>
			static void set_member_function(portable_base* v, C* c){
				static_cast<vtable_n_base*>(v)->pdata[N] = c;
				v->vfptr[N] = &member_function_caller<C, MF, mf>;
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = &static_function_caller<Sf, sf>;
			}

		};

	}



	namespace detail{


			// Calculate return type of callable
			// Adapted from http://stackoverflow.com/questions/11893141/inferring-the-call-signature-of-a-lambda-or-arbitrary-callable-for-make-functio
			template<class F>
			using typer = F;

			template<typename T> struct remove_class { };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) const> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) const volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };

			template<class T>
			using remove_reference_t = typename std::remove_reference<T>::type;
			template<typename T>
			struct get_signature_impl {
				using type = typename remove_class<
				decltype(&remove_reference_t<T>::operator())>::type;
				using return_type = typename remove_class<
					decltype(&remove_reference_t<T>::operator())>::return_type;
			};
			template<typename R, typename... A>
			struct get_signature_impl<R(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename R, typename... A>
			struct get_signature_impl<R(&)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename R, typename... A>
			struct get_signature_impl<R(*)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename T> using get_signature = typename get_signature_impl<T>::type;
			template<typename T> using return_type = typename get_signature_impl<T>::return_type;
			template<typename T> using parameter_types = typename get_signature_impl<T>::parameter_types;


			template<int N,class R,class T>
			struct manipulate_vtable_entry_from_function_helper{};
			
			template<int N, class R, class... P>
			struct manipulate_vtable_entry_from_function_helper<N,R,std::tuple<P...>>; {
				typedef vtable_entry_manipulator<N, R(P...)> vem_t;
			};
			
			template<int N,class F>
			using manipulate_vtable_entry_from_function = typename manipulate_vtable_entry_from_function_helper<N, parameter_types<F>>::vem_t;


	}



}


#define CROSS_COMPILER_INTERFACE_COMMA  ,

#define CROSS_COMPILER_INTERFACE_STRINGIZE_EACH(T,i,x) CROSS_COMPILER_INTERFACE_STR(x)
#define CROSS_COMPILER_INTERFACE_DECLTYPE_EACH(T,i,x) decltype(std::declval<iface_t>().x) 
#define CROSS_COMPILER_INTERFACE_PTM_EACH(T,i,x) &iface_t::x 

#define CROSS_COMPILER_INTERFACE_DECLARE_CROSS_FUNCTION_EACH(T,i,x) decltype(cross_compiler_interface::detail::cf_from_member_function<Interface,i-1>(&T::x)) x
#define CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH(T,i,x) template<class... cppcomponents_static_forward_each_parameters> static cppcomponents::detail::return_type<&T::x> x (cppcomponents_static_forward_each_parameters&&... p){return Derived::factory_interface().template QueryInterface<T>().x(std::forward<cross_compiler_interface_static_forward_each_parameters>(p)...);}
#define CROSS_COMPILER_INTERFACE_DECLARE_MEMBER_FORWARD_EACH(T,i,x) template<class... cppcomponents_member_forward_each_parameters> static cppcomponents::detail::return_type<&T::x> x (cppcomponents_static_forward_each_parameters&&... p){return Derived::factory_interface().template QueryInterface<T>().x(std::forward<cross_compiler_interface_static_forward_each_parameters>(p)...);}
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_NO_PREFIX_EACH(T,i,x) x.template set_mem_fn<decltype(cross_compiler_interface::detail::mem_fn_class_type_t<Derived>::mem_fn_class_type(&Derived::x)),&Derived::x>(pthis)
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_NO_PREFIX_EACH(T,i,x) x.template set_fn<decltype(&Derived::x),&Derived::x>()
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH(T,i,x) x.template set_fn<decltype(&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T,_),x)),&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T,_),x)>()
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH(T,i,x) x.template set_mem_fn<decltype(cross_compiler_interface::detail::mem_fn_class_type_t<Derived>::mem_fn_class_type(&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x))),&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x)>(pthis)
#define CROSS_COMPILER_INTERFACE_DECLARE_CONSTRUCTOR(T,i,x) x(this)

#define CROSS_COMPILER_INTERFACE_HELPER_DEFINE_INTERFACE_CONSTRUCTOR_INTROSPECTION(T,...) \
	template<class Derived> struct cross_compiler_interface_static_interface_mapper \
	: public Interface::base_interface_t::template cross_compiler_interface_static_interface_mapper<Derived>{\
	CROSS_COMPILER_INTERFACE_SPACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH, __VA_ARGS__) \
}; \
	Interface() : CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_CONSTRUCTOR, __VA_ARGS__){}\
	private: \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cross_compiler_interface::detail::bool_to_type<true>){ \
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
	} \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cross_compiler_interface::detail::bool_to_type<false>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
	} \
public:\
	template<class Derived>\
	void map_to_member_functions_no_prefix(Derived* pthis){	\
	map_to_member_functions_no_prefix_helper(pthis, cross_compiler_interface::detail::bool_to_type<cross_compiler_interface::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_member_functions_no_prefix(pthis); \
	}\
	template<class Derived>\
	void map_to_member_functions(Derived* pthis){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_member_functions(pthis); \
	}\
	private: \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cross_compiler_interface::detail::bool_to_type<true>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
	} \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cross_compiler_interface::detail::bool_to_type<false>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
	} \
public:\
	template<class Derived>\
	void map_to_static_functions_no_prefix(){	\
	map_to_static_functions_no_prefix_helper<Derived>(cross_compiler_interface::detail::bool_to_type<cross_compiler_interface::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	base_t::template map_to_static_functions_no_prefix<Derived>(); \
	}\
	template<class Derived>\
	void map_to_static_functions(){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
	typedef typename Interface::base_interface_t base_t; \
	base_t::template map_to_static_functions<Derived>(); \
	}\
struct interface_information{	\
	template<int N> \
	static const char*(&get_type_names())[N]{   \
	static const char* names[] = { #T, CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__) }; \
	return names;    \
}\
	static std::string get_type_name(){ return  #T; } \
	typedef Interface iface_t; \
	typedef cross_compiler_interface::type_list<CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLTYPE_EACH, __VA_ARGS__)> functions; \
	typedef typename cross_compiler_interface::interface_functions_ptrs_to_member<iface_t, functions>::type functions_ptrs_to_members_t; \
	static functions_ptrs_to_members_t& get_ptrs_to_members(){	\
	static functions_ptrs_to_members_t fpm(CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_PTM_EACH, __VA_ARGS__));  \
	return fpm; \
	}\
};


#define CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE(T,B,...)   \
	template<class Type> struct Interface :public B{\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_CROSS_FUNCTION_EACH, __VA_ARGS__)\
	CROSS_COMPILER_INTERFACE_HELPER_DEFINE_INTERFACE_CONSTRUCTOR_INTROSPECTION(T, __VA_ARGS__) \
	};

#define CROSS_COMPILER_INTERFACE_CONSTRUCT_INTERFACE(T,...) \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE(T, cross_compiler_interface::define_interface<Type>, __VA_ARGS__)

#define CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE(T,...)  \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA T::uuid_type>, __VA_ARGS__)


#define CROSS_COMPILER_INTERFACE_HELPER_DEFINE_INTERFACE_CONSTRUCTOR_INTROSPECTION_NO_METHODS(T) \
	template<class Derived> struct cross_compiler_interface_static_interface_mapper \
	:public Interface::base_interface_t::template cross_compiler_interface_static_interface_mapper<Derived>{}; \
	template<class Derived>\
	void map_to_member_functions_no_prefix(Derived* pthis){	\
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_member_functions_no_prefix(pthis); \
	}\
	template<class Derived>\
	void map_to_static_functions_no_prefix(){	\
	typedef typename Interface::base_interface_t base_t; \
	base_t::template map_to_static_functions_no_prefix<Derived>(); \
	}\
	template<class Derived>\
	void map_to_static_functions(){	\
	typedef typename Interface::base_interface_t base_t; \
	base_t::template map_to_static_functions<Derived>(); \
	}\
	template<class Derived>\
	void map_to_member_functions(Derived* pthis){	\
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_member_functions(pthis); \
	}\
struct interface_information{	\
	template<int N> \
	static const char*(&get_type_names())[N]{   \
	static const char* names[] = { #T }; \
	return names;    \
}\
	static std::string get_type_name(){ return  #T; } \
	typedef Interface iface_t; \
	typedef cross_compiler_interface::type_list<> functions; \
	typedef typename cross_compiler_interface::interface_functions_ptrs_to_member<iface_t, functions>::type functions_ptrs_to_members_t; \
	static functions_ptrs_to_members_t& get_ptrs_to_members(){	\
	static functions_ptrs_to_members_t fpm;  \
	return fpm; \
	}\
};

#define CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE_NO_METHODS(T,B)   \
	template<class Type> struct Interface :public B {\
	CROSS_COMPILER_INTERFACE_HELPER_DEFINE_INTERFACE_CONSTRUCTOR_INTROSPECTION_NO_METHODS(T) \
	};

#define CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE_NO_METHODS(T)  \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE_NO_METHODS(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA T::uuid_type>)

#define CROSS_COMPILER_INTERFACE_CONSTRUCT_INTERFACE_NO_METHODS(T) \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE_NO_METHODS(T, cross_compiler_interface::define_interface<Type>)





#endif
