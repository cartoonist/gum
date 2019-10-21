/**
 *    @file  test_alphabet.cpp
 *   @brief  Test cases for `alphabet` module.
 *
 *  This source file includes test scenarios for `alphabet` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sun Oct 20, 2019  12:35
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <algorithm>

#include "alphabet.hpp"

#include "test_base.hpp"


SCENARIO( "Symbol to compressed value conversion and vice versa", "[alphabet]" )
{
  GIVEN( "Any DNA symbols" )
  {
    using alphabet_type = gum::DNA;
    using char_type = typename alphabet_type::char_type;
    using value_type = typename alphabet_type::value_type;

    std::vector< char_type > symbols = { 'A', 'C', 'G', 'T' };
    std::vector< value_type > truth = { 0, 1, 2, 3 };
    WHEN( "They are converted to compressed values" )
    {
      std::vector< value_type > values;
      std::transform( symbols.begin(), symbols.end(), std::back_inserter( values ),
                      alphabet_type::char2comp );
      std::sort( values.begin(), values.end() );
      THEN( "Their values should be equal to their ranks in the alphabet set" )
      {
        REQUIRE( values.size() == truth.size() );
        REQUIRE( std::equal( values.begin(), values.end(), truth.begin() ) );
      }
    }
  }

  GIVEN( "Any DNA compressed value" )
  {
    using alphabet_type = gum::DNA;
    using char_type = typename alphabet_type::char_type;
    using value_type = typename alphabet_type::value_type;

    std::vector< value_type > values = { 0, 1, 2, 3 };
    std::vector< char_type > truth = { 'A', 'C', 'G', 'T' };
    WHEN( "They are converted to symbols" )
    {
      std::vector< char_type > symbols;
      std::transform( values.begin(), values.end(), std::back_inserter( symbols ),
                      alphabet_type::comp2char );
      std::sort( symbols.begin(), symbols.end() );
      THEN( "They should be translated to their corresponding symbols" )
      {
        REQUIRE( symbols.size() == truth.size() );
        REQUIRE( std::equal( symbols.begin(), symbols.end(), truth.begin() ) );
      }
    }
  }

  GIVEN( "Any DNA5 symbols" )
  {
    using alphabet_type = gum::DNA5;
    using char_type = typename alphabet_type::char_type;
    using value_type = typename alphabet_type::value_type;

    std::vector< char_type > symbols = { 'A', 'C', 'G', 'N', 'T' };
    std::vector< value_type > truth = { 0, 1, 2, 3, 4 };
    WHEN( "They are converted to compressed values" )
    {
      std::vector< value_type > values;
      std::transform( symbols.begin(), symbols.end(), std::back_inserter( values ),
                      alphabet_type::char2comp );
      std::sort( values.begin(), values.end() );
      THEN( "Their values should be equal to their ranks in the alphabet set" )
      {
        REQUIRE( values.size() == truth.size() );
        REQUIRE( std::equal( values.begin(), values.end(), truth.begin() ) );
      }
    }
  }

  GIVEN( "Any DNA5 compressed value" )
  {
    using alphabet_type = gum::DNA5;
    using char_type = typename alphabet_type::char_type;
    using value_type = typename alphabet_type::value_type;

    std::vector< value_type > values = { 0, 1, 2, 3, 4 };
    std::vector< char_type > truth = { 'A', 'C', 'G', 'N', 'T' };
    WHEN( "They are converted to symbols" )
    {
      std::vector< char_type > symbols;
      std::transform( values.begin(), values.end(), std::back_inserter( symbols ),
                      alphabet_type::comp2char );
      std::sort( symbols.begin(), symbols.end() );
      THEN( "They should be translated to their corresponding symbols" )
      {
        REQUIRE( symbols.size() == truth.size() );
        REQUIRE( std::equal( symbols.begin(), symbols.end(), truth.begin() ) );
      }
    }
  }
}
