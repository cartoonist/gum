/**
 *    @file  coordinate.hpp
 *   @brief  Coordinate system template class definitions.
 *
 *  This header file defines coordinate classes.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sun Mar 22, 2020  20:07
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_COORDINATE_HPP__
#define GUM_COORDINATE_HPP__

#include <cinttypes>
#include <string>
#include <type_traits>
#include <algorithm>

#include <parallel_hashmap/phmap.h>
#include <sdsl/int_vector.hpp>

#include "basic_utils.hpp"


namespace gum {
  namespace coordinate {
    /**
     *  Coordinate system
     *  =================
     *
     *  Each graph has its own local coordinate system; i.e. each node has some
     *  unique ID and each locus in a node can be addressed by its corresponding
     *  node ID and the offset of the locus in the node sequence. A coordinate
     *  system defines a framework of reference which are shared between
     *  different types of the same graph/genome: e.g. `Dynamic`, `Succinct`, or
     *  `vg` for `GFA` file formats.
     *
     *  A coordinate system is a functor with two `operator()` overloads. One is
     *  'query' call, getting a local ID and return the ID in the coordinate
     *  system:
     *
     *      id_type operator()( lid_type local_id );
     *
     *  Another overload is 'update' call which updates the ID corresponding to
     *  the local ID:
     *
     *      void operator()( lid_type local_id, id_type new_id );
     *
     *  This call is used to construct a coordinate system when IDs are unknown.
     *  It can be a no-op function if the IDs are already known.
     */

    /* None specialization tag. */
    struct None {};
    /* Identity specialization tag. */
    struct Identity {};
    /* Stoid specialization tag. */
    struct Stoid {};
    /* Sparse specialization tag. */
    struct Sparse {};
    /* Dense specialization tag. */
    struct Dense {};

    /**
     *  @brief  None coordinate system class.
     *
     *  Represent a special coordinate system which mimics always-empty Sparse
     *  coordinate system; i.e. always returns zero on queries and do nothing on
     *  insertions.
     */
    template< typename TID, typename TLocalID=TID >
    class NoneBase {
    public:
      /* === TYPEDEFS === */
      using lid_type = TLocalID;
      using id_type = TID;

      /* === LIFECYCLE === */
      NoneBase() = default;
      NoneBase( NoneBase const& other ) = default;
      NoneBase( NoneBase&& other ) noexcept = default;
      ~NoneBase() noexcept = default;

      template< typename TCoordinate >
      NoneBase( TCoordinate const& other )
      { /* noop */ }

      /* === OPERATORS === */
      NoneBase& operator=( NoneBase const& other ) = default;
      NoneBase& operator=( NoneBase&& other ) noexcept = default;

      constexpr inline id_type
      operator()( lid_type const& ) const
      {
        return id_type();
      }

      constexpr inline void
      operator()( lid_type const&, id_type const& )
      { /* noop */ }
    };  /* --- end of template class NoneBase --- */

    /**
     *  @brief  Identity coordinate system class.
     *
     *  Represent a special coordinate system in which the IDs are identical to
     *  those in graph local coordinate system.
     */
    template< typename TID, typename TLocalID=TID >
    class IdentityBase {
    public:
      /* === TYPEDEFS === */
      using lid_type = TLocalID;
      using id_type = TID;

      /* === LIFECYCLE === */
      IdentityBase() = default;
      IdentityBase( IdentityBase const& other ) = default;
      IdentityBase( IdentityBase&& other ) noexcept = default;
      ~IdentityBase() noexcept = default;

      template< typename TCoordinate >
      IdentityBase( TCoordinate const& other )
      { /* noop */ }

      /* === OPERATORS === */
      IdentityBase& operator=( IdentityBase const& other ) = default;
      IdentityBase& operator=( IdentityBase&& other ) noexcept = default;

      constexpr inline id_type
      operator()( lid_type const& lid ) const
      {
        return lid;
      }

      constexpr inline void
      operator()( lid_type const&, id_type const& )
      { /* noop */ }
    };  /* --- end of template class IdentityBase --- */

    /**
     *  @brief  Stoid coordinate system class.
     *
     *  Represent an external coordinate system in a sequence graph whose IDs
     *  are string representation of the graph local coordinate system IDs.
     */
    template< typename TID, typename TLocalID=std::string >
    class StoidBase {
    public:
      static_assert( std::is_integral< TID >::value,
                     "id type should be of integral type" );
      /* === TYPEDEFS === */
      using lid_type = TLocalID;
      using id_type = TID;

      /* === LIFECYCLE === */
      StoidBase() = default;
      StoidBase( StoidBase const& other ) = default;
      StoidBase( StoidBase&& other ) noexcept = default;
      ~StoidBase() noexcept = default;

      template< typename TCoordinate >
      StoidBase( TCoordinate const& other )
      { /* noop */ }

      /* === OPERATORS === */
      StoidBase& operator=( StoidBase const& other ) = default;
      StoidBase& operator=( StoidBase&& other ) noexcept = default;

      inline id_type
      operator()( lid_type const& lid ) const
      {
        return std::stoll( lid );
      }

      constexpr inline void
      operator()( lid_type const&, id_type const& )
      { /* noop */ }
    };  /* --- end of template class StoidBase --- */

    /**
     *  @brief  Sparse coordinate system class.
     *
     *  Represent an external coordinate system in a sequence graph by storing
     *  hash map of its IDs to IDs in graph local coordinate system.
     */
    template< typename TID, typename TLocalID=TID >
    class SparseBase {
    public:
      /* === TYPEDEFS === */
      using lid_type = TLocalID;
      using id_type = TID;
      using map_type = phmap::flat_hash_map< lid_type, id_type >;
      using size_type = typename map_type::size_type;

      /* === LIFECYCLE === */
      SparseBase() = default;
      SparseBase( SparseBase const& other ) = default;
      SparseBase( SparseBase&& other ) noexcept = default;
      ~SparseBase() noexcept = default;

      template< typename TCoordinate >
      SparseBase( TCoordinate const& other )
      {
        other.for_each_element(
            [this]( lid_type lid, id_type id ) {
              this->operator()( lid, id );
              return true;
            });
      }

      /* === OPERATORS === */
      SparseBase& operator=( SparseBase const& other ) = default;
      SparseBase& operator=( SparseBase&& other ) noexcept = default;

      inline id_type
      operator()( lid_type const& lid ) const
      {
        auto found = this->ids.find( lid );
        if ( found == this->ids.end() ) return 0;
        return found->second;
      }

      inline void
      operator()( lid_type const& lid, id_type const& id )
      {
        this->ids[ lid ] = id;
      }

      /* === METHODS === */
      inline bool
      for_each_element( std::function< bool( lid_type, id_type ) > callback ) const
      {
        for ( auto const& elem : this->ids ) {
          if ( !callback( elem.first, elem.second ) ) return false;
        }
        return true;
      }

      inline size_type
      size( ) const
      {
        return this->ids.size();
      }

      inline bool
      empty( ) const
      {
        return this->size() == 0;
      }

      private:
      /* === DATA MEMBERS === */
      map_type ids;
    };  /* --- end of template class SparseBase --- */

    /**
     *  @brief  Dense coordinate system class.
     *
     *  Represent an external coordinate system in a sequence graph by storing
     *  IDs in graph local coordinate system sequentially in an array where IDs
     *  in the external coordinate are ranks with an offset of `id_min`; where
     *  `id_min` is the minimum ID in the external coordinate system.
     *
     *  This coordinate system is a succinct data structure if IDs in the
     *  external coordinate system is sequential.
     */
    template< typename TID, typename TLocalID=TID >
    class DenseBase {
    public:
      static_assert( std::is_integral< TID >::value,
                     "id type should be of integral type" );
      static_assert( std::is_integral< TLocalID >::value,
                     "local id type should be of integral type" );
      /* === TYPEDEFS === */
      using lid_type = TLocalID;
      using id_type = TID;
      using container_type = sdsl::int_vector<>;
      using size_type = typename container_type::size_type;

      static constexpr const size_type INIT_SIZE = 16;

      /* === LIFECYCLE === */
      DenseBase( lid_type min=0, lid_type max=0 )
        : id_min( min ), id_max( max )
      {
        assert( this->id_max >= this->id_min );
        size_type size = this->id_max - this->id_min + 1;
        this->ids = container_type( ( size > INIT_SIZE ? size : INIT_SIZE ), 0 );
      }

      DenseBase( DenseBase const& other ) = default;
      DenseBase( DenseBase&& other ) noexcept = default;
      ~DenseBase() noexcept = default;

      template< typename TCoordinate >
      DenseBase( TCoordinate const& other )
        : ids( other.size(), 0 ), id_min( 0 ), id_max( 0 )
      {
        other.for_each_element(
            [this]( lid_type lid, id_type id ) {
              if ( this->id_min == 0 || this->id_min > lid ) {
                this->id_min = lid;
              }
              if ( this->id_max < lid ) {
                this->id_max = lid;
              }
              return true;
            });

        other.for_each_element(
            [this]( lid_type lid, id_type id ) {
              this->operator()( lid, id );
              return true;
            });
      }

      /* === OPERATORS === */
      DenseBase& operator=( DenseBase const& other ) = default;
      DenseBase& operator=( DenseBase&& other ) noexcept = default;

      inline id_type
      operator()( lid_type lid ) const
      {
        if ( lid < this->id_min || this->id_max < lid ) return 0;
        size_type rank = lid - this->id_min;
        return this->ids[ rank ];
      }

      inline void
      operator()( lid_type lid, id_type id )
      {
        if ( this->id_min == 0 ) this->id_min = lid;

        if ( lid < this->id_min ) {
          this->shift_right( this->id_min - lid );
          this->id_min = lid;
        }
        else if ( lid > this->id_max ) {
          this->id_max = lid;
        }
        size_type rank = lid - this->id_min;
        if ( rank >= this->capacity() ) this->reserve( rank + 1 );
        this->ids[ rank ] = id;
      }

      /* === METHODS === */
      inline bool
      for_each_element( std::function< bool( lid_type, id_type ) > callback )
      {
        size_type rank = 0;
        for ( auto lid = this->id_min; lid <= this->id_max; ++lid ) {
          if ( !callback( lid, this->ids[ rank++ ] ) ) return false;
        }
        return true;
      }

      inline size_type
      size( ) const
      {
        if ( this->id_min == 0 ) return 0;
        return this->id_max - this->id_min + 1;
      }

      inline size_type
      empty( ) const
      {
        return this->size() == 0;
      }

      inline size_type
      capacity( ) const
      {
        return this->ids.size();
      }

      inline void
      reserve( size_type new_size )
      {
        new_size = util::roundup64( new_size );
        if ( new_size <= this->capacity() ) return;
        container_type new_ids( new_size, 0 );
        std::copy( this->ids.begin(), this->ids.end(), new_ids.begin() );
        std::swap( new_ids, this->ids );
      }

      inline void
      shrink_to_fit( )
      {
        this->ids.resize( this->size() );
        sdsl::util::bit_compress( this->ids );
      }

    private:
      /* === METHODS === */
      inline void
      shift_right( size_type offset )
      {
        if ( this->empty() ) return;
        this->reserve( this->size() + offset );
        std::copy_backward( this->ids.begin(), this->ids.begin() + this->size(),
                            this->ids.begin() + this->size() + offset - 1 );
        std::fill( this->ids.begin(), this->ids.begin() + offset, 0 );
      }

      /* === DATA MEMBERS === */
      container_type ids;
      lid_type id_min;
      lid_type id_max;
    };  /* --- end of template class DenseBase --- */
  }  /* --- end of namespace coordinate --- */

  template< typename TGraph, typename TSpec, typename ...TArgs >
  struct Coordinate;

  template< typename TGraph, typename ...TArgs >
  struct Coordinate< TGraph, coordinate::None, TArgs... > {
    using type = coordinate::NoneBase< typename TGraph::id_type, TArgs... >;
  };

  template< typename TGraph, typename ...TArgs >
  struct Coordinate< TGraph, coordinate::Identity, TArgs... > {
    using type = coordinate::IdentityBase< typename TGraph::id_type, TArgs... >;
  };

  template< typename TGraph, typename ...TArgs >
  struct Coordinate< TGraph, coordinate::Stoid, TArgs... > {
    using type = coordinate::StoidBase< typename TGraph::id_type, TArgs... >;
  };

  template< typename TGraph, typename ...TArgs >
  struct Coordinate< TGraph, coordinate::Sparse, TArgs... > {
    using type = coordinate::SparseBase< typename TGraph::id_type, TArgs... >;
  };

  template< typename TGraph, typename ...TArgs >
  struct Coordinate< TGraph, coordinate::Dense, TArgs... > {
    using type = coordinate::DenseBase< typename TGraph::id_type, TArgs... >;
  };

  template< typename ...TArgs >
  using CoordinateType = typename Coordinate< TArgs... >::type;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_COORDINATE_HPP__ --- */
