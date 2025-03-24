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

#include <cstdint>
#include <algorithm>
#include <istream>
#include <functional>


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

  /**
   *  @brief  Meta-function to get the width an integer type.
   */
  template< typename TInteger >
  struct widthof;

  template< >
  struct widthof< int8_t > {
    constexpr static uint8_t value = 8;
  };

  template< >
  struct widthof< uint8_t > {
    constexpr static uint8_t value = 8;
  };

  template< >
  struct widthof< int16_t > {
    constexpr static uint8_t value = 16;
  };

  template< >
  struct widthof< uint16_t > {
    constexpr static uint8_t value = 16;
  };

  template< >
  struct widthof< int32_t > {
    constexpr static uint8_t value = 32;
  };

  template< >
  struct widthof< uint32_t > {
    constexpr static uint8_t value = 32;
  };

  template< >
  struct widthof< int64_t > {
    constexpr static uint8_t value = 64;
  };

  template< >
  struct widthof< uint64_t > {
    constexpr static uint8_t value = 64;
  };

  template< typename T, typename TEnable=void >
  struct add_const_east;

  template< typename T >
  struct add_const_east< T, std::enable_if_t< !std::is_lvalue_reference< T >::value > > {
    using type = std::add_const_t< T >;
  };

  template< typename T >
  struct add_const_east< T, std::enable_if_t< std::is_lvalue_reference< T >::value > > {
    private:
      using const_t = std::add_const_t< std::remove_reference_t< T > >;
    public:
      using type = std::add_lvalue_reference_t< const_t >;
  };

  template< typename T >
  using add_const_east_t = typename add_const_east< T >::type;

  /**
    *  @brief  A native date type wrapping a lambda useful for function overloading.
    */
  template< typename TReturn, typename ...TArgs >
  class CallbackWrapper {
    public:
      /* === TYPEDEFS === */
      using return_type = TReturn;
      using function_type = std::function< return_type( TArgs... ) >;
      /* === LIFECYCLE === */
      CallbackWrapper( ) = default;
      CallbackWrapper( function_type x_callback ) : m_f( x_callback ) { }
      /* === OPERATORS === */
      inline return_type
      operator()( TArgs&&... args )
      {
        return this->m_f( std::forward< TArgs >( args )... );
      }

      inline
      operator bool() const noexcept
      {
        return ( bool )( this->m_f );
      }
    private:
      function_type m_f;
  };

  template< typename TReturn >
  class ExternalLoader
    : public CallbackWrapper< TReturn, std::istream& > {
    public:
      /* === TYPEDEFS === */
      using return_type = TReturn;
      using base_type = CallbackWrapper< return_type, std::istream& >;
      using function_type = typename base_type::function_type;
      /* === LIFECYCLE === */
      using base_type::base_type;  // inherit constructors
  };
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_BASIC_TYPES_HPP__ --- */
