
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


namespace cross_compiler_interface{
	template<class T>
	struct use;
}


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
				combine_uuid_helper<Rest...>::combine(n, u);
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

	// Specializations of uuid_of


	template<class T>
	struct uuid_of<cross_compiler_interface::use<T>>{
		typedef typename T::uuid_type uuid_type;
	};

	typedef cppcomponents::uuid<0x5901761d, 0x50a1, 0x468b, 0xbd59, 0x64ad8c29c519> ptr_uuid_base_t;

	template<class T>
	struct uuid_of<T*>{
		typedef combine_uuid<ptr_uuid_base_t, typename uuid_of<T>::uuid_type> uuid_type;
	};

	typedef cppcomponents::uuid<0xced8b2e3, 0x40c1, 0x4809, 0xa66f, 0x476f6d9e508f> const_uuid_base_t;

	template<class T>
	struct uuid_of<const T>{
		typedef combine_uuid<const_uuid_base_t, typename uuid_of<T>::uuid_type> uuid_type;
	};

	template<>
	struct uuid_of<void>{
		typedef uuid < 0x6be5386a, 0x70ff, 0x4ecf, 0x86e0, 0xda3a6c7459d8>
			uuid_type;
	};
	template<>
	struct uuid_of<void*>{
		typedef cppcomponents::uuid<0xcf0d6d5b, 0x5433, 0x4c7c, 0xab9d, 0xb1b0ef3c53c6>
			uuid_type;
	};

	template<class T>
	struct uuid_of<cross_compiler_interface::out<T>>{
		typedef cppcomponents::uuid<0x5bbe7a3a, 0x978e, 0x472f, 0xab1d, 0x1b4a13a935cd> uuid_type;
	};


	template<>
	struct uuid_of<std::string>{
		typedef uuid<0x0f636a0e, 0x0da4, 0x4d23, 0xa7e4, 0xa8059ec0a219 > uuid_type;
	};
	template<>
	struct uuid_of<std::wstring>{
		typedef cppcomponents::uuid<0xc4a80e08, 0x1400, 0x458d, 0xa6b7, 0x8103230b9561> uuid_type;
	};
	template<>
	struct uuid_of<std::u16string>{
		typedef cppcomponents::uuid<0x923a80d3, 0x059d, 0x4c67, 0x8479, 0xe3fcc4d2d26e> uuid_type;
	};
	template<>
	struct uuid_of<std::u32string>{
		typedef cppcomponents::uuid<0x800e2bba, 0x4275, 0x4fbc, 0x8424, 0x9e08dd0f305e> uuid_type;
	};

	template<>
	struct uuid_of<char>{
		typedef cppcomponents::uuid<0x92e4a128, 0x9332, 0x4604, 0xa6b9, 0x7a8b1abbb625> uuid_type;
	};
	template<>
	struct uuid_of<wchar_t>{
		typedef cppcomponents::uuid<0x88675721, 0x0fe0, 0x4d42, 0x92ca, 0xee6e214db3d2> uuid_type;
	};
	template<>
	struct uuid_of<std::int8_t>{
		typedef cppcomponents::uuid<0xec3ae032, 0xb95d, 0x4f8a, 0xb974, 0x43f27ea54c53> uuid_type;
	};
	template<>
	struct uuid_of<std::int16_t>{
		typedef cppcomponents::uuid<0x79d51b2c, 0xebf7, 0x4a99, 0x8886, 0xf86cf2280c56> uuid_type;
	};
	template<>
	struct uuid_of<std::int32_t>{
		typedef cppcomponents::uuid<0x15bed735, 0x5c8d, 0x42fe, 0xb407, 0x4b44a3f11117> uuid_type;
	};
	template<>
	struct uuid_of<std::int64_t>{
		typedef cppcomponents::uuid<0x6e98f205, 0xc724, 0x4ba2, 0xb9ec, 0xb9814b9855a5> uuid_type;
	};
	template<>
	struct uuid_of<std::uint8_t>{
		typedef cppcomponents::uuid<0x4add7e4e, 0x4199, 0x4453, 0x80f5, 0x80457b6d5ab5> uuid_type;
	};
	template<>
	struct uuid_of<std::uint16_t>{
		typedef cppcomponents::uuid<0x0051f1fc, 0x725f, 0x49e2, 0xaefd, 0x70b528991441> uuid_type;
	};
	template<>
	struct uuid_of<std::uint32_t>{
		typedef cppcomponents::uuid<0x73d6a2c5, 0x75cc, 0x4b5a, 0xb003, 0x2492d234c794> uuid_type;
	};
	template<>
	struct uuid_of<std::uint64_t>{
		typedef cppcomponents::uuid<0x3443f713, 0xf34b, 0x45a1, 0x837c, 0x843257b38075> uuid_type;
	};
	template<>
	struct uuid_of<float>{
		typedef cppcomponents::uuid<0xc47b942a, 0xfc33, 0x4311, 0x999f, 0x37e0da5588a2> uuid_type;
	};	template<>
	struct uuid_of<double>{
		typedef cppcomponents::uuid<0x729769b7, 0x8079, 0x4466, 0x8ecb, 0x95c6a64412f0> uuid_type;
	};
	template<>
	struct uuid_of<portable_base*>{
		typedef cppcomponents::uuid<0xf47812cf, 0x6649, 0x4b0a, 0x93c3, 0xc8498b63ed67> uuid_type;
	};
	#ifndef _MSC_VER
		// In MSVC char16_t and char32_t are not real types
	template<>
	struct uuid_of<char16_t>{
		typedef cppcomponents::uuid<0x20e889f1, 0x2ce9, 0x4786, 0x85ce, 0xc03f2004b1ab> uuid_type;
	};
	template<>
	struct uuid_of<char32_t>{
		typedef cppcomponents::uuid<0xba133a1c, 0x8726, 0x4228, 0x90f6, 0x6ed953db91f7> uuid_type;
	};

	#endif



	template<>
	struct uuid_of<cr_string>{
		typedef cppcomponents::uuid<0x92847be0, 0x5f45, 0x4313, 0x9a1e, 0xae97f14e21c3> uuid_type;
	};
	template<>
	struct uuid_of<cr_wstring>{
		typedef cppcomponents::uuid<0x6ae82ba3, 0x022f, 0x48c2, 0xb756, 0x6b08cd0a9710> uuid_type;
	};
	template<>
	struct uuid_of<cr_u16string>{
		typedef cppcomponents::uuid<0x4b05a753, 0xf033, 0x4255, 0xa578, 0x980f0ef0b820> uuid_type;
	};
	template<>
	struct uuid_of<cr_u32string>{
		typedef cppcomponents::uuid<0x973885bf, 0xec15, 0x4b44, 0xae79, 0x631be6e3784f> uuid_type;
	};
	template<>
	struct uuid_of<uuid_base>{
		typedef cppcomponents::uuid<0x2e1ac203, 0x4ba6, 0x4865, 0x938f, 0xbfac5f765e54> uuid_type;
	};

	typedef cppcomponents::uuid<0x0d0ce59f, 0xb3d1, 0x4bdc, 0xa0e9, 0xf4e55219e7ca> vector_uuid_base_t;
	template<class T>
	struct uuid_of<std::vector<T>>{
		typedef combine_uuid<vector_uuid_base_t,typename uuid_of<T>::uuid_type> uuid_type;
	};

	typedef cppcomponents::uuid<0x1f696587, 0x017f, 0x420b, 0xb77d, 0x84c5af9bf3ff> pair_uuid_base_t;
	template<class T1, class T2>
	struct uuid_of<std::pair<T1,T2>>{
		typedef combine_uuid<pair_uuid_base_t,typename uuid_of<T1>::uuid_type,typename uuid_of<T2>::uuid_type> uuid_type;
	};

	typedef cppcomponents::uuid<0xb0f000e9, 0x37aa, 0x45b9, 0x9574, 0x35bd8dcdfcb4> tuple_uuid_base_t;
	template<>
	struct uuid_of<std::tuple<>>{
		typedef combine_uuid<tuple_uuid_base_t> uuid_type;
	};
	template<class T1,class... T>
	struct uuid_of<std::tuple<T1,T...>>{
		typedef combine_uuid<tuple_uuid_base_t, typename uuid_of<T1>::uuid_type, typename uuid_of<T>::uuid_type...> uuid_type;
	};






} 

#endif // BOOST_UUID_NAME_GENERATOR_HPP
