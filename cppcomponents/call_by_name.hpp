#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_CALL_BY_NAME_HPP_5_5_2014_
#define INCLUDE_GUARD_CPPCOMPONENTS_CALL_BY_NAME_HPP_5_5_2014_


#include "cppcomponents.hpp"
#include "implementation/uuid_combiner.hpp"
#include <functional>

namespace cppcomponents{

	template<class UuidType, class AnyType,class T>
	struct call_by_name_conversion{
		static T convert_from_any(const AnyType&);
		static AnyType convert_to_any(const T&);
	};

	typedef cppcomponents::uuid<0x01a79de5, 0x3510, 0x4cc4, 0xa2a9, 0x7af3c4fa2168> callbyname_base_uuid_t;
	template<class UuidType,class AnyType>
	struct ICallInterfaceByName :define_interface<combine_uuid<callbyname_base_uuid_t,UuidType, typename uuid_of<AnyType>::uuid_type>>{
		std::vector<std::string> GetMethodNames();
		AnyType Call(string_ref name, std::vector<AnyType> args);

		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(ICallInterfaceByName,GetMethodNames,Call)
	};

	inline const char* callinterfaceid(){ return "cppcomponents_call_interface"; }
	template<class UuidType, class AnyType>
	using runtime_class_call_interface = runtime_class<callinterfaceid, object_interfaces<ICallInterfaceByName<UuidType,AnyType>>>;



	namespace detail{


	}

	template<class UuidType,class AnyType,class TInterface>
	struct implement_call_by_name : implement_runtime_class<implement_call_by_name<UuidType,AnyType,TInterface>, runtime_class_call_interface<UuidType,AnyType>>
	{


		implement_call_by_name(use<TInterface> i) :iface_{ i }
		{
			set_names();
			set_functions();
		}

		std::vector<std::string> GetMethodNames(){
			return names_;
		}
		AnyType Call(string_ref name, std::vector<AnyType> args){
			for (unsigned int i = 0; i < names_.size(); ++i){
				if (name == string_ref(names_[i])){
					return functions_.at(i)(iface_,args);
				}
			}
			throw cppcomponents::error_invalid_arg();
		}

	private:
		typedef std::function<AnyType(use<TInterface>, std::vector<AnyType>)> func_type;

		template<class R,int dummy>
		struct return_helper{
			template<class F>
			static AnyType do_return(F f){
				return call_by_name_conversion<UuidType,AnyType, R>::convert_to_any(f());
			}
		};

		template<int dummy>
		struct return_helper<void,dummy>{
			template<class F>
			static AnyType do_return(F f){
				f();
				return AnyType();
			}
		};

		template<class TI>
		struct getter{
			typedef typename TI::type type;
			static type get_and_convert(const std::vector<AnyType>& v){
				return call_by_name_conversion<UuidType,AnyType, type>::convert_from_any(v.at(TI::index));
			}
		};

		template<class R,class... TI>
		struct call_function_getter{
			template<class F>
			static func_type get_call_function(portable_base* p,F f){
				return[p,f](use<TInterface> i, const std::vector<AnyType>& v){
					(void)v;
					(void)i;
					return return_helper<R,0>::do_return([&](){
						return F::call(p,getter<TI>::get_and_convert(v)...);
					});

				};
			}
		};


		template<class F>
		struct signature_helper;

		template<class T, std::size_t I>
		struct type_and_index{
			enum{ index = I };
			typedef T type;

		};

		template<int I, class... T>
		struct to_type_and_index{};
		template<int I, class First, class... Rest>
		struct to_type_and_index<I, First, Rest...>
			:public to_type_and_index<I - 1, Rest..., type_and_index<First, 1 + sizeof...(Rest)-I>>{};

		template<class... T>
		struct to_type_and_index<0, T...>{
			typedef std::tuple<T...> types;

		};
		template<class T>
		struct to_type_and_index<0, T>{
			typedef std::tuple<T> types;

		};

		template<class R, class... P>
		struct signature_helper<R(P...)>{
			typedef R return_type;
			typedef cppcomponents::type_list<P...> argument_types;
			typedef  to_type_and_index<sizeof...(P), P...> tti_t;
			typedef typename tti_t::types argument_type_and_index;
			template<class F, class... TI>
			static func_type get_call_function_helper(portable_base* p,F f, std::tuple<TI...>){
				return call_function_getter<R,TI...>::get_call_function(p,f);
			}
			template<class F>
			static func_type get_call_function(portable_base* p,F f){
				return get_call_function_helper(p,f, argument_type_and_index());
			}


		};

		template<int dummy, class TypeList>
		struct process_cross_functions;



		template<int dummy, class CF, class... Rest>
		struct process_cross_functions<dummy, std::tuple<CF, Rest...>>{
			static void add_to_vector(portable_base* p, std::vector<func_type>& v){
				CF cf;
				v.push_back(signature_helper<typename CF::function_signature>::get_call_function(p,cf));

				process_cross_functions<dummy, cppcomponents::type_list<Rest...>>::add_to_vector(p, v);
			}
		};

		
		template<int dummy>
		struct process_cross_functions<dummy, std::tuple<>>{
			static void add_to_vector(portable_base* , std::vector<func_type>& ){
			}
		};

		void set_names(){
			auto names = ti::get_function_names();
			std::vector<std::string> ret;
			for (std::size_t i = 0; i < ti::get_number_functions(); ++i){
				names_.push_back(names[i]);
			}
		}



		void set_functions(){
			process_cross_functions < 0, typename ti::functions>::add_to_vector(iface_.get_portable_base(), functions_);


		}



		use<TInterface> iface_;
		std::vector < std::string > names_;
		std::vector<func_type> functions_;
		typedef typename TInterface::template Interface<use<TInterface>>::interface_information ti;
		

	};

	namespace detail{
		template<class T>
		struct extract_uuidtype_anytype;

		template<class UuidType, class AnyType>
		struct extract_uuidtype_anytype<ICallInterfaceByName<UuidType,AnyType>>{
			typedef UuidType uuid_type;
			typedef AnyType any_type;
		};
	}
	template<class CallByNameInterface, class Iface>
	use<CallByNameInterface> make_call_by_name(const use<Iface>& i){
		typedef typename detail::extract_uuidtype_anytype<CallByNameInterface>::uuid_type UuidType;
		typedef typename detail::extract_uuidtype_anytype<CallByNameInterface>::any_type AnyType;
		return implement_call_by_name<UuidType, AnyType, Iface>::create(i).template QueryInterface<CallByNameInterface>();
	}

}

#endif