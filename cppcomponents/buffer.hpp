//          Copyright John R. Bandela 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_BUFFER_HPP_05_29_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_BUFFER_HPP_05_29_2013_


#include "cppcomponents.hpp"

#include <memory.h>

namespace cppcomponents{

	// IBuffer - encapsulates a buffer along and frees it
	// when released


	struct IBuffer : cppcomponents::define_interface < cppcomponents::uuid<0xca615379, 0xad25, 0x42a1, 0xa3e0, 0xb807920dd860>>
	{
		char* Begin();
		char* End();
		char* CapacityEnd();
		void SetSize(std::size_t sz);

		CPPCOMPONENTS_CONSTRUCT(IBuffer, Begin, End,CapacityEnd, SetSize)

		CPPCOMPONENTS_INTERFACE_EXTRAS(IBuffer){
			std::size_t Size(){
				return this->get_interface().End() - this->get_interface().Begin();
			}
			std::size_t Capacity(){
				return this->get_interface().CapacityEnd() - this->get_interface().Begin();
			}

		};
	};

	struct IBufferStatics : cppcomponents::define_interface < cppcomponents::uuid<0x5bea5461, 0x1669, 0x46d6, 0xa517, 0x61d417213a38> >
	{
		cppcomponents::use<IBuffer> Create(std::size_t);
		cppcomponents::use<IBuffer> OwningIBufferFromPointer(char*);
		cppcomponents::use<IBuffer> NonOwningIBufferFromPointer(char*);

		CPPCOMPONENTS_CONSTRUCT(IBufferStatics, Create, OwningIBufferFromPointer,NonOwningIBufferFromPointer)
	};
	inline std::string BufferDummyId(){ return "cppcomponents::uuid<0x5bea5461, 0x1669, 0x46d6, 0xa517, 0x61d417213a38>"; }

	typedef cppcomponents::runtime_class < BufferDummyId,
		cppcomponents::object_interfaces<IBuffer>,
		cppcomponents::factory_interface<cppcomponents::NoConstructorFactoryInterface>,
		cppcomponents::static_interfaces<IBufferStatics>
	> Buffer_t;

	typedef cppcomponents::use_runtime_class<Buffer_t> Buffer;


	struct implement_buffer : cppcomponents::implement_runtime_class<implement_buffer, Buffer_t>
	{
		void* allocated_memory_;
		std::size_t size_;
		char* begin_;
		char* end_;
		char* Begin(){
			return begin_ ;
		}
		char* End(){
			return Begin() + size_;
		}
		char* CapacityEnd(){
			return end_;
		}
		void SetSize(std::size_t sz){
			if (sz > std::size_t(end_ - begin_)){
				throw error_invalid_arg();
			}
			else{
				size_ = sz;
			}
		}

		implement_buffer(void* mem, void* e)
			: allocated_memory_{ mem },  end_{ static_cast<char*>(e)}
		{
			auto p = static_cast<void*>(reinterpret_cast<char*>(this) + sizeof(implement_buffer));
			auto pt = this;
			memcpy(p, static_cast<void*>(&pt), sizeof(this));
			begin_ = reinterpret_cast<char*>(this) + sizeof(implement_buffer)+sizeof(void*);
			size_ = end_ - begin_;
		
		}

		static use<IBuffer> Create(std::size_t sz){

			auto total_sz = sz + sizeof(implement_buffer) + sizeof(void*) ;
			void* p = :: operator new(total_sz);
			auto imp = new(p) implement_buffer{p,static_cast<char*>(p) + total_sz};

			return imp->QueryInterface<IBuffer>();

		}
		static cppcomponents::use<IBuffer> NonOwningIBufferFromPointer(char* p){
			if (!p){
				throw error_invalid_arg();
			}

			auto vp = static_cast<void*>(p - sizeof(void*) );
			implement_buffer* imp = nullptr;
			memcpy(static_cast<void*>(&imp), vp, sizeof(imp));
			return imp->QueryInterface<IBuffer>();
		}
		static cppcomponents::use<IBuffer> OwningIBufferFromPointer(char* p){
			if (!p){
				throw error_invalid_arg();
			}

			auto vp = static_cast<void*>(p - sizeof(void*) );
			implement_buffer* imp = nullptr;
			memcpy(static_cast<void*>(&imp), vp, sizeof(imp));
			return use<IBuffer>(*imp->get_implementation<IBuffer>(), false);
		}


		void ReleaseImplementationDestroy(){
			// Run destructor
			auto p = allocated_memory_;
			this->~implement_buffer();
			::operator delete(p);

		}

	};

	CPPCOMPONENTS_REGISTER(implement_buffer)

}


#endif