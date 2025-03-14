/**
 *    @file  test_iterators.cpp
 *   @brief  Test scenarios for iterators header file
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Sun Aug 27, 2023  17:32
 *  Organization:  Universität Bielefeld
 *     Copyright:  Copyright (c) 2023, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>

#include <gum/iterators.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO_SIG( "Random access iterator non-modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessConstIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;
  using value_type = typename container_type::value_type;

  size_type size = W;

  GIVEN ( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    WHEN ( "An iterator is constructed by passing the container without offset" )
    {
      iterator_type itr( &vec );

      THEN ( "The iterator should points to the first element" )
      {
        REQUIRE( *itr == 1 );
      }
    }

    WHEN ( "An iterator is constructed by passing the container and offset" )
    {
      std::random_device rd;
      std::mt19937 gen( rd() );
      std::uniform_int_distribution< value_type > dist( 0, vec.size()-1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      THEN ( "The iterator should point to the first element" )
      {
        REQUIRE( *itr == offset+1 );
      }
    }

    AND_GIVEN ( "An iterator over it" )
    {
      std::random_device rd;
      std::mt19937 gen( rd() );
      std::uniform_int_distribution< size_type > dist( 0, vec.size()-1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type begin ( &vec, 0 );
      iterator_type end ( &vec, vec.size() );

      WHEN ( "A non-'end' iterator is compared with the 'end' one" )
      {
        THEN ( "The former should be always less than the latter" )
        {
          REQUIRE( itr < end );
          REQUIRE( itr <= end );
          REQUIRE( itr != end );
          REQUIRE( !( itr > end ) );
          REQUIRE( !( itr >= end ) );
          REQUIRE( end > itr );
          REQUIRE( end >= itr );
          REQUIRE( end != itr );
          REQUIRE( !( end < itr ) );
          REQUIRE( !( end <= itr ) );
        }
      }

      WHEN ( "The iterator is added by an offset more than 2" )
      {
        value_type buffer = size / 100;
        std::uniform_int_distribution< value_type > dist( buffer, vec.size()-buffer-1 );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN ( "It should point to the forward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 + buffer ) == base + buffer );
          REQUIRE( ( itr2 + buffer ) > base_itr );
        }
      }

      WHEN ( "The iterator is subtracted by an offset more than 2" )
      {
        value_type buffer = size / 100;
        std::uniform_int_distribution< value_type > dist( buffer, vec.size()-buffer-1 );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN ( "It should point to the backward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 - buffer ) == base - buffer );
          REQUIRE( ( itr2 - buffer ) < base_itr );
        }
      }

      WHEN ( "An iterator is exhausted" )
      {
        THEN ( "It should be equal to 'end'" )
        {
          REQUIRE( ( itr + ( end - itr ) ) == end );
        }
      }

      WHEN ( "Constructing another container using the itrator" )
      {
        container_type copy_vec;
        std::copy( begin, end, std::back_inserter( copy_vec ) );
        THEN ( "Two containers should contain identical elements" )
        {
          auto vec_itr = vec.begin();
          REQUIRE( std::all_of( copy_vec.begin(), copy_vec.end(),
                                [&vec_itr]( auto elem ) { return elem == *vec_itr++; } ) );
        }
      }
    }
  }

  GIVEN ( "An empty standard container of a POD type" )
  {
    container_type vec;

    WHEN ( "An iterator is constructed by passing the container and offset" )
    {
      iterator_type itr( &vec );
      iterator_type end( &vec, vec.size() );

      THEN ( "It should be equal to 'end'" )
      {
        REQUIRE( itr == end );
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access iterator modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;
  using value_type = typename container_type::value_type;

  size_type size = W;

  GIVEN ( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN ( "An iterator over it" )
    {
      std::random_device rd;
      std::mt19937 gen( rd() );
      std::uniform_int_distribution< value_type > dist( 0, vec.size()-1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type begin ( &vec, 0 );
      iterator_type end ( &vec, vec.size() );

      WHEN ( "Pre-incrementing the iterator" )
      {
        ++itr;

        THEN ( "It should point to the next element" )
        {
          REQUIRE( ( itr == end || *itr == offset+2 ) );
        }
      }

      WHEN ( "Post-incrementing the iterator" )
      {
        itr++;

        THEN ( "It should point to the next element" )
        {
          REQUIRE( ( itr == end || *itr == offset+2 ) );
        }
      }

      WHEN ( "The iterator is added by an offset more than 2" )
      {
        value_type buffer = size / 100;
        std::uniform_int_distribution< value_type > dist( buffer, vec.size()-buffer-1 );
        itr = iterator_type( &vec, dist( gen ) );
        auto base_itr = itr;
        auto base = *itr;
        itr += buffer;

        THEN ( "It should point to the forward element with the distance of 'offset'" )
        {
          REQUIRE( *itr == base + buffer );
          REQUIRE( itr > base_itr );
        }
      }

      WHEN ( "The iterator is subtracted by an offset more than 2" )
      {
        value_type buffer = size / 100;
        std::uniform_int_distribution< value_type > dist( buffer, vec.size()-buffer-1 );
        itr = begin + dist( gen );
        auto base_itr = itr;
        auto base = *itr;
        itr -= buffer;

        THEN ( "It should point to the backward element with the distance of 'offset'" )
        {
          REQUIRE( *itr == base - buffer );
          REQUIRE( itr < base_itr );
        }
      }

      WHEN ( "An iterator is exhausted" )
      {
        itr += ( end - itr );
        THEN ( "It should be equal to 'end'" )
        {
          REQUIRE( itr == end );
        }
      }
    }
  }
}

template< typename T >
constexpr bool
is_const( T& ) {
  return std::is_const< T >::value;
}

TEMPLATE_SCENARIO_SIG( "Random access iterator of a const container", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),
                       ( std::vector< int >, ( gum::RandomAccessConstIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessConstIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN ( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN ( "An iterator over it" )
    {
      std::random_device rd;
      std::mt19937 gen( rd() );
      std::uniform_int_distribution< size_type > dist( 0, vec.size()-1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      WHEN ( "Accessing an element via dereferencing the iterator" )
      {
        THEN( "It should preserve const-ness of the container" )  // by returning a const reference
        {
          REQUIRE( is_const( *itr ) );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access iterator of a non-const container", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),
                       ( std::vector< int >, ( gum::RandomAccessIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN ( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN ( "An iterator over it" )
    {
      std::random_device rd;
      std::mt19937 gen( rd() );
      std::uniform_int_distribution< size_type > dist( 0, vec.size()-1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      WHEN ( "Accessing an element via dereferencing the iterator" )
      {
        THEN( "It should preserve const-ness of the container" )  // by returning a const reference
        {
          REQUIRE( !is_const( *itr ) );
        }
      }
    }
  }
}
