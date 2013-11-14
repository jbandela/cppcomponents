//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_CPPCOMPONENTS_HPP_05_29_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_CPPCOMPONENTS_HPP_05_29_2013_


#include "../cross_compiler_interface/cross_compiler_introspection.hpp"
#include "../cross_compiler_interface/implementation/safe_static_initialization.hpp"
#include "implementation/spinlock.hpp"
#include <unordered_map>

#define CPPCOMPONENTS_CONSTRUCT(T,...)  \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA  T::uuid_type CROSS_COMPILER_INTERFACE_COMMA base_interface_t::template Interface>, __VA_ARGS__)

#define CPPCOMPONENTS_CONSTRUCT_NO_METHODS(T)  \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE_NO_METHODS(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA  T::uuid_type CROSS_COMPILER_INTERFACE_COMMA base_interface_t::template Interface>)

#define CPPCOMPONENTS_CONSTRUCT_TEMPLATE(T,...)  \
	typedef typename T::base_interface_t base_interface_t; \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA typename T::uuid_type CROSS_COMPILER_INTERFACE_COMMA base_interface_t::template Interface>, __VA_ARGS__)

#define CPPCOMPONENTS_CONSTRUCT_NO_METHODS_TEMPLATE(T)  \
	typedef typename T::base_interface_t base_interface_t; \
	CROSS_COMPILER_INTERFACE_HELPER_CONSTRUCT_INTERFACE_NO_METHODS(T, cross_compiler_interface::define_unknown_interface<Type CROSS_COMPILER_INTERFACE_COMMA typename T::uuid_type CROSS_COMPILER_INTERFACE_COMMA base_interface_t::template Interface>)


namespace cppcomponents{
	struct InterfaceUnknown;
}

namespace cross_compiler_interface{

	namespace detail{

		template<class T, class Base>
		struct is_base{
			typedef typename T::base_interface_t tbase;
			enum{value = is_base<tbase,Base>::value};
		};

		template<class Base>
		struct is_base<Base, Base>{
			enum {value = true};
		};

		template<class Base>
		struct is_base<cppcomponents::InterfaceUnknown, Base>{
			enum {value = false};
		};
		template<>
		struct is_base<cppcomponents::InterfaceUnknown, cppcomponents::InterfaceUnknown>{
			enum {value = true};
		};

	}

    	template<class Iface>
	struct use:private portable_base_holder, public Iface::template Interface<use<Iface> >, public Iface::template InterfaceExtras<use<Iface>>{ // Usage

		typedef Iface interface_t;

		use(std::nullptr_t p = nullptr) :portable_base_holder(nullptr){ (void)p; }

		use(detail::reinterpret_portable_base_t<Iface::template Interface> r,bool bAddRef):portable_base_holder(r.get()){
			if(*this && bAddRef){
				this->AddRef();
			}
		}

		use(use_interface<Iface::template Interface> u,bool bAddRef):portable_base_holder(u.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use(implement_interface<Iface::template Interface>& i,bool bAddRef):portable_base_holder(i.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use(const use<Iface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
		}
		template<class T>
		use(const use<T>& other):portable_base_holder(other.get_portable_base()){
			static_assert(detail::is_base<T,Iface>::value,"Cannot implicitly convert interface, use QueryInterface instead");
			if(*this){
				this->AddRef();
			}
		}

		// Move constructor
		use(use<Iface>&& other):portable_base_holder(other.get_portable_base()){
			other.reset_portable_base();
		}

        // Construct from use_unkown
		use(const use_unknown<Iface::template Interface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
		}

		// Move constructor
 		use(use_unknown<Iface::template Interface>&& other):portable_base_holder(other.get_portable_base()){
			other.reset_portable_base();
		}

		use& operator=(const use<Iface>& other){
			// Note - order will deal with self assignment as we increment the refcount before decrementing
			if(other){
				other.AddRef();
			}
			if(*this){
				this->Release();
			}
			this->p_ = other.get_portable_base();
			static_cast < typename Iface::template Interface < use<Iface >> &>(*this) = other;
			static_cast < typename Iface::template InterfaceExtras < use<Iface >> &>(*this) = other;
			return *this;
		}
		// Move constructor
		use& operator=(use<Iface>&& other){
			// can't move to ourself
			assert(this != &other);
			if(*this){
				this->Release();
			}

			this->p_ = other.get_portable_base();
			static_cast < typename Iface::template Interface < use<Iface >> &>(*this) = other;
			static_cast < typename Iface::template InterfaceExtras < use<Iface >> &>(*this) = other;

			other.reset_portable_base();
			return *this;
		}
		template<class OtherIface>
		use<OtherIface> QueryInterface()const{
			auto ret = QueryInterfaceNoThrow<OtherIface>();
			if (!ret){
				throw error_no_interface();
			}
			return ret;

		}

		template<class OtherIface>
		use<OtherIface> QueryInterfaceNoThrow()const{
			if(!*this){
				return nullptr;
			}
			typedef typename OtherIface::template Interface<use<OtherIface>>::uuid_type uuid_type;
			portable_base* r = this->QueryInterfaceRaw(&uuid_type::get());

			// AddRef already called by QueryInterfaceRaw
			return use<OtherIface>(reinterpret_portable_base<OtherIface::template Interface>(r),false);

		}

		~use(){
			if(*this){
				this->Release();
			}
		}


		portable_base* get_portable_base()const {
			return this->p_;
		}


		portable_base* get_portable_base_addref()const {
			auto r = get_portable_base();
			if(r){
				this->AddRef();
			}
			return r;
		}

		explicit operator bool()const{
			return get_portable_base()!=nullptr;
		}

		use_interface<Iface::template Interface> get_use_interface(){
			return use_interface<Iface::template Interface>(reinterpret_portable_base < Iface::template Interface>(get_portable_base()));
		}

		void reset_portable_base(){
			// This line prevents a release
			this->p_ = nullptr;


			use empty;
			*this = empty;
		}
		//enum{num_functions = sizeof(typename Iface::template Interface<size_only>)/sizeof(cross_function<typename Iface::template Interface<size_only>,0,void()>)};

	private:


		//// Simple checksum that takes advantage of the fact that 1+2+3+4...n = n(n+1)/2
		//enum{checksum = sizeof(typename Iface::template Interface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		//// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		//static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

        // Hide AddRef and Release
        // We make this const because lifetime management would be 
        // analogous to delete in that you can delete a const
        uint32_t AddRef()const{
           return Iface::template Interface<use<Iface>>::AddRef();
        }

        uint32_t Release()const{
           return Iface::template Interface<use<Iface>>::Release();
        }


	};


	template<class T>
	struct cross_conversion<use<T>>{
		typedef use<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			return s.get_portable_base();
		}
		static  original_type to_original_type(converted_type c){
			return use<T>(reinterpret_portable_base<T::template Interface>(c),true);
		}

	};

	// We specialize the default cross_conversion_return 
	// because by doing this we can avoid an AddRef/Release pair
	template<class T>
	struct cross_conversion_return<use<T>>{
		typedef cross_conversion<use<T>> cc;
		typedef typename cc::original_type return_type;
		typedef typename cc::converted_type converted_type;

		static void initialize_return(return_type& , converted_type& ){}

		static void do_return(return_type && r, converted_type& c){
			c = r.get_portable_base();
			r.reset_portable_base();
		}
		static void finalize_return(return_type& r, converted_type& c){
			r = use<T>(cross_compiler_interface::reinterpret_portable_base<T::template Interface>(c), false);
		}

	};


