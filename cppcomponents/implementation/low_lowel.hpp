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
#include "../../cross_compiler_interface/implementation/safe_static_initialization.hpp"

// Include Platform Specific
#ifdef _WIN32
#include "../../cross_compiler_interface/platform/Windows/platform_specific.hpp"
#endif // _WIN32
#ifdef __linux__
#include "../../cross_compiler_interface/platform/Linux/platform_specific.hpp"
#endif // __linux__

#include "uuid.hpp"
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
#include <atomic>

namespace cppcomponents{
    using cross_compiler_interface::general_error_mapper;
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
			static R call(const portable_base* v, const P&... p){
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
					void* pdata = static_cast<const vtable_n_base*>(v)->pdata[N];
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
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&member_function_caller<C,MF,mf>);
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&static_function_caller<Sf,sf>);
			}

		};
		template<int N, class... P>
		struct vtable_entry_manipulator<N, void(P...)>{
			typedef void return_type;
			typedef std::tuple<P...> parameter_types;
			static void call(const portable_base* v, const P&... p){
				typedef error_code(CROSS_CALL_CALLING_CONVENTION *fun_t)(const portable_base*, typename cross_conversion<P>::converted_type...);
				auto f = reinterpret_cast<fun_t>(v->vfptr[N]);
				auto ret = f(v, cross_conversion<P>::to_converted_type(p)...);
				if (ret < 0){
					general_error_mapper::exception_from_error_code(ret);
				}
			}

		private:
			template<class C, class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION member_function_caller(const portable_base* v, typename cross_conversion<P>::converted_type... parms){

				try{
					void* pdata = static_cast<const vtable_n_base*>(v)->pdata[N];
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
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&member_function_caller<C, MF, mf>);
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&static_function_caller<Sf, sf>);
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
			struct remove_class<R(C::*)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) const> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct remove_class<R(C::*)(A...) const volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };

			template<class T>
			using remove_reference_t = typename std::remove_reference<T>::type;
			template<typename T>
			struct get_signature_impl {
				using type = typename remove_class<
				decltype(&remove_reference_t<T>::operator())>::type;
				using return_type = typename remove_class<
					decltype(&remove_reference_t<T>::operator())>::return_type;
				using parameter_types = typename remove_class<
					decltype(&remove_reference_t<T>::operator())>::parameter_types;
			};
			template<typename R, typename... A>
			struct get_signature_impl<R(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename R, typename... A>
			struct get_signature_impl<R(&)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename R, typename... A>
			struct get_signature_impl<R(*)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; };
			template<typename C, typename R, typename... A>
			struct get_signature_impl<R(C::*)(A...)> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct get_signature_impl<R(C::*)(A...) const> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct get_signature_impl<R(C::*)(A...) volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };
			template<typename C, typename R, typename... A>
			struct get_signature_impl<R(C::*)(A...) const volatile> { using type = typer<R(A...)>; using return_type = R; using parameter_types = std::tuple<A...>; using class_type = C; };

			template<typename T> using get_signature = typename get_signature_impl<T>::type;
			template<typename T> using return_type = typename get_signature_impl<T>::return_type;
			template<typename T> using parameter_types = typename get_signature_impl<T>::parameter_types;
			template<typename T> using class_type = typename get_signature_impl<T>::class_type;


			template<int N,class R,class T>
			struct manipulate_vtable_entry_from_function_helper{};
			
			template<int N, class R, class... P>
			struct manipulate_vtable_entry_from_function_helper<N,R,std::tuple<P...>>{
				typedef vtable_entry_manipulator<N, R(P...)> vem_t;
			};
			
			template<int N,class F>
			using vtable_entry_manipulator_from_function = typename manipulate_vtable_entry_from_function_helper<N,return_type<F>, parameter_types<F>>::vem_t;


			template<bool>
			struct bool_to_type{};
	}
        template<class T>
	struct allow_interface_to_map_no_prefix{
		enum{
			value = true
		};
	};
	struct uuid_base;
	namespace detail{
		template<class T>
		struct dummy_low_level_interface{
			enum{ interface_size = 0 };
			enum{ base_interface_size = 0 };
		};


	        template<int N>
		struct qi_vtable_entry_manipulator{
			typedef portable_base* return_type;

			typedef std::tuple<> parameter_types;
			static portable_base* call(const portable_base* v, const uuid_base* u){
				portable_base* ppv = nullptr;
		//error_code(portable_base*, const uuid_base*,portable_base**)
				typedef error_code(CROSS_CALL_CALLING_CONVENTION *fun_t)(const portable_base*,const uuid_base*,portable_base**);
				auto f = reinterpret_cast<fun_t>(v->vfptr[N]);
				auto ret = f(v, u, &ppv);
				if (ret < 0){
					general_error_mapper::exception_from_error_code(ret);
				}
				return ppv;

			}

		private:
			template<class C, class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION member_function_caller(const portable_base* v,const uuid_base* u,portable_base** ppv){

				try{
					void* pdata = static_cast<const vtable_n_base*>(v)->pdata[N];
					C* c = static_cast<C*>(pdata);
					*ppv = (c->*mf)(u);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}

			template<class SF, SF sf>
			static error_code CROSS_CALL_CALLING_CONVENTION static_function_caller(const portable_base* v,const uuid_base* u,portable_base** ppv){

				try{
					void* pdata = static_cast<vtable_n_base*>(v)->pdata[N];
					*ppv = sf(u);
					return 0;
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}
		public:
			template<class C, class MF, MF mf>
			static void set_member_function(portable_base* v,C* c){
				static_cast<vtable_n_base*>(v)->pdata[N] = c;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&member_function_caller<C,MF,mf>);
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&static_function_caller<Sf,sf>);
			}

		};

		        template<int N>
		struct addref_release_vtable_entry_manipulator{
			typedef portable_base* return_type;

			typedef std::tuple<> parameter_types;
			static std::int32_t call(const portable_base* v){
				portable_base* ppv = nullptr;
		//error_code(portable_base*, const uuid_base*,portable_base**)
				typedef std::int32_t(CROSS_CALL_CALLING_CONVENTION *fun_t)(const portable_base*);
				auto f = reinterpret_cast<fun_t>(v->vfptr[N]);
				return f(v);
			}

		private:
			template<class C, class MF, MF mf>
			static std::int32_t CROSS_CALL_CALLING_CONVENTION member_function_caller(const portable_base* v){

				try{
					void* pdata = static_cast<const vtable_n_base*>(v)->pdata[N];
					C* c = static_cast<C*>(pdata);
					return (c->*mf)();
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}
			template<class SF, SF sf>
			static std::int32_t CROSS_CALL_CALLING_CONVENTION static_function_caller(const portable_base* v){

				try{
					void* pdata = static_cast<const vtable_n_base*>(v)->pdata[N];
					return sf();
				}
				catch (std::exception& e){
					return general_error_mapper::error_code_from_exception(e);
				}
			}
		public:
			template<class C, class MF, MF mf>
			static void set_member_function(portable_base* v,C* c){
				static_cast<vtable_n_base*>(v)->pdata[N] = c;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&member_function_caller<C,MF,mf>);
			}
			template<class SF, SF sf>
			static void set_static_function(portable_base* v){
				static_cast<vtable_n_base*>(v)->pdata[N] = nullptr;
				v->vfptr[N] = reinterpret_cast<detail::ptr_fun_void_t>(&static_function_caller<Sf,sf>);
			}

		};

	}

	struct InterfaceUnknown{
		template<class T>
		using cppcomponents_low_level_base_interface_t = detail::dummy_low_level_interface<T>;
		template<class T>
		struct Interface{
		private:
			cppcomponents::portable_base* cppcomponents_portable_base_pointer_;
		public:
			enum{ interface_size = 3 };
			enum{ base_interface_size = 0 };
			Interface(cppcomponents::portable_base* p):cppcomponents_portable_base_pointer_{p}{}
			template<class Derived> struct cross_compiler_interface_static_interface_mapper{};
			portable_base* QueryInterfaceRaw(const uuid_base* u)const{
				return detail::qi_vtable_entry_manipulator<0>::call(cppcomponents_portable_base_pointer_, u);
			}
			std::int32_t AddRef()const{
				return detail::addref_release_vtable_entry_manipulator<1>::call(cppcomponents_portable_base_pointer_);
			}
			std::int32_t Release()const{
				return detail::addref_release_vtable_entry_manipulator<2>::call(cppcomponents_portable_base_pointer_);
			}
			template<class Derived>
			void map_to_member_functions_no_prefix_internal(Derived* pthis){
				detail::qi_vtable_entry_manipulator<0>::set_member_function<Derived,decltype(&Derived::QueryInterfaceRaw),&Derived::QueryInterfaceRaw>(cppcomponents_portable_base_pointer_, pthis);
				detail::addref_release_vtable_entry_manipulator<1>::set_member_function<Derived,decltype(&Derived::AddRef),&Derived::AddRef>(cppcomponents_portable_base_pointer_, pthis);
				detail::addref_release_vtable_entry_manipulator<2>::set_member_function<Derived,decltype(&Derived::Release),&Derived::Release>(cppcomponents_portable_base_pointer_, pthis);

			}
			template<class Derived>
			void map_to_member_functions(Derived* pthis){}
	    template<class Derived>
		void map_to_member_functions_no_prefix(Derived* pthis){}
			template<class Derived>
			void map_to_static_functions_no_prefix(){}
			template<class Derived>
			void map_to_static_functions(){}
			template<class Derived>
			void map_to_constructors(){}


			struct interface_information{

				static const char** get_function_names(){
					static const char* names[] = { "QueryInterface", "AddRef", "Release" };
					return names;
				}
				static std::string get_type_name(){ return  "InterfaceUnknown"; }
				typedef Interface iface_t;
				typedef std::tuple<detail::qi_vtable_entry_manipulator<0>, detail::addref_release_vtable_entry_manipulator<1>, detail::addref_release_vtable_entry_manipulator<2>> functions;
			};
		typedef uuid<0x00000000,0x0000,0x0000,0xc000,0x000000000046>  uuid_type;

		};

		typedef uuid<0x00000000,0x0000,0x0000,0xc000,0x000000000046>  uuid_type;

		template<class T>
		struct InterfaceExtras{};
		template<class T>
		struct StaticInterfaceExtras{};


	};


	namespace detail{
		template<class Derived, template<class> class U, class PT>
		struct activate_instance_parms_caller{};
		template<class Derived, template<class> class U, class... P>
		struct activate_instance_parms_caller <Derived, U, std::tuple<P...>>{
			static U<InterfaceUnknown> call(P&... p){
				Derived::activate_instance_parms(std::forward<P>(p)...);
			}

		};

		template<class Interface>
		struct reinterpret_portable_base_t{
			portable_base* p_;
			explicit reinterpret_portable_base_t(portable_base* p) :p_{ p }{}
			portable_base* get(){ return p_; }
		};


	}

    template<class Interface>
	detail::reinterpret_portable_base_t<Interface> reinterpret_portable_base(portable_base* p){
		return detail::reinterpret_portable_base_t<Interface>{ p };
	}

    template<class... T>
	using type_list = std::tuple<T...>;

	template<template<class> class T>
	struct implement_interface{
	private:
		typedef T<implement_interface> i_t;
		detail::vtable_n < i_t::interface_size + i_t::base_interface_size> vtable;
	public:
		typedef typename i_t::uuid_type uuid_type;
		portable_base* get_portable_base(){
			return &vtable;
		}
		const portable_base* get_portable_base()const{
			return &vtable;
		}
		i_t get_interface()const {
			const portable_base* p = &vtable;
			return i_t( const_cast<portable_base*>(p) );
		}

	};
    	struct object_counter{
	private:
		object_counter(){}

		// Non-copyable/movable
		object_counter(const object_counter&);
		object_counter( object_counter&&);
		object_counter& operator=(const object_counter&);
		object_counter& operator=(object_counter&&);
		std::atomic<std::size_t> count_;

		friend class cross_compiler_interface::detail::safe_static_init<object_counter, object_counter>;
	public:
		// Meyers singleton
		static object_counter& get(){
			// Static initialization is not threadsafe in MSVC 2013
			// so use helper
			return cross_compiler_interface::detail::safe_static_init<object_counter, object_counter>::get();
		}
		std::size_t get_count(){
			return count_;
		}
		void increment(){
			++count_;
		};
		void decrement(){
			--count_;
		}



	};


}
#if 0
// This is how our Interface will be look
struct MyInterface {
        template<class T>
	using cppcomponents_base_interface_t::template Interface = std::tuple<T>;
	typedef cppcomponents::base_interface_t;
	std::string MyFunc();
        template<class T>
	struct Interface :public cppcomponents_base_interface_t::template Interface{
		enum{ interface_size = CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__) };
		enum{ base_interface_size = cppcomponents_base_interface_t::template Interface<T>::interface_size + cppcomponents_base_interface_t::template Interface<T>::base_interface_size };
		template<class Derived> struct cross_compiler_interface_static_interface_mapper;
		std::string MyFunc(){ vtable_entry_manipulator::call() }

