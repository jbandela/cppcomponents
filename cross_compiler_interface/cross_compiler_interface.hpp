//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef cross_compiler_interface_
#define cross_compiler_interface_

#ifdef _MSC_VER
// Disable some MSVC warnings
#pragma warning(push)
#pragma warning(disable:4996)
#pragma  warning(disable: 4099)
#pragma  warning(disable: 4351)
#pragma  warning(disable:4503 )
#endif

#include "implementation/cross_compiler_error_handling.hpp"

// Include Platform Specific
#ifdef _WIN32
#include "platform/Windows/platform_specific.hpp"
#endif // _WIN32
#ifdef __linux__
#include "platform/Linux/platform_specific.hpp"
#endif // __linux__

#include <functional>
#include <assert.h>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>




namespace cross_compiler_interface{

	// Template for converting to/from regular types to cross-compiler compatible types 
	template<class T>
	struct cross_conversion{		
	};

	// Template for converting return types to/from regular types to cross-compiler compatible types 
	// by default use cross_conversion
	template<class T>
	struct cross_conversion_return;

	namespace detail{
		// Calling convention defined in platform specific header
		typedef  void(CROSS_CALL_CALLING_CONVENTION *ptr_fun_void_t)();
	}
    struct  portable_base{
       detail::ptr_fun_void_t* vfptr;
    };

	// Make sure no padding
	static_assert(sizeof(portable_base)==sizeof(detail::ptr_fun_void_t*),"Padding in portable_base");



	namespace detail{

		// Helper functions to cast a vtable function to the correct type and call it
		template<class R, class... Parms>
		R call(const ptr_fun_void_t pFun,Parms... p){
			typedef R( CROSS_CALL_CALLING_CONVENTION *fun_t)(Parms...);
			auto f = reinterpret_cast<fun_t>(pFun);
			return f(p...);
		}

		template<class T>
		T& dummy_conversion(T& t){
			return t;
		}

	}



    // base class for vtable_n
    struct vtable_n_base:public portable_base{
        void** pdata;
        portable_base* runtime_parent_;
        vtable_n_base(void** p):pdata(p),runtime_parent_(0){}
        template<int n,class T>
        T* get_data()const{
            return static_cast<T*>(pdata[n]);
        }

        void set_data(int n,void* d){
            pdata[n] = d;
        }

        template<class R, class... Parms>
        void update(int n,R(CROSS_CALL_CALLING_CONVENTION *pfun)(Parms...)){
            vfptr[n] = reinterpret_cast<detail::ptr_fun_void_t>(pfun);
        }

        template<class R, class... Parms>
        void add(int n,R(CROSS_CALL_CALLING_CONVENTION *pfun)(Parms...)){
            // If you have an assertion here, you have a duplicated number in you interface
            assert(vfptr[n] == nullptr);
            update(n,pfun);
        }
    };

    // Our "vtable" definition
    template<int N>
    struct vtable_n:public vtable_n_base 
    {
    protected:
        detail::ptr_fun_void_t table_n[N];
        void* data[N];
        enum {sz = N};
        vtable_n():vtable_n_base(data),table_n(),data(){
            vfptr = &table_n[0];
        }

    public:
        portable_base* get_portable_base(){return this;}
        const portable_base* get_portable_base()const{return this;}

    };

	namespace detail{
		template<int N,class F>
		F& get_function(const portable_base* v){
			const vtable_n_base* vt = static_cast<const vtable_n_base*>(v);
			return *vt->template get_data<N,F>();
		}

		template<int N,class T>
		T* get_data(const portable_base* v){
			const vtable_n_base* vt = static_cast<const vtable_n_base*>(v);
			return vt->template get_data<N,T>();
		}

		template<bool bImp,template<class> class Iface, int N,class F,class FuncType>
		struct cross_function_implementation{};

		struct conversion_helper{ // Used to Help MSVC++ avoid Internal Compiler Error
			template<class Parm>
			static typename cross_conversion<Parm>::converted_type to_converted(const Parm& p){
				typedef cross_conversion<Parm> cc;
				return cc::to_converted_type(p);
			}
			template<class Parm>
			static typename cross_conversion<Parm>::original_type to_original(typename cross_conversion<Parm>::converted_type p){
				typedef cross_conversion<Parm> cc;
				return cc::to_original_type(p);
			}

		};

        template<bool b, class T>
        struct reference_helper{};

