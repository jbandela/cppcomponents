#pragma once
#ifndef CROSS_COMPILER_INTERFACE_INTERFACE_UNKNOWN_HPP_04_09_2013
#define CROSS_COMPILER_INTERFACE_INTERFACE_UNKNOWN_HPP_04_09_2013

#include "cross_compiler_interface.hpp"
#include <atomic>
#include <utility>
#include <memory>
#include "implementation/safe_static_initialization.hpp"

#include "uuid.hpp"

namespace cross_compiler_interface{



	template<>
	struct cross_conversion<uuid_base*>:trivial_conversion<uuid_base*>{};
	template<>
	struct cross_conversion<const uuid_base*>:trivial_conversion<const uuid_base*>{};



    


	namespace detail{
		template<class Iface, int Id>
		struct query_interface_cross_function
			:public custom_cross_function<Iface,Id,portable_base*(const uuid_base*),error_code(portable_base*, const uuid_base*,portable_base**),
			query_interface_cross_function<Iface,Id>>{


				portable_base* call_vtable_function(const uuid_base* u)const{
					portable_base* r = 0;
					auto ret = this->get_vtable_fn()(this->get_portable_base(),u,&r);
					if(ret < 0){
						//this->exception_from_error_code(ret);
						// if QI fails will return 0, don't throw exception
					}
					return r;
				}
				template<class F>
				static error_code vtable_function(F f,cross_compiler_interface::portable_base* ,const uuid_base* u,portable_base** r){
					*r = f(u);
					if (*r){
						return 0;
					}
					else{
						return cross_compiler_interface::error_no_interface::ec;
					}
				}

				template<class F>
				void operator=(F f){
					this->set_function(f);

				}

				template<class T>
				query_interface_cross_function(T t):query_interface_cross_function::base_t(t){}

		};

		template<class Iface, int Id>
		struct addref_release_cross_function
			:public custom_cross_function<Iface,Id,std::uint32_t(),std::uint32_t(portable_base*),
			addref_release_cross_function<Iface,Id>>{


				std::uint32_t call_vtable_function()const{
					return this->get_vtable_fn()(this->get_portable_base());
				}

				template<class F>
				static std::uint32_t vtable_function(F f,cross_compiler_interface::portable_base* ){
					try{
						return f();
					} catch(std::exception& ){
						return 0;
					}
				}

				template<class F>
				void operator=(F f){
					this->set_function(f);

				}

				template<class T>
				addref_release_cross_function(T t):addref_release_cross_function::base_t(t){}

		};


	}


	//	IUnknown
	typedef uuid<0x00000000,0x0000,0x0000,0xc000,0x000000000046> Unknown_uuid_t;
	template<class b>
	struct InterfaceUnknown:public define_interface<b>{
		detail::query_interface_cross_function<InterfaceUnknown,0> QueryInterfaceRaw;
		detail::addref_release_cross_function<InterfaceUnknown,1> AddRef;
		detail::addref_release_cross_function<InterfaceUnknown,2> Release;


		typedef Unknown_uuid_t uuid_type;


		InterfaceUnknown():
			QueryInterfaceRaw(this),AddRef(this),Release(this){} 

	};

	template<class b,class uuid_type_parameter, template<class> class Base = InterfaceUnknown >
	struct define_unknown_interface:public Base<b>{
		enum{base_sz = sizeof(Base<size_only>)/sizeof(cross_function<Base<size_only>,0,void()>)};

		typedef define_unknown_interface base_t;
		typedef Base<b> base_interface_t;

		typedef uuid_type_parameter uuid_type;

	};

	namespace detail{

		template<class T, class TUUID = typename T::uuid_type>
		struct qi_helper{
			static bool compare(const uuid_base* u){
				typedef typename T::uuid_type uuid_type;
				if(uuid_type::compare(*u)){
					return true;
				}
				else{
					typedef typename T::base_interface_t base_t;
					return qi_helper<base_t>::compare(u);
				}
			}


		};

		template<class T>
		struct qi_helper<T, Unknown_uuid_t>{
			static bool compare(const uuid_base* u){
				typedef Unknown_uuid_t uuid_type;
				return uuid_type::compare(*u);
			}

		};


	}

