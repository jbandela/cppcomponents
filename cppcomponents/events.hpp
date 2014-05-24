#pragma once

#if 0//INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "implementation/spinlock.hpp"

#include "function.hpp"

namespace cppcomponents{


	template<class D,class Delegate,class Add, class Remove>
	struct event{
		cross_compiler_interface::portable_base* p_;
		template<class I>
		event(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}

		template<class F>
		std::int64_t operator +=(F f){
			Add addfunc{p_};
			return addfunc(make_delegate<Delegate>(f));
		}

		void operator -=(std::int64_t i){
			Remove removefunc(p_);
			removefunc(i);
		}

	};
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4673 4672)
#endif

	template<class Delegate>
	struct event_implementation{

		typedef std::vector<use<Delegate>> containter_t;
		containter_t delegates_;

		std::atomic<bool> evt_lock_;

		event_implementation() :evt_lock_(false){}

		std::int64_t add(use<Delegate> d){

			spin_locker lock{ evt_lock_ };

			delegates_.push_back(d);
			return static_cast<std::int64_t>(delegates_.size()-1);

		}

		void remove(std::int64_t token){
			spin_locker lock{ evt_lock_ };

			auto i = static_cast<std::size_t>(token);
			if (i >= delegates_.size()){
				throw cross_compiler_interface::error_out_of_range();
			}
			delegates_[i] = nullptr;
		}

		template<class... P>
		void raise(P&&... p){
			containter_t copy_delegates;
			{
				spin_locker lock{ evt_lock_ };

				copy_delegates = delegates_;
			}

			for (auto& d : copy_delegates){
				if (d){
					d(std::forward<P>(p)...);

				}
			}
		}

	};
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<CppComponentInterfaceExtrasT,Delegate,decltype(Interface<CppComponentInterfaceExtrasT>::Add),decltype(Interface<CppComponentInterfaceExtrasT>::Remove)>



#endif