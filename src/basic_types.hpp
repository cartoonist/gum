/**
 *    @file  basic_types.hpp
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

#ifndef  GUM_BASIC_TYPES_HPP__
#define  GUM_BASIC_TYPES_HPP__

#include <cinttypes>


namespace gum {
  /* Dynamic specialization tag. */
  struct Dynamic;
  /* Succinct specialization tag. */
  struct Succinct;

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

  /**
   *  @brief  Get the common type of `integer_t< T1 >` and `integer_t< T2 >`.
   *
   *  The type would be `integer_t< T1 >` if T1 >= T2; otherwise it would be
   *  `integer_t< T2 >`.
   */
  template< uint8_t T1, uint8_t T2 >
  struct common {
    constexpr static uint8_t value = std::max( T1, T2 );
    using type = integer_t< common::value >;

    constexpr uint8_t operator()( ) const noexcept
    {
      return common::value;
    }
  };

  /**
   *  @brief  Get the common type of `uinteger_t< T1 >` and `uinteger_t< T2 >`.
   *
   *  The type would be `uinteger_t< T1 >` if T1 >= T2; otherwise it would be
   *  `uinteger_t< T2 >`.
   */
  template< uint8_t T1, uint8_t T2 >
  struct ucommon {
    constexpr static uint8_t value = std::max( T1, T2 );
    using type = uinteger_t< ucommon::value >;

    constexpr uint8_t operator()( ) const noexcept
    {
      return ucommon::value;
    }
  };

  template< uint8_t T1, uint8_t T2 >
  using common_t = typename common< T1, T2 >::type;

  template< uint8_t T1, uint8_t T2 >
  using ucommon_t = typename ucommon< T1, T2 >::type;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_BASIC_TYPES_HPP__ --- */