	template<template <class> class Iface>
	struct use_unknown:private portable_base_holder, public Iface<use_unknown<Iface>>{ // Usage

		use_unknown(std::nullptr_t p = nullptr) :portable_base_holder(nullptr){ (void)p; }

		use_unknown(detail::reinterpret_portable_base_t<Iface> r,bool bAddRef):portable_base_holder(r.get()){
			if(*this && bAddRef){
				this->AddRef();
			}
		}

		use_unknown(use_interface<Iface> u,bool bAddRef):portable_base_holder(u.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use_unknown(implement_interface<Iface>& i,bool bAddRef):portable_base_holder(i.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use_unknown(const use_unknown<Iface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
		}

		// Move constructor
		use_unknown(use_unknown<Iface>&& other):portable_base_holder(other.get_portable_base()){
			other.reset_portable_base();
		}

		use_unknown& operator=(const use_unknown<Iface>& other){
			// Note - order will deal with self assignment as we increment the refcount before decrementing
			if(other){
				other.AddRef();
			}
			if(*this){
				this->Release();
			}
			this->p_ = other.get_portable_base();
			static_cast<Iface<use_unknown<Iface>>&>(*this) =  other;
			return *this;
		}
		// Move constructor
		use_unknown& operator=(use_unknown<Iface>&& other){
			// can't move to ourself
			assert(this != &other);
			if(*this){
				this->Release();
			}

			this->p_ = other.get_portable_base();
			static_cast<Iface<use_unknown<Iface>>&>(*this) =  other;

			other.reset_portable_base();
			return *this;
		}
		template< template<class> class OtherIface>
		use_unknown<OtherIface> QueryInterface(){
			if(!*this){
				throw error_pointer();
			}
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid_type uuid_type;
			portable_base* r = this->QueryInterfaceRaw(&uuid_type::get());
			if(!r){
				throw error_no_interface();
			}

			// AddRef already called by QueryInterfaceRaw
			return use_unknown<OtherIface>(reinterpret_portable_base<OtherIface>(r),false);

		}

		template< template<class> class OtherIface>
		use_unknown<OtherIface> QueryInterfaceNoThrow(){
			if(!*this){
				return nullptr;
			}
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid_type uuid_type;
			portable_base* r = this->QueryInterfaceRaw(&uuid_type::get());

			// AddRef already called by QueryInterfaceRaw
			return use_unknown<OtherIface>(reinterpret_portable_base<OtherIface>(r),false);

		}

		~use_unknown(){
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

		use_interface<Iface> get_use_interface(){
			return use_interface<Iface>(unsafe_portable_base_holder(get_portable_base()));
		}

		void reset_portable_base(){
			// This line prevents a release
			this->p_ = nullptr;


			use_unknown empty;
			*this = empty;
		}
		enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};

	private:


		// Simple checksum that takes advantage of the fact that 1+2+3+4...n = n(n+1)/2
		enum{checksum = sizeof(Iface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

        // Hide AddRef and Release
        // We make this const because lifetime management would be 
        // analogous to delete in that you can delete a const
        uint32_t AddRef()const{
           return Iface<use_unknown<Iface>>::AddRef();
        }

        uint32_t Release()const{
           return Iface<use_unknown<Iface>>::Release();
        }


	};


	template<template<class> class T>
	struct cross_conversion<use_unknown<T>>{
		typedef use_unknown<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			return s.get_portable_base();
		}
		static  original_type to_original_type(converted_type c){
			return use_unknown<T>(reinterpret_portable_base<T>(c),true);
		}

	};

	namespace detail{ 
		template<class Derived, template<class> class FirstInterface, template<class> class... Interfaces>
		struct implement_unknown_interfaces_helper:public implement_interface<FirstInterface>,public implement_unknown_interfaces_helper<Derived,Interfaces...>
		{

		};

		// An extra InterfaceUnknown is added by implement_unknown_interfaces to 
		// work around an MSVC bug, filter it out - it is extraneous since all these interfaces
		// inherit from InterfaceUnknown
		template<class Derived,  template<class> class FirstInterface>
		struct implement_unknown_interfaces_helper<Derived,FirstInterface,InterfaceUnknown> :public implement_interface<FirstInterface>{

		};


	}

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
		template<class Imp,template<class> class First, template<class> class... Rest>
		struct implement_unknown_interfaces_helper{
			template<class T>
			static portable_base* qihelper(const uuid_base* u,T* t){
				if(detail::qi_helper<implement_interface<First>>::compare(u)){
					return static_cast<implement_interface<First>*>(t)->get_portable_base();
				}
				else{
					return implement_unknown_interfaces_helper<Imp, Rest...>::qihelper(u, t);
				}
			}

			template<class T>
			static void set_mem_functions(T* t){
				auto p = t->template get_implementation<First>();
				p->QueryInterfaceRaw.template set_mem_fn<Imp,
					&Imp::QueryInterfaceRaw>(t);
				p->AddRef.template set_mem_fn<Imp,
					&Imp::AddRef>(t);
				p->Release.template set_mem_fn<Imp,
					&Imp::Release>(t);

				implement_unknown_interfaces_helper<Imp, Rest...>::set_mem_functions(t);
			}

		};
		template<class Imp, template<class> class First>
		struct implement_unknown_interfaces_helper<Imp, First>{
			template<class T>
			static portable_base* qihelper(const uuid_base* u,T* t){
				if(detail::qi_helper<implement_interface<First>>::compare(u)){
					return static_cast<implement_interface<First>*>(t)->get_portable_base();
				}
				else{
					return nullptr;
				}
			}
			template<class T>
			static void set_mem_functions(T* t){
				auto p = t->template get_implementation<First>();
				p->QueryInterfaceRaw.template set_mem_fn<Imp,
					&Imp::QueryInterfaceRaw>(t);
				p->AddRef.template set_mem_fn<Imp,
					&Imp::AddRef>(t);
				p->Release.template set_mem_fn<Imp,
					&Imp::Release>(t);
			}

		};
	template<class Derived, template<class> class... Interfaces>
	struct implement_unknown_interfaces{
	private:
		// Adding an extra IUnknown fixes an internal compiler error when compiling with MSVC Milan
		// When there is only 1 interface specified
		detail::implement_unknown_interfaces_helper<Derived,Interfaces...,InterfaceUnknown> i_;
	public:

		template<template<class>class Interface>
		implement_interface<Interface>* get_implementation(){
			return &i_;
		}

	private:





		std::atomic<std::size_t> counter_;

	public:
		portable_base* QueryInterfaceRaw(const uuid_base* u){
			auto ret = implement_unknown_interfaces_helper<implement_unknown_interfaces, Interfaces...>::qihelper(u, &i_);
			// Need to increment reference count of successful query interface
			if(ret){
				++counter_;
			}
			return ret;
		}

		std::uint32_t AddRef(){
			counter_.fetch_add(1);

			// Truncate to 32bit, but since return is only for debugging thats ok
			return static_cast<std::uint32_t>(counter_);
		}
		std::uint32_t Release(){
			if(counter_.fetch_sub(1)==1){
				delete static_cast<Derived*>(this);
				return 0;
			}
			// Truncate to 32bit, but since return is only for debugging thats ok
			return static_cast<std::uint32_t>(counter_);
		}




		implement_unknown_interfaces():counter_(1){
			implement_unknown_interfaces_helper<implement_unknown_interfaces, Interfaces...>::set_mem_functions(this);
			object_counter::get().increment();
		}

		~implement_unknown_interfaces(){
			object_counter::get().decrement();
		}

		template<class... T>
		static use_unknown<InterfaceUnknown> create(T&&... t){
            using namespace std; // Need this for MSVC Milan bug

            try{
                std::unique_ptr<Derived> p(new Derived(std::forward<T>(t)...));

                use_unknown<InterfaceUnknown>piu(reinterpret_portable_base<InterfaceUnknown>(p->QueryInterfaceRaw(&Unknown_uuid_t::get())),false);

                p->Release();

                p.release();

                return piu;
            }
            catch(std::exception&){
                return nullptr;
            }

        }

	};


	inline use_unknown<InterfaceUnknown> create_unknown(const module& m,std::string func){
		typedef portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
		auto f = m.load_module_function<CFun>(func);
		return use_unknown<InterfaceUnknown>(reinterpret_portable_base<InterfaceUnknown>(f()),false);


	}
}
#endif