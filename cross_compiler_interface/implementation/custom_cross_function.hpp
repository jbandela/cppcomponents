
namespace cross_compiler_interface{

template<class F>
struct fn_ptr_helper{};


template<class R, class... Parms>
struct fn_ptr_helper<R(Parms...)>{
	typedef R (CROSS_CALL_CALLING_CONVENTION *fn_ptr_t)(Parms...);

};

template<class Iface, int Id,class F1, class F2,class Derived,class FuncType = std::function<F1>>
struct custom_cross_function{};


// for size

	template<template<class> class Iface,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<size_only>,Id,F1,F2,Derived,FuncType>{char a[1024];
	struct helper{};
	};
	
	// for checksum

	template<template<class> class Iface,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<checksum_only>,Id,F1,F2,Derived,FuncType>{ char a[1024*(Id+1+Iface<checksum_only>::base_sz)*(Id+1+Iface<checksum_only>::base_sz)];
	struct helper{};
	};

// For usage
template<class User, template<class> class Iface, int Id,class F1, class F2,class Derived,class FuncType>
struct custom_cross_function<Iface<User>,Id,F1,F2,Derived,FuncType>{
private:
	portable_base* p_;
public:
	enum{N = Iface<User>::base_sz + Id};
	typedef typename std::function<F1>::result_type ret;
	typedef typename fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;

	enum{interface_sz = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>) - Iface<User>::base_sz };
	static_assert(Id < interface_sz,"You have misnumbered a cross_function Id, possibly skipped a number");
	custom_cross_function(Iface<User>* pi):p_(static_cast<User*>(pi)->get_portable_base()){}


	template<class... Parms>
	ret operator()(Parms... p)const{
		if(p_){
			return static_cast<const Derived*>(this)->call_vtable_function(p...);
		}
		else{
			throw error_pointer();
		}
	}

protected:

	struct helper{
		portable_base* p_;
		helper(portable_base* p):p_(p){}
	};

	vtable_fn_ptr_t get_vtable_fn()const{
		return reinterpret_cast<vtable_fn_ptr_t>(p_->vfptr[N]);
	}

	portable_base* get_portable_base()const{
		return p_;
	};

	void exception_from_error_code(error_code e)const{
		error_mapper<Iface>::mapper::exception_from_error_code(e);
	}
};


// For implementation
template<template<class> class Iface, template<class> class T,int Id,class F1, class F2,class Derived,class FuncType>
struct custom_cross_function<Iface<implement_interface<T>>,Id,F1,F2,Derived,FuncType>:public FuncType { // For empty base optimization in case FuncType is of 0 size
private:
		portable_base* p_;
public:

	enum{N = Iface<implement_interface<T>>::base_sz + Id};
	enum{interface_sz = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>) - Iface<implement_interface<T>>::base_sz };
	static_assert(Id < interface_sz,"You have misnumbered a cross_function Id, possibly skipped a number");
	custom_cross_function(Iface<implement_interface<T>>* pi):p_(static_cast<implement_interface<T>*>(pi)->get_portable_base()){
		auto vn = static_cast<vtable_n_base*>(p_);
		vn->set_data(N,static_cast<FuncType*>(this));
		vn->add(N,&Derived::vtable_func);

	}

	typedef typename std::function<F1>::result_type ret;

	typedef typename fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;
		template<class... Parms>
		ret operator()(Parms... p){
			if(p_){
				return static_cast<Derived*>(this)->call_vtable_function(p...);
			}
			else{
				throw error_pointer();
			}
		}

		typedef detail::mem_fn_helper<F1> tm;
		template<class C, typename tm:: template inner<C,Iface,N>::MFT mf>
		void set_mem_fn (C* c){
			typedef typename tm:: template inner<C,Iface,N>::MFT MF;
			typedef typename tm:: template inner<C,Iface,N>::ret_t R;
			typedef typename tm:: template inner<C,Iface,N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn =static_cast<vn_t*>(p_);
			vn->set_data(N,c);
			vn->update(N,&Derived:: template vtable_func_mem_fn<C,MF,mf>);

		}

protected:

	template<class Func>
	void set_function(Func f){
		detail::get_function<N,FuncType>(p_) = f;
	}

	vtable_fn_ptr_t get_vtable_fn(){
		return reinterpret_cast<vtable_fn_ptr_t>(p_->vfptr[N]);
	}

	portable_base* get_portable_base(){
		return p_;
	};

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





};







}