        template<class Iface > 
    struct type_information<use<Iface>>{
        enum{is_interface = 1}; 
        enum{is_unknown_interface = 1}; 
		typedef typename use<Iface>::interface_information ii_t;
        static std::string name(){
			
			return ii_t::get_type_name() ;
		} 
        enum{names_size = 1+use_interface<Iface::template Interface>::num_functions - use_interface<Iface::template Interface>::base_sz};
        static const char* (&names())[names_size]{return  ii_t:: template get_type_names<names_size>();}
        typedef typename ii_t::functions functions; 
        static_assert(functions::size == names_size-1,"Functions defined and functions specified to CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION macro does not match");
        typedef typename ii_t::functions_ptrs_to_members_t functions_ptrs_to_members_t;
        static functions_ptrs_to_members_t& get_ptrs_to_members(){
            return ii_t::get_ptrs_to_members();
        }

        template<class CF>
        static CF& get_cross_function(use<Iface> iface){
            return iface.*get_ptrs_to_members().template get<CF>();
        }
    };  
}
namespace cppcomponents{
	using cross_compiler_interface::use;
	using cross_compiler_interface::portable_base;
	using cross_compiler_interface::error_code;
	using cross_compiler_interface::type_list;
	using cross_compiler_interface::uuid;
	using cross_compiler_interface::uuid_base;

	typedef cross_compiler_interface::cross_compiler_interface_error_base cppcomponent_error;

	using cross_compiler_interface::error_fail; 
	using cross_compiler_interface::error_handle; 
	using cross_compiler_interface::error_invalid_arg;
	using cross_compiler_interface::error_no_interface;
	using cross_compiler_interface::error_not_implemented; 
	using cross_compiler_interface::error_out_of_memory; 
	using cross_compiler_interface::error_out_of_range; 
	using cross_compiler_interface::error_pending;
	using cross_compiler_interface::error_pointer; 
	using cross_compiler_interface::error_unexpected; 
	using cross_compiler_interface::error_abort; 
	using cross_compiler_interface::error_access_denied;
		
	using cross_compiler_interface::error_shared_function_not_found;
		
	using cross_compiler_interface::error_unable_to_load_library;
	
	typedef cross_compiler_interface::general_error_mapper error_mapper;

	using cross_compiler_interface::basic_cr_string;
	using cross_compiler_interface::cr_string;
	using cross_compiler_interface::cr_u16string;
	using cross_compiler_interface::cr_u32string;
	using cross_compiler_interface::cr_wstring;


	inline void throw_if_error(error_code e){
		if (e < 0) cross_compiler_interface::general_error_mapper::exception_from_error_code(e);
	}

	template<class Iface>
	cross_compiler_interface::detail::reinterpret_portable_base_t<Iface::template Interface> reinterpret_portable_base(portable_base* p){
		return cross_compiler_interface::detail::reinterpret_portable_base_t<Iface::template Interface>(p);
	}


	struct InterfaceUnknown{
			typedef cross_compiler_interface::Unknown_uuid_t uuid_type;

		template<class T>
		struct Interface : public cross_compiler_interface::InterfaceUnknown<T>{

			typedef cross_compiler_interface::map_to_functions_dummy base_interface_t;
		
			CROSS_COMPILER_INTERFACE_HELPER_DEFINE_INTERFACE_CONSTRUCTOR_INTROSPECTION_NO_METHODS(InterfaceUnknown)
		
		};

		template<class T>
		struct InterfaceExtras{};
		template<class T>
		struct StaticInterfaceExtras{};
	};


	// Now that we have IUnknown, we can define relational operators of use<>
	namespace detail{

		inline portable_base* portable_base_from_unknown(const use<InterfaceUnknown>& i){
			if (!i)return nullptr;
			return i.get_portable_base();
		}

		template<class I>
		portable_base* portable_base_from_unknown(const use<I>& i){
			if (!i)return nullptr;
			return i.template QueryInterface<InterfaceUnknown>().get_portable_base();
		}

	}
	template<class I1, class I2>
	bool operator == (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::equal_to<portable_base*> comp;
		return comp(p1, p2);
	}
	template<class I1, class I2>
	bool operator != (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::not_equal_to<portable_base*> comp;
		return comp(p1, p2);
	}
	template<class I1, class I2>
	bool operator < (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::less<portable_base*> comp;
		return comp(p1, p2);
	}
	template<class I1, class I2>
	bool operator <= (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::less_equal<portable_base*> comp;
		return comp(p1, p2);
	}
	template<class I1, class I2>
	bool operator > (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::greater<portable_base*> comp;
		return comp(p1, p2);
	}
	template<class I1, class I2>
	bool operator >= (const use<I1>& i1, const use<I2>& i2){
		auto p1 = detail::portable_base_from_unknown(i1);
		auto p2 = detail::portable_base_from_unknown(i2);
		std::greater_equal<portable_base*> comp;
		return comp(p1, p2);
	}


	


	template < class TUUID, class Base = InterfaceUnknown >
	struct define_interface{
		typedef Base base_interface_t;
		typedef TUUID uuid_type;

		template<class T>
		struct InterfaceExtras : public Base::template InterfaceExtras<T>{};

		template<class T>
		struct InterfaceExtrasBase : public InterfaceExtras<T>{
			
		protected:
			T& get_interface(){
				return *static_cast<T*>(this);
			}
			const T& get_interface()const {
				return *static_cast<const T*>(this);
			}


		};

		template<class T> struct StaticInterfaceExtras :public Base::template StaticInterfaceExtras<T>{};

		template<class T>
		struct StaticInterfaceExtrasBase : public StaticInterfaceExtras<T>{
		};

	};



	template<class... T>
	struct static_interfaces{

	};
	// Define a runtime_class_base
	template < class T, T(*pfun_runtime_class_name)(),
	class DefaultInterface,
	class FactoryInterface,
	class StaticInterface,
	class... OtherInterfaces >
	struct runtime_class_base{
		static  T get_runtime_class_name(){
			return pfun_runtime_class_name();
		}
	};

	// Enables implementing runtime class
	template<class Derived, class RC>
	struct implement_runtime_class_base{};

	namespace detail{

		template<class Iface, class T>
		void caster(use<Iface>& r, T& t){
			r = t.template QueryInterface<Iface>();
		}


		// Helpers for using constructors to automatically map interface
		template<class CF, class F>
		struct factory_to_constructor_helper{};

		template<class CF, class R, class... Parms>
		struct factory_to_constructor_helper<CF, R(Parms...)>{

			template<class ImpFactHelper, class... P>
			struct constructor_helper{
				static R call_function(P... p){
					R ret;
					auto t = ImpFactHelper::activate_instance_parms(p...);
					caster(ret, t);
					return ret;

				}
			};

			template<class ImpFactHelper, class MPS, class Interface>
			static void set(ImpFactHelper& , MPS& m, Interface& i){
				auto ptm = m.template get<CF>();
				typedef constructor_helper<ImpFactHelper, Parms...> h_t;
				(i.*ptm). template set_executor<h_t>();
			}
		};

		template<class... CF>
		struct factory_to_constructor{};

		template<>
		struct factory_to_constructor<>{
			template<class ImpFactHelper, class MPS, class Interface>
			static void set(ImpFactHelper& , MPS& , Interface& ){	}
		};

		template<class First>
		struct factory_to_constructor<First>{
			template<class ImpFactHelper, class MPS, class Interface>
			static void set(ImpFactHelper& helper, MPS& m, Interface& i){
				factory_to_constructor_helper<First, typename First::function_signature>::set(helper, m, i);
			}
		};

		template<class First, class... Rest>
		struct factory_to_constructor<First, Rest...>{
			template<class ImpFactHelper, class MPS, class Interface>
			static void set(ImpFactHelper& helper, MPS& m, Interface& i){
				factory_to_constructor_helper<First, typename First::function_signature>::set(helper, m, i);
				factory_to_constructor<Rest...>::set(helper, m, i);
			}


		};

		template<class TypesList>
		struct forward_to_factory_to_constructor{};

		template<class... T>
		struct forward_to_factory_to_constructor<cross_compiler_interface::type_list<T...>>{
			typedef factory_to_constructor<T...> type;
		};