        template<class T>
        struct reference_helper<true,T>{
            typedef T type;
        };   
        template<class T>
        struct reference_helper<false,T>{
            typedef const T& type;
        };

        template<class T>
        struct arg{
            typedef typename reference_helper<std::is_fundamental<T>::value,T>::type type;
        };


		template<template<class> class Iface, int N>
		struct call_adaptor{

			template<class R,class... Parms>
			struct vtable_caller{
				static R call_vtable_func(const detail::ptr_fun_void_t pFun,const portable_base* v,typename arg<Parms>::type... p){
					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details
					typedef cross_conversion_return<R> ccr;
					typedef typename ccr::converted_type cret_t;
					typename ccr::return_type r;
					cret_t cret;
					ccr::initialize_return(r,cret);
					auto ret =  detail::call<error_code,const portable_base*, typename cross_conversion<Parms>::converted_type..., cret_t*>(pFun,
						v,conversion_helper::to_converted<Parms>(p)...,&cret);
					if(ret < 0){
						error_mapper<Iface>::mapper::exception_from_error_code(ret);
					}
					ccr::finalize_return(r,cret);
					return r;
				}

			};

			template<class... Parms>
			struct vtable_caller<void,Parms...>{

				static void call_vtable_func(const detail::ptr_fun_void_t pFun,const portable_base* v,typename arg<Parms>::type... p){

					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details
					auto ret =  detail::call<error_code,const portable_base*,typename cross_conversion<Parms>::converted_type...>(pFun,
						v,conversion_helper::to_converted<Parms>(p)...);
					if(ret < 0){
						error_mapper<Iface>::mapper::exception_from_error_code(ret);
					}
					return;
				}

			};

            template<class... CParms>
            struct move_ret_to_beginning{

 
                // Just used to calculate raw_function_signature
                static error_code CROSS_CALL_CALLING_CONVENTION func(const portable_base* v, CParms... cp);


            };

            template<class P>
            struct ctype{
                typedef typename P::converted_type type;
            };

            template<class VTE,class CR, class... P>
            struct move_ret_to_beginning_helper:public move_ret_to_beginning<typename ctype<P>::type...,CR>
            {};


			template<class R,class... Parms>
			struct vtable_entry:public move_ret_to_beginning_helper<vtable_entry<R,Parms...>,typename cross_conversion_return<R>::converted_type*, cross_conversion<Parms>...>{

			};

			template<class... Parms>
			struct vtable_entry<void,Parms...>{


				static error_code CROSS_CALL_CALLING_CONVENTION func(const portable_base* v, typename cross_conversion<Parms>::converted_type... p);
            };

            // This handles all vtable entries
			template<int d,class ... Parms>
			struct vtable_entry_fast{

				template<class C, class MF, MF mf, class R>
				static error_code CROSS_CALL_CALLING_CONVENTION func(const portable_base* v,typename cross_conversion<Parms>::converted_type... p, typename cross_conversion_return<R>::converted_type* r){
					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details

					typedef cross_conversion_return<R> ccr;

					try{
						C* f = detail::get_data<N,C>(v);
						ccr::do_return((f->*mf)(conversion_helper::to_original<Parms>(p)...),*r);
						return 0;
					} catch(std::exception& e){
						return error_mapper<Iface>::mapper::error_code_from_exception(e);
					}
				}
			};
            // MSVC Milan can't handle Parms... when it is empty
            // So need to specialize for case where no parms'
            // However, while MSVC with compile, GCC (rightly) will not allow explicit specialization
            // But only partial specialization. Thus we have a dummy parameter to keep it partial
			template<int d>
			struct vtable_entry_fast<d>{

				template<class C, class MF, MF mf, class R>
				static error_code CROSS_CALL_CALLING_CONVENTION func(const portable_base* v, typename cross_conversion_return<R>::converted_type* r){
					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details

					typedef cross_conversion_return<R> ccr;

					try{
						C* f = detail::get_data<N,C>(v);
						ccr::do_return((f->*mf)(),*r);
						return 0;
					} catch(std::exception& e){
						return error_mapper<Iface>::mapper::error_code_from_exception(e);
					}
				}
			};		
            
            template<class ... Parms>
			struct vtable_entry_fast_void{

				template<class C, class MF, MF mf, class R>
				static error_code CROSS_CALL_CALLING_CONVENTION func(const portable_base* v, typename cross_conversion<Parms>::converted_type... p){
					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details


					try{
						C* f = detail::get_data<N,C>(v);
						(f->*mf)(conversion_helper::to_original<Parms>(p)...);
						return 0;
					} catch(std::exception& e){
						return error_mapper<Iface>::mapper::error_code_from_exception(e);
					}
				}
			};

		};


