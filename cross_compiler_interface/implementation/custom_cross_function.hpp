//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

namespace cross_compiler_interface{

	namespace detail {
		template<class F>
		struct fn_ptr_helper{};


		template<class R, class... Parms>
		struct fn_ptr_helper<R(Parms...)>{
			typedef R (CROSS_CALL_CALLING_CONVENTION *fn_ptr_t)(Parms...);

		};

		template<class C,class R,class MF, MF mf>
		struct mem_fn_caller{
			C* c_;
			mem_fn_caller(C* c):c_(c){}

			template<class... T>
			R operator()(T... t){
				return (c_->*mf)(t...);
			}
		};

		template<template<class> class Iface, class Derived,int N, class FuncType, class F>
		struct custom_function_vtable_functions{};

		template<template<class> class Iface, class Derived, int N, class FuncType, class R, class... P>
		struct custom_function_vtable_functions<Iface,Derived,N,FuncType, R(P...)>{

			typedef typename fn_ptr_helper<R(P...)>::fn_ptr_t vtable_fn_ptr_t;

			struct helper{
				portable_base* p_;
				helper(portable_base* p):p_(p){}

				FuncType& get_function(){
					return detail::get_function<N,FuncType>(p_);
				}

				template<class C>
				C* get_mem_fn_object(){

					return detail::get_data<N,C>(p_);
				}

				error_code error_code_from_exception(std::exception& e){
					return error_mapper<Iface>::mapper::error_code_from_exception(e);
				}

				template<class... Parms>
				error_code forward_to_runtime_parent(Parms... t){
					// See if runtime inheritance present with parent
					vtable_n_base* vn = static_cast<vtable_n_base*>(p_);
					if(vn->runtime_parent_){
						// call the parent
						return reinterpret_cast<vtable_fn_ptr_t>(vn->runtime_parent_->vfptr[N])(vn->runtime_parent_,t...);
					}
					else{
						return error_not_implemented::ec;
					}
				}


			};
			template<class... Parms>
			static R vtable_entry_function_helper(portable_base* p, Parms... parms){
				helper h(p);
				try{
					auto f = h.get_function();
					if(!f){
						return h.forward_to_runtime_parent(parms...);
					}
					return Derived::vtable_function(f,p,parms...);
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}

			}		

			static R CROSS_CALL_CALLING_CONVENTION vtable_entry_function(P... parms){
				return vtable_entry_function_helper(parms...);
			}
			template<class C, class MF, MF mf,class... Parms>
			static R vtable_entry_function_mem_fn_helper(portable_base* p, Parms... parms){
				helper h(p);
				try{
					C* c = h.template get_mem_fn_object<C>();
					mem_fn_caller<C,typename FuncType::result_type,MF,mf> f(c);
					return Derived::vtable_function(f,p,parms...);
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}

			}
			template<class C, class MF, MF mf>
			static R CROSS_CALL_CALLING_CONVENTION vtable_entry_function_mem_fn(P... parms){
				return vtable_entry_function_mem_fn_helper<C,MF,mf>(parms...);
			}

		};

	}


	template<class Iface, int Id,class F1, class F2,class Derived,class FuncType = std::function<F1>>
	struct custom_cross_function{};


	// for size

	template<template<class> class Iface,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<size_only>,Id,F1,F2,Derived,FuncType>{char a[1024];
	template<class T>
	custom_cross_function(T t){}
	typedef custom_cross_function base_t;

	};

	// for checksum

	template<template<class> class Iface,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<checksum_only>,Id,F1,F2,Derived,FuncType>{ char a[1024*(Id+1+Iface<checksum_only>::base_sz)*(Id+1+Iface<checksum_only>::base_sz)];
	template<class T>
	custom_cross_function(T t){}
	typedef custom_cross_function base_t;

	};


	namespace detail{
		template<class Derived,class F>
		struct custom_cross_function_op_call{};
		template<class Derived, class R, class... Parms>
		struct custom_cross_function_op_call<Derived,R(Parms...)>{

			R operator()(const Parms&... p)const{
				if(static_cast<const Derived*>(this)->get_portable_base()){
					return static_cast<const Derived*>(this)->call_vtable_function(p...);
				}
				else{
					throw error_pointer();
				}
			}
		};
	}

	// For usage
	template<class User, template<class> class Iface, int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<User>,Id,F1,F2,Derived,FuncType>
		:public detail::custom_cross_function_op_call<Derived,F1>{
	private:
		friend detail::custom_cross_function_op_call<Derived,F1>;
		portable_base* p_;
	public:
		typedef custom_cross_function base_t;
		enum{N = Iface<User>::base_sz + Id};
		typedef typename std::function<F1>::result_type ret;
		typedef typename detail::fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;

		custom_cross_function(Iface<User>* pi):p_(static_cast<User*>(pi)->get_portable_base()){
			//static_assert(static_cast<int>(N) < User::num_functions,"Error in calculating size of vtable");
		}	
        custom_cross_function(portable_base* p):p_(p){
			//static_assert(static_cast<int>(N) < User::num_functions,"Error in calculating size of vtable");
		}

        typedef F1 function_signature;
        typedef F2 function_signature_raw;

	protected:

		vtable_fn_ptr_t get_vtable_fn()const{
			return reinterpret_cast<vtable_fn_ptr_t>(p_->vfptr[N]);
		}

		portable_base* get_portable_base()const{
			return p_;
		}

		void exception_from_error_code(error_code e)const{
			error_mapper<Iface>::mapper::exception_from_error_code(e);
		}
	};



	// For implementation
	template<template<class> class Iface, template<class> class T,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<implement_interface<T>>,Id,F1,F2,Derived,FuncType>:public FuncType,
		public detail::custom_cross_function_op_call<Derived,F1>{ // For empty base optimization in case FuncType is of 0 size
	private:
		friend detail::custom_cross_function_op_call<Derived,F1>;
		portable_base* p_;
	public:
		typedef custom_cross_function base_t;
		enum{N = Iface<implement_interface<T>>::base_sz + Id};
	private:
		typedef detail::custom_function_vtable_functions<Iface,Derived,N,FuncType, F2> vtable_functions_t;
	public:
        typedef F1 function_signature;
        typedef F2 function_signature_raw;

		custom_cross_function(Iface<implement_interface<T>>* pi):p_(static_cast<implement_interface<T>*>(pi)->get_portable_base()){
			auto vn = static_cast<vtable_n_base*>(p_);
			vn->set_data(N,static_cast<FuncType*>(this));
			vn->add(N,&vtable_functions_t::vtable_entry_function);
			static_assert(static_cast<int>(N) < implement_interface<T>::num_functions,"Error in calculating size of vtable");
		}

		typedef typename std::function<F1>::result_type ret;

		typedef typename detail::fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;
		typedef detail::mem_fn_helper<F1> tm;
		template<class C, typename tm:: template inner<C,Iface,N>::MFT mf>
		void set_mem_fn (C* c){
			typedef typename tm:: template inner<C,Iface,N>::MFT MF;

			typedef vtable_n_base vn_t;
			vn_t* vn =static_cast<vn_t*>(p_);
			vn->set_data(N,c);
			vn->update(N,&vtable_functions_t:: template vtable_entry_function_mem_fn<C,MF,mf>);

		}

	protected:

		template<class Func>
		void set_function(Func f){
			detail::get_function<N,FuncType>(p_) = f;
		}

		vtable_fn_ptr_t get_vtable_fn()const{
			return reinterpret_cast<vtable_fn_ptr_t>(p_->vfptr[N]);
		}

		portable_base* get_portable_base()const{
			return p_;
		};


	};







}