		template<class Derived>
		void map_to_member_functions_no_prefix(Derived* pthis);
		template<class Derived>
		void map_to_member_functions(Derived* pthis)
			template<class Derived>
		void map_to_static_functions_no_prefix();
		template<class Derived>
		void map_to_static_functions()
		struct interface_information{

			template<int N>
			static const char*(&get_type_names())[N]{
				static const char* names[] = { #T, CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__) };
				return names;
			}
			static std::string get_type_name(){ return  #T; }
			typedef Interface iface_t;
			typedef std::tuple<CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLTYPE_EACH, __VA_ARGS__)> functions;
		};
	}



}

#endif

#define CROSS_COMPILER_INTERFACE_COMMA  ,

#define CROSS_COMPILER_INTERFACE_STRINGIZE_EACH(T,i,x) CROSS_COMPILER_INTERFACE_STR(x)

#define CROSS_COMPILER_INTERFACE_VEMT_EACH(T,i,x) cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::x)>

#define CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH(T,i,x) template<class... cppcomponents_static_forward_each_parameters> static auto x (cppcomponents_static_forward_each_parameters&&... p)->cppcomponents::detail::return_type<decltype(&T::x)>{return Derived::factory_interface().template QueryInterface<T>().x(std::forward<cppcomponents_static_forward_each_parameters>(p)...);}
#define CROSS_COMPILER_INTERFACE_DECLARE_MEMBER_FORWARD_EACH(T,i,x) template<class... cppcomponents_member_forward_each_parameters> cppcomponents::detail::return_type<decltype(&T::x)> x (cppcomponents_member_forward_each_parameters&&... p)const{return cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::x)>::call(cppcomponents_portable_base_pointer_,std::forward<cppcomponents_member_forward_each_parameters>(p)...);}

