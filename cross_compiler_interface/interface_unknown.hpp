#include "cross_compiler_interface.hpp"
#include <atomic>
#include <cstddef>
namespace cross_compiler_interface{

	// Same structure as windows GUID
	struct uuid_base{
		std::uint32_t Data1;
		std::uint16_t Data2;
		std::uint16_t Data3;
		std::uint8_t Data4[8];
	};

	template<
		std::uint32_t d1,
		std::uint16_t d2,
		std::uint16_t d3,
		std::uint8_t d4,
		std::uint8_t d5,
		std::uint8_t d6,
		std::uint8_t d7,
		std::uint8_t d8,
		std::uint8_t d9,
		std::uint8_t d10,
		std::uint8_t d11>
	struct uuid{
		static bool compare(const uuid_base& u){

			return ( d1 == u.Data1 &&
				d2 == u.Data2 &&
				d3 == u.Data3 &&
				d4 == u.Data4[0] &&
				d5 == u.Data4[1] &&
				d6 == u.Data4[2] &&
				d7 == u.Data4[3] &&
				d8 == u.Data4[4] &&
				d9 == u.Data4[5] &&
				d10 == u.Data4[6] &&
				d11 == u.Data4[7] );
		}

		static uuid_base& get(){
			static uuid_base b = {d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11};
			return b;
		}
#ifdef _WIN32
		static bool compare(const GUID& u){

			return ( d1 == u.Data1 &&
				d2 == u.Data2 &&
				d3 == u.Data3 &&
				d4 == u.Data4[0] &&
				d5 == u.Data4[1] &&
				d6 == u.Data4[2] &&
				d7 == u.Data4[3] &&
				d8 == u.Data4[4] &&
				d9 == u.Data4[5] &&
				d10 == u.Data4[6] &&
				d11 == u.Data4[7] );



		}

		static GUID& get_windows_guid(){
			static GUID g = {d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11};
			return g;
		}

#endif
	};

