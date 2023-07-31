/**
 *    @file  test_types.cpp
 *   @brief  Test cases for `basic_types` module.
 *
 *  This source file includes test scenarios for `basic_types` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Mon Jul 24, 2023  15:09
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */


#include <gum/basic_types.hpp>
#include <type_traits>

#include "test_base.hpp"


using namespace gum;

TEMPLATE_SCENARIO_SIG( "Determine the bitwise width of an integer", "[types]",
                       ( ( typename T /* dummy type */, int W ), T, W ),
                       ( int, 8 ),
                       ( int, 16 ),
                       ( int, 32 ),
                       ( int, 64 ) )
{
  GIVEN ( "The integer width in bits" ) {
    int width = W;

    WHEN ( "Using integer<> meta functions" ) {
      THEN ( "It gets the corresponding integer types" ) {
        REQUIRE( gum::widthof< typename gum::integer< W >::type >::value == width );
        REQUIRE( gum::widthof< typename gum::uinteger< W >::type >::value == width );
        REQUIRE( gum::widthof< typename gum::integer_t< W > >::value == width );
        REQUIRE( gum::widthof< typename gum::uinteger_t< W > >::value == width );
        REQUIRE( !std::is_signed< typename gum::uinteger< W >::type >::value );
        REQUIRE( std::is_signed< typename gum::integer< W >::type >::value );
      }
    }
  }
}
