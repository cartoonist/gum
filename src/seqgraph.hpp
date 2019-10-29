/**
 *    @file  seqgraph.hpp
 *   @brief  Sequence graph class definition
 *
 *  This header file defines `SeqGraph` class which represents a sequence graph.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Feb 21, 2019  14:24
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_SEQGRAPH_HPP__
#define  GUM_SEQGRAPH_HPP__

#include <algorithm>

#include "seqgraph_base.hpp"


namespace gum {
  /**
   *  @brief  Bidirected graph class (dynamic).
   *
   *  Represent the connectivity of a bidirected graph in a dynamic way; i.e.
   *  the connectivity can be modified after it is constructed in contrast with
   *  `succinct` specialization which is immutable.
   */
  template< typename TDir, uint8_t ...TWidths >
  class DirectedGraph< Dynamic, TDir, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using trait_type = DirectedGraphTrait< spec_type, TDir, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using nodes_type = typename trait_type::nodes_type;
    using size_type = typename trait_type::size_type;
    using rank_type = typename trait_type::rank_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using side_type = typename trait_type::side_type;
    using link_type = typename trait_type::link_type;
    using linktype_type = typename trait_type::linktype_type;
    using adjs_type = typename trait_type::adjs_type;
    using adj_map_type = typename trait_type::adj_map_type;

    /* === LIFECYCLE === */
    DirectedGraph( )                                            /* constructor      */
      : max_id( 0 ), node_count( 0 ), edge_count( 0 )
    {
      trait_type::init_rank_map( this->node_rank );
      trait_type::init_adj_map( this->adj_from );
      trait_type::init_adj_map( this->adj_to );
    }

    DirectedGraph( DirectedGraph const& other ) = default;      /* copy constructor */
    DirectedGraph( DirectedGraph&& other ) noexcept = default;  /* move constructor */
    ~DirectedGraph() noexcept = default;                        /* destructor       */

    /* === ACCESSORS === */
    inline nodes_type const&
    get_nodes( ) const
    {
      return this->nodes;
    }

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

    /* === OPERATORS === */
    DirectedGraph& operator=( DirectedGraph const& other ) = default;      /* copy assignment operator */
    DirectedGraph& operator=( DirectedGraph&& other ) noexcept = default;  /* move assignment operator */

    /* === METHODS === */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      this->check_id( id );
      auto found = this->node_rank.find( id );
      if ( found == this->node_rank.end() ) return 0;
      return found->second;
    }

    inline id_type
    rank_to_id( rank_type rank ) const
    {
      this->check_rank( rank );
      return this->nodes[ rank - 1 ];
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
      return this->rank_to_id( this->id_to_rank( id ) + 1 );
    }

    inline id_type
    add_node( )
    {
      id_type new_id = this->add_node_imp( );
      this->set_last_rank();
      return new_id;
    }

    inline void
    add_nodes( size_type count,
               std::function< void( id_type ) > callback = []{} )
    {
      for ( size_type i = 0; i < count; ++i ) callback( this->add_node_imp() );
      this->set_rank();
    }

    inline bool
    has_node( id_type id ) const
    {
      return this->id_to_rank( id ) != 0;
    }

    /**
     *  @brief  Call a callback on each nodes in rank order.
     *
     *  @param  callback The callback function.
     *  @return `true` if it has iterated over all nodes, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    inline bool
    for_each_node( std::function< bool( rank_type, id_type ) > callback ) const
    {
      rank_type rank = 1;
      for ( id_type id : this->nodes ) {
        if ( !callback( rank, id ) ) return false;
        ++rank;
      }
      return true;
    }

    inline void
    check_id( id_type id ) const
    {
      trait_type::check_id( id );
    }

    inline void
    check_rank( rank_type rank ) const
    {
      trait_type::check_rank( rank );
    }

    inline void
    check_linktype( linktype_type type ) const
    {
      trait_type::check_linktype( type );
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
    from_side( id_type id, linktype_type type=get_default_linktype() ) const
    {
      return trait_type::from_side( id, type );
    }

    constexpr inline side_type
    to_side( link_type sides ) const
    {
      return trait_type::to_side( sides );
    }

    constexpr inline side_type
    to_side( id_type id, linktype_type type=get_default_linktype() ) const
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

    constexpr inline side_type
    opposite_side( side_type side ) const
    {
      return trait_type::opposite_side( side );
    }

    inline bool
    for_each_side( id_type id, std::function< bool( side_type ) > callback ) const
    {
      return trait_type::for_each_side( id, callback );
    }

    constexpr inline link_type
    make_link( side_type from, side_type to ) const
    {
      return trait_type::make_link( from, to );
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

    inline void
    add_edge( side_type from, side_type to )
    {
      this->add_edge_imp( from, to );
    }

    inline void
    add_edge( link_type sides )
    {
      this->add_edge( this->from_side( sides ), this->to_side( sides ) );
    }

    inline bool
    has_edge( side_type from, side_type to ) const
    {
      auto oit = this->adj_to.find( from );
      auto iit = this->adj_from.find( to );
      auto const& outs = oit->second;
      auto const& ins = iit->second;
      if ( oit == this->adj_to.end() || iit == this->adj_from.end() ) return false;
      if ( outs.size() < ins.size() )
        return std::find( outs.begin(), outs.end(), to ) != outs.end();
      return std::find( ins.begin(), ins.end(), from ) != ins.end();
    }

    inline bool
    has_edge( link_type sides ) const
    {
      return this->has_edge( this->from_side( sides ), this->to_side( sides ) );
    }

    inline bool
    has_edge( id_type from, id_type to, linktype_type type=get_default_linktype() )
    {
      this->check_linktype( type );
      return this->has_edge( this->from_side( from, type ), this->to_side( to, type ) );
    }

    inline adjs_type
    adjacents_to( side_type from ) const
    {
      auto found = this->adj_to.find( from );
      if ( found == this->adj_to.end() ) return adjs_type();
      return found->second;
    }

    inline adjs_type
    adjacents_from( side_type to ) const
    {
      auto found = this->adj_from.find( to );
      if ( found == this->adj_from.end() ) return adjs_type();
      return found->second;
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
    inline bool
    for_each_edges_to( side_type from,
                       std::function< bool( side_type ) > callback ) const
    {
      auto found = this->adj_to.find( from );
      if ( found == this->adj_to.end() ) return true;
      for ( side_type to : found->second ) {
        if ( !callback( to ) ) return false;
      }
      return true;
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
    inline bool
    for_each_edges_to( id_type id,
                       std::function< bool( id_type, linktype_type ) > callback ) const
    {
      return this->for_each_side(
          id,
          [this, callback]( side_type from ) {
            auto found = this->adj_to.find( from );
            if ( found == this->adj_to.end() ) return true;
            for ( side_type to : found->second ) {
              if ( !callback( this->id_of( to ), this->linktype( from, to ) ) )
                return false;
            }
            return true;
          } );
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
    inline bool
    for_each_edges_from( side_type to,
                         std::function< bool( side_type ) > callback ) const
    {
      auto found = this->adj_from.find( to );
      if ( found == this->adj_from.end() ) return true;
      for ( side_type from : found->second ) {
        if ( !callback( from ) ) return false;
      }
      return true;
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
    inline bool
    for_each_edges_from( id_type id,
                         std::function< bool( id_type, linktype_type ) > callback ) const
    {
      return this->for_each_side(
          id,
          [this, callback]( side_type to ) {
            auto found = this->adj_from.find( to );
            if ( found == this->adj_from.end() ) return true;
            for ( side_type from : found->second ) {
              if ( !callback( this->id_of( from ), this->linktype( from, to ) ) )
                return false;
            }
            return true;
          } );
    }

    inline rank_type
    outdegree( side_type side ) const
    {
      auto found = this->adj_to.find( side );
      if ( found == this->adj_to.end() ) return 0;
      return found->second.size();
    }

    inline rank_type
    outdegree( id_type id ) const
    {
      rank_type retval = 0;
      this->for_each_side(
          id,
          [this, &retval]( side_type side ) {
            retval += this->outdegree( side );
            return true;
          }
        );
      return retval;
    }

    inline rank_type
    indegree( side_type side ) const
    {
      auto found = this->adj_from.find( side );
      if ( found == this->adj_from.end() ) return 0;
      return found->second.size();
    }

    inline rank_type
    indegree( id_type id ) const
    {
      rank_type retval = 0;
      this->for_each_side(
          id,
          [this, &retval]( side_type side ) {
            retval += this->indegree( side );
            return true;
          }
        );
      return retval;
    }

    inline bool
    has_edges_from( side_type side ) const
    {
      return this->indegree( side ) != 0;
    }

    inline bool
    has_edges_from( id_type id ) const
    {
      return this->indegree( id ) != 0;
    }

    inline bool
    has_edges_to( side_type side ) const
    {
      return this->outdegree( side ) != 0;
    }

    inline bool
    has_edges_to( id_type id ) const
    {
      return this->outdegree( id ) != 0;
    }

  protected:
    /* === ACCESSORS === */
    inline nodes_type&
    get_nodes( )
    {
      return this->nodes;
    }

    /* === METHODS === */
    inline id_type
    add_node_imp( )
    {
      this->nodes.push_back( ++this->max_id );
      return this->max_id;
    }

    inline void
    add_edge_imp( side_type from, side_type to, bool safe=true )
    {
      if ( safe && this->has_edge( from, to ) ) return;
      this->adj_to[ from ].push_back( to );
      this->adj_from[ to ].push_back( from );
      ++this->edge_count;
    }

    inline void
    add_edge_imp( link_type sides, bool safe=true )
    {
      this->add_edge_imp( this->from_side( sides ), this->to_side( sides ), safe );
    }

  private:
    /* === DATA MEMBERS === */
    nodes_type nodes;
    rank_map_type node_rank;
    adj_map_type adj_to;
    adj_map_type adj_from;
    id_type max_id;
    rank_type node_count;
    rank_type edge_count;

    /* === METHODS === */
    inline void
    set_rank( typename nodes_type::const_iterator begin,
              typename nodes_type::const_iterator end )
    {
      assert( end - begin + this->node_count == this->nodes.size() );
      for ( ; begin != end; ++begin ) {
        bool inserted;
        std::tie( std::ignore, inserted ) =
            this->node_rank.insert( { *begin, ++this->node_count } );
        assert( inserted );  // avoid duplicate insersion from upstream.
      }
    }

    inline void
    set_rank( )
    {
      this->set_rank( this->nodes.begin(), this->nodes.end() );
    }

    inline void
    set_last_rank( )
    {
      this->set_rank( this->nodes.end() - 1, this->nodes.end() );
    }
  };  /* --- end of template class DirectedGraph --- */

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
  template< typename TDir, uint8_t ...TWidths >
  class DirectedGraph< Succinct, TDir, TWidths... > {
  public:
    /* === TYPEDEFS  === */
    using spec_type = Succinct;
    using trait_type = DirectedGraphTrait< spec_type, TDir, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using nodes_type = typename trait_type::nodes_type;
    using size_type = typename trait_type::size_type;
    using rank_type = typename trait_type::rank_type;
    using bv_type = typename trait_type::bv_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using id_map_type = typename trait_type::id_map_type;
    using padding_type = typename trait_type::padding_type;
    using side_type = typename trait_type::side_type;
    using link_type = typename trait_type::link_type;
    using linktype_type = typename trait_type::linktype_type;

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

    DirectedGraph( DirectedGraph< Dynamic, TDir, TWidths... > const& d_graph,
                   padding_type npadding = 0,
                   padding_type epadding = 0)
      : np_padding( npadding ),
        ep_padding( epadding )
    {
      this->construct( d_graph );
    }

    /* copy constructor */
    DirectedGraph( DirectedGraph const& other )
      : np_padding( other.np_padding ),
        ep_padding( other.epadding ),
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
        ep_padding( other.epadding ),
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

    /* === OPERATORS === */
    /* copy assignment operator */
    DirectedGraph& operator=( DirectedGraph const& other )
    {
      this->np_padding = other.np_padding;
      this->ep_padding = other.ep_padding;
      this->node_count = other.node_count;
      this->edge_count = other.edge_count;
      this->nodes = other.edge_nodes;
      this->ids_bv = other.ids_bv;
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
    }

    /* move assignment operator */
    DirectedGraph& operator=( DirectedGraph&& other ) noexcept
    {
      this->np_padding = other.np_padding;
      this->ep_padding = other.ep_padding;
      this->node_count = other.node_count;
      this->edge_count = other.edge_count;
      this->nodes = std::move( other.edge_nodes );
      this->ids_bv = std::move( other.ids_bv );
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
      sdsl::util::clear( other.node_rank );
      sdsl::util::clear( other.node_id );
    }

    /* === METHODS === */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      this->check_id( id );
      if ( this->ids_bv[ id - 1 ] != 1 ) return 0;
      return this->node_rank[ id ];
    }

    inline id_type
    rank_to_id( rank_type rank ) const
    {
      this->check_rank( rank );
      return this->node_id[ rank ] + 1;
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
      this->check_id( id );
      return this->_successor_id( id );
    }

    inline bool
    has_node( id_type id ) const
    {
      return this->id_to_rank( id ) != 0;
    }

    /**
     *  @brief  Call a callback on each nodes in rank order.
     *
     *  @param  callback The callback function.
     *  @return `true` if it has iterated over all nodes, and `false` if the
     *  iteration has been interrupted by `callback`.
     */
    inline bool
    for_each_node( std::function< bool( rank_type, id_type ) > callback ) const
    {
      id_type id = 1;
      rank_type rank = 1;
      while ( id != 0 ) {
        if ( !callback( rank, id ) ) return false;
        id = this->_successor_id( id );
        ++rank;
      }
      return true;
    }

    inline void
    check_id( id_type id ) const
    {
      trait_type::check_id( id );
      if ( this->ids_bv[ id - 1 ] != 1 ) throw std::runtime_error( "invalid node ID" );
    }

    inline void
    check_rank( rank_type rank ) const
    {
      trait_type::check_rank( rank );
    }

    inline void
    check_linktype( linktype_type type ) const
    {
      trait_type::check_linktype( type );
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
    from_side( id_type id, linktype_type type=get_default_linktype() ) const
    {
      return trait_type::from_side( id, type );
    }

    constexpr inline side_type
    to_side( link_type sides ) const
    {
      return trait_type::to_side( sides );
    }

    constexpr inline side_type
    to_side( id_type id, linktype_type type=get_default_linktype() ) const
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

    constexpr inline side_type
    opposite_side( side_type side ) const
    {
      return trait_type::opposite_side( side );
    }

    inline bool
    for_each_side( id_type id, std::function< bool( side_type ) > callback ) const
    {
      return trait_type::for_each_side( id, callback );
    }

    constexpr inline link_type
    make_link( side_type from, side_type to ) const
    {
      return trait_type::make_link( from, to );
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
    has_edge( id_type from, id_type to, linktype_type type=get_default_linktype() ) const
    {
      this->check_linktype( type );
      auto fod = this->outdegree( from );
      auto tod = this->outdegree( to );
      auto findto =
          [to, type]( id_type tid, linktype_type ttype ) {
            if ( tid == to && ttype == type ) return false;
          };
      auto findfrom =
          [from, type]( id_type fid, linktype_type ftype ) {
            if ( fid == from && ftype == type ) return false;
          };
      if ( fod < tod ) return !this->for_each_edges_to( to, findto );
      else return !this->for_each_edges_from( from, findfrom );
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
    inline bool
    for_each_edges_to( side_type from,
                       std::function< bool( side_type ) > callback ) const
    {
      return this->for_each_edges_to(
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
    inline bool
    for_each_edges_to( id_type id,
                       std::function< bool( id_type, linktype_type ) > callback ) const
    {
      if ( !this->has_edges_to( id ) ) return true;
      return this->for_each_edges_to_pos(
          id,
          [this, callback]( size_type pos ) {
            if ( !callback( this->nodes[ pos ],
                            trait_type::get_adj_linktype( nodes, pos ) ) )
              return false;
            return true;
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
    inline bool
    for_each_edges_from( side_type to,
                         std::function< bool( side_type ) > callback ) const
    {
      return this->for_each_edges_from(
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
    inline bool
    for_each_edges_from( id_type id,
                         std::function< bool( id_type, linktype_type ) > callback ) const
    {
      if ( !this->has_edges_from( id ) ) return true;
      return this->for_each_edges_from_pos(
          id,
          [this, callback]( size_type pos ) {
            if ( !callback( this->nodes[ pos ],
                            trait_type::get_adj_linktype( nodes, pos ) ) )
              return false;
            return true;
          }
        );
    }

    inline rank_type
    outdegree( id_type id ) const
    {
      this->check_id( id );
      return this->nodes[ id + trait_type::OUTDEGREE_OFFSET ];
    }

    inline rank_type
    outdegree( side_type side ) const
    {
      rank_type retval = 0;
      this->for_each_edges_to(
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
      this->check_id( id );
      return this->nodes[ id + trait_type::INDEGREE_OFFSET ];
    }

    inline rank_type
    indegree( side_type side ) const
    {
      rank_type retval = 0;
      this->for_each_edges_from(
          side,
          [&retval]( side_type ) {
            ++retval;
            return true;
          }
        );
      return retval;
    }

    inline bool
    has_edges_from( side_type side ) const
    {
      return this->indegree( side ) != 0;
    }

    inline bool
    has_edges_from( id_type id ) const
    {
      return this->indegree( id ) != 0;
    }

    inline bool
    has_edges_to( side_type side ) const
    {
      return this->outdegree( side ) != 0;
    }

    inline bool
    has_edges_to( id_type id ) const
    {
      return this->outdegree( id ) != 0;
    }

  protected:
    /* === METHODS === */
    inline size_type
    header_entry_len( ) const
    {
      return trait_type::HEADER_CORE_LEN + this->np_padding;
    }

    inline size_type
    edge_entry_len( ) const
    {
      return trait_type::EDGE_CORE_LEN + this->ep_padding;
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
    edges_to_pos( id_type id ) const
    {
      return id + this->header_entry_len();
    }

    inline size_type
    edges_from_pos( id_type id ) const
    {
      return this->edges_to_pos( id ) +
          this->outdegree( id ) * this->edge_entry_len();
    }

    inline bool
    for_each_edges_to_pos( id_type id,
                           std::function< bool( size_type ) > callback ) const
    {
      size_type pos = this->edges_to_pos( id );
      for ( rank_type i = 0; i < this->outdegree( id ); ++i ) {
        if ( !callback( pos ) ) return false;
        pos += this->edge_entry_len();
      }
      return true;
    }

    inline bool
    for_each_edges_from_pos( id_type id,
                             std::function< bool( size_type ) > callback ) const
    {
      size_type pos = this->edges_from_pos( id );
      for ( rank_type i = 0; i < this->indegree( id ); ++i ) {
        if ( !callback( pos ) ) return false;
        pos += this->edge_entry_len();
      }
      return true;
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
    inline void
    construct( DirectedGraph< Dynamic, TDir, TWidths... > const& d_graph )
    {
      this->node_count = d_graph.get_node_count();
      this->edge_count = d_graph.get_edge_count();
      sdsl::util::assign( this->nodes, nodes_type( this->int_vector_len(), 0 ) );
      sdsl::util::assign( this->ids_bv, bv_type( this->int_vector_len(), 0 ) );
      size_type pos = 1;  // Leave the first entry as dummy.
      for ( rank_type rank = 1; rank < d_graph.get_node_count(); ++rank ) {
        id_type d_id = d_graph.rank_to_id( rank );
        // Set the bit at index `pos - 1` denoting the start of a node record.
        this->ids_bv[ pos - 1 ] = 1;
        // Fill out the `nodes` vector by rank in the first pass.
        this->nodes[ pos ] = rank;
        this->nodes[ pos + trait_type::OUTDEGREE_OFFSET ] = d_graph.outdegree( d_id );
        this->nodes[ pos + trait_type::INDEGREE_OFFSET ] = d_graph.indegree( d_id );
        // Add EDGES_TO and EDGES_FROM entries
        this->fill_edges_entries( d_graph, d_id, static_cast< id_type >( pos ) );
        // Get the next node entry position.
        pos += this->node_entry_len( static_cast< id_type >( pos ) );
      }
      // The `identificate` function uses rank/select supports.
      sdsl::util::init_support( this->node_rank, &this->ids_bv );
      sdsl::util::init_support( this->node_id, &this->ids_bv );
      this->identificate( );
    }

    inline void
    fill_edges_entries( DirectedGraph< Dynamic, TDir, TWidths... > const& d_graph,
                        id_type d_id,
                        id_type new_id )
    {
      d_graph.for_each_edges_to(
          d_id,
          [this, new_id, &d_graph]( id_type to, linktype_type type ) {
            this->for_each_edges_to_pos(
                new_id,
                [this, to, type, &d_graph]( size_type pos ) {
                  // Fill out the `nodes` vector by rank in the first pass.
                  this->nodes[ pos ] = d_graph.id_to_rank( to );
                  trait_type::set_adj_linktype( nodes, pos, type );
                  return true;
                } );
            return true;
          } );
      d_graph.for_each_edges_from(
          d_id,
          [this, new_id, &d_graph]( id_type from, linktype_type type ) {
            this->for_each_edges_from_pos(
                new_id,
                [this, from, type, &d_graph]( size_type pos ) {
                  // Fill out the `nodes` vector by rank in the first pass.
                  this-nodes[ pos ] = d_graph.id_to_rank( from );
                  trait_type::set_adj_linktype( nodes, pos, type );
                  return true;
                } );
            return true;
          } );
    }

    /**
     *  @brief  Return the ID of the successor node in rank.
     *
     *  @param  id A node id.
     *  @return The node ID of the successor node of a node whose ID is `id` in the rank.
     */
    inline id_type
    _successor_id( id_type id ) const
    {
      assert( id > 0 );
      assert( this->nodes[ id ] == id );
      id += this->node_entry_len();
      return id < this->nodes.size() ? id : 0;
    }

    /**
     *  @brief  Replace node ranks with node ID in the second pass.
     */
    inline void
    identificate( )
    {
      id_type id = 1;
      // Replace all `id` field in the headers with their indices in the nodes list.
      this->for_each_node(
          [this]( rank_type rank, id_type id ) {
            this->nodes[ id ] = id;
            return true;
          }
        );
      // Replace other node IDs in adjacency lists.
      this->for_each_node(
          [this]( rank_type rank, id_type id ) {
            this->for_each_edges_to_pos(
                id,
                [this]( size_type pos ) {
                  // Replace node IDs for edges to.
                  this->nodes[ pos ] = this->rank_to_id( this->nodes[ pos ] );
                  return true;
                }
              );
            this->for_each_edges_from_pos(
                id,
                [this]( size_type pos ) {
                  // Replace node IDs for edges from.
                  this->nodes[ pos ] = this->rank_to_id( this->nodes[ pos ] );
                  return true;
                }
              );
            return true;
          }
        );
    }
  };  /* --- end of template class DirectedGraph --- */

  /**
   *  @brief  Node property class (dynamic).
   *
   *  Represent data associated with each node, mainly node sequences. Each data
   *  structure associated with each node are stored in a container in node rank
   *  order.
   */
  template< uint8_t ...TWidths >
  class NodeProperty< Dynamic, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using trait_type = NodePropertyTrait< spec_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using node_type = typename trait_type::node_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;
    using sequence_type = typename trait_type::sequence_type;
    using string_type = typename trait_type::string_type;
    using size_type = typename trait_type::size_type;
    using const_reference = typename trait_type::const_reference;
    using const_iterator = typename trait_type::const_iterator;

    /* === LIFECYCLE === */
    /* constructor */
    NodeProperty( )
      : sequences_len_sum( 0 ), names_len_sum( 0 )
    { }

    NodeProperty( NodeProperty const& other ) = default;      /* copy constructor */
    NodeProperty( NodeProperty&& other ) noexcept = default;  /* move constructor */
    ~NodeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_nodes( ) const
    {
      return this->nodes;
    }

    inline typename sequence_type::size_type
    get_sequences_len_sum( ) const
    {
      return this->sequences_len_sum;
    }

    inline typename string_type::size_type
    get_names_len_sum( ) const
    {
      return this->names_len_sum;
    }

    /* === OPERATORS === */
    NodeProperty& operator=( NodeProperty const& other ) = default;      /* copy assignment operator */
    NodeProperty& operator=( NodeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline const_reference
    operator[]( size_type i ) const
    {
      return this->nodes[ i ];
    }

    inline const_reference
    operator()( rank_type rank ) const
    {
      return ( *this )[ rank - 1 ];
    }

    /* === METHODS === */
    inline const_reference
    at( size_type i ) const
    {
      return this->nodes.at( i );
    }

    inline const_iterator
    begin( ) const
    {
      return this->nodes.begin();
    }

    inline const_iterator
    end( ) const
    {
      return this->nodes.end();
    }

    inline const_reference
    back( ) const
    {
      return this->nodes.back();
    }

    inline const_reference
    front( ) const
    {
      return this->nodes.front();
    }

    inline size_type
    size( ) const
    {
      return this->nodes.size();
    }

    inline void
    add_node( value_type node )
    {
      this->nodes.push_back( std::move( node ) );
      this->sequences_len_sum += node.sequence.size();
      this->names_len_sum += node.name.size();
    }

  private:
    /* === DATA MEMBERS === */
    container_type nodes;
    typename sequence_type::size_type sequences_len_sum;
    typename string_type::size_type names_len_sum;
  };  /* --- end of template class NodeProperty --- */

  /**
   *  @brief  Edge property class (dynamic).
   *
   *  Represent data associated with each edge, mainly directionality. Each data
   *  structure associated with each edge are stored in a hash map with node ID
   *  pairs as keys.
   */
  template< typename TDir, uint8_t ...TWidths >
  class EdgeProperty< Dynamic, TDir, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using trait_type = EdgePropertyTrait< spec_type, TDir, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using link_type = typename trait_type::link_type;
    using edge_type = typename trait_type::edge_type;
    using key_type = typename trait_type::key_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;

    /* === LIFECYCLE === */
    EdgeProperty( )
    {                                         /* constructor      */
      trait_type::init_container( this->edges );
    }

    EdgeProperty( EdgeProperty const& other ) = default;      /* copy constructor */
    EdgeProperty( EdgeProperty&& other ) noexcept = default;  /* move constructor */
    ~EdgeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_edges( ) const
    {
      return this->edges;
    }

    /* === OPERATORS === */
    EdgeProperty& operator=( EdgeProperty const& other ) = default;      /* copy assignment operator */
    EdgeProperty& operator=( EdgeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline edge_type const&
    operator[]( key_type sides ) const
    {
      return this->edges.find( sides )->second;
    }

    /* === METHODS === */
    inline edge_type const&
    at( key_type sides ) const
    {
      auto found = this->edges.find( sides );
      if ( found == this->edges.end() ) throw std::runtime_error( "no such edge" );
      return found->second;
    }

    inline void
    add_edge( key_type sides, value_type edge )
    {
      this->edges[ sides ] = edge;
    }

    inline bool
    has_edge( key_type sides ) const
    {
      return this->edges.find( sides ) != this->edges.end();
    }

  private:
    /* === DATA MEMBERS === */
    container_type edges;
  };  /* --- end of template class EdgeProperty --- */

  /**
   *  @brief  Bidirected sequence graph representation (dynamic).
   *
   *  Represent a sequence graph (node-labeled bidirected graph).
   */
  template< template< class, uint8_t ... > class TNodeProp,
            template< class, class, uint8_t ... > class TEdgeProp,
            uint8_t ...TWidths >
  class SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >
    : public DirectedGraph< Dynamic, Bidirected, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using dir_type = Bidirected;
    using base_type = DirectedGraph< spec_type, dir_type, TWidths... >;
    using node_prop_type = TNodeProp< spec_type, TWidths... >;
    using edge_prop_type = TEdgeProp< spec_type, dir_type, TWidths ... >;
    using typename base_type::id_type;
    using typename base_type::offset_type;
    using typename base_type::rank_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using node_type = typename node_prop_type::node_type;
    using edge_type = typename edge_prop_type::edge_type;

    /* === LIFECYCLE === */
    SeqGraph() = default;                                  /* constructor      */
    SeqGraph( SeqGraph const& other ) = default;           /* copy constructor */
    SeqGraph( SeqGraph&& other ) noexcept = default;       /* move constructor */
    ~SeqGraph() noexcept = default;                        /* destructor       */

    /* === ACCESSORS === */
    inline node_prop_type const&
    get_node_prop( ) const
    {
      return this->node_prop;
    }

    inline node_type const&
    get_node_prop( rank_type rank ) const
    {
      return this->node_prop( rank );
    }

    inline edge_prop_type const&
    get_edge_prop( ) const
    {
      return this->edge_prop;
    }

    /* === OPERATORS === */
    SeqGraph& operator=( SeqGraph const& other ) = default;      /* copy assignment operator */
    SeqGraph& operator=( SeqGraph&& other ) noexcept = default;  /* move assignment operator */

    /* === METHODS === */
    inline id_type
    add_node( node_type node=node_type() )
    {
      this->node_prop.add_node( node );
      return base_type::add_node( );
    }

    inline void
    add_edge( link_type sides, edge_type edge=edge_type() )
    {
      if ( this->has_edge( sides ) ) return;
      base_type::add_edge_imp( sides, false );
      this->edge_prop.add_edge( sides, edge );
    }

    inline void
    add_edge( side_type from, side_type to, edge_type edge=edge_type() )
    {
      this->add_edge( base_type::make_link( from, to ), edge );
    }

    inline bool
    has_edge( link_type sides ) const
    {
      return this->edge_prop.has_edge( sides );
    }

    inline bool
    has_edge( side_type from, side_type to ) const
    {
      return this->has_edge( base_type::make_link( from, to ) );
    }

    inline typename node_type::sequence_type
    node_sequence( id_type id ) const
    {
      rank_type rank = base_type::id_to_rank( id );
      return this->node_prop( rank ).sequence;
    }

    inline typename node_type::sequence_type::size_type
    node_length( id_type id ) const
    {
      return this->node_sequence( id ).size();
    }

  protected:
    /* === ACCESSORS === */
    inline node_prop_type&
    get_node_prop( )
    {
      return this->node_prop;
    }

    inline node_type&
    get_node_prop( rank_type rank )
    {
      return this->node_prop( rank );
    }

    inline edge_prop_type&
    get_edge_prop( )
    {
      return this->edge_prop;
    }

  private:
    /* === DATA MEMBERS === */
    node_prop_type node_prop;
    edge_prop_type edge_prop;
  };  /* --- end of template class SeqGraph --- */

  /**
   *  @brief  Directed sequence graph representation (dynamic).
   *
   *  Represent a directed sequence graph (node-labeled directed graph).
   */
  template< uint8_t ...TWidths >
  class DiSeqGraph< Dynamic, TWidths... >
    : public DirectedGraph< Dynamic, Directed, TWidths... > {
  };

  /**
   *  @brief  Directed sequence graph representation (succinct).
   *
   *  Represent a directed sequence graph (node-labeled directed graph).
   */
  template< uint8_t ...TWidths >
  class DiSeqGraph< Succinct, TWidths... >
    : public DirectedGraph< Succinct, Directed, TWidths... > {
  };
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_HPP__ --- */
