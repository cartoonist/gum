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
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_BASIC_UTILS_HPP__ --- */