#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_NO_PREFIX_EACH(T,i,x)cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::x)>::set_member_function<cppcomponents::detail::class_type<decltype(&Derived::x)>,decltype(&Derived::x),&Derived::x>(cppcomponents_portable_base_pointer_,pthis) 
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH(T,i,x)cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x))>::set_member_function<cppcomponents::detail::class_type<decltype(&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x))>,decltype(&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x)),&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x)>(cppcomponents_portable_base_pointer_,pthis) 

#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_NO_PREFIX_EACH(T,i,x)cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::x)>::set_static_function<decltype(&Derived::x),&Derived::x>(cppcomponents_portable_base_pointer_) 
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH(T,i,x)cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x))>::set_static_function<decltype(&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x)),&Derived::CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_CAT(T, _), x)>(cppcomponents_portable_base_pointer_) 

//activate_instance_parms
#define CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_CONSTRUCTORS_EACH(T,i,x)cppcomponents::detail::vtable_entry_manipulator_from_function<(i-1) + base_interface_size,decltype(&T::x)>::set_static_function<decltype(&detail::activate_instance_parms<Derived,cppcomponents::use,cppcomponents::detail::parameter_types<&Derived::x>>::call),&detail::activate_instance_parms<Derived,cppcomponents::use,cppcomponents::detail::parameter_types<&Derived::x>>::call>(cppcomponents_portable_base_pointer_) 

