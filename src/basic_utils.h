/**
 *    @file  basic_utils.h
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


#ifndef  GUM_BASIC_UTILS_H__
#define  GUM_BASIC_UTILS_H__

#include <iterator>

namespace gum {
  namespace utils {
    /**
     *  @brief  Check whether a string ends with another string.
     *
     *  @param  str The first string.
     *  @param  suf The second string.
     *  @return `true` if `suf` is a suffix of `str`; otherwise `false`.
     *
     *  NOTE: The string type should have `length` method and support reversed iterator.
     */
    template< typename TText >
        inline bool
      ends_with( TText const& str, TText const& suf )
      {
        if ( suf.length() <= str.length() &&
            std::equal( std::rbegin( suf ), std::rend( suf ), std::rbegin( str ) ) ) {
          return true;
        }
        return false;
      }  /* -----  end of template function ends_with  ----- */

    /**
     *  @brief  Check whether a string starts with another string.
     *
     *  @param  str The first string.
     *  @param  pre The second string.
     *  @return `true` if `pre` is a prefix of `str`; otherwise `false`.
     *
     *  It checks the first string whether the second one is one of its prefixes or not.
     */
    template< typename TText >
        inline bool
      starts_with( TText const& str, TText const& pre )
      {
        if ( pre.length() <= str.length() &&
            std::equal( std::begin( pre ), std::end( pre ), std::begin( str ) ) ) {
          return true;
        }
        return false;
      }  /* -----  end of template function starts_with  ----- */
  }  /* -----  end of namespace utils  ----- */
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_BASIC_UTILS_H__  ----- */
