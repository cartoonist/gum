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

    /* === MUTATORS === */
    inline void
    set_nodes( nodes_type value )
    {
      this->nodes = std::move( value );
      this->set_rank();
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
      return this->add_node_imp( );
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
    template< typename TCallback >
    inline bool
    for_each_node( TCallback callback ) const
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

    template< typename TCallback >
    inline bool
    for_each_side( id_type id, TCallback callback ) const
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
    template< typename TCallback >
    inline bool
    for_each_edges_to( side_type from, TCallback callback ) const
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
    template< typename TCallback >
    inline bool
    for_each_edges_to( id_type id, TCallback callback ) const
    {
      return this->for_each_side(
          id,
          [this, callback]( side_type from ) {
            auto found = this->adj_to.find( from );
            if ( found == this->adj_to.end() ) return true;
            for ( side_type to : found->second ) {
              if ( !callback( to.first, this->linktype( from, to ) ) ) return false;
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
    template< typename TCallback >
    inline bool
    for_each_edges_from( side_type to, TCallback callback ) const
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
    template< typename TCallback >
    inline bool
    for_each_edges_from( id_type id, TCallback callback ) const
    {
      return this->for_each_side(
          id,
          [this, callback]( side_type to ) {
            auto found = this->adj_from.find( to );
            if ( found == this->adj_from.end() ) return true;
            for ( side_type from : found->second ) {
              if ( !callback( from.first, this->linktype( from, to ) ) ) return false;
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
          [&retval]( side_type side ) {
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
          [&retval]( side_type side ) {
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
      this->set_last_rank();
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
    using name_type = typename trait_type::name_type;

    /* === LIFECYCLE === */
    NodeProperty() = default;                                 /* constructor      */
    NodeProperty( NodeProperty const& other ) = default;      /* copy constructor */
    NodeProperty( NodeProperty&& other ) noexcept = default;  /* move constructor */
    ~NodeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_nodes( ) const
    {
      return this->nodes;
    }

    /* === OPERATORS === */
    NodeProperty& operator=( NodeProperty const& other ) = default;      /* copy assignment operator */
    NodeProperty& operator=( NodeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline value_type const&
    operator[]( rank_type rank ) const
    {
      return this->nodes[ rank - 1 ];
    }

    /* === METHODS === */
    inline value_type const&
    at( rank_type rank ) const
    {
      return this->nodes.at( rank - 1 );
    }

    inline void
    add_node( value_type node )
    {
      this->nodes.push_back( std::move( node ) );
    }

  private:
    /* === DATA MEMBERS === */
    container_type nodes;
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
      return base_type::add_node_imp( );
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
      return this->node_prop[ rank ].sequence;
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
   *  Represent a sequence graph (node-labeled bidirected graph).
   */
  template< uint8_t ...TWidths >
  class DiSeqGraph< Dynamic, TWidths... >
    : public DirectedGraph< Dynamic, Directed, TWidths... > {
  };

  /**
   *  @brief  Directed sequence graph representation (succinct).
   *
   *  Represent a sequence graph (node-labeled bidirected graph).
   */
  template< uint8_t ...TWidths >
  class DiSeqGraph< Succinct, TWidths... >
    : public DirectedGraph< Succinct, Directed, TWidths... > {
  };
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_HPP__ --- */
