/**
 *    @file  alphabet.hpp
 *   @brief  Alphabet class
 *
 *  This header file defines alphabets.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Oct 20, 2019  01:16
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */
#ifndef GUM_ALPHABET_HPP__
#define GUM_ALPHABET_HPP__

#include <cassert>

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>

#include "basic_types.hpp"


namespace gum {
  template< typename TAlphabet1, typename TAlphabet2 >
  struct is_superset {
    constexpr static bool value = false;
    /* // Requires C++20
     * constexpr static bool value = std::equal(
     *  TAlphabet1::alphabets.begin(), TAlphabet1::alphabets.end(),
     *  TAlphabet2::alphabets.begin(),
     *  []( auto const& l, auto const& r ) {
     *    return l >= r;
     *  } );
     */
  };

  template< typename TAlphabet >
  struct is_superset< TAlphabet, TAlphabet > {
    constexpr static bool value = true;
  };

  template< uint8_t TWidth >
  class Alphabet {
  public:
    static_assert( TWidth <= 64, "width cannot be larger than 64 bits" );
    /* === DATA MEMBERS === */
    constexpr static uint8_t width = TWidth;

    /* === TYPEDEFS === */
    using value_type = unsigned char;
    using char_type = char;

    /* === METHODS === */
    constexpr static uint8_t
    get_width( )
    {
      return Alphabet::width;
    }
  };  /* --- end of template class Alphabet --- */

  class DNA : public Alphabet< 2 > {
  public:
    template< typename TAlphabet1, typename TAlphabet2 >
    friend struct is_superset;

    using base_type = Alphabet< 2 >;
    using typename base_type::value_type;
    using typename base_type::char_type;
    using bv_type = sdsl::bit_vector;
    using rs_type = typename bv_type::rank_1_type;
    using ss_type = typename bv_type::select_1_type;
  private:
    inline static const bv_type alphabets =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x0f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x1f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x2f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x3f
      0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x4f
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x5f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x6f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x7f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x8f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x9f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xaf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xbf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xcf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xdf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xef
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0xff
    inline static const ss_type select = ss_type( &DNA::alphabets );
  public:
    inline static const rs_type char2comp = rs_type( &DNA::alphabets );

    inline static char_type
    comp2char( value_type v )
    {
      return DNA::select( v + 1 );
    }

    inline static value_type
    complement( value_type v )
    {
      switch( v ) {
        case 0: return 3;
        case 1: return 2;
        case 2: return 1;
        case 3: return 0;
        default: assert( false );
      }
      return v;
    }
  };  /* --- end of class DNA --- */

  class DNA5 : public Alphabet< 3 > {
  public:
    template< typename TAlphabet1, typename TAlphabet2 >
    friend struct is_superset;

    using base_type = Alphabet< 3 >;
    using typename base_type::value_type;
    using typename base_type::char_type;
    using bv_type = sdsl::bit_vector;
    using rs_type = typename bv_type::rank_1_type;
    using ss_type = typename bv_type::select_1_type;
  private:
    inline static const bv_type alphabets =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x0f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x1f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x2f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x3f
      0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,    // 0x4f
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x5f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x6f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x7f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x8f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0x9f
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xaf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xbf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xcf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xdf
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // 0xef
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0xff
    inline static const ss_type select = ss_type( &DNA5::alphabets );
  public:
    inline static const rs_type char2comp = rs_type( &DNA5::alphabets );

    inline static char_type
    comp2char( value_type v )
    {
      return DNA5::select( v + 1 );
    }

    inline static value_type
    complement( value_type v )
    {
      switch( v ) {
        case 0: return 3;
        case 1: return 2;
        case 2: return 1;
        case 3: return 0;
        case 4: return 4;
        default: assert( false );
      }
      return v;
    }
  };  /* --- end of class DNA5 --- */

  template<>
  struct is_superset< DNA5, DNA > {
    constexpr static bool value = true;
  };

  class Char : public Alphabet< 8 > {
  public:
    using base_type = Alphabet< 8 >;
    using typename base_type::value_type;
    using typename base_type::char_type;

    inline static value_type
    char2comp( char_type c )
    {
      return static_cast< value_type >( c );
    }

    inline static char_type
    comp2char( value_type v )
    {
      return static_cast< char_type >( v );
    }

    inline static value_type
    complement( value_type v )
    {
      switch ( v ) {
        case 'A': return 'T';
        case 'C': return 'G';
        case 'G': return 'C';
        case 'T': return 'A';
        case 'N': return 'N';
        default: assert( false );
      }
      return v;
    }
  };  /* --- end of class Char --- */

  template< typename TAlphabet >
  struct is_superset< Char, TAlphabet > {
    constexpr static bool value = true;
  };
}  /* --- end of namespace gum --- */

#endif /* --- ifndef GUM_ALPHABET_HPP__ --- */
