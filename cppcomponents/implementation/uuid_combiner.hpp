
// Adapted by John R. Bandela from
// Boost name_generator.hpp header file  ----------------------------------------------//

// Copyright 2010 Andy Tompkins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDE_GUARD_CPPCOMPONENTS_UUID_COMBINER_HPP_
#define INCLUDE_GUARD_CPPCOMPONENTS_UUID_COMBINER_HPP_

#include "../../cross_compiler_interface/uuid.hpp"
#include "../../cross_compiler_interface/implementation/safe_static_initialization.hpp"
#include "sha1.hpp"
#include <string>
#include <array>
#include <cstring> // for strlen, wcslen




namespace cppcomponents {

	using cross_compiler_interface::uuid_base;
	namespace detail {

		// generate a name-based uuid
		class name_generator {
		public:
			typedef uuid_base result_type;
			typedef std::array<uint8_t, 16> uuid_array_type;

			void process_uuid(const uuid_base& u){
				sha.process_bytes(&u, sizeof(uuid_base));
			}

			uuid_base sha_to_uuid()
			{
				unsigned int digest[5];

				sha.get_digest(digest);

				uuid_array_type u;
				for (int i = 0; i < 4; ++i) {
					*(u.begin() + i * 4 + 0) = ((digest[i] >> 24) & 0xFF);
					*(u.begin() + i * 4 + 1) = ((digest[i] >> 16) & 0xFF);
					*(u.begin() + i * 4 + 2) = ((digest[i] >> 8) & 0xFF);
					*(u.begin() + i * 4 + 3) = ((digest[i] >> 0) & 0xFF);
				}

				// set variant
				// must be 0b10xxxxxx
				*(u.begin() + 8) &= 0xBF;
				*(u.begin() + 8) |= 0x80;

				// set version
				// must be 0b0101xxxx
				*(u.begin() + 6) &= 0x5F; //0b01011111
				*(u.begin() + 6) |= 0x50; //0b01010000

				uuid_base ret;
				memcpy(&ret, u.data(), u.size());
				return ret;
			}

		private:
			detail::sha1 sha;
		};

	
		template<class... U>
		struct combine_uuid_helper;

		template<class First, class... Rest>
		struct combine_uuid_helper<First,Rest...>{
			static void combine(name_generator& n, uuid_base& u){
				n.process_uuid(First::get());
			}
		};
		template<class First>
		struct combine_uuid_helper<First>{
			static void combine(name_generator& n, uuid_base& u){
				n.process_uuid(First::get());
				u = n.sha_to_uuid();
			}
		};
	

	}
	// uuid in canonical form
	template <class... TUUIDS>
	class combine_uuid{
		uuid_base u_;
	public:
		combine_uuid(){

			detail::name_generator n;
			detail::combine_uuid_helper<TUUIDS...>::combine(n, u_);
		}
		static bool compare(const uuid_base& u){

			return std::memcmp(&u, &get(), sizeof(uuid_base)) == 0;
		}

		static const uuid_base& get(){
			typedef combine_uuid cu_t;
			return cross_compiler_interface::detail::safe_static_init<cu_t, cu_t>::get().u_;
		}

#ifdef _WIN32

		// Convenience function for Windows GUID

		template<class G>
		static bool compare_windows_guid(const G& g){
			static_assert(sizeof(G) == sizeof(uuid_base), "GUID and uuid_base have different sizes");
			return combine_uuid::compare(*reinterpret_cast<const uuid_base*>(&g));
		}
		template<class G>
		static const G& get_windows_guid(){
			// GUID is same as uuid_base
			static_assert(sizeof(G) == sizeof(uuid_base), "GUID and uuid_base have different sizes");
			return *reinterpret_cast<const G*>(&combine_uuid::get());
		}

#endif

	};


	template<class T>
	struct uuid_of{
		typedef typename T::uuid_type uuid_type;
	};

	// Definitions for some primitives

	template<>
	struct uuid_of<void>{
		typedef uuid < 0x6be5386a , 0x70ff , 0x4ecf , 0x86e0 , 0xda3a6c7459d8>
			uuid_type;
	};

	template<>
	struct uuid_of<std::string>{
		typedef uuid<0x0f636a0e,0x0da4 , 0x4d23 , 0xa7e4 , 0xa8059ec0a219 > uuid_type;
	};
} 

#endif // BOOST_UUID_NAME_GENERATOR_HPP