	namespace detail{
	template<class Iface, int Id>
	struct query_interface_cross_function
		:public custom_cross_function<Iface,Id,portable_base*(uuid_base*),error_code(portable_base*,uuid_base*,portable_base**),
		query_interface_cross_function<Iface,Id>>{
			typedef custom_cross_function<Iface,Id,portable_base*(uuid_base*),error_code(portable_base*,uuid_base*,portable_base**),
				query_interface_cross_function<Iface,Id>> base_t;
			typedef typename base_t::helper helper;

			portable_base* call_vtable_function(uuid_base* u)const{
				portable_base* r = 0;
				auto ret = this->get_vtable_fn()(this->get_portable_base(),u,&r);
				if(ret){
					//this->exception_from_error_code(ret);
					// if QI fails will return 0, don't throw exception
				}
				return r;
			}
			template<class C,class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(cross_compiler_interface::portable_base* v,uuid_base* u,portable_base** r){
				helper h(v);
				try{
					*r = 0;
					C* f = h.template get_mem_fn_object<C>();
					*r = (f->*mf)(u);
					return 0;
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}
			static error_code CROSS_CALL_CALLING_CONVENTION vtable_func(cross_compiler_interface::portable_base* v,uuid_base* u,portable_base** r){
				helper h(v);
				try{
					*r = 0;
					auto& f = h.get_function();
					if(!f){
						return h.forward_to_runtime_parent(u,r);
					}
					*r = f(u);
					return 0;
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}

			template<class F>
			void operator=(F f){
				this->set_function(f);

			}

			template<class T>
			query_interface_cross_function(T t):base_t(t){}

	};

		template<class Iface, int Id>
	struct addref_release_cross_function
		:public custom_cross_function<Iface,Id,std::uint32_t(),std::uint32_t(portable_base*),
		addref_release_cross_function<Iface,Id>>{
			typedef custom_cross_function<Iface,Id,std::uint32_t(),std::uint32_t(portable_base*),
				addref_release_cross_function<Iface,Id>> base_t;
			typedef typename base_t::helper helper;

			std::uint32_t call_vtable_function()const{
				return this->get_vtable_fn()(this->get_portable_base());
			}
			template<class C,class MF, MF mf>
			static std::uint32_t CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(cross_compiler_interface::portable_base* v){
				helper h(v);
				try{
					C* f = h.template get_mem_fn_object<C>();
					return (f->*mf)();
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}
			static std::uint32_t CROSS_CALL_CALLING_CONVENTION vtable_func(cross_compiler_interface::portable_base* v){
				helper h(v);
				try{
					auto& f = h.get_function();
					if(!f){
						return h.forward_to_runtime_parent();
					}

					return f();
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}

			template<class F>
			void operator=(F f){
				this->set_function(f);

			}

			template<class T>
			addref_release_cross_function(T t):base_t(t){}

	};


	}


	//	IUnknown
	typedef uuid<0x00000000,0x0000,0x0000,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46> Unknown_uuid_t;
	template<class b>
	struct InterfaceUnknown:public define_interface<b>{
		detail::query_interface_cross_function<InterfaceUnknown,0> QueryInterfaceRaw;
		detail::addref_release_cross_function<InterfaceUnknown,1> AddRef;
		detail::addref_release_cross_function<InterfaceUnknown,2> Release;


		typedef Unknown_uuid_t uuid;


		InterfaceUnknown():
			QueryInterfaceRaw(this),AddRef(this),Release(this){} 

	};

	template<class b,class uuid_type, template<class> class Base = InterfaceUnknown >
	struct define_interface_unknown:public Base<b>{
		enum{base_sz = sizeof(Base<size_only>)/sizeof(cross_function<Base<size_only>,0,void()>)};

		typedef define_interface_unknown base_t;
		typedef Base<b> base_interface_t;

		typedef uuid_type uuid;

	};

	namespace detail{

	template<class T>
	struct qi_helper{
		static bool compare(uuid_base* u){
			typedef typename T::uuid uuid_t;
			if(uuid_t::compare(*u)){
				return true;
			}
			else{
				typedef typename T::base_interface_t base_t;
				return qi_helper<base_t>::compare(u);
			}
		}


	};

	template<template<class> class T>
	struct qi_helper<InterfaceUnknown<implement_interface<T>>>{
		static bool compare(uuid_base* u){
			typedef typename InterfaceUnknown<implement_interface<T>>::uuid uuid_t;
			return uuid_t::compare(*u);
		}

	};


	template<class T,class... Rest>
	struct qi_imp{
		static portable_base* query(portable_base** begin, portable_base** end, uuid_base* u){
			if(begin==end){
				return nullptr;
			}
			if(qi_helper<T>::compare(u)){
				return *begin;
			}
			else{
				++begin;
				return qi_imp<Rest...>::query(begin,end,u);
			}
		}

	};

	template<class T>
	struct qi_imp<T>{
		static portable_base* query(portable_base** begin, portable_base** end, uuid_base* u){
			if(begin==end){
				return nullptr;
			}
			if(qi_helper<T>::compare(u)){
				return *begin;
			}
			else{
				return nullptr;
			}
		}


	};

	}

	template<class Outer, class... Imps>
	struct implement_iunknown{
		typedef portable_base* pbase_t;
		Outer* pOuter_;
		std::atomic<std::uint32_t> counter_;
		pbase_t bases_[sizeof...(Imps)];
		portable_base* QueryInterfaceRaw(uuid_base* u){
			auto ret =  detail::qi_imp<Imps...>::query(&bases_[0], &bases_[0] + sizeof...(Imps),
				u);
			// Need to increment reference count of successful query interface
			if(ret){
				++counter_;
			}
			return ret;
		}

		std::uint32_t AddRef(){
			counter_++;
			return counter_;
		}
		std::uint32_t Release(){
			counter_--;
			if(counter_==0){
				delete pOuter_;
				return 0;
			}
			return counter_;
		}
		
		template<class T, class... Rest>
		void process_imps(T& t, Rest&... rest){
			bases_[sizeof...(Imps) - sizeof...(Rest) - 1] = t.get_portable_base();
			t.QueryInterfaceRaw.template set_mem_fn<implement_iunknown,
				&implement_iunknown::QueryInterfaceRaw>(this);
			t.AddRef.template set_mem_fn<implement_iunknown,
				&implement_iunknown::AddRef>(this);
			t.Release.template set_mem_fn<implement_iunknown,
				&implement_iunknown::Release>(this);


			process_imps(rest...);
		}

		void process_imps(){
		}


		implement_iunknown(Outer* p,Imps&... imps):pOuter_(p), counter_(1){
			process_imps(imps...);

		}
		


	};

	template<template <class> class Iface>
	struct use_unknown:private portable_base_holder, public Iface<use_unknown<Iface>>{ // Usage

		use_unknown(std::nullptr_t ):portable_base_holder(nullptr){}

		use_unknown(portable_base* v,bool bAddRef):portable_base_holder(v){
			if(*this && bAddRef){
				this->AddRef();
			}
		}

		use_unknown(use_interface<Iface> i):portable_base_holder(i.get_portable_base()){
			// Do not AddRef use_interface since already valid
				
		}

		use_unknown(const use_unknown<Iface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
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
		template< template<class> class OtherIface>
		use_unknown<OtherIface> QueryInterface(){
			if(!*this){
				throw error_pointer();
			}
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());
			if(!r){
				throw error_no_interface();
			}

			// AddRef already called by QueryInterfaceRaw
			return use_unknown<OtherIface>(r,false);

		}

		template< template<class> class OtherIface>
		use_unknown<OtherIface> QueryInterfaceNoThrow(){
			if(!*this){
				return nullptr;
			}
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());

			// AddRef already called by QueryInterfaceRaw
			return use_unknown<OtherIface>(r,false);

		}

		~use_unknown(){
			if(*this){
				this->Release();
			}
		}


		portable_base* get_portable_base()const {
			return this->p_;
		}
		explicit operator bool()const{
			return get_portable_base()!=nullptr;
		}

		enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};

		// Padding etc that makes an interface larger than a multiple of cross_function
		enum{extra = sizeof(Iface<size_only>)%sizeof(cross_function<Iface<size_only>,0,void()>)};

		// Simple checksum that takes advantage of the fact that 1+2+3+4...n = n(n+1)/2
		enum{checksum = sizeof(Iface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Sanity check to make sure the total size is evenly divisible by the size of size_only cross function
		static_assert(extra==0,"Possible error in calculating number of functions");

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");


	};


		template<template<class> class T>
	struct cross_conversion<use_unknown<T>>{
		typedef use_unknown<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			portable_base* p = s.get_portable_base();
			// Increment the reference count
			// Because the destructor of the eventual use_unknown will call Release
			use_interface<T> i(p);
			i.AddRef();

			return p;
		}
		static  original_type to_original_type(converted_type c){
			return use_unknown<T>(c,false);
		}

	};
}
