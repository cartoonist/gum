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

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>

#include "basic_types.hpp"


namespace gum {
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
    using base_type = Alphabet< 2 >;
    using base_type::value_type;
    using base_type::char_type;
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
  };  /* --- end of class DNA --- */

  class DNA5 : public Alphabet< 3 > {
  public:
    using base_type = Alphabet< 3 >;
    using base_type::value_type;
    using base_type::char_type;
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
  };  /* --- end of class DNA5 --- */
}  /* --- end of namespace gum --- */

#endif /* --- ifndef GUM_ALPHABET_HPP__ --- */