		template<class T>
		struct others_helper{
			template<class U>
			struct Interface : public T::template Interface<U>{};

		};


		// Copied from interface_unknown
		template<class Derived, class FirstInterface, class... Interfaces>
		struct implement_unknown_interfaces_helper : public cross_compiler_interface::implement_interface<FirstInterface::template Interface>, public implement_unknown_interfaces_helper<Derived, Interfaces...>
		{

		};

		// An extra InterfaceUnknown is added by implement_unknown_interfaces to 
		// work around an MSVC bug, filter it out - it is extraneous since all these interfaces
		// inherit from InterfaceUnknown
		template<class Derived, class FirstInterface>
		struct implement_unknown_interfaces_helper<Derived, FirstInterface, InterfaceUnknown> :public cross_compiler_interface::implement_interface<FirstInterface::template Interface>{

		};

		// Copied from interface_unknown
		template<class T>
		struct qi_helper{
			static bool compare(const uuid_base* u){
				typedef typename T::uuid_type uuid_type;
				if (uuid_type::compare(*u)){
					return true;
				}
				else{
					typedef typename T::base_interface_t base_t;
					return qi_helper<base_t>::compare(u);
				}
			}


		};

		template<template<class> class T>
		struct qi_helper < InterfaceUnknown::Interface < cross_compiler_interface::implement_interface<T >> >{
			static bool compare(const uuid_base* u){
				return InterfaceUnknown::uuid_type::compare(*u);
			}

		};
		template<>
		struct qi_helper < cross_compiler_interface::implement_interface<InterfaceUnknown::Interface> >{
			static bool compare(const uuid_base* u){
				return InterfaceUnknown::uuid_type::compare(*u);
			}

		};


		//  Copied from interface_unknown
		template<class Derived, class... Interfaces>
		struct implement_unknown_interfaces{
		private:
			//Adding an extra IUnknown fixes an internal compiler error when compiling with MSVC Milan
			//When there is only 1 interface specified
			detail::implement_unknown_interfaces_helper<Derived, Interfaces..., InterfaceUnknown> i_;
		public:

			template<class ImpInterface>
			cross_compiler_interface::implement_interface<ImpInterface::template Interface>* get_implementation(){
				return &i_;
			}

		private:


			template<class First, class... Rest>
			struct helper{
				template<class T>
				static portable_base* get_unknown(T* t){
					return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
				}

				template<class T>
				static portable_base* qihelper(const uuid_base* u, T* t){
					if (detail::qi_helper < cross_compiler_interface::implement_interface < First::template Interface >> ::compare(u)){
						return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
					}
					else{
						return helper<Rest...>::qihelper(u, t);
					}
				}

				template<class T>
				static void set_mem_functions(T* t){
					auto p = t->template get_implementation<First>();
					p->QueryInterfaceRaw.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::QueryInterfaceRaw>(t);
					p->AddRef.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::AddRef>(t);
					p->Release.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::Release>(t);

					helper<Rest...>::set_mem_functions(t);
				}

			};
			template<class First>
			struct helper<First>{
				template<class T>
				static portable_base* get_unknown(T* t){
					return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
				}

				template<class T>
				static portable_base* qihelper(const uuid_base* u, T* t){
					if (detail::qi_helper < cross_compiler_interface::implement_interface < First::template Interface >> ::compare(u)){
						return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
					}
					else{
						return nullptr;
					}
				}
				template<class T>
				static void set_mem_functions(T* t){
					auto p = t->template get_implementation<First>();
					p->QueryInterfaceRaw.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::QueryInterfaceRaw>(t);
					p->AddRef.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::AddRef>(t);
					p->Release.template set_mem_fn<implement_unknown_interfaces,
						&implement_unknown_interfaces::Release>(t);
				}

			};


			std::atomic<std::size_t> counter_;

		private:
			void ReleaseImplementationDestroy(){
				delete static_cast<Derived*>(this);

			}
			std::uint32_t AddRefImplementation(){
				auto c = counter_.fetch_add(1);
				// First Reference
				if (c == 0){
					cross_compiler_interface::object_counter::get().increment();
				}
				//Truncate to 32bit, but since return is only for debugging thats ok
				return static_cast<std::uint32_t>(c-1);
			}
			std::uint32_t ReleaseImplementation(){

				auto c = counter_.fetch_sub(1);
				// Counter should never be 0;
				assert(c);
				if (c == 1){

					cross_compiler_interface::object_counter::get().decrement();
					static_cast<Derived*>(this)->ReleaseImplementationDestroy();
					return 0;
				}
				//Truncate to 32bit, but since return is only for debugging thats ok
				return static_cast<std::uint32_t>(c-1);
			}



		public:
			portable_base* QueryInterfaceRaw(const uuid_base* u){
				auto ret = helper<Interfaces...>::qihelper(u, &i_);
				//Need to increment reference count of successful query interface
				if (ret){
					AddRef();
				}
				return ret;
			}

			std::uint32_t AddRef(){
				return static_cast<Derived*>(this)->AddRefImplementation();
			}
			std::uint32_t Release(){
				return static_cast<Derived*>(this)->ReleaseImplementation();

			}

			template<class OtherIface>
			use<OtherIface> QueryInterface(){
				auto ret = QueryInterfaceNoThrow<OtherIface>();
				if (!ret){
					throw error_no_interface();
				}
				return ret;

			}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#endif

			template<class OtherIface>
			use<OtherIface> QueryInterfaceNoThrow(){
				if (std::is_same<OtherIface, InterfaceUnknown>::value){
					use<OtherIface> ret(reinterpret_portable_base<OtherIface::template Interface>(helper<Interfaces...>::get_unknown(&i_)), true);
					 return ret;
				}

				typedef typename OtherIface::template Interface<use<OtherIface>>::uuid_type uuid_type;
				portable_base* r = this->QueryInterfaceRaw(&uuid_type::get());

				// AddRef already called by QueryInterfaceRaw
				return use<OtherIface>(reinterpret_portable_base<OtherIface::template Interface>(r), false);

			}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

			portable_base* get_unknown_portable_base(){
				return helper<Interfaces...>::get_unknown(&i_);
			}

			implement_unknown_interfaces() : counter_(0){
				helper<Interfaces...>::set_mem_functions(this);
			}

			~implement_unknown_interfaces(){
			}

			template<class... T>
			static use<InterfaceUnknown> create(T && ... t){
				using namespace std; // Need this for MSVC Milan bug

				try{
					// Refcount is 0 at creation
					std::unique_ptr<Derived> p(new Derived(std::forward<T>(t)...));

					auto ret = p->template QueryInterface<InterfaceUnknown>();

					p.release();

					return ret;
				}
				catch (std::exception&){
					return nullptr;
				}

			}

		};


		inline use<InterfaceUnknown> create_unknown(const cross_compiler_interface::module& m, std::string func){
			typedef portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
			auto f = m.load_module_function<CFun>(func);
			return use<InterfaceUnknown>(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(f()), false);


		}






		template<class Derived, class FactoryInterface, class StaticInterface>
		struct implement_factory_static_helper
			: public implement_unknown_interfaces<Derived, FactoryInterface, StaticInterface>{
				void ReleaseImplementationDestroy(){

				}

		};
		template<class Derived, class FactoryInterface, class... StaticInterfaces>
		struct implement_factory_static_helper<Derived, FactoryInterface, static_interfaces<StaticInterfaces...> >
			: public implement_unknown_interfaces<Derived, FactoryInterface, StaticInterfaces...>{
				void ReleaseImplementationDestroy(){

				}
		};


		template<class StaticInterface>
		struct default_static_interface{
			typedef StaticInterface type;
		};

		template<class First, class... StaticInterfaces>
		struct default_static_interface<static_interfaces<First, StaticInterfaces...>>{
			typedef First type;
		};