		template<bool bImp, template<class> class Iface, int N,class R, class... Parms>
		struct cross_function_implementation_base{
			portable_base* p_;
			R operator()(const Parms&... p)const{

				if(p_){

					using namespace std; // Workaround for MSVC bug http://connect.microsoft.com/VisualStudio/feedback/details/772001/codename-milan-c-11-compilation-issue#details
					// See also http://connect.microsoft.com/VisualStudio/feedback/details/769988/codename-milan-total-mess-up-with-variadic-templates-and-namespaces
					typedef typename call_adaptor<Iface,N>::template vtable_caller<R,Parms...> adapter;
					return adapter::call_vtable_func(p_->vfptr[N],p_,p...);

				}
				else{
					throw error_pointer();
				}

			}
			cross_function_implementation_base(portable_base* v):p_(v){}
		};
                // Help us to get the raw function signature

        template<class F>
        struct typedef_function_signature_raw{
            typedef F function_signature_raw;
        };      
        template<class F>
        struct typedef_function_signature{
            typedef F function_signature;
        };

        template<class F>
        struct cross_function_signature_raw_helper{
//            static_assert(false,"should never be instantiated");
        };


        template<class R, class... Parms>
        struct cross_function_signature_raw_helper<R(CROSS_CALL_CALLING_CONVENTION *)(Parms...)>
            :public typedef_function_signature_raw<R(Parms...)>
        {
            typedef typename typedef_function_signature_raw<R(Parms...)>::function_signature_raw
                function_signature_raw;
        };

		template<template<class> class Iface, int N,class R,class FuncType, class... Parms>
		struct cross_function_implementation<true, Iface,N,R(Parms...),FuncType>
			:public cross_function_implementation_base<true,Iface,N,R,Parms...>,
			public call_adaptor<Iface,N>::template vtable_entry<R,Parms...>,private FuncType
		{ //Implementation
			typedef R return_type;
			typedef std::function<R(Parms...)> fun_t;


			cross_function_implementation(portable_base* p):cross_function_implementation_base<true,Iface,N,R,Parms...>(p){}

			template<class F>
			static void set_function(cross_function_implementation& cfi,F f){
				cfi.func_() = f;
			}
			typedef R(*func_ptr)(Parms...);

			template < func_ptr fn >
			struct fn_helper{
				static R call_function(Parms... p){
					return fn(p...);
				}
			};

			template<func_ptr f>
			R call_static_function(Parms... p){
				return fn_helper<f>::call_function(p...);
			}

			template<class Executor>
			R call_executor(Parms... p){
				return Executor::call_function(p...);
			}


            R call_stored_function(Parms... p){
                if(!func_()){
                    auto v = this->p_;
                    const vtable_n_base* vt = static_cast<const vtable_n_base*>(v);
                    if(vt->runtime_parent_){
                        typedef typename call_adaptor<Iface,N>::template vtable_caller<R,Parms...> adapter;
                        return adapter::call_vtable_func(vt->runtime_parent_->vfptr[N],vt->runtime_parent_,p...);
                    }
                    else{
                        throw error_not_implemented();
                    }
                }
                return func_()(p...);
            }
		private:
			FuncType& func_(){
				return *this;
			}
		};




		template<template<class> class Iface, int N,class R, class FuncType,class... Parms>
		struct cross_function_implementation<false, Iface,N,R(Parms...), FuncType>
			:public cross_function_implementation_base<false,Iface,N,R,Parms...>
		{ //Usage
			typedef R return_type;
			typedef std::function<R(Parms...)> fun_t;

			cross_function_implementation(portable_base* p):
				cross_function_implementation_base<false,Iface,N,R,Parms...>(p){}

		};



	}
	template<template<class> class Iface>
	struct implement_interface;


	template<class Iface, int Id,class F, class FuncType = std::function<F> >
	struct cross_function{};


	struct size_only{};
	struct checksum_only{};

	// size only
	template<template<class> class Iface,int Id,class F, class FuncType>
	struct cross_function<Iface<size_only>,Id,F,FuncType>{char a[1024];
	template<class T>
	cross_function(T t){}


	};
	// checksum only
	template<template<class> class Iface,int Id,class F,class FuncType>
	struct cross_function<Iface<checksum_only>,Id,F,FuncType>{char a[1024*(Id+1+Iface<checksum_only>::base_sz)*(Id+1+Iface<checksum_only>::base_sz)];
	template<class T>
	cross_function(T t){}

	};


