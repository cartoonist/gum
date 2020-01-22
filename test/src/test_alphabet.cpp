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

#include <gum/alphabet.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Symbol to compressed value conversion and vice versa", "[alphabet]",
                   ( gum::DNA ),
                   ( gum::DNA5 ),
                   ( gum::Char ) )
{
  using alphabet_type = TestType;
  using char_type = typename alphabet_type::char_type;
  using value_type = typename alphabet_type::value_type;

  auto get_symbols =
      []( ) {
        using return_type = std::vector< char_type >;
        if ( std::is_same< alphabet_type, gum::DNA >::value )
          return return_type( { 'A', 'C', 'G', 'T' } );
        if ( std::is_same< alphabet_type, gum::DNA5 >::value )
          return return_type( { 'A', 'C', 'G', 'N', 'T' } );
        if ( std::is_same< alphabet_type, gum::Char >::value )
          return return_type( { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' } );
      };

  auto get_values =
      []( ) {
        using return_type = std::vector< value_type >;
        if ( std::is_same< alphabet_type, gum::DNA >::value )
          return return_type( { 0, 1, 2, 3 } );
        if ( std::is_same< alphabet_type, gum::DNA5 >::value )
          return return_type( { 0, 1, 2, 3, 4 } );
        if ( std::is_same< alphabet_type, gum::Char >::value )
          return return_type( { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
                                78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90 } );
      };

  GIVEN( "Any symbols in the given alphabet" )
  {
    std::vector< char_type > symbols = get_symbols();
    std::vector< value_type > truth = get_values();
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

  GIVEN( "Any compressed value of the given alphabet" )
  {
    std::vector< value_type > values = get_values();
    std::vector< char_type > truth = get_symbols();
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