		template<class Derived, class... Interfaces>
		struct helper_map_to_static_functions_with_prefix{};
		template<class Derived, class First, class... Rest>
		struct helper_map_to_static_functions_with_prefix<Derived, First, Rest...>{

			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* imp){
				imp->template get_implementation<First>()-> template map_to_static_functions<Derived>();
				helper_map_to_static_functions_with_prefix<Derived, Rest...>::map(imp);
			}

		};
		template<class Derived>
		struct helper_map_to_static_functions_with_prefix<Derived>{
			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* ){
			}


		};
		template<class Derived, class... Interfaces>
		struct helper_map_to_static_functions_no_prefix{};
		template<class Derived, class First, class... Rest>
		struct helper_map_to_static_functions_no_prefix<Derived, First, Rest...>{

			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* imp){
				auto i = imp->template get_implementation<First>();
				i-> template map_to_static_functions_no_prefix<Derived>();
				helper_map_to_static_functions_no_prefix<Derived, Rest...>::map(imp);
			}

		};
		template<class Derived>
		struct helper_map_to_static_functions_no_prefix<Derived>{
			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* ){
			}


		};

		template<class Derived, class StaticInterface>
		struct helper_map_to_static_functions{
			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* imp){
				helper_map_to_static_functions_no_prefix<Derived, StaticInterface>::map(imp);
			}

		};

		template<class Derived, class First, class... Rest>
		struct helper_map_to_static_functions<Derived, static_interfaces<First, Rest...> >{
			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* imp){
				helper_map_to_static_functions_no_prefix<Derived, First>::map(imp);
				helper_map_to_static_functions_with_prefix<Derived, Rest...>::map(imp);
			}

		};
		template<class Derived>
		struct helper_map_to_static_functions<Derived, static_interfaces<> >{
			template<class StaticFunctionImp>
			static void map(StaticFunctionImp* ){}

		};

		template<class... Interfaces>
		struct helper_map_to_member_functions_with_prefix{};

		template<class First, class...Rest>
		struct helper_map_to_member_functions_with_prefix<First, Rest...>{
			template<class Derived>
			static void map(Derived* pthis){
				pthis-> template get_implementation<First>()->map_to_member_functions(pthis);
				helper_map_to_member_functions_with_prefix<Rest...>::map(pthis);
			}
		};	
		template<>
		struct helper_map_to_member_functions_with_prefix<>{
			template<class Derived>
			static void map(Derived* ){}
		};

		template<class... I>
		struct set_runtime_parent_helper{};

		template<class First, class... Rest>
		struct set_runtime_parent_helper<First, Rest...>{

			template<class IRCB,class T,class... TR>
			static void set_imp(IRCB* pthis, const T& pt, const TR&... pr){
				auto imp = pthis->template get_implementation<First>();
				auto parent = pt.template try_as<First>();
				if (parent){
					imp->set_runtime_parent(parent.get_use_interface());

					// Increment the reference count to keep
					parent.get_use_interface().AddRef();
				}
				else{
					set_imp(pthis, pr...);
				}
			}

			template<class IRCB>
			static void set_imp(IRCB* ){}

			template<class IRCB, class... T>
			static void set(IRCB* pthis, const T&... pt){
				set_imp(pthis, pt...);
				set_runtime_parent_helper<Rest...>::set(pthis, pt...);
			}

			template<class IRCB>
			static void cleanup(IRCB* pthis){
				auto imp = pthis->template get_implementation<First>();
				auto parent = imp->get_runtime_parent();
				if (parent){
					imp->set_runtime_parent(nullptr);
					parent.Release();
				}
				set_runtime_parent_helper<Rest...>::cleanup(pthis);

			}



		};

		template<>
		struct set_runtime_parent_helper<>{

			template<class IRCB,class... T>

			static void set(IRCB* , const T&... ){}
			template<class IRCB>
			static void cleanup(IRCB* ){}
		};
	}

	namespace detail{
		// Holds the factory map
		template<class T>
		struct factory_map{
			typedef std::unordered_map<T, portable_base*> map_t;

			static map_t& get(){
				struct uniq{};
				return cross_compiler_interface::detail::safe_static_init<map_t, uniq>::get();
			}
		};

		struct factory_map_lock{
		private:
			static std::atomic_flag& factory_map_lock_(){
				static std::atomic_flag f = ATOMIC_FLAG_INIT;
				return f;
			}
		public:

			factory_map_lock(){
				while (factory_map_lock_().test_and_set()){
				}
			}
			~factory_map_lock(){
				factory_map_lock_().clear();
			}
		};
	}


	struct do_not_map_to_member_functions{};


	// Template to help implement a runtime class
	// You inherit from this class, providing the name of your class in derived
	template<class NameType, NameType(*pfun_runtime_class_name)(), class Derived, class DefaultInterface, class FactoryInterface, class StaticInterface, class... Others>
	struct implement_runtime_class_base<Derived, runtime_class_base<NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...>>
		:public detail::implement_unknown_interfaces<Derived, DefaultInterface, Others...>
	{

		// The runtime class default interface
		typedef runtime_class_base<NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...> runtime_class_t;
		cross_compiler_interface::implement_interface<DefaultInterface::template Interface>* default_interface(){
			return this->template get_implementation<DefaultInterface::template Interface>();

		}

		void map_default_implementation_to_member_functions(){

			this->template get_implementation<DefaultInterface>()->map_to_member_functions_no_prefix(static_cast<Derived*>(this));
			detail::helper_map_to_member_functions_with_prefix<Others...>::map(static_cast<Derived*>(this));

		}

		template<class T,class... TR>
		implement_runtime_class_base(const T& pt,const TR&&... pr):has_parents_(true){
			detail::set_runtime_parent_helper<DefaultInterface, Others...>::set(this, pt, pr...);
		}

		implement_runtime_class_base() : has_parents_(false){
			static_cast<Derived*>(this)->map_default_implementation_to_member_functions();
		}
		implement_runtime_class_base(do_not_map_to_member_functions) : has_parents_(false){

		}

		 ~implement_runtime_class_base(){
			if (has_parents_)
				detail::set_runtime_parent_helper<DefaultInterface, Others...>::cleanup(this);
		}



		struct implement_factory_static_interfaces
			: public detail::implement_factory_static_helper<implement_factory_static_interfaces, FactoryInterface, StaticInterface>{

				typedef runtime_class_base<NameType,pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...> runtime_class_t;

				cross_compiler_interface::implement_interface<FactoryInterface::template Interface>* factory_interface(){
					return this->template get_implementation<FactoryInterface>();
				}

				template<class... T>
				static use<InterfaceUnknown> activate_instance_parms(T... t){
					return Derived::create(t...).template QueryInterface<InterfaceUnknown>();
				}

				implement_factory_static_interfaces(){
					detail::factory_map_lock lock;


					auto memp = cross_compiler_interface::type_information<cross_compiler_interface::implement_interface<FactoryInterface::template Interface>>::get_ptrs_to_members();
					typedef typename detail::forward_to_factory_to_constructor < typename cross_compiler_interface::type_information <
						cross_compiler_interface::implement_interface<FactoryInterface::template Interface >> ::functions>::type f_t;
					f_t::set(*this, memp, *factory_interface());

					detail::helper_map_to_static_functions<Derived, StaticInterface>::map(this);
					auto& m = detail::factory_map<NameType>::get();
					m.insert(std::make_pair(pfun_runtime_class_name(), this->get_unknown_portable_base()));
				}

		};

		static const implement_factory_static_interfaces* cppcomponents_get_fsi(){
			return &cppcomponents_fsi_;
		}


		static use<InterfaceUnknown> get_activation_factory(const NameType& s){
			if (s == runtime_class_t::get_runtime_class_name()){
				return cppcomponents_fsi_.template QueryInterface<InterfaceUnknown>();
			}
			else{
				return nullptr;
			}
		}

		static implement_factory_static_interfaces cppcomponents_fsi_;
	private:
		bool has_parents_;
		// Non copyable
		implement_runtime_class_base(const implement_runtime_class_base&);
		implement_runtime_class_base& operator=(const implement_runtime_class_base&);
		static const implement_factory_static_interfaces* pfsi  ;

	};

	template<class NameType, NameType(*pfun_runtime_class_name)(), class Derived, 
	class DefaultInterface, class FactoryInterface, class StaticInterface, class... Others >
		typename implement_runtime_class_base < Derived,
		runtime_class_base < NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others... >> ::implement_factory_static_interfaces

		implement_runtime_class_base < Derived,
		runtime_class_base < NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others... >> ::cppcomponents_fsi_;



	template<class T, class... Imps>
	struct get_activation_factory_helper{
		template<class... I>
		struct helper{};

		template<class First, class... Rest>
		struct helper<First, Rest...>{
			static use<InterfaceUnknown> get_activation_factory(const T& hs){
				auto r = First::get_activation_factory(hs);
				if (r){
					return r;
				}
				else{
					return helper<Rest...>::get_activation_factory(hs);

				}
			}

		};

		template<class First>
		struct helper<First>{
			static use<InterfaceUnknown> get_activation_factory(const T& hs){
				auto r = First::get_activation_factory(hs);
				if (r){
					return r;
				}
				else{
					return nullptr;
				}
			}

		};
		static use<InterfaceUnknown> get_activation_factory(const T& hs){
			return helper<Imps...>::get_activation_factory(hs);
		}

	};


	template<class T>
	error_code get_activation_factory(const T& activatibleClassId, portable_base** factory){

		try{
			*factory = nullptr;
			detail::factory_map_lock lock;
			const auto& m = detail::factory_map<T>::get();
			auto iter = m.find(activatibleClassId);
			if (iter == m.end()){
				return cross_compiler_interface::error_class_not_available::ec; 
			}
			auto p = iter->second;
			cross_compiler_interface::use_interface<InterfaceUnknown::Interface> i(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(p));
			i.AddRef();
			*factory = p;
			return 0;
		}
		catch (std::exception& e){
			return cross_compiler_interface::general_error_mapper::error_code_from_exception(e);
		}
	}

	namespace detail{
		class runtime_class_name_mapper{

			typedef std::pair<std::string, std::string> p_t;
			std::vector<p_t> v_;
			bool sorted_;



			void sort(){
				std::sort(v_.begin(), v_.end(), [](const p_t& a, const p_t& b){
					return a.first < b.first;

				});
			}

			void sort_if_needed(){
				if (!sorted_){
					sort();
					sorted_ = true;
				}
			}

		public:
			runtime_class_name_mapper() :sorted_{ false }{}

			void add(std::string k, std::string v){
				v_.push_back(std::make_pair(k, v));
			}

			void finalize(){
				sort();

			}
			static std::string get_module_name_from_string(const std::string& s){
				auto iter = std::find(s.begin(), s.end(), '!');
				if (iter == s.end()) return std::string();
				return std::string(s.begin(), iter);
			}

			std::string match_no_module_name(const std::string& s){
				sort_if_needed();
				auto i = std::lower_bound(v_.begin(), v_.end(), s, [](const p_t& a, const std::string& b){
					return a.first < b;

				});
				// Exact match
				if (i != v_.end()){
					if (i->first == s)
						return i->second;
				}
				// Check if already at beginning
				if (i == v_.begin()){
					return std::string{};
				}

				--i;
				if (s.substr(0, i->first.size()) == i->first){
					return i->second;
				}

				return std::string{};

			}

			std::string match(const std::string& s){

				auto ret = match_no_module_name(s);
				if (!ret.empty()){
					return ret;
				}
				else{
					return get_module_name_from_string(s);
				}
			};


		};
	}



	struct DefaultFactoryInterface : public cppcomponents::define_interface < cppcomponents::uuid<0x7175f83c, 0x6803, 0x4472, 0x8d5a, 0x199e478bd8ed>> {


		use<InterfaceUnknown> Create();

		CPPCOMPONENTS_CONSTRUCT(DefaultFactoryInterface, Create)





	};
	struct NoConstructorFactoryInterface : public cppcomponents::define_interface <cppcomponents::uuid<0x70844160, 0x352c, 0x4007, 0x8be2, 0xd69fb415de77>> {


		use<InterfaceUnknown> Create();

		CPPCOMPONENTS_CONSTRUCT_NO_METHODS(NoConstructorFactoryInterface)





	};


	template<class... T>
	struct object_interfaces{};

	template<class T>
	struct factory_interface{};

	namespace detail{


		struct empty_interfaces{
			typedef object_interfaces<InterfaceUnknown> oi;
			typedef factory_interface<DefaultFactoryInterface> fi;
			typedef static_interfaces<> si;
		};

		template<class OI, class FI, class SI>
		struct oi_si_fi{
			typedef OI oi;
			typedef FI fi;
			typedef SI si;
		};
		template<class B, class... T>
		struct to_oi_fi_si{

		};
		template<class B, class... I, class... T>
		struct to_oi_fi_si<B, object_interfaces<I...>, T...>{
			typedef oi_si_fi < object_interfaces<I...>, typename B::fi, typename B::si> initial;
			typedef to_oi_fi_si<initial, T...> updated;

			typedef typename updated::oi oi;
			typedef typename updated::fi fi;
			typedef typename updated::si si;


		};
		template<class B, class I, class... T>
		struct to_oi_fi_si<B, factory_interface<I>, T...>{
			typedef oi_si_fi <typename B::oi, factory_interface<I>, typename B::si> initial;
			typedef to_oi_fi_si<initial, T...> updated;

			typedef typename updated::oi oi;
			typedef typename updated::fi fi;
			typedef typename updated::si si;
		};
		template<class B, class... I, class... T>
		struct to_oi_fi_si<B, static_interfaces<I...>, T...>{
			typedef oi_si_fi <typename B::oi, typename B::fi, static_interfaces<I...> > initial;
			typedef to_oi_fi_si<initial, T...> updated;

			typedef typename updated::oi oi;
			typedef typename updated::fi fi;
			typedef typename updated::si si;
		};
		template<class B>
		struct to_oi_fi_si<B>{
			typedef typename B::oi oi;
			typedef typename B::fi fi;
			typedef typename B::si si;
		};


		template<class T, T(*pfun_runtime_class_name)(), class OI, class FI, class SI>
		struct runtime_class_helper{
			// Must use object_interfaces, factory_interface, static_interfaces to specify runtime_class
		};

		template<class T, T(*pfun_runtime_class_name)(), class DefaultInterface, class... OI, class FI, class... SI>
		struct runtime_class_helper<T, pfun_runtime_class_name, object_interfaces<DefaultInterface, OI...>, factory_interface<FI>, static_interfaces<SI...> >
		{
			typedef runtime_class_base < T, pfun_runtime_class_name, DefaultInterface, FI, static_interfaces<SI...>, OI...> type;
		};



	}


	// Define a runtime_class
	template < std::string(*pfun_runtime_class_name)(),
	class... I  >
	struct runtime_class{
		typedef detail::to_oi_fi_si<detail::empty_interfaces, I...> oifisi;

		typedef detail::runtime_class_helper<std::string, pfun_runtime_class_name, typename oifisi::oi, typename oifisi::fi, typename oifisi::si> helper;


		typedef typename helper::type type;
	};



	template<class Derived, class RC>
	using implement_runtime_class = implement_runtime_class_base<Derived, typename RC::type>;



	struct IStringFactoryCreator : public cppcomponents::define_interface<cppcomponents::uuid<0x33e78ea2, 0xb89f, 0x479a, 0x8f10, 0xfd3b4234b446>>
	{
		void AddMapping(std::string class_name, std::string module_name);
		use<InterfaceUnknown> GetClassFactory(std::string class_name);
		use<InterfaceUnknown> GetClassFactoryFromModule(std::string class_name, std::string module_name);
		void FreeUnusedModules();

		CPPCOMPONENTS_CONSTRUCT(IStringFactoryCreator, AddMapping, GetClassFactory, GetClassFactoryFromModule, FreeUnusedModules)
	};


	inline std::string StringFactoryCreatorId(){ return "cppcomponents.StringFactoryCreator"; }

	template<class Factory>
	struct implement_string_factory_creator : implement_runtime_class<implement_string_factory_creator<Factory>,
		runtime_class<StringFactoryCreatorId, object_interfaces<IStringFactoryCreator>>> 
	{
		rw_lock factories_lock_;
		rw_lock modules_lock_;
		rw_lock mapper_lock_;

		typedef    cross_compiler_interface::error_code(CROSS_CALL_CALLING_CONVENTION* cppcomponents_factory_func)(const char* s,
			cross_compiler_interface::portable_base** p);

		typedef    cross_compiler_interface::error_code(CROSS_CALL_CALLING_CONVENTION* cppcomponents_module_in_use)();
		typedef    cross_compiler_interface::error_code(CROSS_CALL_CALLING_CONVENTION* cppcomponents_module_initialize)(portable_base*);

		typedef std::unordered_map < std::string, use<InterfaceUnknown> > factories_t;
		typedef std::unordered_map< std::string, cross_compiler_interface::module > modules_t;

		factories_t factories_;
		modules_t modules_;

		detail::runtime_class_name_mapper mapper_;
		implement_string_factory_creator(){}

		void AddMapping(std::string class_name, std::string module_name){
			rw_locker lock{ factories_lock_ };
			mapper_.add(class_name, module_name);
		}
		use<InterfaceUnknown> GetClassFactory(std::string class_name){
			factories_t::iterator iter{};
			{
				rw_locker flock{ factories_lock_ };
				iter = factories_.find(class_name);
			}
			if (iter != factories_.end()){
				return iter->second;
			}
			else{
				std::string module_name;
				{
					rw_locker mlock{ mapper_lock_ };

					module_name = mapper_.match(class_name);
				}

				auto ret = GetClassFactoryFromModule(class_name, module_name);

				if (!ret){
					throw error_unable_to_load_library();
				}
				// Cache in factories
				{
					rw_locker flock{ factories_lock_, true };
					factories_[class_name] = ret;
				}

				return ret;
			}
		}
		use<InterfaceUnknown> GetClassFactoryFromModule(std::string class_name, std::string module_name){
			if (module_name.empty()){
				portable_base* p = nullptr;
				auto ec = cppcomponents::get_activation_factory<std::string>(class_name, &p);
				throw_if_error(ec);
				return use<InterfaceUnknown>{cppcomponents::reinterpret_portable_base<InterfaceUnknown>(p), false};
			}
			rw_locker mlock{ modules_lock_ };
			portable_base* p = nullptr;
			auto iter = modules_.find(module_name);
			
			if (iter == modules_.end()){
				mlock.upgrade();
				// double checked locking pattern
				iter = modules_.find(module_name);
				if (iter == modules_.end()){
					cross_compiler_interface::module m{ module_name };
					if (m.valid() == false){
						throw error_unable_to_load_library();
					}
					auto result_pair = modules_.insert(modules_t::value_type{ module_name, std::move(m) });
					if (result_pair.second == false){
						throw error_unable_to_load_library();
					}
					iter = result_pair.first;
				}
				auto finit = iter->second.template load_module_function<cppcomponents_module_initialize>("cppcomponents_module_initialize");
				auto e = finit(Factory::get_factory_portable_base());
				throw_if_error(e);
			}



			auto f = iter->second.template load_module_function< cppcomponents_factory_func>("get_cppcomponents_factory");
			auto e = f(class_name.c_str(), &p);
			throw_if_error(e);
			return use<InterfaceUnknown>{cppcomponents::reinterpret_portable_base<InterfaceUnknown>(p), false};

		}
		void FreeUnusedModules(){
			{
				rw_locker flock{ factories_lock_, true };
				factories_.clear();

			}



			{
				rw_locker mlock{ modules_lock_ };
				for (auto iter = modules_.begin(); iter != modules_.end();){
					auto f = iter->second.template load_module_function< cppcomponents_module_in_use>("cppcomponents_module_in_use");
					auto e = f();
					if (e == 0){
						modules_.erase(iter++);
					}
					else{
						++iter;
					}

				}



			}

		}

		~implement_string_factory_creator(){
			try{
				// Free up unused modules
				FreeUnusedModules();

				// Some more modules may have been freed up by last
				FreeUnusedModules();

				// At this point we just have used moduls
				// Just release them
				for (auto& p : modules_){
					p.second.release();
				}
			}
			catch (...){

			}
		}


	
	
	};

	struct factory{

	private:

		struct factory_init{
			use<IStringFactoryCreator> creator_;

			static use<IStringFactoryCreator> init(portable_base* p){
				if (p){
					return use<IStringFactoryCreator>{ cppcomponents::reinterpret_portable_base<IStringFactoryCreator>(p), true };
				}
				else{
					return implement_string_factory_creator<factory>::create().QueryInterface<IStringFactoryCreator>();
				}
			}

			factory_init(portable_base* p) :creator_{init(p)}{};
		};

		static use<IStringFactoryCreator>&  get_factory_internal(portable_base* p = nullptr){
			struct uniq{};
			return cross_compiler_interface::detail::safe_static_init<
				factory_init, uniq>::get(p).creator_;
		}

	public:


		static void set_factory(use<IStringFactoryCreator> c){

			get_factory_internal(c.get_portable_base());
		}
		static void set_factory(portable_base* p){

			get_factory_internal(p);
		}


		static void add_mapping(std::string class_name, std::string module_name){
			return get_factory_internal().AddMapping(std::move(class_name), std::move(module_name));
		}
		static use<InterfaceUnknown> get_class_factory(std::string class_name){
			return get_factory_internal().GetClassFactory(std::move(class_name));
		}
		static use<InterfaceUnknown> get_class_factory_from_module(std::string class_name, std::string module_name){
			return get_factory_internal().GetClassFactoryFromModule(std::move(class_name), std::move(module_name));
		}
		static void free_unused_modules(){
			return get_factory_internal().FreeUnusedModules();
		}

		static portable_base* get_factory_portable_base(){
			return get_factory_internal().get_portable_base();
		}
	};




	// Usage
	template<class RC, class AFH>
	struct use_runtime_class_base{};


	namespace detail{

		template<class Interface, class Base>
		struct inherit_use_interface_helper : public use<Interface>, Base{};

		template<class... Interfaces>
		struct inherit_use_interfaces_linearly{};

		template<class First, class... Rest>
		struct inherit_use_interfaces_linearly<First, Rest...>
			:public inherit_use_interface_helper<First, inherit_use_interfaces_linearly<Rest...>> { };

		struct unknown_holder{
			use<InterfaceUnknown> unknown_;

			unknown_holder(use<InterfaceUnknown> i) : unknown_(i){}
			unknown_holder(portable_base* p, bool addref)
				: unknown_(use<InterfaceUnknown>(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(p), addref)){}


			use<InterfaceUnknown>& get_unknown(){
				return unknown_;
			}
			const use<InterfaceUnknown>& get_unknown()const{
				return unknown_;
			}

		};


	}

	namespace detail{
		template<class... Interfaces>
		struct use_runtime_class_helper{};

		template<>
		struct use_runtime_class_helper<>{
			template<class T>
			static void set_use_unknown(T* ){
				// do nothing
			}
		};
		template<class First, class... Rest>
		struct use_runtime_class_helper<First, Rest...>{
			template<class T>
			static void set_use_unknown(T* pthis){
				use<First>* pfirst = pthis;
				*pfirst = pthis->template as<First>();

				use_runtime_class_helper<Rest...>::set_use_unknown(pthis);
			}
		};

		template<class Base, class CF, class F>
		struct interface_overload_function_helper{


		};
		template<class Base, class CF, class R, class... Parms>
		struct interface_overload_function_helper<Base, CF, R(Parms...)>:public Base{

			static R overloaded_call(cross_compiler_interface::portable_base* p, Parms... parms){
				CF cf(p);
				return cf(parms...);
			}


		};
		template<class Base, class CF>
		struct interface_overload_function : public interface_overload_function_helper<Base, CF, typename CF::function_signature>{
			using interface_overload_function_helper<Base, CF, typename CF::function_signature>::overloaded_call;
			using Base::overloaded_call;

		};



		template<class Interface,class... CF>
		struct inheritance_overload_helper{};
		template<class Interface, class First, class... CF>
		struct inheritance_overload_helper<Interface,First, CF...>:public interface_overload_function<inheritance_overload_helper<Interface,CF...>, First>{
			typedef First first_cf_type;
			typedef inheritance_overload_helper<Interface,CF...> rest_cf_types;

		};

		struct factory_overloads_no_match{};
		template<class Interface>
		struct inheritance_overload_helper<Interface>{

			// All calls to overloaded call have portable_base as first parameter so this will not resolve
			template<class T0, class... T>
			static use<InterfaceUnknown> overloaded_call_template(cross_compiler_interface::portable_base* p, T0 && t0, T && ... t){
				use<typename Interface::interface_t> i(cppcomponents::reinterpret_portable_base<typename Interface::interface_t>(p), true);
				return i.TemplatedConstructor(std::forward<T0>(t0), std::forward<T>(t)...);
			}
			static use<InterfaceUnknown> overloaded_call_template(cross_compiler_interface::portable_base* p){
				use<typename Interface::interface_t> i(cppcomponents::reinterpret_portable_base<typename Interface::interface_t>(p), true);
				return i.TemplatedConstructor();
			}
			static factory_overloads_no_match overloaded_call(...);
		};

		template<class Any, class Helper>
		struct call_if_overload_no_match{
			template<class... T>
			static use<InterfaceUnknown> overloaded_call(portable_base* p, T && ... t){
				return Helper::overloaded_call(p, std::forward<T>(t)...).template QueryInterface<InterfaceUnknown>();
			}
		};
		template<class Helper>
		struct call_if_overload_no_match < factory_overloads_no_match,Helper>{
			template<class... T>
			static use<InterfaceUnknown> overloaded_call(portable_base* p, T && ... t){
				return Helper::overloaded_call_template(p, std::forward<T>(t)...).template QueryInterface<InterfaceUnknown>();
			}
		};

		template<class Interface,class TypeList>
		struct forward_to_inheritance_overload_helper{};

		template<class Interface,class... T>
		struct forward_to_inheritance_overload_helper<Interface,cross_compiler_interface::type_list<T...>>{
			typedef inheritance_overload_helper<Interface,T...> type;
		};

		template<class Interface, class... Parms>
		static use<InterfaceUnknown> overloaded_creator(Interface i, Parms... p){
			typedef typename cross_compiler_interface::type_information<Interface>::functions functions;
			typedef typename forward_to_inheritance_overload_helper<Interface,functions>::type helper;
			cross_compiler_interface::portable_base* pb = i.get_portable_base();
			return call_if_overload_no_match<decltype(helper::overloaded_call(pb, p...)),helper>::overloaded_call(pb, p...).template QueryInterface<InterfaceUnknown>();
		}

		
		inline portable_base* get_local_activation_factory(const std::string& class_name){
			portable_base* p = nullptr;
			get_activation_factory(class_name, &p);
			if (p){
				return p;
			}
			else{
				return nullptr;
			}

		}

		// Holds factory and the module
		struct default_activation_factory_holder{
		private:
			std::string class_name_;
		public:

			default_activation_factory_holder(const std::string& class_name)
				: class_name_{ class_name }
			{}

			static use<InterfaceUnknown> create(const std::string module_name , const std::string& class_name){
				return factory::get_class_factory_from_module(class_name, module_name);
			}

			use<InterfaceUnknown> get(){ 
				if (!class_name_.empty() && class_name_[0] == '!'){
					portable_base* p = nullptr;
					auto ec = cppcomponents::get_activation_factory<std::string>(class_name_, &p);
					throw_if_error(ec);
					return use<InterfaceUnknown>{cppcomponents::reinterpret_portable_base<InterfaceUnknown>(p), false};
				}
				return factory::get_class_factory(class_name_); 
			};


		};


		template<class Derived, class StaticInterface>
		struct inherit_static_interface_mapper
			: public StaticInterface::template Interface<use<StaticInterface> >
			::template cross_compiler_interface_static_interface_mapper<Derived>
			, public StaticInterface::template StaticInterfaceExtras<Derived>
		{};

		template<class Derived, class... Interfaces>
		struct inherit_static_interface_mapper < Derived, static_interfaces<Interfaces...> >
			:public inherit_static_interface_mapper<Derived, Interfaces>...
		{};

	}

	template<class NameType, NameType(*pfun_runtime_class_name)(), class DefaultInterface, class FactoryInterface,
	class StaticInterface, class... Others, class AFH>
	struct use_runtime_class_base<runtime_class_base<NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...>, AFH>
		:private detail::unknown_holder,
		public detail::inherit_use_interfaces_linearly<DefaultInterface, Others...>
		, public detail::inherit_static_interface_mapper<
		use_runtime_class_base<runtime_class_base<NameType,pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...>,AFH >, StaticInterface >
	{
		typedef runtime_class_base<NameType, pfun_runtime_class_name, DefaultInterface, FactoryInterface, StaticInterface, Others...> runtime_class_t;
		use<DefaultInterface> default_interface(){
			use<DefaultInterface>* p = this;
			return *p;
		}
		template<class Interface>
		use<Interface> as()const{
			return this->get_unknown().template QueryInterface<Interface>();
		}
		template<class Interface>
			use<Interface> try_as()const{
				return this->get_unknown().template QueryInterfaceNoThrow<Interface>();
		}

		static use<FactoryInterface> factory_interface(){
			// Cache the activation factory
			//static detail::activation_factory_holder afh_(runtime_class_t::get_runtime_class_name());
			//return afh_.af_.QueryInterface<FactoryInterface>();
			struct uniq{};
			return cross_compiler_interface::detail::safe_static_init<
				AFH, uniq>::get(runtime_class_t::get_runtime_class_name())
				.get().template QueryInterface<FactoryInterface>();

		}


		template<class P0, class... P>
		static use<FactoryInterface> factory_interface(P0 && p0, P && ... p){
			return AFH::create(std::forward<P0>(p0), std::forward<P>(p)...)
				.template QueryInterface<FactoryInterface>();

		}
		static use<typename detail::default_static_interface<StaticInterface>::type> static_interface(){
			return factory_interface().template QueryInterface<typename detail::default_static_interface<StaticInterface>::type>();
		}

		template<class P0, class... P>
		static use<typename detail::default_static_interface<StaticInterface>::type> static_interface(P0 && p0, P && ... p){
			return AFH::create(std::forward<P0>(p0), std::forward<P>(p)...)
				.template QueryInterface<typename detail::default_static_interface<StaticInterface>::type>();

		}




		use_runtime_class_base()
			: detail::unknown_holder(detail::overloaded_creator(factory_interface()))
		{
			typedef detail::use_runtime_class_helper<DefaultInterface, Others...> h_t;
			h_t::set_use_unknown(this);
		}
		use_runtime_class_base(std::nullptr_t):detail::unknown_holder(nullptr){
		}
		use_runtime_class_base(portable_base* p, bool addref)
			: detail::unknown_holder(p, addref)
		{
			typedef detail::use_runtime_class_helper<DefaultInterface, Others...> h_t;
			h_t::set_use_unknown(this);

		}

		template<class P, class... Parms>
		use_runtime_class_base(P p0, Parms... p)
			: detail::unknown_holder(detail::overloaded_creator(factory_interface(), p0, p...))
		{
			typedef detail::use_runtime_class_helper<DefaultInterface, Others...> h_t;
			h_t::set_use_unknown(this);
		}

		struct dynamic_creator_helper{
			use<FactoryInterface> f_;

			template<class... P>
			use_runtime_class_base operator()(P && ... p){
				return use_runtime_class_base::from_interface(detail::overloaded_creator(f_, std::forward<P>(p)...));
			}

			dynamic_creator_helper(use<FactoryInterface> && f) : f_(f){};

		};
		template<class... P>
		static dynamic_creator_helper dynamic_creator(P && ... p){
			return dynamic_creator_helper{ factory_interface(std::forward<P>(p)...) };
		}



		template<class I>
		static use_runtime_class_base from_interface(I i){
			// Make sure I is a unknown_interface
			// To do this, check that it has uuid typedef

			typedef typename I::uuid_type u_t;
			// To prevent unused type warning
			auto unused = (u_t*)nullptr;
			(void)unused;

			return use_runtime_class_base(i.get_portable_base(), true);
		}

	private:




		};




	template<class RC>
	using use_runtime_class =use_runtime_class_base<typename RC::type, detail::default_activation_factory_holder>;


	// Properties

	template<class D,class T> struct read_only_property{};

	template<class D,class Iface, int Id, class R, class FuncType >
	struct read_only_property<D,cross_compiler_interface::cross_function<Iface, Id, R(), FuncType>>{

		typedef cross_compiler_interface::cross_function<Iface, Id, R(), FuncType> Getter_t;

		template<class I>
		read_only_property(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}


		// Conversion operator
		operator R()const{ return (*this)(); }

		// Also allow function call type interface
		R operator()()const{ return Getter_t(p_) (); }

	private:
		cross_compiler_interface::portable_base* p_;
	};



	template<class D, class Getter,class Setter> struct property{};

	template<class D, class Iface1,class Iface2, int Id1,int Id2, class R1, class P,class R2, class FuncType1, class FuncType2 >
	struct property<D, cross_compiler_interface::cross_function<Iface1, Id1, R1(), FuncType1>,
		cross_compiler_interface::cross_function < Iface2, Id2, R2(P), FuncType2 >> {

			typedef cross_compiler_interface::cross_function<Iface1, Id1, R1(), FuncType1> Getter_t;
			typedef cross_compiler_interface::cross_function < Iface2, Id2, R2(P), FuncType2 > Setter_t;

		template<class I>
		property(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}


		// Conversion operator
		operator R1()const{ return (*this)(); }

		// Also allow function call type interface
		R1 operator()()const{ return Getter_t(p_) (); }

		template<class TO>
		property& operator=(TO && t){
			(*this)(std::forward<TO>(t));
			return *this;
		}

		template<class DO, class TO>
		property& operator=(const read_only_property<DO, TO> & t){
			(*this)(t());
			return *this;
		}
		template<class DO, class T1,class T2>
		property& operator=(const property<DO, T1,T2>& t){
			(*this)(t());
			return *this;
		}
		template<class TO>
		R2 operator()(TO && t){ return Setter_t(p_) (std::forward<TO>(t)); }

	private:
		cross_compiler_interface::portable_base* p_;
	};


	template<class D, class T> struct write_only_property{};

	template<class D, class Iface, int Id, class R, class P, class FuncType >
	struct write_only_property<D, cross_compiler_interface::cross_function<Iface, Id, R(P), FuncType>>{
		typedef  cross_compiler_interface::cross_function<Iface, Id, R(P), FuncType> Setter_t;

		template<class I>
		write_only_property(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}

		template<class TO>
		write_only_property& operator=(TO && t){
			(*this)(std::forward<TO>(t));
			return *this;
		}

		template<class DO, class TO>
		write_only_property& operator=(const read_only_property<DO, TO> & t){
			(*this)(t());
			return *this;
		}
		template<class DO, class T1, class T2>
		write_only_property& operator=(const property<DO, T1,T2> & t){
			(*this)(t());
			return *this;
		}


		template<class TO>
		R operator()(TO && t){ return Setter_t(p_) (std::forward<TO>(t)); }

	private:
		cross_compiler_interface::portable_base* p_;
	};
}


