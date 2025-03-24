/**
 *    @file  graph_prop_succinct.hpp
 *   @brief  `GraphProperty` Succinct specification
 *
 *  This header file includes Succinct specification of `GraphProperty` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  20:32
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_GRAPH_PROP_SUCCINCT_HPP__
#define GUM_GRAPH_PROP_SUCCINCT_HPP__

#include "coordinate.hpp"
#include "digraph_traits_succinct.hpp"
#include "graph_prop_base.hpp"


namespace gum {
  /**
   *  @brief  Succinct graph property trait.
   *
   *  In the graph property class, paths are stored as an integer vector.
   *
   *  PROPERTY := {PATH, ...}
   *  PATH := {HEADER, NODES}
   *  HEADER := {id, plen, NAME}
   *  NAME := {start, nlen}
   *  NODES := {node, ...}
   *
   *  id: integer      // path ID
   *  plen: integer    // path length
   *  start: integer   // starting position of the path name
   *  nlen: integer    // length of the path name
   *  node: integer    // encoded value of node ID and orientation
   */
  template< typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait< Succinct, TDir, TWidths... > {
  private:
    using spec_type = Succinct;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;
    using stringsize_type = typename string_type::size_type;
    using value_type = typename common_type::type;
    using container_type = sdsl::int_vector< common_type::value >;
    using size_type = typename container_type::size_type;
    using bv_type = sdsl::bit_vector;
    using rank_map_type = typename bv_type::rank_1_type;
    using id_map_type = typename bv_type::select_1_type;

    class Path : public PathBase< id_type > {
    public:
      /* === TYPEDEFS === */
      using base_type = PathBase< id_type >;
      using typename base_type::value_type;
      using const_reference = typename container_type::const_reference;
      using const_iterator = typename container_type::const_iterator;

      /* === LIFECYCLE === */
      Path( id_type id_,
            typename string_type::const_iterator name_itr_, stringsize_type name_len_,
            const_iterator nodes_itr_, size_type nodes_len_ )
        : id( id_ ), name_itr( name_itr_ ), name_len( name_len_ ),
          nodes_itr( nodes_itr_ ), nodes_len( nodes_len_ )
      { }

      /* === ACCESSORS === */
      inline id_type
      get_id( ) const
      {
        return this->id;
      }

      inline string_type
      get_name( ) const
      {
        string_type name( this->name_len, '\0' );
        std::copy( this->name_itr, this->name_itr+this->name_len, name.begin() );
        return name;
      }

      inline container_type
      get_nodes( ) const
      {
        container_type nodes( nodes_len, 0 );
        std::copy( this->nodes_itr, this->nodes_itr+this->nodes_len, nodes.begin() );
        return nodes;
      }

      /* === METHODS === */
      template< typename TCallback >
      inline void
      for_each_node( TCallback callback )
      {
        static_assert( std::is_invocable_r_v< bool, TCallback, id_type, bool >, "received a non-invocable as callback" );

        auto nodes_end = this->end();
        for ( auto it = this->begin(); it != nodes_end; ++it ) {
          callback( base_type::id_of( *it ), base_type::is_reverse( *it ) );
        }
      }

      inline const_iterator
      begin( ) const
      {
        return this->nodes_itr;
      }

      inline const_iterator
      end( ) const
      {
        return this->nodes_itr + this->nodes_len;
      }

      inline const_reference
      back( ) const
      {
        return *( this->nodes_itr + this->nodes_len - 1 );
      }

      inline const_reference
      front( ) const
      {
        return *this->nodes_itr;
      }

      inline id_type
      id_of( value_type value ) const
      {
        return base_type::id_of( value );
      }

      inline bool
      is_reverse( value_type value ) const
      {
        return base_type::is_reverse( value );
      }

      inline rank_type
      size( ) const
      {
        return this->nodes_len;
      }

    private:
      /* === DATA MEMBERS === */
      id_type id;
      typename string_type::const_iterator name_itr;
      stringsize_type name_len;
      const_iterator nodes_itr;
      size_type nodes_len;
    };  /* --- end of class Path --- */

    using path_type = Path;

    constexpr static size_type HEADER_ENTRY_LEN = 4;
    constexpr static size_type PATH_LEN_OFFSET = 1;
    constexpr static size_type NAME_POS_OFFSET = 2;
    constexpr static size_type NAME_LEN_OFFSET = 3;

    static inline rank_type
    get_path_length( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::PATH_LEN_OFFSET ];
    }

    static inline void
    set_path_length( container_type& paths, id_type id, rank_type value )
    {
      paths[ id + GraphPropertyTrait::PATH_LEN_OFFSET ] = value;
    }

    static inline stringsize_type
    get_name_position( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::NAME_POS_OFFSET ];
    }

    static inline void
    set_name_position( container_type& paths, id_type id, stringsize_type value )
    {
      paths[ id + GraphPropertyTrait::NAME_POS_OFFSET ] = value;
    }

    static inline stringsize_type
    get_name_length( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::NAME_LEN_OFFSET ];
    }

    static inline void
    set_name_length( container_type& paths, id_type id, stringsize_type value )
    {
      paths[ id + GraphPropertyTrait::NAME_LEN_OFFSET ] = value;
    }
  };  /* --- end of template class GraphPropertyTrait --- */

  /**
   *  @brief  Graph property class (succinct).
   *
   *  Represent data associated with each graph, mainly paths.
   */
  template< typename TDir, uint8_t ...TWidths >
  class GraphProperty< Succinct, TDir, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Succinct;
    using dir_type = TDir;
    using trait_type = GraphPropertyTrait< spec_type, dir_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;
    using stringsize_type = typename trait_type::stringsize_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;
    using size_type = typename trait_type::size_type;
    using bv_type = typename trait_type::bv_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using id_map_type = typename trait_type::id_map_type;
    using path_type = typename trait_type::path_type;
    using dynamic_type = GraphProperty< Dynamic, dir_type, TWidths... >;
    using succinct_type = GraphProperty;

    /* === LIFECYCLE === */
    GraphProperty( )
      : path_count( 0 ),
        paths( container_type( 1, 0 ) ),
        ids_bv( bv_type( 1, 0 ) )
    {
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
    }

    template < typename TCoordinate = coordinate::IdentityBase< id_type > >
    GraphProperty( dynamic_type const& other, TCoordinate&& coord={} )
    {
      this->construct( other, coord );
    }

    /* copy constructor */
    GraphProperty( GraphProperty const& other )
      : path_count( other.path_count ),
        paths( other.paths ),
        ids_bv( other.ids_bv )
    {
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
    }

    /* move constructor */
    GraphProperty( GraphProperty&& other ) noexcept
      : path_count( other.path_count ),
        paths( std::move( other.paths ) ),
        ids_bv( std::move( other.ids_bv ) )
    {
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
    }

    ~GraphProperty() noexcept
    {
      sdsl::util::clear( this->path_rank );
      sdsl::util::clear( this->path_id );
    }

    /* === ACCESSORS === */
    inline rank_type
    get_path_count( ) const
    {
      return this->path_count;
    }

    /* === OPERATORS === */
    /* copy assignment operator */
    GraphProperty&
    operator=( GraphProperty const& other )
    {
      this->path_count = other.path_count;
      this->paths = other.paths;
      this->ids_bv = other.ids_bv;
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
      return *this;
    }

    /* move assignment operator */
    GraphProperty&
    operator=( GraphProperty&& other ) noexcept
    {
      this->path_count = other.path_count;
      this->paths = std::move( other.paths );
      this->ids_bv = std::move( other.ids_bv );
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
      return *this;
    }

    GraphProperty&
    operator=( dynamic_type const& other )
    {
      this->construct( other );
      return *this;
    }

    /* === METHODS === */
    /**
     *  @brief  Apply the given coordinate system.
     *
     *  Replace each node `id` in all paths with `coord( id )`.
     *
     *  @param  coord Given coordinate system.
     */
    template< typename TCoordinate >
    inline void
    apply_coordinate( TCoordinate&& coord )
    {
      this->for_each_path(
          [&]( auto rank, auto id ) {
            auto pos = this->nodes_pos( id );
            auto path = this->path( id );
            for ( auto const& node : path ) {
              this->paths[ pos++ ] = path.encode( coord( path.id_of( node ) ),
                                                  path.is_reverse( node ) );
            }
            return true;
          });
    }

    /**
     *  @brief  Return the rank of a path by its ID.
     *
     *  NOTE: This function assumes that path ID exists in the graph, otherwise
     *  the behaviour is undefined. The path ID can be verified by `has_path`
     *  method before calling this one.
     *
     *  @param  id A path ID.
     *  @return The corresponding path rank.
     */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      assert( this->has_path( id ) );
      return this->path_rank( id );
    }

    /**
     *  @brief  Return the ID of a path by its rank.
     *
     *  NOTE: This function assumes that path rank is within the range
     *  [1, path_count], otherwise the behaviour is undefined. The path rank
     *  should be verified beforehand.
     *
     *  @param  id A path rank.
     *  @return The corresponding path ID.
     */
    inline id_type
    rank_to_id( rank_type rank ) const
    {
      assert( 0 < rank && rank <= this->path_count );
      return this->path_id( rank ) + 1;
    }

    /**
     *  @brief  Return the ID of the successor path in rank.
     *
     *  @param  id A path id.
     *  @return The path ID of the successor path of a path whose ID is `id` in the rank.
     */
    inline id_type
    successor_id( id_type id ) const
    {
      assert( this->has_path( id ) );
      id += this->path_entry_len( id );
      return static_cast< size_type >( id ) < this->paths.size() ? id : 0;
    }

    inline bool
    has_path( id_type id ) const
    {
      if ( id <= 0 || static_cast<size_type>( id ) >= this->paths.size() ) return false;
      return this->ids_bv[ id - 1 ] == 1;
    }

    template< typename TCallback >
    inline bool
    for_each_path( TCallback callback,
                   rank_type rank=1 ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, rank_type, id_type >, "received a non-invocable as callback" );

      id_type id = ( this->get_path_count() < rank ) ? 0 : this->rank_to_id( rank );
      while ( id != 0 ) {
        if ( !callback( rank, id ) ) return false;
        id = this->successor_id( id );
        ++rank;
      }
      return true;
    }

    inline rank_type
    path_length( id_type id ) const
    {
      assert( this->has_path( id ) );
      return trait_type::get_path_length( this->paths, id );
    }

    inline string_type
    path_name( id_type id ) const
    {
      assert( this->has_path( id ) );
      auto namelen = this->get_name_length( id );
      string_type name( namelen, '\0' );
      auto begin = this->names.begin() + this->get_name_position( id );
      auto end = begin + namelen;
      std::copy( begin, end, name.begin() );
      return name;
    }

    inline path_type
    path( id_type id ) const
    {
      assert( this->has_path( id ) );
      return path_type( id,
                        this->names.begin() + this->get_name_position( id ),
                        this->get_name_length( id ),
                        this->paths.begin() + this->nodes_pos( id ),
                        this->path_length( id ) );
    }

    inline void
    clear( )
    {
      this->path_count = 0;
      this->paths.resize( 0 );
      sdsl::util::clear( this->ids_bv );
      sdsl::util::clear( this->path_rank );
      sdsl::util::clear( this->path_id );
      this->names.clear();
    }

  protected:
    /* === METHODS === */
    inline size_type
    header_entry_len( ) const
    {
      return trait_type::HEADER_ENTRY_LEN;
    }

    inline size_type
    path_entry_len( id_type id ) const
    {
      return this->header_entry_len() + this->path_length( id );
    }

    inline size_type
    int_vector_len( rank_type nof_nodes ) const
    {
      return this->path_count * this->header_entry_len( ) +
          nof_nodes +
          1 /* the first dummy entry */;
    }

    inline size_type
    nodes_pos( id_type id ) const
    {
      return id + this->header_entry_len();
    }

    inline void
    set_path_length( id_type id, rank_type value )
    {
      trait_type::set_path_length( this->paths, id, value );
    }

    inline stringsize_type
    get_name_position( id_type id ) const
    {
      return trait_type::get_name_position( this->paths, id );
    }

    inline void
    set_name_position( id_type id, stringsize_type value )
    {
      trait_type::set_name_position( this->paths, id, value );
    }

    inline stringsize_type
    get_name_length( id_type id ) const
    {
      return trait_type::get_name_length( this->paths, id );
    }

    inline void
    set_name_length( id_type id, stringsize_type value )
    {
      trait_type::set_name_length( this->paths, id, value );
    }

  private:
    /* === DATA MEMBERS === */
    rank_type path_count;
    container_type paths;
    bv_type ids_bv;
    rank_map_type path_rank;
    id_map_type path_id;
    string_type names;

    /* === METHODS === */
    inline rank_type
    total_nof_nodes( dynamic_type const& other ) const
    {
      rank_type nof_nodes = 0;
      for ( auto const& path : other.get_paths() ) {
        nof_nodes += path.size();
      }
      return nof_nodes;
    }

    template< typename TCoordinate = coordinate::IdentityBase< id_type > >
    inline void
    construct( dynamic_type const& other, TCoordinate&& coord={} )
    {
      this->path_count = other.get_path_count();
      auto len = this->int_vector_len( this->total_nof_nodes( other ) );
      sdsl::util::assign( this->paths, container_type( len, 0 ) );
      sdsl::util::assign( this->ids_bv, bv_type( len, 0 ) );
      this->names = "";
      size_type pos = 1;  // Leave the first entry as dummy.

      for ( auto const& path : other.get_paths() ) {
        // Set the bit at index `pos - 1` denoting the start of a node record.
        this->ids_bv[ pos - 1 ] = 1;
        // Fill out the path ID.
        id_type id = static_cast< id_type >( pos );
        this->paths[ pos ] = id;
        this->set_path_length( id, path.size() );
        auto old_size = this->names.size();
        this->names += path.get_name();
        this->set_name_position( id, old_size );
        this->set_name_length( id, this->names.size() - old_size );
        pos = this->nodes_pos( id );
        for ( auto const& node : path ) {
          this->paths[ pos++ ] = path.encode( coord( path.id_of( node ) ),
                                              path.is_reverse( node ) );
        }
      }
      sdsl::util::init_support( this->path_rank, &this->ids_bv );
      sdsl::util::init_support( this->path_id, &this->ids_bv );
    }
  };  /* --- end of template class GraphProperty --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_GRAPH_PROP_SUCCINCT_HPP__ --- */
