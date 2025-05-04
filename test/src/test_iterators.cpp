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
#include <type_traits>
#include <vector>

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

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    WHEN( "An iterator is constructed by passing the container without offset" )
    {
      iterator_type itr( &vec );

      THEN( "The iterator should points to the first element" )
      {
        REQUIRE( *itr == 1 );
        REQUIRE( itr[ 0 ] == 1 );
      }

      THEN( "Indexing iterator should work the same" )
      {
        auto gen = rnd::get_rgn();
        std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
        auto idx = dist( gen );
        REQUIRE( itr[ idx ] == idx + 1 );
      }
    }

    WHEN( "An iterator is constructed by passing the container and offset" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      THEN( "The iterator should point to the first element" )
      {
        REQUIRE( *itr == offset + 1 );
      }
    }

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type begin( &vec, 0 );
      iterator_type end( &vec, vec.size() );

      WHEN( "A non-'end' iterator is compared with the 'end' one" )
      {
        THEN( "The former should be always less than the latter" )
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

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the forward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 + buffer ) == base + buffer );
          REQUIRE( ( itr2 + buffer ) > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the backward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 - buffer ) == base - buffer );
          REQUIRE( ( itr2 - buffer ) < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( ( itr + ( end - itr ) ) == end );
        }
      }

      WHEN( "Constructing another container using the itrator" )
      {
        container_type copy_vec;
        std::copy( begin, end, std::back_inserter( copy_vec ) );
        THEN( "Two containers should contain identical elements" )
        {
          auto vec_itr = vec.begin();
          REQUIRE( std::all_of(
              copy_vec.begin(), copy_vec.end(),
              [&vec_itr]( auto elem ) { return elem == *vec_itr++; } ) );
        }
      }
    }
  }

  GIVEN( "An empty standard container of a POD type" )
  {
    container_type vec;

    WHEN( "An iterator is constructed by passing the container and offset" )
    {
      iterator_type itr( &vec );
      iterator_type end( &vec, vec.size() );

      THEN( "It should be equal to 'end'" )
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

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type begin( &vec, 0 );
      iterator_type end( &vec, vec.size() );

      WHEN( "Pre-incrementing the iterator" )
      {
        ++itr;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( itr == end || *itr == offset + 2 ) );
        }
      }

      WHEN( "Post-incrementing the iterator" )
      {
        itr++;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( itr == end || *itr == offset + 2 ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        itr = iterator_type( &vec, dist( gen ) );
        auto base_itr = itr;
        auto base = *itr;
        itr += buffer;

        THEN( "It should point to the forward element with the distance of 'offset'" )
        {
          REQUIRE( *itr == base + buffer );
          REQUIRE( itr > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        itr = begin + dist( gen );
        auto base_itr = itr;
        auto base = *itr;
        itr -= buffer;

        THEN( "It should point to the backward element with the distance of 'offset'" )
        {
          REQUIRE( *itr == base - buffer );
          REQUIRE( itr < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        itr += ( end - itr );
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( itr == end );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Modifying container using random access iterator", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type begin( &vec, 0 );
      iterator_type itr = offset + begin;

      WHEN( "Modifying the underlying container using the iterator" )
      {
        *itr = W * 2;

        THEN( "The element should equals to the set value" )
        {
          REQUIRE( *itr == W * 2 );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access REVERSE iterator non-modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessReverseIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessReverseIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstReverseIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessConstReverseIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    WHEN( "An iterator is constructed by passing the container without offset" )
    {
      iterator_type itr( &vec );

      THEN( "The iterator should points to the last element" )
      {
        REQUIRE( *itr == W );
        REQUIRE( itr[ 0 ] == W );
      }

      THEN( "Indexing iterator should work in reverse" )
      {
        auto gen = rnd::get_rgn();
        std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
        auto idx = dist( gen );
        REQUIRE( itr[ idx ] == W - idx );
      }
    }

    WHEN( "An iterator is constructed by passing the container and offset" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 1, vec.size() );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      THEN( "The iterator should point to the first element" )
      {
        REQUIRE( *itr == offset );
      }
    }

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 1, vec.size() );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type rbegin( &vec, vec.size() );
      iterator_type rend( &vec, 0 );

      WHEN( "A non-'end' iterator is compared with the 'end' one" )
      {
        THEN( "The former should be always less than the latter" )
        {
          REQUIRE( itr < rend );
          REQUIRE( itr <= rend );
          REQUIRE( itr != rend );
          REQUIRE( !( itr > rend ) );
          REQUIRE( !( itr >= rend ) );
          REQUIRE( rend > itr );
          REQUIRE( rend >= itr );
          REQUIRE( rend != itr );
          REQUIRE( !( rend < itr ) );
          REQUIRE( !( rend <= itr ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the forward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *( itr2 + buffer ) == base - buffer );
          REQUIRE( ( itr2 + buffer ) > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        iterator_type itr2( &vec, dist( gen ) );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the backward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *( itr2 - buffer ) == base + buffer );
          REQUIRE( ( itr2 - buffer ) < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( ( itr + ( rend - itr ) ) == rend );
        }
      }

      WHEN( "Constructing another container using the itrator" )
      {
        container_type copy_vec;
        std::copy( rbegin, rend, std::back_inserter( copy_vec ) );
        THEN( "Two containers should contain identical elements" )
        {
          auto vec_itr = vec.rbegin();
          REQUIRE( std::all_of(
              copy_vec.begin(), copy_vec.end(),
              [ &vec_itr ]( auto elem ) { return elem == *vec_itr++; } ) );
        }
      }
    }
  }

  GIVEN( "An empty standard container of a POD type" )
  {
    container_type vec;

    WHEN( "An iterator is constructed by passing the container and offset" )
    {
      iterator_type itr( &vec );
      iterator_type rend( &vec, 0 );
      iterator_type rbegin( &vec, vec.size() );

      THEN( "It should be equal to 'end'" )
      {
        REQUIRE( itr == rend );
        REQUIRE( itr == rbegin );
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access REVERSE iterator modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessReverseIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstReverseIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 1, vec.size() );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );
      iterator_type rbegin( &vec, vec.size() );
      iterator_type rend( &vec, 0 );

      WHEN( "Pre-incrementing the iterator" )
      {
        ++itr;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( itr == rend || *itr == offset - 1 ) );
        }
      }

      WHEN( "Post-incrementing the iterator" )
      {
        itr++;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( itr == rend || *itr == offset - 1 ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        itr = iterator_type( &vec, dist( gen ) );
        auto base_itr = itr;
        auto base = *itr;
        itr += buffer;

        THEN( "It should point to the forward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *itr == base - buffer );
          REQUIRE( itr > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        itr = rbegin + dist( gen );
        auto base_itr = itr;
        auto base = *itr;
        itr -= buffer;

        THEN( "It should point to the backward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *itr == base + buffer );
          REQUIRE( itr < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        itr += ( rend - itr );
        THEN( "It should be equal to 'end'" ) { REQUIRE( itr == rend ); }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Modifying container using REVERSE random access iterator", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessReverseIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type begin( &vec );
      iterator_type itr = offset + begin;

      WHEN( "Modifying the underlying container using the iterator" )
      {
        *itr = W * 2;

        THEN( "The element should equals to the set value" )
        {
          REQUIRE( *itr == W * 2 );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access Bidirected iterator non-modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessBidiIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessBidiIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstBidiIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( const gum::RandomAccessConstBidiIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    WHEN( "An iterator is constructed by passing the container without offset and directionality" )
    {
      iterator_type itr( &vec );

      THEN( "The iterator should points to the first element" )
      {
        REQUIRE( *itr == 1 );
        REQUIRE( itr[ 0 ] == 1 );
      }

      THEN( "Indexing iterator should work in forward direction" )
      {
        auto gen = rnd::get_rgn();
        std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
        auto idx = dist( gen );
        REQUIRE( itr[ idx ] == idx + 1 );
      }
    }

    WHEN( "An iterator is constructed by passing the container and directionality but without offset" )
    {
      iterator_type fitr( &vec, true );
      iterator_type ritr( &vec, false );

      THEN( "The forward iterator should points to the first element" )
      {
        REQUIRE( *fitr == 1 );
        REQUIRE( fitr[ 0 ] == 1 );
      }

      THEN( "The reverse iterator should points to the last element" )
      {
        REQUIRE( *ritr == W );
        REQUIRE( ritr[ 0 ] == W );
      }

      THEN( "Indexing iterator should work in the designated direction" )
      {
        auto gen = rnd::get_rgn();
        std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
        auto idx = dist( gen );
        REQUIRE( fitr[ idx ] == idx + 1 );
        REQUIRE( ritr[ idx ] == W - idx );
      }
    }

    WHEN( "An iterator is constructed by passing the container and offset without directionality" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      THEN( "The iterator should be fowrad and point to the offset element" )
      {
        REQUIRE( *itr == offset + 1 );
      }
    }

    AND_GIVEN( "A reverse iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 1, vec.size() );
      auto offset = dist( gen );
      iterator_type ritr( &vec, offset, false );
      iterator_type rbegin( &vec, vec.size(), false );
      iterator_type rend( &vec, 0ul, false );

      WHEN( "A non-'end' iterator is compared with the 'end' one" )
      {
        THEN( "The former should be always less than the latter" )
        {
          REQUIRE( ritr < rend );
          REQUIRE( ritr <= rend );
          REQUIRE( ritr != rend );
          REQUIRE( !( ritr > rend ) );
          REQUIRE( !( ritr >= rend ) );
          REQUIRE( rend > ritr );
          REQUIRE( rend >= ritr );
          REQUIRE( rend != ritr );
          REQUIRE( !( rend < ritr ) );
          REQUIRE( !( rend <= ritr ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        iterator_type ritr2( &vec, dist( gen ), false );
        auto base_itr = ritr2;
        auto base = *ritr2;

        THEN( "It should point to the forward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *( ritr2 + buffer ) == base - buffer );
          REQUIRE( ( ritr2 + buffer ) > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        iterator_type ritr2( &vec, dist( gen ), false );
        auto base_itr = ritr2;
        auto base = *ritr2;

        THEN( "It should point to the backward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *( ritr2 - buffer ) == base + buffer );
          REQUIRE( ( ritr2 - buffer ) < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( ( ritr + ( rend - ritr ) ) == rend );
        }
      }

      WHEN( "Constructing another container using the itrator" )
      {
        container_type copy_vec;
        std::copy( rbegin, rend, std::back_inserter( copy_vec ) );
        THEN( "Two containers should contain identical elements" )
        {
          auto vec_itr = vec.rbegin();
          REQUIRE( std::all_of(
              copy_vec.begin(), copy_vec.end(),
              [ &vec_itr ]( auto elem ) { return elem == *vec_itr++; } ) );
        }
      }
    }

    AND_GIVEN( "A forward iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset, true );
      iterator_type begin( &vec, 0ul, true );
      iterator_type end( &vec, vec.size(), true );

      WHEN( "A non-'end' iterator is compared with the 'end' one" )
      {
        THEN( "The former should be always less than the latter" )
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

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        iterator_type itr2( &vec, dist( gen ), true );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the forward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 + buffer ) == base + buffer );
          REQUIRE( ( itr2 + buffer ) > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        iterator_type itr2( &vec, dist( gen ), true );
        auto base_itr = itr2;
        auto base = *itr2;

        THEN( "It should point to the backward element with the distance of 'offset'" )
        {
          REQUIRE( *( itr2 - buffer ) == base - buffer );
          REQUIRE( ( itr2 - buffer ) < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( ( itr + ( end - itr ) ) == end );
        }
      }

      WHEN( "Constructing another container using the itrator" )
      {
        container_type copy_vec;
        std::copy( begin, end, std::back_inserter( copy_vec ) );
        THEN( "Two containers should contain identical elements" )
        {
          auto vec_itr = vec.begin();
          REQUIRE( std::all_of(
              copy_vec.begin(), copy_vec.end(),
              [&vec_itr]( auto elem ) { return elem == *vec_itr++; } ) );
        }
      }
    }
  }

  GIVEN( "An empty standard container of a POD type" )
  {
    container_type vec;

    WHEN( "An iterator is constructed by passing the container and directionality" )
    {
      iterator_type fitr( &vec, true );
      iterator_type ritr( &vec, false );
      iterator_type begin( &vec, 0ul );
      iterator_type end( &vec, vec.size() );
      iterator_type rbegin( &vec, vec.size(), false );
      iterator_type rend( &vec, 0ul, false );

      THEN( "It should be equal to 'end'" )
      {
        REQUIRE( fitr == end );
        REQUIRE( fitr == begin );
        REQUIRE( ritr == rend );
        REQUIRE( ritr == rbegin );
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Random access Bidirected iterator modifying usage", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessBidiIterator< std::vector< int > >, 3141 ) ),
                       ( std::vector< int >, ( gum::RandomAccessConstBidiIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "A reverse iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 1, vec.size() );
      auto offset = dist( gen );
      iterator_type ritr( &vec, offset, false );
      iterator_type rbegin( &vec, vec.size(), false );
      iterator_type rend( &vec, 0ul, false );

      WHEN( "Pre-incrementing the iterator" )
      {
        ++ritr;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( ritr == rend || *ritr == offset - 1 ) );
        }
      }

      WHEN( "Post-incrementing the iterator" )
      {
        auto piter = ritr++;

        THEN( "It should point to the next element" )
        {
          REQUIRE( *piter == offset );
          REQUIRE( ( ritr == rend || *ritr == offset - 1 ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        ritr = iterator_type( &vec, dist( gen ), false );
        auto base_itr = ritr;
        auto base = *ritr;
        ritr += buffer;

        THEN( "It should point to the forward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *ritr == base - buffer );
          REQUIRE( ritr > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        ritr = rbegin + dist( gen );
        auto base_itr = ritr;
        auto base = *ritr;
        ritr -= buffer;

        THEN( "It should point to the backward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *ritr == base + buffer );
          REQUIRE( ritr < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        ritr += ( rend - ritr );
        THEN( "It should be equal to 'end'" ) { REQUIRE( ritr == rend ); }
      }
    }

    AND_GIVEN( "A forward iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset, true );
      iterator_type begin( &vec, 0ul, true );
      iterator_type end( &vec, vec.size(), true );

      WHEN( "Pre-incrementing the iterator" )
      {
        ++itr;

        THEN( "It should point to the next element" )
        {
          REQUIRE( ( itr == end || *itr == offset + 2 ) );
        }
      }

      WHEN( "Post-incrementing the iterator" )
      {
        auto piter = itr++;

        THEN( "It should point to the next element" )
        {
          REQUIRE( *piter == offset + 1 );
          REQUIRE( ( itr == end || *itr == offset + 2 ) );
        }
      }

      WHEN( "The iterator is added by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer, vec.size() - buffer - 1 );
        itr = iterator_type( &vec, dist( gen ), true );
        auto base_itr = itr;
        auto base = *itr;
        itr += buffer;

        THEN( "It should point to the forward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *itr == base + buffer );
          REQUIRE( itr > base_itr );
        }
      }

      WHEN( "The iterator is subtracted by an offset more than 2" )
      {
        size_type buffer = size / 100;
        std::uniform_int_distribution< size_type > dist(
            buffer + 1, vec.size() - buffer );
        itr = begin + dist( gen );
        auto base_itr = itr;
        auto base = *itr;
        itr -= buffer;

        THEN( "It should point to the backward element with the distance of "
              "'offset'" )
        {
          REQUIRE( *itr == base - buffer );
          REQUIRE( itr < base_itr );
        }
      }

      WHEN( "An iterator is exhausted" )
      {
        itr += ( end - itr );
        THEN( "It should be equal to 'end'" )
        {
          REQUIRE( itr == end );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO_SIG( "Modifying container using Bidirected random access iterator", "[iterators]",
                       ( ( typename T, typename U, int W ), T, U, W ),  // NOTE: Requirement: W >= 200
                       ( std::vector< int >, ( gum::RandomAccessBidiIterator< std::vector< int > >, 3141 ) ) )
{
  using container_type = T;
  using iterator_type = U;
  using size_type = typename container_type::size_type;

  size_type size = W;

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "A forward iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type begin( &vec );
      iterator_type itr = offset + begin;

      WHEN( "Modifying the underlying container using the iterator" )
      {
        *itr = W * 2;

        THEN( "The element should equals to the set value" )
        {
          REQUIRE( *itr == W * 2 );
        }
      }
    }

    AND_GIVEN( "A reverse iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type rbegin( &vec, false );
      iterator_type ritr = offset + rbegin;

      WHEN( "Modifying the underlying container using the iterator" )
      {
        *ritr = W * 2;

        THEN( "The element should equals to the set value" )
        {
          REQUIRE( *ritr == W * 2 );
        }
      }
    }
  }
}

template< typename T >
constexpr bool
is_const( T& )
{
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

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      WHEN( "Accessing an element via dereferencing the iterator" )
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

  GIVEN( "A non-empty standard container of a POD type" )
  {
    container_type vec( size, 0 );
    std::iota( vec.begin(), vec.end(), 1 );

    AND_GIVEN( "An iterator over it" )
    {
      auto gen = rnd::get_rgn();
      std::uniform_int_distribution< size_type > dist( 0, vec.size() - 1 );
      auto offset = dist( gen );
      iterator_type itr( &vec, offset );

      WHEN( "Accessing an element via dereferencing the iterator" )
      {
        THEN( "It should preserve const-ness of the container" )  // by returning a const reference
        {
          REQUIRE( !is_const( *itr ) );
        }
      }
    }
  }
}
