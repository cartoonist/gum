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

#include <sdsl/bit_vectors.hpp>

#include <gum/utils.hpp>

#include "test_base.hpp"


using namespace gum;

TEMPLATE_SCENARIO( "Prefix and suffix check of a string", "[utils][template]",
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

SCENARIO( "Word-wise range operations for bit-vectors", "[utils]" )
{
  GIVEN( "A bit vector whose size is less than a word length (64-bit)" )
  {
    sdsl::bit_vector sbv( 12, 0 );
    sbv[ 0 ] = 1;
    sbv[ 5 ] = 1;
    sbv[ 11 ] = 1;

    WHEN( "The whole bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 30, 1 );
      util::bv_icopy( sbv, dbv );

      THEN( "All first bits of length of source bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xfffffffffffff821 );
      }
    }

    WHEN( "The bit vector is partially copied to another" )
    {
      sdsl::bit_vector dbv( 30, 1 );
      util::bv_icopy( sbv, dbv, 6, 1 );

      THEN( "That part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xffffffffffffffbf );
      }
    }

    WHEN( "A suffix of the bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 30, 1 );
      util::bv_icopy( sbv, dbv, 5 );

      THEN( "The suffix part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xfffffffffffff83f );
      }
    }

    WHEN( "The whole bit vector is set to zero" )
    {
      util::bv_izero( sbv );
      THEN( "All bits should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0 );
      }
    }

    WHEN( "The bit vector is partially set to zero" )
    {
      util::bv_izero( sbv, 5, 6 );
      THEN( "All bits in the region should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0x801 );
      }
    }

    WHEN( "A suffix of the bit vector is set to zero" )
    {
      util::bv_izero( sbv, 6 );
      THEN( "The bits in the suffix should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0x21 );
      }
    }
  }

  GIVEN( "A bit vector whose size is a multiplier of a word length (64-bit)" )
  {
    sdsl::bit_vector sbv( 7872, 0 );
    sbv.set_int( 542, 0x900000000fafabcd );
    sbv.set_int( 7808, 0xaaaaaaaaaaaaaaaa );

    WHEN( "The whole bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 7872, 1 );
      util::bv_icopy( sbv, dbv );

      THEN( "All first bits of length of source bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0x0 );
        REQUIRE( dbv.get_int( 100 ) == 0x0 );
        REQUIRE( dbv.get_int( 478 ) == 0x0 );
        REQUIRE( dbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( dbv.get_int( 893 ) == 0x0 );
        REQUIRE( dbv.get_int( 7744 ) == 0x0 );
        REQUIRE( dbv.get_int( 7808 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "The bit vector is partially copied to another" )
    {
      sdsl::bit_vector dbv( 8000, 1 );
      util::bv_icopy( sbv, dbv, 542, 64 );

      THEN( "That part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 100 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 478 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( dbv.get_int( 893 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 6936 ) == 0xffffffffffffffff );
      }
    }

    WHEN( "A suffix of the bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 8000, 1 );
      util::bv_icopy( sbv, dbv, 7808 );

      THEN( "The suffix part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 100 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 478 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 542 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 893 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 6936 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 7744 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 7808 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "The whole bit vector is set to zero" )
    {
      util::bv_izero( sbv );
      THEN( "All bits should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0 );
      }
    }

    WHEN( "The bit vector is partially set to zero" )
    {
      util::bv_izero( sbv, 542, 60 );
      THEN( "All bits in the region should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0x0 );
        REQUIRE( sbv.get_int( 100 ) == 0x0 );
        REQUIRE( sbv.get_int( 478 ) == 0x0 );
        REQUIRE( sbv.get_int( 542 ) == 0x9000000000000000 );
        REQUIRE( sbv.get_int( 893 ) == 0x0 );
        REQUIRE( sbv.get_int( 7744 ) == 0x0 );
        REQUIRE( sbv.get_int( 7808 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "A suffix of the bit vector is set to zero" )
    {
      util::bv_izero( sbv, 7812 );
      THEN( "All bits in the region should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0x0 );
        REQUIRE( sbv.get_int( 100 ) == 0x0 );
        REQUIRE( sbv.get_int( 478 ) == 0x0 );
        REQUIRE( sbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( sbv.get_int( 893 ) == 0x0 );
        REQUIRE( sbv.get_int( 7744 ) == 0x0 );
        REQUIRE( sbv.get_int( 7808 ) == 0x000000000000000a );
      }
    }
  }

  GIVEN( "A bit vector whose size is larger than word length (64-bit)" )
  {
    sdsl::bit_vector sbv( 7800, 0 );
    sbv.set_int( 0, 0xdddddddddddddddd );
    sbv.set_int( 542, 0x900000000fafabcd );
    sbv.set_int( 7736, 0xaaaaaaaaaaaaaaaa );

    WHEN( "The whole bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 7872, 1 );
      util::bv_icopy( sbv, dbv );

      THEN( "All first bits of length of source bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xdddddddddddddddd );
        REQUIRE( dbv.get_int( 100 ) == 0x0 );
        REQUIRE( dbv.get_int( 478 ) == 0x0 );
        REQUIRE( dbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( dbv.get_int( 893 ) == 0x0 );
        REQUIRE( dbv.get_int( 7672 ) == 0x0 );
        REQUIRE( dbv.get_int( 7736 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "The bit vector is partially copied to another" )
    {
      sdsl::bit_vector dbv( 8000, 1 );
      util::bv_icopy( sbv, dbv, 542, 74 );

      THEN( "That part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 100 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 478 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( dbv.get_int( 606 ) == 0xfffffffffffffc00 );
        REQUIRE( dbv.get_int( 893 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 6936 ) == 0xffffffffffffffff );
      }
    }

    WHEN( "A suffix of the bit vector is copied to another" )
    {
      sdsl::bit_vector dbv( 8000, 1 );
      util::bv_icopy( sbv, dbv, 7736 );

      THEN( "The suffix part of destination bit vector should be equal to the source" )
      {
        REQUIRE( dbv.get_int( 0 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 100 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 478 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 542 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 893 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 6936 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 7672 ) == 0xffffffffffffffff );
        REQUIRE( dbv.get_int( 7736 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "The whole bit vector is set to zero" )
    {
      util::bv_izero( sbv );
      THEN( "All bits should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0 );
      }
    }

    WHEN( "The bit vector is partially set to zero" )
    {
      util::bv_izero( sbv, 528, 74 );
      THEN( "All bits in the region should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0xdddddddddddddddd );
        REQUIRE( sbv.get_int( 100 ) == 0x0 );
        REQUIRE( sbv.get_int( 478 ) == 0x0 );
        REQUIRE( sbv.get_int( 528 ) == 0x0 );
        REQUIRE( sbv.get_int( 542 ) == 0x9000000000000000 );
        REQUIRE( sbv.get_int( 893 ) == 0x0 );
        REQUIRE( sbv.get_int( 7736 ) == 0xaaaaaaaaaaaaaaaa );
      }
    }

    WHEN( "A suffix of the bit vector is set to zero" )
    {
      util::bv_izero( sbv, 7740 );
      THEN( "All bits in the region should be zero" )
      {
        REQUIRE( sbv.get_int( 0 ) == 0xdddddddddddddddd );
        REQUIRE( sbv.get_int( 100 ) == 0x0 );
        REQUIRE( sbv.get_int( 478 ) == 0x0 );
        REQUIRE( sbv.get_int( 542 ) == 0x900000000fafabcd );
        REQUIRE( sbv.get_int( 893 ) == 0x0 );
        REQUIRE( sbv.get_int( 7672 ) == 0x0 );
        REQUIRE( sbv.get_int( 7736 ) == 0x000000000000000a );
      }
    }
  }
}

SCENARIO( "Get and apply sorting permutation", "[utils]" )
{
  GIVEN( "An unsorted array of integers" )
  {
    std::uniform_int_distribution< int > distribution( -3000 , 3000 );
    std::mt19937 engine;
    auto generator = std::bind( distribution, engine );
    std::size_t n = 10000;

    std::vector< int > array( n );
    std::generate_n( array.begin(), n, generator );
    std::vector< int > sorted( array.size() );
    std::partial_sort_copy( array.begin(), array.end(), sorted.begin(), sorted.end() );

    WHEN( "Getting the permutation induced by sorting" )
    {
      auto perm = util::sort_permutation( array );

      THEN( "Permutation should map positions in sorted array to ones in original array" )
      {
        for ( std::size_t i = 0; i < sorted.size(); ++i ) {
          REQUIRE( sorted[ i ] == array[ perm[ i ] ] );
        }
      }

      AND_WHEN( "Applying the permutation to the array" )
      {
        auto applied = util::permutated( perm, array );

        THEN( "The resulting array should be equal to the sorted one" )
        {
          REQUIRE( std::equal( applied.begin(), applied.end(), sorted.begin() ) );
        }
      }

      AND_WHEN( "Applying the permutation to the array in place" )
      {
        util::permute( perm, array );

        THEN( "It should sort the array itself" )
        {
          REQUIRE( std::equal( array.begin(), array.end(), sorted.begin() ) );
        }
      }

      AND_WHEN( "Sorting multiple arrays by the permutation" )
      {
        std::vector< int > new_array( array.size() );
        std::iota( new_array.begin(), new_array.end(), 0 );

        util::permute( perm, array, new_array );

        THEN( "It should sort the array itself" )
        {
          REQUIRE( std::equal( array.begin(), array.end(), sorted.begin() ) );
          REQUIRE( std::equal( new_array.begin(), new_array.end(), perm.begin() ) );
        }
      }
    }

    WHEN( "Sorting zipped containers" )
    {
      auto perm = util::sort_permutation( array );
      std::vector< int > new_array( array.size() );
      std::iota( new_array.begin(), new_array.end(), 0 );

      util::sort_zip( array, new_array );

      THEN( "The arrays should be sorted" )
      {
        REQUIRE( std::equal( array.begin(), array.end(), sorted.begin() ) );
        REQUIRE( std::equal( new_array.begin(), new_array.end(), perm.begin() ) );
      }
    }
  }
}
