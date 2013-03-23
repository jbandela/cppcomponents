#include "cross_compiler_interface.hpp"
#include <atomic>
#include <utility>
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


				portable_base* call_vtable_function(uuid_base* u)const{
					portable_base* r = 0;
					auto ret = this->get_vtable_fn()(this->get_portable_base(),u,&r);
					if(ret){
						//this->exception_from_error_code(ret);
						// if QI fails will return 0, don't throw exception
					}
					return r;
				}
				template<class F>
				static error_code vtable_function(F f,cross_compiler_interface::portable_base* p,uuid_base* u,portable_base** r){
					*r = f(u);
					return 0;
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
				static std::uint32_t vtable_function(F f,cross_compiler_interface::portable_base* v){
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

		use_unknown(std::nullptr_t p = nullptr ):portable_base_holder(nullptr){}

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
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());
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
			typedef typename OtherIface<use_unknown<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());

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

	private:

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
			// Increment the reference count
			// Because the destructor of the eventual use_unknown will call Release

			return s.get_portable_base_addref();
		}
		static  original_type to_original_type(converted_type c){
			return use_unknown<T>(reinterpret_portable_base<T>(c),false);
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


		template<template<class> class First, template<class> class... Rest>
		struct helper{
			template<class T>
			static portable_base* qihelper(uuid_base* u,T* t){
				if(detail::qi_helper<implement_interface<First>>::compare(u)){
					return static_cast<implement_interface<First>*>(t)->get_portable_base();
				}
				else{
					return helper<Rest...>::qihelper(u,t);
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
		template<template<class> class First>
		struct helper<First>{
			template<class T>
			static portable_base* qihelper(uuid_base* u,T* t){
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
				p->QueryInterfaceRaw.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::QueryInterfaceRaw>(t);
				p->AddRef.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::AddRef>(t);
				p->Release.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::Release>(t);
			}

		};


		std::atomic<std::uint32_t> counter_;

	public:
		portable_base* QueryInterfaceRaw(uuid_base* u){
			auto ret =  helper<Interfaces...>::qihelper(u,&i_);
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
				delete static_cast<Derived*>(this);
				return 0;
			}
			return counter_;
		}




		implement_unknown_interfaces():counter_(1){
			helper<Interfaces...>::set_mem_functions(this);
		}


		template<class... T>
		static use_unknown<InterfaceUnknown> create(T&&... t){
			Derived* p = new Derived(std::forward<T>(t)...);

			use_unknown<InterfaceUnknown>piu(reinterpret_portable_base<InterfaceUnknown>(p->QueryInterfaceRaw(&Unknown_uuid_t::get())),false);

			p->Release();

			return piu;
		}

	};


	use_unknown<InterfaceUnknown> create_unknown(const module& m,std::string func){
		typedef portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
		auto f = m.load_module_function<CFun>(func);
		return use_unknown<InterfaceUnknown>(reinterpret_portable_base<InterfaceUnknown>(f()),false);


	}
}