	template< class User,template<class> class Iface,int Id,class F,class FuncType>
	struct cross_function<Iface<User>,Id,F, FuncType>                 :public detail::cross_function_implementation<false,Iface,Id + Iface<User>::base_sz,F,FuncType>,
        public detail::cross_function_signature_raw_helper<decltype(&detail::cross_function_implementation<true,Iface,Id + Iface<User>::base_sz,F,FuncType>::func)>{
		enum{N = Id + Iface<User>::base_sz};
		cross_function(Iface<User>* pi):detail::cross_function_implementation<false,Iface,N,F,FuncType>(static_cast<User*>(pi)->get_portable_base()){
			//static_assert(static_cast<int>(N) < User::num_functions,"Error in calculating size of vtable");

		}
        cross_function(portable_base* p):detail::cross_function_implementation<false,Iface,N,F,FuncType>(p){
			//static_assert(static_cast<int>(N) < User::num_functions,"Error in calculating size of vtable");

		}
        typedef F function_signature;
	};	


	namespace detail{

		// MSVC Milan has trouble with variadic templates
		// and mem_fn. We use this template to help us with mem_fn

		template<class F>
		struct mem_fn_helper{};

		template<class R,class... Parms>
		struct mem_fn_helper<R(Parms...)>
		{
			template<class C,template<class>class Iface, int N>
			struct inner{

				typedef R (C::*MFT)(Parms...);

				typedef R ret_t;
				typedef typename call_adaptor<Iface,N>:: template vtable_entry_fast<0,Parms...> vte_t;

			};
		};


		template<class... Parms>
		struct mem_fn_helper<void(Parms...)>
		{
			template<class C,template<class>class Iface, int N>
			struct inner{

				typedef void (C::*MFT)(Parms...);

				typedef void ret_t;
				typedef typename call_adaptor<Iface,N>:: template vtable_entry_fast_void<Parms...> vte_t;

			};
		};

		template<class F>
		struct fn_helper{};
		template<class R, class... P>
		struct fn_helper < R(P...)>{
			typedef R(*type)(P...);
		};
	}


	template<template<class> class Iface,template<class> class T, int Id,class F,class FuncType>
	struct cross_function<Iface<implement_interface<T>>,Id,F,FuncType>:public detail::cross_function_implementation<true,Iface,Id + Iface<implement_interface<T>>::base_sz,F,FuncType>,
        public detail::cross_function_signature_raw_helper<decltype(&detail::cross_function_implementation<true,Iface,Id + Iface<implement_interface<T>>::base_sz,F,FuncType>::func)>{
		enum{N = Id + Iface<implement_interface<T>>::base_sz};
		typedef detail::cross_function_implementation<true,Iface,Id + Iface<implement_interface<T>>::base_sz,F,FuncType> cfi_t;
		cross_function(Iface<implement_interface<T>>* pi):cfi_t(
			static_cast<implement_interface<T>*>(pi)->get_portable_base()){	
				static_assert(static_cast<int>(N) < implement_interface<T>::num_functions,"Error in calculating size of vtable");
                cfi_t* cp = this;
                set_mem_fn<cfi_t,&cfi_t::call_stored_function>(cp);
		}	
        cross_function(portable_base* p):cfi_t(p){	
				static_assert(static_cast<int>(N) < implement_interface<T>::num_functions,"Error in calculating size of vtable");
                cfi_t* cp = this;
                set_mem_fn<cfi_t,&cfi_t::call_stored_function>(cp);
		}

		template<class Func>
		void operator=(Func f){
			cfi_t::set_function(*this,f);
            cfi_t* cp = this;
            set_mem_fn<cfi_t,&cfi_t::call_stored_function>(cp);

		}
		typedef detail::mem_fn_helper<F> tm;
		template<class C, typename tm:: template inner<C,Iface,N>::MFT mf>
		void set_mem_fn (C* c){
			typedef typename tm:: template inner<C,Iface,N>::MFT MF;
			typedef typename tm:: template inner<C,Iface,N>::ret_t R;
			typedef typename tm:: template inner<C,Iface,N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn = static_cast<vn_t*>(cfi_t::p_);
			vn->set_data(N,c);
			vn->update(N,&vte_t:: template func<C,MF,mf,R>);

		}
		typedef typename detail::fn_helper<F>::type func_ptr;
		template<class Func,Func func>
		void set_fn(){
			static_assert(std::is_same<Func, func_ptr>::value, "Invalid function pointer passed to set_fn. Did you forget to make a function static?");
			typedef typename tm:: template inner<cfi_t, Iface, N>::MFT MF;
			typedef typename tm:: template inner<cfi_t, Iface, N>::ret_t R;
			typedef typename tm:: template inner<cfi_t, Iface, N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn = static_cast<vn_t*>(cfi_t::p_);
			vn->set_data(N, static_cast <cfi_t*>(this));
			vn->update(N, &vte_t:: template func<cfi_t, MF, &cfi_t::template call_static_function<func>, R>);
		}
		template<class Executor>
		void set_executor(){
			typedef typename tm:: template inner<cfi_t, Iface, N>::MFT MF;
			typedef typename tm:: template inner<cfi_t, Iface, N>::ret_t R;
			typedef typename tm:: template inner<cfi_t, Iface, N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn = static_cast<vn_t*>(cfi_t::p_);
			vn->set_data(N, static_cast <cfi_t*>(this));
			vn->update(N, &vte_t:: template func<cfi_t, MF, &cfi_t::template call_executor<Executor>, R>);
		}

        typedef F function_signature;
	};

