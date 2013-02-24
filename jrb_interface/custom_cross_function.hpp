#pragma once

#ifndef JRB_CUSTOM_CROSS_FUNCTION_H_02_23_2013
#define JRB_CUSTOM_CROSS_FUNCTION_H_02_23_2013
#include "jrb_interface.hpp"

namespace jrb_interface{

template<class F>
struct fn_ptr_helper{};


template<class R, class... Parms>
struct fn_ptr_helper<R(Parms...)>{
	typedef R (CROSS_CALL_CALLING_CONVENTION *fn_ptr_t)(Parms...);

};

//template<class F>
//struct general_vtable_entry_helper{};
//
//
//template<class F>
//struct general_vtable_entry_helper_helper{
//	typedef F type;
//
//};
//
//template<class R, class... Parms>
//struct general_vtable_entry_helper<R(Parms...){
//
//	typedef typename general_vtable_entry_helper_helper<
//		error_code (const portable_base* v, typename cross_conversion<R>::converted_type* r,typename cross_conversion<Parms>::converted_type... p)
//	>::type type;
//	
//		
//		typedef error_code (CROSS_CALL_CALLING_CONVENTION *fn_ptr_t)(const portable_base* v, typename cross_conversion<R>::converted_type* r,typename cross_conversion<Parms>::converted_type... p);
//
//};
//template<class... Parms>
//struct general_vtable_entry_helper<void(Parms...){
//
//	typedef typename general_vtable_entry_helper_helper<
//		error_code (const portable_base* v, typename cross_conversion<Parms>::converted_type... p)
//	>::type type;
//	
//		
//		typedef error_code (CROSS_CALL_CALLING_CONVENTION *fn_ptr_t)(const portable_base* v, typename cross_conversion<Parms>::converted_type... p);
//
//};



template<class Iface, int Id,class F1, class F2,class Derived,class FuncType = std::function<F1>>
struct custom_cross_function{};


// For usage
template<template<bool> class Iface, int Id,class F1, class F2,class Derived,class FuncType>
struct custom_cross_function<Iface<false>,Id,F1,F2,Derived,FuncType>{
private:
	portable_base* pV_;
public:
	enum{N = Iface<false>::base_sz + Id};
	typedef typename std::function<F1>::result_type ret;
	typedef typename fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;


			template<int n>
		custom_cross_function(vtable_n<false,n>* vn):pV_(vn->get_portable_base()){
			static_assert(N<n,"Interface::sz too small");
#ifdef _DEBUG
			vn->template check_interface_nums<N>();
#endif

		}
	template<class... Parms>
	ret operator()(Parms... p){
		return static_cast<Derived*>(this)->call_vtable_function(p...);
	}

protected:

	struct helper{
		portable_base* pV_;
		helper(portable_base* p):pV_(p){}
	};

	vtable_fn_ptr_t get_vtable_fn(){
		return (vtable_fn_ptr_t)pV_->vfptr[N];
	}

	portable_base* get_portable_base(){
		return pV_;
	};

	void exception_from_error_code(error_code e){
		error_mapper<Iface>::mapper::exception_from_error_code(e);
	}
};


// For implementation
template<template<bool> class Iface, int Id,class F1, class F2,class Derived,class FuncType>
struct custom_cross_function<Iface<true>,Id,F1,F2,Derived,FuncType>:public FuncType { // For empty base optimization in case FuncType is of 0 size
private:
		portable_base* pV_;
public:

		enum{N = Iface<true>::base_sz + Id};
	typedef typename std::function<F1>::result_type ret;

	typedef typename fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;
		template<class... Parms>
		ret operator()(Parms... p){
			return static_cast<Derived*>(this)->call_vtable_function(p...);
		}
		template<int sz>
		custom_cross_function(vtable_n<true,sz>* vn):pV_(vn->get_portable_base()){
			static_assert(N<sz,"Interface::sz too small");
			vn->template set_function<N>(static_cast<FuncType*>(this));
			vn->template add<N>(Derived::vtable_func);
		}

		typedef detail::mem_fn_helper<F1> tm;
		template<class C, typename tm:: template inner<C,Iface,N>::MFT mf>
		void set_mem_fn (C* c){
			typedef typename tm:: template inner<C,Iface,N>::MFT MF;
			typedef typename tm:: template inner<C,Iface,N>::ret_t R;
			typedef typename tm:: template inner<C,Iface,N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn =(vn_t*)pV_;
			vn->template set_function<N>(c);
			vn->template update<N>(&Derived:: template vtable_func_mem_fn<C,MF,mf>);

		}

protected:

	template<class Func>
	void set_function(Func f){
		vtable_n_base* vn =(vtable_n_base*)pV_;
		*( (FuncType*)vn->pFunctions_[N]) = f;
	}

	vtable_fn_ptr_t get_vtable_fn(){
		return (vtable_fn_ptr_t)pV_->vfptr[N];
	}

	portable_base* get_portable_base(){
		return pV_;
	};

	struct helper{
		portable_base* pV_;
		helper(portable_base* p):pV_(p){}

		FuncType& get_function(){
			vtable_n_base* vn =(vtable_n_base*)pV_;

			return *( (FuncType*)vn->pFunctions_[N]);


		}

		template<class C>
		C* get_mem_fn_object(){
			vtable_n_base* vn =(vtable_n_base*)pV_;

			return (C*)vn->pFunctions_[N];


		}

		error_code error_code_from_exception(std::exception& e){
			return error_mapper<Iface>::mapper::error_code_from_exception(e);
		}


	};





};







}



#endif
