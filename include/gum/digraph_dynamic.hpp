/**
 *    @file  digraph_dynamic.hpp
 *   @brief  Definitions for Dynamic `DirectedGraph`
 *
 *  This header file includes definitions for Dynamic specification of
 *  `DirectedGraph` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  18:57
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_DYNAMIC_HPP__
#define GUM_DIGRAPH_DYNAMIC_HPP__

#include "digraph_traits_dynamic.hpp"
#include "digraph_base.hpp"
#include "coordinate.hpp"


namespace gum {
  /**
   *  @brief  Bidirected graph class (dynamic).
   *
   *  Represent the connectivity of a bidirected graph in a dynamic way; i.e.
   *  the connectivity can be modified after it is constructed in contrast with
   *  `succinct` specialization which is immutable.
   */
  template< typename TDir, typename TCoordSpec, uint8_t ...TWidths >
  class DirectedGraph< Dynamic, TDir, TCoordSpec, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using value_type = typename trait_type::value_type;
    using nodes_type = typename trait_type::nodes_type;
    using size_type = typename trait_type::size_type;
    using rank_type = typename trait_type::rank_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using string_type = typename trait_type::string_type;
    using side_type = typename trait_type::side_type;
    using link_type = typename trait_type::link_type;
    using linktype_type = typename trait_type::linktype_type;
    using adjs_type = typename trait_type::adjs_type;
    using adj_map_type = typename trait_type::adj_map_type;
    using coordspec_type = std::conditional_t< std::is_same< TCoordSpec, void >::value,
                                               coordinate::Identity, TCoordSpec >;
    using coordinate_type = CoordinateType< DirectedGraph, coordspec_type >;

    template< typename TCSpec = void >
    using succinct_template = DirectedGraph< Succinct, dir_type, TCSpec, TWidths... >;

    template< typename TCSpec = coordspec_type >
    using dynamic_template = DirectedGraph< Dynamic, dir_type, TCSpec, TWidths... >;

    using succinct_type = succinct_template<>;
    using dynamic_type = dynamic_template<>;

    /* === LIFECYCLE === */
    DirectedGraph( )                                            /* constructor      */
      : node_count( 0 ), edge_count( 0 )
    {
      trait_type::init_rank_map( this->node_rank );
      trait_type::init_adj_map( this->adj_in );
      trait_type::init_adj_map( this->adj_out );
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

    inline coordinate_type const&
    get_coordinate( ) const
    {
      return this->coordinate;
    }

    /* === OPERATORS === */
    DirectedGraph& operator=( DirectedGraph const& other ) = default;      /* copy assignment operator */
    DirectedGraph& operator=( DirectedGraph&& other ) noexcept = default;  /* move assignment operator */

    /* === METHODS === */
    /**
     *  @brief  Return the rank of a node by its ID.
     *
     *  @param  id A node ID.
     *  @return The corresponding node rank.
     */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      assert( id > 0 );
      auto found = this->node_rank.find( id );
      if ( found == this->node_rank.end() ) return 0;
      return found->second;
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
      return this->nodes[ rank - 1 ];
    }

    /**
     *  @brief  Return the embedded coordinate ID of a node by its internal ID.
     *
     *  `Dynamic` graphs only hold one coordinate system. So, this is an
     *  identity function to maintain the same interface for both `Dynamic` and
     *  `Succinct` graphs.
     *
     *  NOTE: This function assumes that node ID exists in the graph, otherwise
     *  the behaviour is undefined. The node ID can be verified by `has_node`
     *  method before calling this one.
     *
     *  @param  id A node ID.
     *  @return The corresponding node embedded coordinate ID.
     */
    constexpr inline id_type
    coordinate_id( id_type id ) const
    {
      return id;
    }

    /**
     *  @brief  Return the internal ID of a node by its external coordinate ID.
     *
     *  `Dynamic` graphs only hold one coordinate system. So, this is, by
     *  default, an identity function to maintain the same interface for both
     *  `Dynamic` and `Succinct` graphs.
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
      rank_type rank = this->id_to_rank( id );
      if ( rank == this->node_count ) return 0;
      return this->rank_to_id( rank + 1 );
    }

    /**
     *  @brief  Add a node to the graph.
     *
     *  The ID of the new node can be specified by `ext_id` which should have
     *  not been already in the graph. If `ext_id` is not specified or is zero,
     *  the node ID are chosen internally.
     *
     *  NOTE: The time complexity of adding node is constant if either no ID or
     *  all IDs are specified externally. Switching between these two approaches
     *  may result going through all nodes to determine the new node ID.
     *
     *  @param  ext_id External node id.
     *  @return The node ID of the added node in the graph.
     */
    inline id_type
    add_node( id_type ext_id=0 )
    {
      id_type new_id = this->add_node_imp( ext_id );
      this->set_last_rank();
      return new_id;
    }

    /**
     *  @brief  Add `count` number of nodes to the graph.
     *
     *  NOTE: The node whose IDs are given via the callback functions are not
     *  ready to be queried/manipulated. This is merely a way to return added
     *  node IDs without keeping them all in memory.
     *
     *  @param  count The number of nodes to be added.
     *  @param  callback A function to be called on IDs of added nodes.
     */
    template< typename TCallback = void(*)( id_type ) >
    inline void
    add_nodes( size_type count,
               TCallback callback = []( id_type ){} )
    {
      static_assert( std::is_invocable_v< TCallback, id_type >, "received a non-invocable as callback" );

      for ( size_type i = 0; i < count; ++i ) callback( this->add_node_imp() );
      this->set_last_rank( count );
    }

    inline bool
    has_node( id_type id ) const
    {
      return this->node_rank.find( id ) != this->node_rank.end();
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
    template< typename TCallback >
    inline bool
    for_each_node( TCallback callback,
                   rank_type rank=1 ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, rank_type, id_type >, "received a non-invocable as callback" );

      if ( rank > this->get_node_count() ) return true;

      for ( auto itr = this->nodes.begin() + rank - 1;
            itr != this->nodes.end(); ++itr ) {
        if ( !callback( rank, *itr ) ) return false;
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
      auto oit = this->adj_out.find( from );
      auto iit = this->adj_in.find( to );
      if ( oit == this->adj_out.end() || iit == this->adj_in.end() ) return false;
      auto const& outs = oit->second;
      auto const& ins = iit->second;
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
    has_edge( id_type from, id_type to, linktype_type type=trait_type::get_default_linktype() )
    {
      return this->has_edge( this->from_side( from, type ), this->to_side( to, type ) );
    }

    inline adjs_type
    adjacents_out( side_type from ) const
    {
      auto found = this->adj_out.find( from );
      if ( found == this->adj_out.end() ) return adjs_type();
      return found->second;
    }

    inline adjs_type
    adjacents_in( side_type to ) const
    {
      auto found = this->adj_in.find( to );
      if ( found == this->adj_in.end() ) return adjs_type();
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
    for_each_edges_out( side_type from,
                        TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

      auto found = this->adj_out.find( from );
      if ( found == this->adj_out.end() ) return true;
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
    for_each_edges_out( id_type id,
                        TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, id_type, linktype_type >, "received a non-invocable as callback" );

      return this->for_each_side(
          id,
          [this, callback]( side_type from ) {
            auto found = this->adj_out.find( from );
            if ( found == this->adj_out.end() ) return true;
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
    template< typename TCallback >
    inline bool
    for_each_edges_in( side_type to,
                       TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

      auto found = this->adj_in.find( to );
      if ( found == this->adj_in.end() ) return true;
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
    for_each_edges_in( id_type id,
                       TCallback callback ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, id_type, linktype_type >, "received a non-invocable as callback" );

      return this->for_each_side(
          id,
          [this, callback]( side_type to ) {
            auto found = this->adj_in.find( to );
            if ( found == this->adj_in.end() ) return true;
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
      auto found = this->adj_out.find( side );
      if ( found == this->adj_out.end() ) return 0;
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
      auto found = this->adj_in.find( side );
      if ( found == this->adj_in.end() ) return 0;
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

    template< typename TContainer >
    inline void
    sort_nodes( TContainer const& perm )
    {
      util::permute( perm, this->nodes );
      this->reset_ranks();
    }

    inline auto
    sort_nodes( )
    {
      auto perm = util::sort_permutation( this->nodes );  // sort by node ids
      this->sort_nodes( perm );
      return perm;
    }

    inline void
    clear( )
    {
      this->nodes.clear();
      this->node_rank.clear();
      this->adj_out.clear();
      this->adj_in.clear();
      this->node_count = 0;
      this->edge_count = 0;
    }

    inline void
    shrink_to_fit( )
    {
      this->nodes.shrink_to_fit();
    }

  protected:
    /* === ACCESSORS === */
    inline nodes_type&
    get_nodes( )
    {
      return this->nodes;
    }

    inline coordinate_type&
    get_coordinate( )
    {
      return this->coordinate;
    }

    /* === METHODS === */
    inline id_type
    add_node_imp( id_type ext_id=0 )
    {
      if ( ext_id == 0 ) {                      // ID is not externally specified.
        if ( this->nodes.empty() ) ext_id = 1;  // IDs start from 1.
        else {
          ext_id = this->nodes.back() + 1;      // and they are assigned sequentially.
          if ( this->has_node( ext_id ) ) {     // Otherwise, find the max (expensive).
            ext_id = *std::max_element( this->nodes.begin(), this->nodes.end() ) + 1;
          }
        }
      }
      if ( this->has_node( ext_id ) )
        throw std::runtime_error( "adding a node with invalid/duplicate ID" );
      this->nodes.push_back( ext_id );
      return ext_id;
    }

    inline void
    add_edge_imp( side_type from, side_type to, bool safe=true )
    {
      assert( this->has_node( from ) && this->has_node( to ) );
      assert( !safe || !this->has_edge( from, to ) );
      this->adj_out[ from ].push_back( to );
      this->adj_in[ to ].push_back( from );
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
    adj_map_type adj_out;
    adj_map_type adj_in;
    rank_type node_count;
    rank_type edge_count;
    coordinate_type coordinate;

    /* === METHODS === */
    inline void
    reset_ranks()
    {
      rank_type rank = 0;
      this->node_rank.clear();
      for ( auto const& n : this->nodes ) this->node_rank[ n ] = ++rank;
    }

    inline void
    set_rank( typename nodes_type::const_iterator begin,
              typename nodes_type::const_iterator end )
    {
      assert( end - begin + this->node_count == this->nodes.size() );
      for ( ; begin != end; ++begin ) {
        bool inserted;
        std::tie( std::ignore, inserted ) =
            this->node_rank.insert( { *begin, ++this->node_count } );
        assert( inserted );  // avoid duplicate insertion from upstream.
      }
    }

    inline void
    set_rank( )
    {
      this->set_rank( this->nodes.begin(), this->nodes.end() );
    }

    inline void
    set_last_rank( size_type count=1 )
    {
      this->set_rank( this->nodes.end() - count, this->nodes.end() );
    }
  };  /* --- end of template class DirectedGraph --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_DYNAMIC_HPP__ --- */
