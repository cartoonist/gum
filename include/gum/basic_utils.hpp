/**
 *    @file  basic_utils.hpp
 *   @brief  Basic utility data types and functions.
 *
 *  This header file includes auxiliary data types and utility functions.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Fri Aug 02, 2019  14:20
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_BASIC_UTILS_HPP__
#define  GUM_BASIC_UTILS_HPP__

#include <iterator>


namespace gum {
  namespace util {
    /**
     *  @brief  Check whether a string ends with another string.
     *
     *  @param  str The first string.
     *  @param  suf The second string.
     *  @return `true` if `suf` is a suffix of `str`; otherwise `false`.
     *
     *  NOTE: The string type should have `size` method and support reversed iterator.
     */
    template< typename TText >
    inline bool
    ends_with( TText const& str, TText const& suf )
    {
      if ( suf.size() <= str.size() &&
           std::equal( std::rbegin( suf ), std::rend( suf ), std::rbegin( str ) ) ) {
        return true;
      }
      return false;
    }

    /**
     *  @brief  Check whether a string starts with another string.
     *
     *  @param  str The first string.
     *  @param  pre The second string.
     *  @return `true` if `pre` is a prefix of `str`; otherwise `false`.
     *
     *  NOTE: The string type should have `size` method and support forward iterator.
     */
    template< typename TText >
    inline bool
    starts_with( TText const& str, TText const& pre )
    {
      if ( pre.size() <= str.size() &&
           std::equal( std::begin( pre ), std::end( pre ), std::begin( str ) ) ) {
        return true;
      }
      return false;
    }

    /**
     *  @brief  Copy all bits in [idx...idx+len) to the same range in the destination.
     *
     *  @param  src The source bit vector.
     *  @param  dst The destination bit vector.
     *  @param  idx The start index in `src` to copy to the "identical" index in `dst`.
     *  @param  len The length of the range that should be copied.
     *
     *  Bitvector identical-range copy. The [idx...idx+len) from `src` is copied
     *  to the same range in `dst`.
     */
    template< typename TBitVector >
    inline void
    bv_icopy( TBitVector const& src,
              TBitVector& dst,
              typename TBitVector::size_type idx=0,
              typename TBitVector::size_type len=0 )
    {
      static const short int WLEN = 64;

      assert( idx < src.size() );
      assert( dst.size() >= src.size() );

      if ( len == 0 ) len = src.size();
      if ( len + idx > src.size() ) len = src.size() - idx;

      auto i = idx + WLEN;
      for ( ; i < idx + len /* && i < src.size() */; i += WLEN ) {
        dst.set_int( i - WLEN, src.get_int( i - WLEN, WLEN ), WLEN );
      }
      i -= WLEN;
      for ( ; i < idx + len; ++i ) dst[ i ] = src[ i ];
    }

    /**
     *  @brief  Round up the given number to the closest power of 2.
     *
     *  @param  x The input integer
     *  @return The closest power of 2.
     *
     *  XXX: If input is greater than 2^31, it returns zero.
     */
    inline uint32_t
    roundup32( uint32_t x )
    {
      if ( x == 0 ) return 1;

      --x;
      x |= x >> 1;
      x |= x >> 2;
      x |= x >> 4;
      x |= x >> 8;
      x |= x >> 16;
      ++x;
      return x;
    }

    /**
     *  @brief  Round up the given number to the closest power of 2.
     *
     *  @param  x The input integer
     *  @return The closest power of 2.
     *
     *  XXX: If input is greater than 2^63, it returns zero.
     */
    inline uint64_t
    roundup64( uint64_t x )
    {
      if ( x == 0 ) return 1;

      --x;
      x |= x >> 1;
      x |= x >> 2;
      x |= x >> 4;
      x |= x >> 8;
      x |= x >> 16;
      x |= x >> 32;
      ++x;
      return x;
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_BASIC_UTILS_HPP__ --- */
