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

#include <gum/stringset.hpp>

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
  using stdstringset_type = std::vector< std::string >;

  auto get_stdstrset =
      []( ) {
        stdstringset_type retval;
        if ( std::is_same< alphabet_type, gum::DNA >::value ) {
          retval.push_back( "ATTTCTAGCGCTAGCTATTACAACACACGAGATATA" );
          retval.push_back( "CGCATGCTGATTACGCGAGGGGTACGCGATA" );
          retval.push_back( "TTTTTTTTTTCCCCCGGAG" );
          retval.push_back( "" );
          retval.push_back( "GGGGAGGGATCAGTAAGAGAGAGATAGATCCC" );
          retval.push_back( "GGC" );
        }
        else if ( std::is_same< alphabet_type, gum::DNA5 >::value ) {
          retval.push_back( "NTNNNNNNNGCTAGNTATTACNACANNCGAGATATA" );
          retval.push_back( "CGCANGCTGATTACGCGAGGNGTANGCGATA" );
          retval.push_back( "TTTTTTTTTTCCCCCGGAG" );
          retval.push_back( "" );
          retval.push_back( "NGGGAGGGATCAGTAAGAGAGANATAGATCCC" );
          retval.push_back( "NNN" );
        }
        return retval;
      };

  stdstringset_type stdstrset;

  auto basic_test =
      []( stringset_type& strset, stdstringset_type& stdstrset ) {
        using size_type = typename stringset_type::size_type;

        REQUIRE( !strset.empty() );
        REQUIRE( strset.size() == stdstrset.size() );
        size_type iter_diff = strset.end() - strset.begin();
        REQUIRE( iter_diff == stdstrset.size() );
        auto begin_stdstrset = stdstrset.begin();
        for ( auto str : strset ) {
          assert( begin_stdstrset != stdstrset.end() );
          REQUIRE( str == *begin_stdstrset );
          ++begin_stdstrset;
        }
        for ( size_type i = 0; i < strset.size(); ++i ) {
          REQUIRE( strset[ i ] == stdstrset[ i ] );
          REQUIRE( strset.length( i ) == stdstrset[ i ].size() );
          REQUIRE( strset.at( i ) == stdstrset.at( i ) );
        }
        REQUIRE_THROWS( strset.at( strset.size() ) );
        REQUIRE_THROWS( strset.at( strset.size() + 1 ) );
        REQUIRE_THROWS( strset.at( -1 ) );
        REQUIRE( strset.front() == stdstrset.front() );
        REQUIRE( strset.back() == stdstrset.back() );
        if ( std::is_same< alphabet_type, gum::DNA >::value ) {
          REQUIRE( strset( 21, 5 ) == "AACAC" );
          REQUIRE( strset( 32, 18 ) == "TATAACGCATGCTGATTA" );
          REQUIRE( strset( 120, 4 ) == "CCAG" );
        }
        else if ( std::is_same< alphabet_type, gum::DNA5 >::value ) {
          REQUIRE( strset( 21, 5 ) == "NACAN" );
          REQUIRE( strset( 32, 18 ) == "TATAACGCANGCTGATTA" );
          REQUIRE( strset( 120, 4 ) == "CCAN" );
        }
      };

  auto basic_test_empty =
      []( stringset_type& strset ){
        REQUIRE( strset.empty() );
        REQUIRE( strset.size() == 0 );
        REQUIRE( strset.length_sum() == 0 );
        REQUIRE( strset.begin() == strset.end() );
        REQUIRE_THROWS( strset.at( 0 ) );
        REQUIRE_THROWS( strset.at( 1 ) );
        REQUIRE_THROWS( strset.at( -1 ) );
      };

  GIVEN( "A set of standard strings in the given alphabet" )
  {
    stdstrset = get_stdstrset( );
    WHEN( "StringSet is constructed by passing the iterator" )
    {
      stringset_type strset( stdstrset.begin(), stdstrset.end() );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed after calling default constructor" )
    {
      stringset_type strset;
      strset.extend( stdstrset.begin(), stdstrset.end() );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed by pushing back each element passed by value" )
    {
      stringset_type strset;
      for ( auto const& str : stdstrset ) strset.push_back( str );
      strset.shrink_to_fit();
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed by pushing back each element passed by rvalue reference" )
    {
      stringset_type strset;
      for ( auto str : stdstrset ) strset.push_back( std::move( str ) );
      strset.shrink_to_fit();
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is extended by another set of strings" )
    {
      stringset_type strset;
      strset.extend( stdstrset.begin(), stdstrset.end() );
      strset.extend( stdstrset.begin(), stdstrset.end() );
      auto old_size = stdstrset.size();
      stdstrset.resize( old_size * 2 );
      std::copy_n( stdstrset.begin(), old_size, stdstrset.begin() + old_size );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 242 );
      }
    }

    WHEN( "An empty StringSet is extended by an empty set of strings" )
    {
      stdstringset_type emptystrset;
      stringset_type strset;
      strset.extend( stdstrset.begin(), stdstrset.end() );
      strset.extend( emptystrset.begin(), emptystrset.end() );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
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
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed by using move constructor" )
    {
      stringset_type yastrset( stdstrset.begin(), stdstrset.end() );
      stringset_type strset( std::move( yastrset ) );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed by using copy assignment operator" )
    {
      stringset_type yastrset( stdstrset.begin(), stdstrset.end() );
      stringset_type strset;
      strset = yastrset;
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is constructed by using move assignment operator" )
    {
      stringset_type yastrset( stdstrset.begin(), stdstrset.end() );
      stringset_type strset;
      strset = std::move( yastrset );
      THEN( "It should pass basic test cases" )
      {
        basic_test( strset, stdstrset );
        REQUIRE( strset.length_sum() == 121 );
      }
    }

    WHEN( "StringSet is cleared" )
    {
      stringset_type strset( stdstrset.begin(), stdstrset.end() );
      THEN( "It should be an empty string set" )
      {
        strset.clear();
        basic_test_empty( strset );
      }
    }
  }

  GIVEN( "An empty string set" )
  {
    stdstringset_type emptystrset;
    WHEN( "StringSet is constructed from an empty set of strings" )
    {
      stringset_type strset( emptystrset.begin(), emptystrset.end() );
      THEN( "It should be an empty string set" )
      {
        basic_test_empty( strset );
      }
    }

    WHEN( "An empty StringSet is extended by an empty set of strings" )
    {
      stringset_type strset;
      strset.extend( emptystrset.begin(), emptystrset.end() );
      THEN( "It should be an empty string set" )
      {
        basic_test_empty( strset );
      }
    }
  }
}
