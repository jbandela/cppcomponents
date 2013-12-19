#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_CLONABLE_HPP_12_18_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_CLONABLE_HPP_12_18_2013_
#include "cppcomponents.hpp"

namespace cppcomponents{

  struct IClonable :define_interface<cppcomponents::uuid<0x123ab045, 0xbded, 0x46a1, 0xa419, 0x1fd5e1d80df9>>
  {
    use<InterfaceUnknown>  Clone();

    CPPCOMPONENTS_CONSTRUCT(IClonable, Clone)
  };

}
#endif