#define CROSS_COMPILER_INTERFACE_DECLARE_CONSTRUCTOR(T,i,x) x(this)

//#define CROSS_COMPILER_INTERFACE_CONSTRUCT_INTERFACE(T,...)   \
//	template<class T>struct Interface :public base_interface_t::template Interface<T>{\
//	private: \
//	cppcomponents::portable_base* cppcomponents_portable_base_pointer_; \
//	public: \
//	Interface(cppcomponents::portable_base* p) :cppcomponents_portable_base_pointer_{ p }{} \
//	CROSS_COMPILER_INTERFACE_SPACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH, __VA_ARGS__) \
//enum{ interface_size = CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__) }; \
//enum{ base_interface_size = base_interface_t::template Interface<T>::interface_size + base_interface_t::template Interface<T>::base_interface_size }\
//	template<class Derived> struct cross_compiler_interface_static_interface_mapper \
//	: public Interface::base_interface_t::template cross_compiler_interface_static_interface_mapper<Derived>{\
//	CROSS_COMPILER_INTERFACE_SPACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH, __VA_ARGS__) \
//}; \
//	private: \
//	template<class Derived> \
//	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
//	cppcomponents::detail::bool_to_type<true>){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
//	} \
//	template<class Derived> \
//	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
//	cppcomponents::detail::bool_to_type<false>){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
//	} \
//	public:\
//	template<class Derived>\
//	void map_to_member_functions_no_prefix(Derived* pthis){\
//	map_to_member_functions_no_prefix_helper(pthis, cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
//	typedef typename Interface::base_interface_t base_t; \
//	base_t::map_to_member_functions_no_prefix(pthis); \
//	}\
//	template<class Derived>\
//	void map_to_member_functions(Derived* pthis){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
//	typedef typename Interface::base_interface_t base_t; \
//	base_t::map_to_member_functions(pthis); \
//	}\
//	private: \
//	template<class Derived> \
//	void map_to_static_functions_no_prefix_helper(\
//	cppcomponents::detail::bool_to_type<true>){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
//	} \
//	template<class Derived> \
//	void map_to_static_functions_no_prefix_helper(\
//	cppcomponents::detail::bool_to_type<false>){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
//	} \
//	public:\
//	template<class Derived>\
//	void map_to_static_functions_no_prefix(){\
//	map_to_static_functions_no_prefix_helper<Derived>(cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
//	typedef typename Interface::base_interface_t base_t; \
//	base_t::map_to_static_functions_no_prefix<Derived>(); \
//	}\
//	template<class Derived>\
//	void map_to_static_functions(){\
//	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
//	typedef typename Interface::base_interface_t base_t; \
//	base_t::map_to_static_functions<Derived>(); \
//	}\
//struct interface_information{\
//	template<int N> \
//	static const char*(&get_type_names())[N]{   \
//	static const char* names[] = { #T, CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__) }; \
//	return names;    \
//}\
//	static std::string get_type_name(){ return  #T; } \
//	typedef Interface iface_t; \
//	typedef std::tuple<CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_VEMT_EACH, __VA_ARGS__)> functions; \
//}\
//	};