// Specialize hash for use<>
namespace std{

	template<class T>
	struct hash<cppcomponents::use<T>>{
		std::size_t operator()(const cppcomponents::use<T>& i) const{
			auto p = cppcomponents::detail::portable_base_from_unknown(i);
			std::hash<cppcomponents::portable_base*> h;
			return h(p);
		}
	};

}

namespace cppcomponents{

	namespace detail{

		inline cppcomponents::error_code module_intialize(cppcomponents::portable_base* p){
			try{
				factory::set_factory(p);


			}
			catch (std::exception& e){
				return cppcomponents::error_mapper::error_code_from_exception(e);
			}
			return 0;
		}
	

	}

}

#define CPPCOMPONENTS_DEFINE_FACTORY() \
	extern "C"{ \
	CROSS_CALL_EXPORT_FUNCTION cppcomponents::error_code CROSS_CALL_CALLING_CONVENTION  get_cppcomponents_factory(const char* s, \
	cppcomponents::portable_base** p){ \
	return cppcomponents::get_activation_factory(std::string(s), p); \
	}\
	CROSS_CALL_EXPORT_FUNCTION cppcomponents::error_code CROSS_CALL_CALLING_CONVENTION  cppcomponents_module_in_use(){ \
if (cross_compiler_interface::object_counter::get().get_count() == 0) return 0; \
	else return 1; \
	}\
	CROSS_CALL_EXPORT_FUNCTION cppcomponents::error_code CROSS_CALL_CALLING_CONVENTION  cppcomponents_module_initialize(cppcomponents::portable_base* p){ \
	return cppcomponents::detail::module_intialize(p); \
	}\
}



