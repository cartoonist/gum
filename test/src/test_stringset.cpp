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

#include <algorithm>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

#include <gum/stringset.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Conversion between standard and compressed string for sequence representation",
                   "[stringset]",
                   ( gum::Char ),
                   ( gum::DNA ),
                   ( gum::DNA5 ) )
{
  using alphabet_type = TestType;

  auto get_seq =
      []( ) {
        if constexpr ( std::is_same< alphabet_type, gum::DNA >::value )
          return "CACGTTATTAGGTGCTTGGCTAGCTGATC";
        if constexpr ( std::is_same< alphabet_type, gum::DNA5 >::value )
          return "NACGTTNNTANGTGNNNNNCTTGGCTAGCTNATC";
        if constexpr ( std::is_same< alphabet_type, gum::Char >::value )
          return "The quick brown fox jumps over the lazy dog";
      };

  GIVEN( "A character string of the given alphabet" )
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

TEMPLATE_SCENARIO_SIG( "String and StringView conversions and assignments", "[stringset]",
                       ( ( typename T, typename U, int W /*dummy*/ ), T, U, W ),
                       ( gum::Char, gum::Char, 0 ),
                       ( gum::DNA5, gum::Char, 0 ),
                       ( gum::DNA5, gum::DNA5, 0 ),
                       ( gum::DNA, gum::Char, 0 ),
                       ( gum::DNA, gum::DNA5, 0 ),
                       ( gum::DNA, gum::DNA, 0 ) )
{
  using alphabet_1_type = T;
  using alphabet_2_type = U;

  auto get_decoding_map = []( auto alpha ) {
    using alphabet_type = decltype( alpha );
    using char_type = typename alphabet_type::char_type;

    std::vector< char_type > map;
    auto alpha_size = static_cast< std::size_t >(
        alphabet_type::char2comp( std::numeric_limits< char_type >::max() ) );
    map.resize( alpha_size );

    while ( alpha_size-- > 0 )
      map[ alpha_size ] = alphabet_type::comp2char( alpha_size );
    return map;
  };

  auto get_seq = []( auto alpha ) {
        if constexpr ( std::is_same< decltype( alpha ), gum::DNA >::value )
          return "CACGTTATTAGGTGCTTGGCTAGCTGATC";
        if constexpr ( std::is_same< decltype( alpha ), gum::DNA5 >::value )
          return "NACGTTNNTANGTGNNNNNCTTGGCTAGCTNATC";
        if constexpr ( std::is_same< decltype( alpha ), gum::Char >::value )
          return "The quick brown fox jumps over the lazy dog";
      };

  GIVEN( "A char string of the given alphabet" )
  {
    std::string seq = get_seq( alphabet_1_type{} );

    WHEN( "Encoded in the compressed alphabet using `encode` interface function" )
    {
      gum::String< alphabet_1_type > cseq;
      cseq.resize( seq.size() );
      gum::util::encode( seq.begin(), seq.end(), cseq.begin(), alphabet_1_type{} );

      THEN( "The resulted string should contain encoded values" )
      {
        auto map = get_decoding_map( alphabet_1_type{} );
        REQUIRE( cseq.size() != 0 );
        REQUIRE( cseq.size() == seq.size() );
        REQUIRE( std::equal( seq.begin(), seq.end(), cseq.begin(),
                             [&map]( auto const& l, auto const& r ) {
                               return l == map[ r ];
                             } ) );
      }

      AND_WHEN( "Encoded string is decoded back using `decode` interface function" )
      {
        std::string dseq;
        dseq.resize( cseq.size() );
        gum::util::decode( cseq.begin(), cseq.end(), dseq.begin(), alphabet_1_type{} );

        THEN( "The result should be identical to the original string" ) {
          REQUIRE( dseq.size() != 0 );
          REQUIRE( dseq.size() == seq.size() );
          REQUIRE( std::equal( dseq.begin(), dseq.end(), seq.begin() ) );
        }
      }
    }

    WHEN( "Encoded in the compressed alphabet using `convert` interface function" )
    {
      gum::String< alphabet_1_type > cseq;
      cseq.resize( seq.size() );
      gum::util::convert( seq.begin(), seq.end(), cseq.begin(), gum::Char{},
                          alphabet_1_type{} );

      THEN( "The resulted string should contain encoded values" )
      {
        auto map = get_decoding_map( alphabet_1_type{} );
        REQUIRE( cseq.size() != 0 );
        REQUIRE( cseq.size() == seq.size() );
        REQUIRE( std::equal( seq.begin(), seq.end(), cseq.begin(),
                             [&map]( auto const& l, auto const& r ) {
                               return l == map[ r ];
                             } ) );
      }

      AND_WHEN( "Encoded string is decoded back using the same interface function" )
      {
        std::string dseq;
        dseq.resize( cseq.size() );
        gum::util::convert( cseq.begin(), cseq.end(), dseq.begin(), alphabet_1_type{}, gum::Char{} );

        THEN( "The result should be identical to the original string" ) {
          REQUIRE( dseq.size() != 0 );
          REQUIRE( dseq.size() == seq.size() );
          REQUIRE( std::equal( dseq.begin(), dseq.end(), seq.begin() ) );
        }
      }
    }
  }

  GIVEN( "A string of a compressed alphabet" )
  {
    gum::String< alphabet_1_type > cseq1;

    WHEN( "It is assigned by a char string" )
    {
      std::string seq = get_seq( alphabet_1_type{} );
      gum::util::assign( cseq1, seq );

      THEN( "It should contain the encoded string" )
      {
        auto map = get_decoding_map( alphabet_1_type{} );
        REQUIRE( cseq1.size() != 0 );
        REQUIRE( cseq1.size() == seq.size() );
        REQUIRE( std::equal( cseq1.begin(), cseq1.end(), seq.begin(),
                             [&map]( auto const& l, auto const& r ) {
                               return map[ l ] == r;
                             } ) );
      }

      AND_WHEN( "Converted to another compressed string using `convert`" )
      {
        gum::String< alphabet_2_type > cseq2;
        cseq2.resize( cseq1.size() );
        gum::util::convert( cseq1.begin(), cseq1.end(), cseq2.begin(),
                            alphabet_1_type{}, alphabet_2_type{} );

        THEN( "It should contain the encoded string in another alphabet" )
        {
          auto map1 = get_decoding_map( alphabet_1_type{} );
          auto map2 = get_decoding_map( alphabet_2_type{} );
          REQUIRE( cseq2.size() != 0 );
          REQUIRE( cseq2.size() == cseq1.size() );
          REQUIRE( std::equal( cseq2.begin(), cseq2.end(), cseq1.begin(),
                               [&map1, &map2]( auto const& l, auto const& r ) {
                                 return map2[ l ] == map1[ r ];
                               } ) );
        }
      }

      AND_WHEN( "Converted to another compressed string using `assign`" )
      {
        gum::String< alphabet_2_type > cseq2;
        gum::util::assign( cseq2, cseq1 );

        THEN( "It should contain the encoded string in another alphabet" )
        {
          auto map1 = get_decoding_map( alphabet_2_type{} );
          auto map2 = get_decoding_map( alphabet_1_type{} );
          REQUIRE( cseq2.size() != 0 );
          REQUIRE( cseq2.size() == cseq1.size() );
          REQUIRE( std::equal( cseq2.begin(), cseq2.end(), cseq1.begin(),
                               [&map1, &map2]( auto const& l, auto const& r ) {
                                 return map1[ l ] == map2[ r ];
                               } ) );
        }
      }

      AND_WHEN( "Converted to another char string using `assign`" )
      {
        std::string seq2;
        gum::util::assign( seq2, cseq1 );

        THEN( "It should contain the encoded string in another alphabet" )
        {
          auto map = get_decoding_map( alphabet_1_type{} );
          REQUIRE( seq2.size() != 0 );
          REQUIRE( seq2.size() == cseq1.size() );
          REQUIRE( std::equal( seq2.begin(), seq2.end(), cseq1.begin(),
                               [&map]( auto const& l, auto const& r ) {
                                 return l == map[ r ];
                               } ) );
        }
      }
    }
  }

  GIVEN( "A string with compressed alphabet" ) {
    gum::String< alphabet_1_type > cseq1;
    std::string seq = get_seq( alphabet_1_type{} );
    gum::util::assign( cseq1, seq );

    AND_GIVEN( "A StringView over it" )
    {
      auto begin = 4;
      auto length = 5u;
      gum::StringView< gum::String< alphabet_1_type > > view( cseq1, begin, length );

      THEN( "The view should yeild the content of the pointed substring" )
      {
        auto map = get_decoding_map( alphabet_1_type{} );
        REQUIRE( view.size() == length );
        REQUIRE( std::equal( cseq1.begin() + begin,
                             cseq1.begin() + begin + length, view.begin(),
                             [ &map ]( auto const& l, auto const& r ) {
                               return map[ l ] == r;
                             } ) );
      }

      WHEN( "The view is assigned to a new char string" )
      {
        std::string new_seq;
        gum::util::assign( new_seq, view );

        THEN( "The new string should contains the view content" )
        {
          REQUIRE( new_seq.size() == length );
          REQUIRE( view == new_seq );
          REQUIRE( new_seq == view );
          REQUIRE( new_seq == static_cast< std::string >( view ) );
          if constexpr ( std::is_same< alphabet_1_type, gum::Char >::value )
            REQUIRE( view == "quick" );
          if constexpr ( std::is_same< alphabet_1_type, gum::DNA5 >::value )
            REQUIRE( view == "TTNNT" );
          if constexpr ( std::is_same< alphabet_1_type, gum::DNA >::value )
            REQUIRE( view == "TTATT" );
        }
      }

      WHEN( "The view is assigned to a new string of the same compressed alphabet" )
      {
        gum::String< alphabet_1_type > cseq2;
        gum::util::assign( cseq2, view );

        THEN( "The new string should contains the view content" )
        {
          REQUIRE( cseq2.size() == length );
          REQUIRE( view == cseq2 );
          REQUIRE( cseq2 == view );
          REQUIRE( cseq2 == static_cast< gum::String< alphabet_1_type > >( view ) );
          if constexpr ( std::is_same< alphabet_1_type, gum::Char >::value ) {
            REQUIRE( view == "quick" );
            REQUIRE( cseq2 == "quick" );
          }
          if constexpr ( std::is_same< alphabet_1_type, gum::DNA5 >::value ) {
            REQUIRE( view == "TTNNT" );
            REQUIRE( cseq2 == "TTNNT" );
            REQUIRE( "TTNNT" == cseq2 );
            REQUIRE( "TTNNT" == view );
          }
          if constexpr ( std::is_same< alphabet_1_type, gum::DNA >::value ) {
            REQUIRE( view == "TTATT" );
            REQUIRE( cseq2 == "TTATT" );
            REQUIRE( "TTATT" == cseq2 );
            REQUIRE( "TTATT" == view );
          }
        }

        AND_WHEN( "Another instance of the same view is constructed" )
        {
          gum::StringView view2 ( cseq1, begin, length );
          THEN( "Both views should be equal" ) {
            REQUIRE( view == view2 );
          }
        }
      }

      WHEN( "The view is assigned to a new string of the different compressed alphabet" )
      {
        gum::String< alphabet_2_type > cseq2;
        gum::util::assign( cseq2, view );

        THEN( "The new string should contains the view content in its own alphabet" )
        {
          auto map = get_decoding_map( alphabet_2_type{} );
          REQUIRE( cseq2.size() == length );
          //REQUIRE( cseq2 == view );  // NOTE: Not implemented
          //REQUIRE( view == cseq2 );  // NOTE: Not implemented
          REQUIRE( std::equal( view.begin(), view.end(), cseq2.begin(),
                               [&map]( auto const& l, auto const& r ) {
                                 return l == map[ r ];
                               } ) );
        }
      }
    }
  }

  GIVEN( "A string of a compressed alphabet" )
  {
    gum::String< alphabet_1_type > cseq1;

    WHEN( "It is assigned by a char string using assignment operator" )
    {
      std::string seq = get_seq( alphabet_1_type{} );
      cseq1 = seq;

      THEN( "It should contain the encoded string" )
      {
        auto map = get_decoding_map( alphabet_1_type{} );
        REQUIRE( cseq1.size() == seq.size() );
        REQUIRE( std::equal( cseq1.begin(), cseq1.end(), seq.begin(),
                             [&map]( auto const& l, auto const& r ) {
                               return map[ l ] == r;
                             } ) );
      }
    }
  }

  GIVEN( "A string of a compressed alphabet" )
  {
    gum::String< alphabet_1_type > cseq1;
    cseq1 = get_seq( alphabet_1_type{} );

    AND_GIVEN( "Another string of a compressed alphabet" )
    {
      gum::String< alphabet_2_type > cseq2;

      WHEN( "The latter is assigned by the former using assignment operator" )
      {
        cseq2 = cseq1;

        THEN( "It should contain the encoded string in another alphabet" )
        {
          auto map1 = get_decoding_map( alphabet_1_type{} );
          auto map2 = get_decoding_map( alphabet_2_type{} );
          REQUIRE( cseq2.size() != 0 );
          REQUIRE( cseq2.size() == cseq1.size() );
          REQUIRE( std::equal( cseq1.begin(), cseq1.end(), cseq2.begin(),
                               [&map1, &map2]( auto const& l, auto const& r ) {
                                 return map1[ l ] == map2[ r ];
                               } ) );
        }
      }
    }

    AND_GIVEN( "A StringView over all of the string" )
    {
      gum::StringView view( cseq1 );

      THEN( "Both view and string should have the same size" )
      {
        REQUIRE( view.size() == cseq1.size() );
      }

      WHEN( "The view is assigned to another string with the same alphabet" )
      {
        gum::String< alphabet_1_type > cseq2;
        cseq2 = view;

        THEN( "The contents of the string and the view should be identical" )
        {
          REQUIRE( cseq2.size() != 0 );
          REQUIRE( cseq2.size() == cseq1.size() );
          REQUIRE( std::equal( cseq2.begin(), cseq2.end(), cseq1.begin(),
                               []( auto const& l, auto const& r ) {
                                 return l == r;
                               } ) );
        }
      }

      WHEN( "The view is assigned to another string with compressed alphabet" )
      {
        gum::String< alphabet_2_type > cseq2 = view;

        THEN( "It should contain the encoded string in another alphabet" )
        {
          auto map = get_decoding_map( alphabet_2_type{} );
          REQUIRE( cseq2.size() != 0 );
          REQUIRE( cseq2.size() == cseq1.size() );
          REQUIRE( std::equal( cseq2.begin(), cseq2.end(), view.begin(),
                               [&map]( auto const& l, auto const& r ) {
                                 return map[ l ] == r;
                               } ) );
        }
      }
    }

    AND_GIVEN( "A StringView over a suffix of it" )
    {
      auto begin = 4;
      gum::StringView view( cseq1, begin );

      THEN( "The size of the view should be equal to the size of the suffix" )
      {
        REQUIRE( view.size() == cseq1.size() - begin );
      }

      WHEN( "The view is assigned to a char string" )
      {
        std::string seq = view;

        THEN( "The char string should contain the view substring" )
        {
          REQUIRE( seq.size() != 0 );
          REQUIRE( seq.size() == view.size() );
          REQUIRE( std::equal( view.begin(), view.end(), seq.begin() ) );
        }
      }

      WHEN( "A substring of a the view is requested with no argument" )
      {
        auto new_view = view.substr( );

        THEN( "Both views should contain the same content" )
        {
          REQUIRE( new_view.size() != 0 );
          REQUIRE( new_view.size() == view.size() );
          REQUIRE( new_view == view );
        }
      }

      WHEN( "A substring of a the view is requested with `pos` argument" )
      {
        auto nbegin = 6;
        auto new_view = view.substr( nbegin );

        THEN( "The second view should contain the correct suffix of the original string" )
        {
          REQUIRE( new_view.size() != 0 );
          REQUIRE( new_view.size() == view.size() - nbegin );
          REQUIRE( std::equal( cseq1.begin() + (begin + nbegin), cseq1.end(),
                               new_view.base() ) );
        }
      }

      WHEN( "A substring of a the view is requested with `pos` and 'len' arguments" )
      {
        auto nbegin = 6;
        auto nlen = 4u;
        auto new_view = view.substr( nbegin, nlen );

        THEN( "The second view should contain the correct suffix of the original string" )
        {
          auto substr_begin = cseq1.begin() + begin + nbegin;
          auto substr_end = substr_begin + nlen;

          REQUIRE( new_view.size() != 0 );
          REQUIRE( new_view.size() == nlen );
          REQUIRE( std::equal( substr_begin, substr_end, new_view.base() ) );
        }
      }

      WHEN( "A substring of a the view is requested with `pos` larger than its size" )
      {
        THEN( "It should throw `std::out_of_range` exception" )
        {
          REQUIRE_THROWS( view.substr( view.size() + 1 ) );
        }
      }

      WHEN( "A substring of a the view is requested with `pos` as exactly its size" )
      {
        auto new_view = view.substr( view.size() );

        THEN( "The new view should be an empty range" )
        {
          REQUIRE( new_view.size() == 0 );
          REQUIRE( new_view.begin() == new_view.end() );
        }
      }

      WHEN( "A substring of a the view is requested with `len` exceeding the view" )
      {
        auto nbegin = 5;
        auto new_view = view.substr( 5, view.size() - nbegin + 1 );

        THEN( "The second view should contain the correct suffix of the original string" )
        {
          REQUIRE( new_view.size() != 0 );
          REQUIRE( new_view.size() == view.size() - nbegin );
          REQUIRE( std::equal( cseq1.begin() + begin + nbegin, cseq1.end(),
                               new_view.base() ) );
        }
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
        if constexpr ( std::is_same< alphabet_type, gum::DNA >::value ) {
          retval.push_back( "ATTTCTAGCGCTAGCTATTACAACACACGAGATATA" );
          retval.push_back( "CGCATGCTGATTACGCGAGGGGTACGCGATA" );
          retval.push_back( "TTTTTTTTTTCCCCCGGAG" );
          retval.push_back( "" );
          retval.push_back( "GGGGAGGGATCAGTAAGAGAGAGATAGATCCC" );
          retval.push_back( "GGC" );
        }
        if constexpr ( std::is_same< alphabet_type, gum::DNA5 >::value ) {
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
        size_type cursor = 0;
        for ( size_type i = 0; i < strset.size(); ++i ) {
          REQUIRE( strset[ i ] == stdstrset[ i ] );
          REQUIRE( strset.length( i ) == stdstrset[ i ].size() );
          REQUIRE( strset.at( i ) == stdstrset.at( i ) );
          REQUIRE( strset.start_position( i ) == cursor );
          cursor += strset.length( i );
          REQUIRE( strset.end_position( i ) == cursor );
          REQUIRE( strset.idx( cursor ) == i );
          ++cursor;  /* sentinel */
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
