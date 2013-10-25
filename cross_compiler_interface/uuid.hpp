#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_UUID_HPP
#define INCLUDE_GUARD_CPPCOMPONENTS_UUID_HPP

#include <cstdint>

namespace cross_compiler_interface{


	namespace detail{

		template<std::uint64_t integer_value, std::uint8_t n>
		struct get_byte{
			enum{ value = (integer_value >> (8 * n)) & 0xFF };
		};
	}

	// Same structure as windows GUID
	struct uuid_base{
		std::uint32_t Data1;
		std::uint16_t Data2;
		std::uint16_t Data3;
		std::uint8_t Data4[8];
	};


	static_assert(sizeof(uuid_base) == 16, "Incorrect size of uuid_base");


	// uuid in canonical form
	template <
		std::uint32_t g1, // 8
		std::uint16_t g2, // 4
		std::uint16_t g3, // 4
		std::uint16_t g4, // 4
		std::uint64_t g5 // 12
	>
	struct uuid{
		enum : std::uint32_t{
			d1 = g1,
			d2 = g2,
			d3 = g3,
			d4 = detail::get_byte<g4, 1>::value,
			d5 = detail::get_byte<g4, 0>::value,
			d6 = detail::get_byte<g5, 5>::value,
			d7 = detail::get_byte<g5, 4>::value,
			d8 = detail::get_byte<g5, 3>::value,
			d9 = detail::get_byte<g5, 2>::value,
			d10 = detail::get_byte<g5, 1>::value,
			d11 = detail::get_byte<g5, 0>::value,



		};
		static bool compare(const uuid_base& u){

			return (d1 == u.Data1 &&
				d2 == u.Data2 &&
				d3 == u.Data3 &&
				d4 == u.Data4[0] &&
				d5 == u.Data4[1] &&
				d6 == u.Data4[2] &&
				d7 == u.Data4[3] &&
				d8 == u.Data4[4] &&
				d9 == u.Data4[5] &&
				d10 == u.Data4[6] &&
				d11 == u.Data4[7]);
		}

		static const uuid_base& get(){
			static uuid_base b = { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11 };
			return b;
		}

#ifdef _WIN32

		// Convenience function for Windows GUID

		template<class G>
		static bool compare_windows_guid(const G& g){
			static_assert(sizeof(G) == sizeof(uuid_base), "GUID and uuid_base have different sizes");
			return uuid::compare(*reinterpret_cast<const uuid_base*>(&g));
		}
		template<class G>
		static const G& get_windows_guid(){
			// GUID is same as uuid_base
			static_assert(sizeof(G) == sizeof(uuid_base), "GUID and uuid_base have different sizes");
			return *reinterpret_cast<const G*>(&uuid::get());
		}

#endif

	};

}
#endif