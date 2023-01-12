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

#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <vector>

#include "basic_types.hpp"


namespace gum {
  namespace util {
    /**
     *  @brief  Check if the given file exists and is readable.
     *
     *  @param  file_name The name of the file to be checked.
     *  @return `true` if exists and is readable; otherwise `false`.
     *
     *  Use `std::ifstream` to check for readability of the given file because of
     *  portability.
     */
    inline bool
    readable( const std::string& file_name )
    {
      std::ifstream ifs( file_name );
      return ifs.good();
    }

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
    template< typename TBitVector, uint8_t WLEN=64 >
    inline void
    bv_icopy( TBitVector const& src,
              TBitVector& dst,
              typename TBitVector::size_type idx=0,
              typename TBitVector::size_type len=0 )
    {
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
     *  @brief  Set all bits in [idx...idx+len) to zero.
     *
     *  @param  bv The bit vector.
     *  @param  idx The start of the range.
     *  @param  len The length of the range.
     */
    template< typename TBitVector, uint8_t WLEN=64 >
    inline void
    bv_izero( TBitVector& bv,
              typename TBitVector::size_type idx=0,
              typename TBitVector::size_type len=0 )
    {
      assert( idx < bv.size() );
      if ( len == 0 ) len = bv.size();
      if ( len + idx > bv.size() ) len = bv.size() - idx;
      auto i = idx + WLEN;
      for ( ; i < idx + len /* && i < bv.size() */; i += WLEN ) {
        bv.set_int( i - WLEN, 0, WLEN );
      }
      i -= WLEN;
      for ( ; i < idx + len; ++i ) bv[ i ] = 0;
    }

    /**
     *  @brief  Find the first occurrance of an integer in range [idx...idx+len) of a
     *          bit vector.
     *
     *  @param  bv The bit vector.
     *  @param  idx The start of the range.
     *  @param  len The length of the range.
     *  @param  key The search key.
     */
    template< typename TBitVector, uint8_t WLEN=64 >
    inline typename TBitVector::size_type
    bv_ifind( TBitVector const& bv,
              bool key=false,
              typename TBitVector::size_type idx=0,
              typename TBitVector::size_type len=0 )
    {
      using uint_type = uinteger_t< WLEN >;

      assert( idx < bv.size() );
      if ( len == 0 ) len = bv.size();
      if ( len + idx > bv.size() ) len = bv.size() - idx;
      uint_type int_key = key ? 0 : std::numeric_limits< uint_type >::max();

      auto i = idx + WLEN;
      for ( ; i < idx + len /* && i < src.size() */; i += WLEN ) {
        if ( bv.get_int( i - WLEN, WLEN ) != int_key ) break;
      }
      i -= WLEN;
      for ( ; i < idx + len; ++i ) {
        if ( bool( bv[ i ] ) == key ) break;
      }
      return i;
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

    // Sort zipped containers: https://stackoverflow.com/a/17074810/357257
    template< typename TContainer, typename TCompare >
    inline std::vector< std::size_t >
    sort_permutation( TContainer const& container, TCompare compare )
    {
      std::vector< std::size_t > perm( container.size() );
      std::iota( perm.begin(), perm.end(), 0 );
      std::sort( perm.begin(), perm.end(),
                 [&compare, &container]( std::size_t i, std::size_t j )
                 {
                   return compare( container[ i ], container[ j ] );
                 } );
      return perm;
    }

    template< typename TContainer >
    inline std::vector< std::size_t >
    sort_permutation( TContainer const& container )
    {
      auto compare =
          []( typename TContainer::value_type const& a,
              typename TContainer::value_type const& b )
          { return a < b; };
      return sort_permutation( container, compare );
    }

    template< typename TContainer >
    inline TContainer
    permutated( std::vector< std::size_t > const& perm, TContainer& container )
    {
      assert( container.size() == perm.size() );

      TContainer applied;
      applied.resize( container.size() );
      std::transform( perm.begin(), perm.end(), applied.begin(),
                      [&container]( std::size_t i ) { return container[ i ]; } );
      return applied;
    }

    template< typename TContainer >
    inline void
    permute( std::vector< std::size_t > const& perm, TContainer& container )
    {
      assert( container.size() == perm.size() );

      std::vector< bool > done( container.size(), false );
      for ( std::size_t i = 0; i < container.size(); ++i ) {
        if ( done[ i ] ) continue;
        done[ i ] = true;
        auto prev_j = i;
        auto j = perm[ i ];
        while ( j != i ) {
          std::swap( container[ prev_j ], container[ j ] );
          done[ j ] = true;
          prev_j = j;
          j = perm[ j ];
        }
      }
    }

    template< typename TFirst, typename ...TContainers >
    inline void
    permute( std::vector< std::size_t > const& perm, TFirst& first, TContainers&&... rest )
    {
      permute( perm, first );
      permute( perm, std::forward< TContainers >( rest )... );
    }

    template< typename TCompare, typename TFirst, typename ...TContainers >
    inline void
    sort_zip_c( TCompare compare, TFirst& first, TContainers&&... rest )
    {
      auto perm = sort_permutation( first, compare );
      permute( perm, first, std::forward< TContainers >( rest )... );
    }

    template< typename TFirst, typename ...TContainers >
    inline void
    sort_zip( TFirst& first, TContainers&&... rest )
    {
      sort_zip_c( []( auto const& a, auto const& b ) { return a < b; }, first,
                  std::forward< TContainers >( rest )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_BASIC_UTILS_HPP__ --- */