#define CPPCOMPONENTS_INTERFACE_EXTRAS(I) template<class CppComponentInterfaceExtrasT> struct InterfaceExtras : I::template InterfaceExtrasBase<CppComponentInterfaceExtrasT>
#define CPPCOMPONENTS_STATIC_INTERFACE_EXTRAS(I) template<class Class> struct StaticInterfaceExtras : I::template StaticInterfaceExtrasBase<Class>

#define CPPCOMPONENTS_R_PROPERTY(Reader)cppcomponents::read_only_property < CppComponentInterfaceExtrasT, decltype(Interface<CppComponentInterfaceExtrasT>::Reader)>
#define CPPCOMPONENTS_W_PROPERTY(Writer)cppcomponents::write_only_property < CppComponentInterfaceExtrasT, decltype(Interface<CppComponentInterfaceExtrasT>::Writer)>
#define CPPCOMPONENTS_RW_PROPERTY(Reader,Writer)cppcomponents::property < CppComponentInterfaceExtrasT, decltype(Interface<CppComponentInterfaceExtrasT>::Reader), decltype(Interface<CppComponentInterfaceExtrasT>::Writer) >

#define CPPCOMPONENTS_INTERFACE_EXTRAS_CONSTRUCTOR(...) InterfaceExtras():CROSS_COMPILER_INTERFACE_APPLY(CppComponentInterfaceExtrasT, CROSS_COMPILER_INTERFACE_DECLARE_CONSTRUCTOR, __VA_ARGS__) {}

#define CPPCOMPONENTS_REGISTER(T) namespace{auto CROSS_COMPILER_INTERFACE_CAT(cppcomponents_registration_variable , __LINE__) = T::cppcomponents_get_fsi();  }

#endif
