/**
 *    @file  basic_types.h
 *   @brief  Basic type definitions.
 *
 *  This header file defines some basic types.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Wed Mar 06, 2019  13:16
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_BASIC_TYPES_H__
#define  GUM_BASIC_TYPES_H__

#include <cinttypes>

namespace gum {
  /**
   *  @brief  Wrap signed integer type of width TWidth.
   */
  template< uint8_t TWidth >
    struct integer;

  /**
   *  @brief  Wrap unsigned integer type of width TWidth.
   */
  template< uint8_t TWidth >
    struct uinteger;

  template< >
    struct integer< 8 > {
      using type = int8_t;
    };

  template< >
    struct uinteger< 8 > {
      using type = uint8_t;
    };

  template< >
    struct integer< 16 > {
      using type = int16_t;
    };

  template< >
    struct uinteger< 16 > {
      using type = uint16_t;
    };

  template< >
    struct integer< 32 > {
      using type = int32_t;
    };

  template< >
    struct uinteger< 32 > {
      using type = uint32_t;
    };

  template< >
    struct integer< 64 > {
      using type = int64_t;
    };

  template< >
    struct uinteger< 64 > {
      using type = uint64_t;
    };

  template< uint8_t TWidth >
    using integer_t = typename integer< TWidth >::type;

  template< uint8_t TWidth >
    using uinteger_t = typename uinteger< TWidth >::type;
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_BASIC_TYPES_H__  ----- */