	struct portable_base_holder{
		portable_base* p_;
		portable_base_holder(portable_base* p):p_(p){};

	};
	template<class b>
	struct InterfaceBase{
	public:
		enum{base_sz = 0};
		enum{sz = 0};
	};



	namespace detail{

		template<template<class> class Iface>
		class reinterpret_portable_base_t{
			portable_base* p_;
		public:
			explicit reinterpret_portable_base_t(portable_base* p):p_(p){}
			portable_base* get()const{return p_;}
		};
	}

	template<template<class> class Iface>
	detail::reinterpret_portable_base_t<Iface> reinterpret_portable_base(portable_base* p){
		return detail::reinterpret_portable_base_t<Iface>(p);
	}

	template<template <class> class Iface>
	struct use_interface:private portable_base_holder, public Iface<use_interface<Iface>>{ // Usage


		use_interface(std::nullptr_t p = nullptr) :portable_base_holder(nullptr){ (void)p; }

		explicit use_interface(detail::reinterpret_portable_base_t<Iface> r):portable_base_holder(r.get()){}

		portable_base* get_portable_base()const{
			return this->p_;
		}

		explicit operator bool()const{
			return get_portable_base() != nullptr;
		}

		void reset_portable_base(){
			*this = nullptr;
		}

		enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};

	private:


		// Simple checksum that takes advantage that sum of squares can be calculated using the formula
        // n(n+1)(2n+1)/6
		enum{checksum = sizeof(Iface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

	};



	template<template <class> class Iface>
	use_interface<Iface> create(const module& m,std::string func){
		typedef portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
		auto f = m.load_module_function<CFun>(func);
		return use_interface<Iface>(reinterpret_portable_base<Iface>(f()));


	}



	template<template<class> class Iface>
	struct implement_interface:private vtable_n<sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)>,public Iface<implement_interface<Iface>>{ // Implementation


		implement_interface(){}

		
		use_interface<Iface> get_runtime_parent(){
			vtable_n_base* vnb = this;
			return use_interface<Iface>(reinterpret_portable_base<Iface>(vnb->runtime_parent_));
		}

		void set_runtime_parent(use_interface<Iface> parent){
			vtable_n_base* vnb = this;
			vnb->runtime_parent_ = parent.get_portable_base();
		}

		enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};

		// Simple checksum that takes advantage of sum of squares
		// Note that on implementations where the MAX_SIZE is not able to accommodate array elements in the millions this could fail
		enum{checksum = sizeof(Iface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

		using  vtable_n<sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)>::get_portable_base;
		const use_interface<Iface> get_use_interface(){return use_interface<Iface>(reinterpret_portable_base<Iface>(get_portable_base()));}
	};




	template<class b,template<class> class Base = InterfaceBase >
	struct define_interface:public Base<b>{

		enum{base_sz = sizeof(Base<size_only>)/sizeof(cross_function<Base<size_only>,0,void()>)};

		typedef define_interface base_t;
	};

}

#include "implementation/cross_compiler_conversions.hpp"
#include "implementation/custom_cross_function.hpp"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif