/**
 *    @file  digraph_succinct.hpp
 *   @brief  Definitions for Succinct `DirectedGraph`
 *
 *  This header file includes definitions for Succinct specification of
 *  `DirectedGraph` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  19:07
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_SUCCINCT_HPP__
#define GUM_DIGRAPH_SUCCINCT_HPP__

#include "digraph_traits_succinct.hpp"
#include "digraph_base.hpp"
#include "coordinate.hpp"


namespace gum {
  /**
   *  @brief  Bidirected graph class (succinct).
   *
   *  Represent the connectivity of a bidirected graph in a succinct way. It is
   *  implemented as a locally traversable graph -- an idea inspired by xg graphs
   *  in vg <https://github.com/vgteam/vg> with some slight modifications. The
   *  goal is to keep the amount of required memory for succinct graph as close as
   *  to the theoretical lower bound while it supports efficient traversal
   *  operations.
   *
   *  NOTE: In contrast with xg graphs, the IDs in these graphs are handles. So,
   *  they can be used for efficient traversal directly.
   */
  template< typename TDir, typename TCoordSpec, uint8_t ...TWidths >
  class DirectedGraph< Succinct, TDir, TCoordSpec, TWidths... > {
  public:
    /* === TYPEDEFS  === */
    using spec_type = Succinct;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using value_type = typename trait_type::value_type;
    using nodes_type = typename trait_type::nodes_type;
    using size_type = typename trait_type::size_type;
    using rank_type = typename trait_type::rank_type;
    using bv_type = typename trait_type::bv_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using id_map_type = typename trait_type::id_map_type;
    using string_type = typename trait_type::string_type;
    using padding_type = typename trait_type::padding_type;
    using side_type = typename trait_type::side_type;
    using link_type = typename trait_type::link_type;
    using linktype_type = typename trait_type::linktype_type;
    using adjs_type = typename trait_type::adjs_type;
    using coordspec_type = std::conditional_t< std::is_same< TCoordSpec, void >::value,
                                               coordinate::Dense, TCoordSpec >;
    using coordinate_type = CoordinateType< DirectedGraph, coordspec_type >;

    template< typename TCSpec = void >
    using dynamic_template = DirectedGraph< Dynamic, dir_type, TCSpec, TWidths... >;

    template< typename TCSpec = coordspec_type >
    using succinct_template = DirectedGraph< Succinct, dir_type, TCSpec, TWidths... >;

    using dynamic_type = dynamic_template<>;
    using succinct_type = succinct_template<>;

    /* === LIFECYCLE  === */
    DirectedGraph( padding_type npadding = 0, padding_type epadding = 0 )
      : np_padding( npadding ),
        ep_padding( epadding ),
        node_count( 0 ),
        edge_count( 0 ),
        nodes( nodes_type( 1, 0 ) ),
        ids_bv( bv_type( 1, 0 ) )
    {
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
    }

    template< typename TCSpec >
    DirectedGraph( dynamic_template< TCSpec > const& d_graph,
                   padding_type npadding = 0,
                   padding_type epadding = 0 )
      : np_padding( npadding ),
        ep_padding( epadding )
    {
      this->construct( d_graph );
    }

    /* copy constructor */
    DirectedGraph( DirectedGraph const& other )
      : np_padding( other.np_padding ),
        ep_padding( other.ep_padding ),
        node_count( other.node_count ),
        edge_count( other.edge_count ),
        nodes( other.nodes ),
        ids_bv( other.ids_bv )
    {
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
    }

    /* move constructor */
    DirectedGraph( DirectedGraph&& other ) noexcept
      : np_padding( other.np_padding ),
        ep_padding( other.ep_padding ),
        node_count( other.node_count ),
        edge_count( other.edge_count ),
        nodes( std::move( other.nodes ) ),
        ids_bv( std::move( other.ids_bv ) )
    {
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
      sdsl::util::clear( other.node_rank );
      sdsl::util::clear( other.node_id );
    }

    /* destructor       */
    ~DirectedGraph() noexcept
    {
      sdsl::util::clear( this->node_rank );
      sdsl::util::clear( this->node_id );
    }

    /* === ACCESSORS === */
    inline rank_type
    get_node_count( ) const
    {
      return this->node_count;
    }

    inline rank_type
    get_edge_count( ) const
    {
      return this->edge_count;
    }

    inline coordinate_type const&
    get_coordinate( ) const
    {
      return this->coordinate;
    }

    /* === OPERATORS === */
    /* copy assignment operator */
    DirectedGraph&
    operator=( DirectedGraph const& other )
    {
      this->np_padding = other.np_padding;
      this->ep_padding = other.ep_padding;
      this->node_count = other.node_count;
      this->edge_count = other.edge_count;
      this->nodes = other.nodes;
      this->ids_bv = other.ids_bv;
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
    }

    /* move assignment operator */
    DirectedGraph&
    operator=( DirectedGraph&& other ) noexcept
    {
      this->np_padding = other.np_padding;
      this->ep_padding = other.ep_padding;
      this->node_count = other.node_count;
      this->edge_count = other.edge_count;
      this->nodes = std::move( other.nodes );
      this->ids_bv = std::move( other.ids_bv );
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
      sdsl::util::clear( other.node_rank );
      sdsl::util::clear( other.node_id );
    }

    template< typename TCSpec >
    DirectedGraph&
    operator=( dynamic_template< TCSpec > const& d_graph )
    {
      this->construct( d_graph );
      return *this;
    }

    /* === METHODS === */
    /**
     *  @brief  Return the rank of a node by its ID.
     *
     *  NOTE: This function assumes that node ID exists in the graph, otherwise
     *  the behaviour is undefined. The node ID can be verified by `has_node`
     *  method before calling this one.
     *
     *  @param  id A node ID.
     *  @return The corresponding node rank.
     */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      assert( this->has_node( id ) );
      return this->node_rank( id );
    }

    /**
     *  @brief  Return the ID of a node by its rank.
     *
     *  NOTE: This function assumes that node rank is within the range
     *  [1, node_count], otherwise the behaviour is undefined. The node rank
     *  should be verified beforehand.
     *
     *  @param  rank A node rank.
     *  @return The corresponding node ID.
     */
    inline id_type
    rank_to_id( rank_type rank ) const
    {
      assert( 0 < rank && rank <= this->node_count );
      return this->node_id( rank ) + 1;
    }

    /**
     *  @brief  Return the embedded coordinate ID of a node by its internal ID.
     *
     *  NOTE: This function assumes that node ID exists in the graph, otherwise
     *  the behaviour is undefined. The node ID can be verified by `has_node`
     *  method before calling this one.
     *
     *  @param  id A node ID.
     *  @return The corresponding node embedded coordinate ID.
     */
    inline id_type
    coordinate_id( id_type id ) const
    {
      assert( this->has_node( id ) );
      return this->nodes[ id ];
    }

    /**
     *  @brief  Return the internal ID of a node by its external coordinate ID.
     *
     *  @param  ext_id node ID in the coordinate system.
     *  @return The corresponding node ID in the graph.
     */
    inline id_type
    id_by_coordinate( typename coordinate_type::lid_type const& ext_id ) const
    {
      return this->coordinate( ext_id );
    }

    /**
     *  @brief  Return the ID of the successor node in rank.
     *
     *  @param  id A node id.
     *  @return The node ID of the successor node of a node whose ID is `id` in the rank.
     */
    inline id_type
    successor_id( id_type id ) const
    {
      assert( this->has_node( id ) );
      id += this->node_entry_len( id );
      return static_cast< size_type >( id ) < this->nodes.size() ? id : 0;
    }

    inline bool
    has_node( id_type id ) const
    {
      if ( id <= 0 || static_cast<size_type>( id ) >= this->nodes.size() ) return false;
      return this->ids_bv[ id - 1 ] == 1;
    }

    inline bool
    has_node( side_type side ) const
    {
      return this->has_node( this->id_of( side ) );
    }

    /**
     *  @brief  Call a callback on each nodes in rank order.
     *
     *  @param  callback The callback function.
     *  @return `true` if it has iterated over all nodes, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    template < typename TCallback >
    inline bool
    for_each_node( TCallback callback,
                   rank_type rank=1 ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, rank_type, id_type >, "received a non-invocable as callback" );

      id_type id = ( this->get_node_count() < rank ) ? 0 : this->rank_to_id( rank );
      while ( id != 0 ) {
        if ( !callback( rank, id ) ) return false;
        id = this->successor_id( id );
        ++rank;
      }
      return true;
    }

    constexpr inline id_type
    from_id( link_type sides ) const
    {
      return trait_type::from_id( sides );
    }

    constexpr inline id_type
    to_id( link_type sides ) const
    {
      return trait_type::to_id( sides );
    }

    constexpr inline id_type
    id_of( side_type side ) const
    {
      return trait_type::id_of( side );
    }

    constexpr inline side_type
    from_side( link_type sides ) const
    {
      return trait_type::from_side( sides );
    }

    constexpr inline side_type
    from_side( id_type id, linktype_type type=trait_type::get_default_linktype() ) const
    {
      return trait_type::from_side( id, type );
    }

    constexpr inline side_type
    to_side( link_type sides ) const
    {
      return trait_type::to_side( sides );
    }

    constexpr inline side_type
    to_side( id_type id, linktype_type type=trait_type::get_default_linktype() ) const
    {
      return trait_type::to_side( id, type );
    }

    constexpr inline side_type
    start_side( id_type id ) const
    {
      return trait_type::start_side( id );
    }

    constexpr inline side_type
    end_side( id_type id ) const
    {
      return trait_type::end_side( id );
    }

    constexpr inline bool
    is_start_side( side_type side ) const
    {
      return trait_type::is_start_side( side );
    }

    constexpr inline bool
    is_end_side( side_type side ) const
    {
      return trait_type::is_end_side( side );
    }

    constexpr inline side_type
    opposite_side( side_type side ) const
    {
      return trait_type::opposite_side( side );
    }

    template< typename TCallback >
    inline bool
    for_each_side( id_type id, TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

      return trait_type::for_each_side( id, callback );
    }

    constexpr inline link_type
    make_link( side_type from, side_type to ) const
    {
      return trait_type::make_link( from, to );
    }

    constexpr inline link_type
    make_link( id_type from, id_type to,
               linktype_type type=trait_type::get_default_linktype() ) const
    {
      return trait_type::make_link( from, to, type );
    }

    constexpr inline linktype_type
    get_default_linktype( ) const
    {
      return trait_type::get_default_linktype();
    }

    constexpr inline linktype_type
    linktype( side_type from, side_type to ) const
    {
      return trait_type::linktype( from, to );
    }

    constexpr inline linktype_type
    linktype( link_type sides ) const
    {
      return trait_type::linktype( sides );
    }

    constexpr inline bool
    is_from_start( link_type sides ) const
    {
      return trait_type::is_from_start( sides );
    }

    constexpr inline bool
    is_from_start( linktype_type type ) const
    {
      return trait_type::is_from_start( type );
    }

    constexpr inline bool
    is_to_end( link_type sides ) const
    {
      return trait_type::is_to_end( sides );
    }

    constexpr inline bool
    is_to_end( linktype_type type ) const
    {
      return trait_type::is_to_end( type );
    }

    constexpr inline bool
    is_valid( linktype_type type ) const
    {
      return trait_type::is_valid( type );
    }

    constexpr inline bool
    is_valid_from( side_type from, linktype_type type ) const
    {
      return trait_type::is_valid_from( from, type );
    }

    constexpr inline bool
    is_valid_to( side_type to, linktype_type type ) const
    {
      return trait_type::is_valid_to( to, type );
    }

    inline bool
    has_edge( id_type from, id_type to, linktype_type type=trait_type::get_default_linktype() ) const
    {
      if ( !this->has_node( from ) || !this->has_node( to ) ) return false;
      auto fod = this->outdegree( from );
      auto tod = this->indegree( to );
      auto findto =
          [to, type]( id_type tid, linktype_type ttype ) {
            if ( tid == to && ttype == type ) return false;
            return true;
          };
      auto findfrom =
          [from, type]( id_type fid, linktype_type ftype ) {
            if ( fid == from && ftype == type ) return false;
            return true;
          };
      if ( fod < tod ) return !this->for_each_edges_out( from, findto );
      else return !this->for_each_edges_in( to, findfrom );
    }

    inline bool
    has_edge( side_type from, side_type to ) const
    {
      return this->has_edge( this->id_of( from ), this->id_of( to ),
                             this->linktype( from, to ) );
    }

    inline bool
    has_edge( link_type sides ) const
    {
      return this->has_edge( this->from_id( sides ), this->to_id( sides ),
                             this->linktype( sides ) );
    }

    inline adjs_type
    adjacents_out( side_type from ) const
    {
      adjs_type adjs;
      // Getting outdegree of node (rather than side) is faster, although not exact.
      adjs.reserve( this->outdegree( this->id_of( from ) ) );
      this->for_each_edges_out(
          from,
          [&adjs]( side_type to ) {
            adjs.push_back( to );
            return true;
          } );
      return adjs;
    }

    inline adjs_type
    adjacents_in( side_type to ) const
    {
      adjs_type adjs;
      // Getting outdegree of node (rather than side) is faster, although not exact.
      adjs.reserve( this->indegree( this->id_of( to ) ) );
      this->for_each_edges_in(
          to,
          [&adjs]( side_type from ) {
            adjs.push_back( from );
            return true;
          } );
      return adjs;
    }

    /**
     *  @brief  Call a `callback` on each outgoing edges from `from` side.
     *
     *  The `callback` function should get the outgoing side and return `true`
     *  to continue the iteration, and `false` to stop it.
     *
     *  @param  from  The side whose outgoing edges are considered.
     *  @param  callback  The `callback` function.
     *  @return `true` if it has iterated over all edges, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    template< typename TCallback >
    inline bool
    for_each_edges_out( side_type from,
                        TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

      return this->for_each_edges_out(
          this->id_of( from ),
          [this, from, callback]( id_type id, linktype_type type ) {
            if ( !this->is_valid_from( from, type ) ) return true;
            if ( !callback( this->to_side( id, type ) ) ) return false;
            return true;
          }
        );
    }

    /**
     *  @brief  Call a `callback` on each outgoing edges from each side of a node.
     *
     *  The `callback` function should get the outgoing node ID and the edge
     *  type; and return `true` to continue the iteration, and `false` to stop
     *  it.
     *
     *  @param  id  The node ID whose outgoing edges are considered.
     *  @param  callback  The `callback` function.
     *  @return `true` if it has iterated over all edges, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    template< typename TCallback >
    inline bool
    for_each_edges_out( id_type id,
                        TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, id_type, linktype_type >, "received a non-invocable as callback" );

      if ( !this->has_edges_out( id ) ) return true;
      return this->for_each_edges_out_pos(
          id,
          [this, callback]( size_type pos ) {
            return callback( this->get_adj_id( pos ),
                             this->get_adj_linktype( pos ) );
          }
        );
    }

    /**
     *  @brief  Call a `callback` on each incoming edges to `to` side.
     *
     *  The `callback` function should get the incoming side and return `true`
     *  to continue the iteration, and `false` to stop it.
     *
     *  @param  to  The side whose incoming edges are considered.
     *  @param  callback  The `callback` function.
     *  @return `true` if it has iterated over all edges, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    template< typename TCallback >
    inline bool
    for_each_edges_in( side_type to,
                       TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

      return this->for_each_edges_in(
          this->id_of( to ),
          [this, to, callback]( id_type id, linktype_type type ) {
            if ( !this->is_valid_to( to, type ) ) return true;
            if ( !callback( this->from_side( id, type ) ) ) return false;
            return true;
          }
        );
    }

    /**
     *  @brief  Call a `callback` on each incoming edges to each side of a node.
     *
     *  The `callback` function should get the incoming node ID and the edge
     *  type; and return `true` to continue the iteration, and `false` to stop
     *  it.
     *
     *  @param  id  The node ID whose incoming edges are considered.
     *  @param  callback  The `callback` function.
     *  @return `true` if it has iterated over all edges, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    template< typename TCallback >
    inline bool
    for_each_edges_in( id_type id,
                       TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, id_type, linktype_type >, "received a non-invocable as callback" );

      if ( !this->has_edges_in( id ) ) return true;
      return this->for_each_edges_in_pos(
          id,
          [this, callback]( size_type pos ) {
            return callback( this->get_adj_id( pos ),
                             this->get_adj_linktype( pos ) );
          }
        );
    }

    inline rank_type
    outdegree( id_type id ) const
    {
      assert( this->has_node( id ) );
      return trait_type::get_outdegree( nodes, id );
    }

    inline rank_type
    outdegree( side_type side ) const
    {
      rank_type retval = 0;
      this->for_each_edges_out(
          side,
          [&retval]( side_type ) {
            ++retval;
            return true;
          }
        );
      return retval;
    }

    inline rank_type
    indegree( id_type id ) const
    {
      assert( this->has_node( id ) );
      return trait_type::get_indegree( this->nodes, id );
    }

    inline rank_type
    indegree( side_type side ) const
    {
      rank_type retval = 0;
      this->for_each_edges_in(
          side,
          [&retval]( side_type ) {
            ++retval;
            return true;
          }
        );
      return retval;
    }

    inline bool
    has_edges_in( side_type side ) const
    {
      return this->indegree( side ) != 0;
    }

    inline bool
    has_edges_in( id_type id ) const
    {
      return this->indegree( id ) != 0;
    }

    inline bool
    has_edges_out( side_type side ) const
    {
      return this->outdegree( side ) != 0;
    }

    inline bool
    has_edges_out( id_type id ) const
    {
      return this->outdegree( id ) != 0;
    }

    inline bool
    is_branch( id_type id ) const
    {
      return this->outdegree( id ) > 1;
    }

    inline bool
    is_branch( side_type side ) const
    {
      return this->outdegree( side ) > 1;
    }

    inline bool
    is_merge( id_type id ) const
    {
      return this->indegree( id ) > 1;
    }

    inline bool
    is_merge( side_type side ) const
    {
      return this->indegree( side ) > 1;
    }

    inline void
    clear( )
    {
      this->node_count = 0;
      this->edge_count = 0;
      this->nodes.resize( 1 );
      this->nodes[ 0 ] = 0;
      this->ids_bv.resize( 1 );
      this->ids_bv[ 0 ] = 0;
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
    }

  protected:
    /* === ACCESSORS === */
    inline coordinate_type&
    get_coordinate( )
    {
      return this->coordinate;
    }

    /* === METHODS === */
    inline size_type
    header_core_len( ) const
    {
      return trait_type::HEADER_CORE_LEN;
    }

    inline size_type
    header_entry_len( ) const
    {
      return this->header_core_len() + this->np_padding;
    }

    inline size_type
    edge_core_len( ) const
    {
      return trait_type::EDGE_CORE_LEN;
    }

    inline size_type
    edge_entry_len( ) const
    {
      return this->edge_core_len() + this->ep_padding;
    }

    inline size_type
    node_entry_len( id_type id ) const
    {
      return this->header_entry_len() +
          ( this->outdegree( id ) + this->indegree( id ) ) * this->edge_entry_len();
    }

    inline size_type
    int_vector_len( ) const
    {
      return this->get_node_count() * this->header_entry_len() +
          2 * this->get_edge_count() * this->edge_entry_len() +
          1 /* the first dummy entry */;
    }

    inline size_type
    edges_out_pos( id_type id ) const
    {
      return id + this->header_entry_len();
    }

    inline size_type
    edges_in_pos( id_type id ) const
    {
      return this->edges_out_pos( id ) +
          this->outdegree( id ) * this->edge_entry_len();
    }

    template< typename TCallback >
    inline bool
    for_each_edges_out_pos( id_type id,
                            TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, size_type >, "received a non-invocable as callback" );

      size_type pos = this->edges_out_pos( id );
      for ( rank_type i = 0; i < this->outdegree( id ); ++i ) {
        if ( !callback( pos ) ) return false;
        pos += this->edge_entry_len();
      }
      return true;
    }

    template< typename TCallback >
    inline bool
    for_each_edges_in_pos( id_type id,
                            TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, size_type >, "received a non-invocable as callback" );

      size_type pos = this->edges_in_pos( id );
      for ( rank_type i = 0; i < this->indegree( id ); ++i ) {
        if ( !callback( pos ) ) return false;
        pos += this->edge_entry_len();
      }
      return true;
    }

    inline value_type
    get_nodes_at( size_type pos ) const
    {
      return this->nodes[ pos ];
    }

    inline void
    set_nodes_at( size_type pos, value_type value )
    {
      this->nodes[ pos ] = value;
    }

    inline void
    set_outdegree( id_type id, rank_type value )
    {
      trait_type::set_outdegree( this->nodes, id, value );
    }

    inline void
    set_indegree( id_type id, rank_type value )
    {
      trait_type::set_indegree( this->nodes, id, value );
    }

    inline id_type
    get_adj_id( size_type pos ) const
    {
      return trait_type::get_adj_id( this->nodes, pos );
    }

    inline void
    set_adj_id( size_type pos, id_type value )
    {
      trait_type::set_adj_id( this->nodes, pos, value );
    }

    inline linktype_type
    get_adj_linktype( size_type pos ) const
    {
      return trait_type::get_adj_linktype( this->nodes, pos );
    }

    inline void
    set_adj_linktype( size_type pos, linktype_type value )
    {
      trait_type::set_adj_linktype( this->nodes, pos, value );
    }

  private:
    /* === DATA MEMBERS === */
    padding_type np_padding;
    padding_type ep_padding;
    rank_type node_count;
    rank_type edge_count;
    nodes_type nodes;
    bv_type ids_bv;
    rank_map_type node_rank;
    id_map_type node_id;
    coordinate_type coordinate;

    /* === METHODS === */
    /**
    *  @brief  Construct the succinct graph from the Dynamic one.
    *
    *  The construction includes filling out node entries and its corresponding bit
    *  vector (`this->nodes` and `this->ids_bv`) and initializing the rank/select
    *  supports for the bit vector. Afterwards, the nodes will be identified
    *  (node IDs will be assigned).
    *
    *  @param  d_graph A Dynamic graph.
    *
    *  NOTE: This function assumes that the node and edge paddings has been
    *  initialiased.
    */
    template< typename TCSpec >
    inline void
    construct( dynamic_template< TCSpec > const& d_graph )
    {
      this->node_count = d_graph.get_node_count();
      this->edge_count = d_graph.get_edge_count();
      sdsl::util::assign( this->nodes, nodes_type( this->int_vector_len(), 0 ) );
      sdsl::util::assign( this->ids_bv, bv_type( this->int_vector_len(), 0 ) );
      size_type pos = 1;  // Leave the first entry as dummy.
      for ( rank_type rank = 1; rank <= d_graph.get_node_count(); ++rank ) {
        id_type d_id = d_graph.rank_to_id( rank );
        // Set the bit at index `pos - 1` denoting the start of a node record.
        this->ids_bv[ pos - 1 ] = 1;
        // Embed the coordinate system of `Dynamic` graph.
        id_type id = static_cast< id_type >( pos );
        this->nodes[ pos ] = d_id;
        this->coordinate( d_id, id );
        this->set_outdegree( pos, d_graph.outdegree( d_id ) );
        this->set_indegree( pos, d_graph.indegree( d_id ));
        // Add EDGES_OUT and EDGES_IN entries
        this->fill_edges_entries( d_graph, d_id, id );
        // Get the next node entry position.
        pos += this->node_entry_len( id );
      }
      // The `identificate` function uses rank/select supports.
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
      this->identificate( );
    }

    template< typename TCSpec >
    inline void
    fill_edges_entries( dynamic_template< TCSpec > const& d_graph,
                        id_type d_id,
                        id_type new_id )
    {
      size_type pos = this->edges_out_pos( new_id );
      d_graph.for_each_edges_out(
          d_id,
          [this, &pos, &d_graph]( id_type to, linktype_type type ) {
            // Fill out the `nodes` vector by rank in the first pass.
            this->set_adj_id( pos, d_graph.id_to_rank( to ) );
            this->set_adj_linktype( pos, type );
            pos += this->edge_entry_len();
            return true;
          } );

      pos = this->edges_in_pos( new_id );
      d_graph.for_each_edges_in(
          d_id,
          [this, &pos, &d_graph]( id_type from, linktype_type type ) {
            // Fill out the `nodes` vector by rank in the first pass.
            this->set_adj_id( pos, d_graph.id_to_rank( from ) );
            this->set_adj_linktype( pos, type );
            pos += this->edge_entry_len();
            return true;
          } );
    }

    /**
     *  @brief  Replace node ranks with node ID in the second pass.
     */
    inline void
    identificate( )
    {
      // Replace other node IDs in adjacency lists.
      this->for_each_node(
          [this]( rank_type rank, id_type id ) {
            this->for_each_edges_out_pos(
                id,
                [this]( size_type pos ) {
                  // Replace node IDs for edges to.
                  this->set_adj_id( pos, this->rank_to_id( this->get_adj_id( pos ) ) );
                  return true;
                }
              );
            this->for_each_edges_in_pos(
                id,
                [this]( size_type pos ) {
                  // Replace node IDs for edges from.
                  this->set_adj_id( pos, this->rank_to_id( this->get_adj_id( pos ) ) );
                  return true;
                }
              );
            return true;
          }
        );
    }
  };  /* --- end of template class DirectedGraph --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_SUCCINCT_HPP__ --- */