#define CROSS_COMPILER_INTERFACE_CONSTRUCT_INTERFACE(T,...)   \
	template<class TI>struct Interface :public base_interface_t::template Interface<TI>{\
	private: \
	cppcomponents::portable_base* cppcomponents_portable_base_pointer_; \
    		typedef base_interface_t::template Interface<TI> BaseInterface; \
	public: \
        typedef typename T::uuid_type uuid_type;\
	typedef typename T::base_interface_t base_interface_t;\
	Interface(cppcomponents::portable_base* p) :BaseInterface{p},cppcomponents_portable_base_pointer_{ p }{} \
enum{ interface_size = CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__) }; \
enum{ base_interface_size = BaseInterface::interface_size + BaseInterface::base_interface_size }; \
	CROSS_COMPILER_INTERFACE_SPACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MEMBER_FORWARD_EACH, __VA_ARGS__) \
	template<class Derived> struct cross_compiler_interface_static_interface_mapper \
	: public BaseInterface::template cross_compiler_interface_static_interface_mapper<Derived>{\
	CROSS_COMPILER_INTERFACE_SPACE_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_STATIC_FORWARD_EACH, __VA_ARGS__) \
}; \
	private: \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cppcomponents::detail::bool_to_type<true>){ \
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
	} \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cppcomponents::detail::bool_to_type<false>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
	} \
public:\
	template<class Derived>\
	void map_to_member_functions_no_prefix(Derived* pthis){	\
	map_to_member_functions_no_prefix_helper(pthis, cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_member_functions_no_prefix(pthis); \
	}\
	template<class Derived>\
	void map_to_member_functions(Derived* pthis){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_MEMBER_FUNCTIONS_EACH, __VA_ARGS__); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_member_functions(pthis); \
	}\
	private: \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cppcomponents::detail::bool_to_type<true>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_NO_PREFIX_EACH, __VA_ARGS__); \
	} \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cppcomponents::detail::bool_to_type<false>){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
	} \
