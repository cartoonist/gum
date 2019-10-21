/**
 *    @file  test_stringset.cpp
 *   @brief  Test cases for `stringset` module.
 *
 *  This source file includes test scenarios for `stringset` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sun Oct 20, 2019  13:39
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <string>
#include <vector>

#include "stringset.hpp"
#include "test_base.hpp"


TEMPLATE_SCENARIO( "Conversion between standard and compressed string for sequence representation",
                   "[stringset]",
                   ( gum::DNA ),
                   ( gum::DNA5 ) )
{
  using alphabet_type = TestType;

  auto get_seq =
      []( ) {
        if ( std::is_same< alphabet_type, gum::DNA >::value )
          return "CACGTTATTAGGTGCTTGGCTAGCTGATC";
        if ( std::is_same< alphabet_type, gum::DNA5 >::value )
          return "NACGTTNNTANGTGNNNNNCTTGGCTAGCTNATC";
      };

  GIVEN( "A character string of in the given alphabet" )
  {
    std::string seq = get_seq( );
    WHEN( "It is converted to compressed representation" )
    {
      gum::String< alphabet_type > compseq;
      gum::util::assign( compseq, seq, alphabet_type() );
      THEN( "Converting back to standard string should yield the same sequence" )
      {
        std::string reseq;
        gum::util::assign( reseq, compseq, alphabet_type() );
        REQUIRE( reseq == seq );
      }
    }
  }
}

TEMPLATE_SCENARIO( "StringSet basic functionalities", "[stringset]",
                   ( gum::DNA ),
                   ( gum::DNA5 ) )
{
  using alphabet_type = TestType;
  using stringset_type = gum::StringSet< alphabet_type >;

  auto get_stdstrset =
      []( ) {
        std::vector< std::string > retval;
        if ( std::is_same< alphabet_type, gum::DNA >::value ) {
          retval.push_back( "ATTTCTAGCGCTAGCTATTACAACACACGAGATATA" );
          retval.push_back( "CGCATGCTGATTACGCGAGGGGTACGCGATA" );
          retval.push_back( "TTTTTTTTTTCCCCCGGAG" );
          retval.push_back( "GGGGAGGGATCAGTAAGAGAGAGATAGATCCC" );
          retval.push_back( "GGC" );
        }
        else if ( std::is_same< alphabet_type, gum::DNA5 >::value ) {
          retval.push_back( "NTNNNNNNNGCTAGNTATTACNACANNCGAGATATA" );
          retval.push_back( "CGCANGCTGATTACGCGAGGNGTANGCGATA" );
          retval.push_back( "TTTTTTTTTTCCCCCGGAG" );
          retval.push_back( "NGGGAGGGATCAGTAAGAGAGANATAGATCCC" );
          retval.push_back( "NNN" );
        }
        return retval;
      };

  std::vector< std::string > stdstrset;

  auto basic_test =
      []( stringset_type& strset, std::vector< std::string >& stdstrset ) {
        using size_type = typename stringset_type::size_type;

        REQUIRE( !strset.empty() );
        REQUIRE( strset.size() == stdstrset.size() );
        auto begin_stdstrset = stdstrset.begin();
        for ( auto str : strset ) {
          assert( begin_stdstrset != stdstrset.end() );
          REQUIRE( str == *begin_stdstrset );
          ++begin_stdstrset;
        }
        for ( size_type i = 0; i < strset.size(); ++i ) {
          REQUIRE( strset[ i ] == stdstrset[ i ] );
          REQUIRE( strset.at( i ) == stdstrset.at( i ) );
        }
        REQUIRE_THROWS( strset.at( strset.size() ) );
        REQUIRE( strset.front() == stdstrset.front() );
        REQUIRE( strset.back() == stdstrset.back() );
        if ( std::is_same< alphabet_type, gum::DNA >::value ) {
          REQUIRE( strset( 21, 5 ) == "AACAC" );
          REQUIRE( strset( 32, 18 ) == "TATACGCATGCTGATTAC" );
          REQUIRE( strset( 117, 4 ) == "CGGC" );
        }
        else if ( std::is_same< alphabet_type, gum::DNA5 >::value ) {
          REQUIRE( strset( 21, 5 ) == "NACAN" );
          REQUIRE( strset( 32, 18 ) == "TATACGCANGCTGATTAC" );
          REQUIRE( strset( 117, 4 ) == "CNNN" );
        }
      };

  GIVEN( "A set of standard strings in the given alphabet" )
  {
    stdstrset = get_stdstrset( );
    WHEN( "StringSet is constructed by passing the container" )
    {
      stringset_type strset( stdstrset );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed by passing the iterator" )
    {
      stringset_type strset( stdstrset.begin(), stdstrset.end() );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed after calling default constructor" )
    {
      stringset_type strset;
      strset.extend( stdstrset.begin(), stdstrset.end() );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is extended by another set of strings" )
    {
      stringset_type strset;
      strset.extend( stdstrset );
      strset.extend( stdstrset.begin(), stdstrset.end() );
      auto old_size = stdstrset.size();
      stdstrset.resize( old_size * 2 );
      std::copy_n( stdstrset.begin(), old_size, stdstrset.begin() + old_size );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed by using copy constructor" )
    {
      stringset_type yastrset;
      yastrset.extend( stdstrset.begin(), stdstrset.end() );
      stringset_type strset( yastrset );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed by using move constructor" )
    {
      stringset_type yastrset( stdstrset.begin(), stdstrset.end() );
      stringset_type strset( std::move( yastrset ) );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed by using copy assignment operator" )
    {
      stringset_type yastrset( stdstrset );
      stringset_type strset;
      strset = yastrset;
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }

    WHEN( "StringSet is constructed by using move assignment operator" )
    {
      stringset_type yastrset( stdstrset );
      stringset_type strset;
      strset = std::move( yastrset );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
      }
    }
  }
}
