/**
 *    @file  test_utils.cpp
 *   @brief  Test cases for `utils` module.
 *
 *  This source file includes test scenarios for `utils` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sun Aug 04, 2019  23:01
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include "catch2/catch.hpp"

#include "utils.hpp"


using namespace gum;

TEMPLATE_TEST_CASE( "Prefix and suffix check of a string", "[utils][template]",
    std::string, ( std::vector< char > ) )
{
  GIVEN( "A non-empty string" )
  {
    TestType s( { 'f', 'a', 'b', 'r', 'i', 'c', 'a', 't', 'e' } );
    WHEN( "Check for prefix with different strings" )
    {
      TestType pre1( { 'f', 'a', 'b' } );
      TestType pre2( { 'f', 'a', 'b', 'r', 'i', 'c' } );
      TestType pre3( { 'f', 'a', 'c', 't' } );
      TestType pre4( { 'f', 'a', 'b', 'r', 'i', 'c', 'a', 't', 'e', 's' } );
      TestType pre5;
      THEN( "It should verify the prefixes" )
      {
        REQUIRE( util::starts_with( s, s ) );
        REQUIRE( util::starts_with( s, pre1 ) );
        REQUIRE( util::starts_with( s, pre2 ) );
        REQUIRE( !util::starts_with( s, pre3 ) );
        REQUIRE( !util::starts_with( s, pre4 ) );
        REQUIRE( util::starts_with( s, pre5 ) );
      }
    }

    WHEN( "Check for suffix with different strings" )
    {
      TestType suf1( { 'a', 't', 'e' } );
      TestType suf2( { 'b', 'r', 'i', 'c', 'a', 't', 'e' } );
      TestType suf3( { 'f', 'a', 't', 'e' } );
      TestType suf4( { 'f', 'a', 'b', 'r', 'i', 'c', 'a', 't', 'e', 's' } );
      TestType suf5;
      THEN( "It should verify the suffixes" )
      {
        REQUIRE( util::ends_with( s, s ) );
        REQUIRE( util::ends_with( s, suf1 ) );
        REQUIRE( util::ends_with( s, suf2 ) );
        REQUIRE( !util::ends_with( s, suf3 ) );
        REQUIRE( !util::ends_with( s, suf4 ) );
        REQUIRE( util::ends_with( s, suf5 ) );
      }
    }
  }

  GIVEN( "An empty string" )
  {
    TestType s;
    WHEN( "Check for prefix with different strings" )
    {
      TestType pre1( { 'f', 'a', 'b' } );
      TestType pre2( { 'f', 'a', 'b', 'r', 'i', 'c' } );
      TestType pre3( { 'f', 'a', 'c', 't' } );
      TestType pre4( { 'f', 'a', 'b', 'r', 'i', 'c', 'a', 't', 'e', 's' } );
      TestType pre5;
      THEN( "It should verify the prefixes" )
      {
        REQUIRE( util::starts_with( s, s ) );
        REQUIRE( !util::starts_with( s, pre1 ) );
        REQUIRE( !util::starts_with( s, pre2 ) );
        REQUIRE( !util::starts_with( s, pre3 ) );
        REQUIRE( !util::starts_with( s, pre4 ) );
        REQUIRE( util::starts_with( s, pre5 ) );
      }
    }

    WHEN( "Check for suffix with different strings" )
    {
      TestType suf1( { 'a', 't', 'e' } );
      TestType suf2( { 'b', 'r', 'i', 'c', 'a', 't', 'e' } );
      TestType suf3( { 'f', 'a', 't', 'e' } );
      TestType suf4( { 'f', 'a', 'b', 'r', 'i', 'c', 'a', 't', 'e', 's' } );
      TestType suf5;
      THEN( "It should verify the suffixes" )
      {
        REQUIRE( util::ends_with( s, s ) );
        REQUIRE( !util::ends_with( s, suf1 ) );
        REQUIRE( !util::ends_with( s, suf2 ) );
        REQUIRE( !util::ends_with( s, suf3 ) );
        REQUIRE( !util::ends_with( s, suf4 ) );
        REQUIRE( util::ends_with( s, suf5 ) );
      }
    }
  }
}