public:\
	template<class Derived>\
	void map_to_static_functions_no_prefix(){	\
	map_to_static_functions_no_prefix_helper<Derived>(cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_static_functions_no_prefix<Derived>(); \
	}\
	template<class Derived>\
	void map_to_static_functions(){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_STATIC_FUNCTIONS_EACH, __VA_ARGS__); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_static_functions<Derived>(); \
	}\
	template<class Derived>\
	void map_to_constructors(){	\
	CROSS_COMPILER_INTERFACE_SEMICOLON_APPLY(T, CROSS_COMPILER_INTERFACE_DECLARE_MAP_TO_CONSTRUCTORS_EACH, __VA_ARGS__); \
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_constructors<Derived>(); \
	}\
struct interface_information{	\
	static const char** get_function_names(){   \
	static const char* names[] = { #T, CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__) }; \
	return names;    \
}\
	static std::string get_type_name(){ return  #T; } \
	typedef Interface iface_t; \
	typedef std::tuple<CROSS_COMPILER_INTERFACE_APPLY(T, CROSS_COMPILER_INTERFACE_VEMT_EACH, __VA_ARGS__)> functions; \
};\
	};

#define CROSS_COMPILER_INTERFACE_CONSTRUCT_INTERFACE_NO_MEMBERS(T)   \
	template<class TI >struct Interface :public base_interface_t::template Interface<TI>{\
	private: \
	cppcomponents::portable_base* cppcomponents_portable_base_pointer_; \
        		typedef base_interface_t::template Interface<TI> BaseInterface; \
	public: \
        typedef typename T::uuid_type uuid_type;\
	typedef typename T::base_interface_t base_interface_t;\
	Interface(cppcomponents::portable_base* p) :BaseInterface{p},cppcomponents_portable_base_pointer_{ p }{} \
enum{ interface_size = 0 }; \
enum{ base_interface_size = base_interface_t::template Interface<TI>::interface_size + base_interface_t::template Interface<TI>::base_interface_size }; \
	template<class Derived> struct cross_compiler_interface_static_interface_mapper \
	: public BaseInterface::template cross_compiler_interface_static_interface_mapper<Derived>{\
}; \
	private: \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cppcomponents::detail::bool_to_type<true>){ \
	} \
	template<class Derived> \
	void map_to_member_functions_no_prefix_helper(Derived* pthis, \
	cppcomponents::detail::bool_to_type<false>){	\
	} \
public:\
	template<class Derived>\
	void map_to_member_functions_no_prefix(Derived* pthis){	\
	map_to_member_functions_no_prefix_helper(pthis, cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_member_functions_no_prefix(pthis); \
	}\
	template<class Derived>\
	void map_to_member_functions(Derived* pthis){	\
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_member_functions(pthis); \
	}\
	private: \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cppcomponents::detail::bool_to_type<true>){	\
	} \
	template<class Derived> \
	void map_to_static_functions_no_prefix_helper(\
	cppcomponents::detail::bool_to_type<false>){	\
	} \
public:\
	template<class Derived>\
	void map_to_static_functions_no_prefix(){	\
	map_to_static_functions_no_prefix_helper<Derived>(cppcomponents::detail::bool_to_type<cppcomponents::allow_interface_to_map_no_prefix<T>::value>()); \
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_static_functions_no_prefix<Derived>(); \
	}\
	template<class Derived>\
	void map_to_static_functions(){	\
	typedef typename Interface::base_interface_t base_t; \
	BaseInterface::map_to_static_functions<Derived>(); \
	}\
	template<class Derived>\
	void map_to_constructors(){	\
	typedef typename Interface::base_interface_t base_t; \
	base_t::map_to_constructors<Derived>(); \
	}\
    struct interface_information{	\
	static const char** get_function_names(){   \
	return nullptr;\
}\
	static std::string get_type_name(){ return  #T; } \
	typedef Interface iface_t; \
	typedef std::tuple<> functions; \
};\
	};






#endif
