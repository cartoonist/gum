/**
 *    @file  seqgraph_dynamic.hpp
 *   @brief  Definitions for Dynamic `SeqGraph`
 *
 *  This header file includes definitions for Dynamic specification of
 *  `SeqGraph` class.
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

#ifndef GUM_SEQGRAPH_DYNAMIC_HPP__
#define GUM_SEQGRAPH_DYNAMIC_HPP__

#include "seqgraph_base.hpp"
#include "digraph_dynamic.hpp"
#include "node_prop_dynamic.hpp"
#include "edge_prop_dynamic.hpp"
#include "graph_prop_dynamic.hpp"


namespace gum {
  /**
   *  @brief  Bidirected sequence graph representation (dynamic).
   *
   *  Represent a sequence graph (node-labeled bidirected graph).
   */
  template< typename TCoordSpec,
            template< class, uint8_t ... > class TNodeProp,
            template< class, class, uint8_t ... > class TEdgeProp,
            template< class, class, uint8_t ... > class TGraphProp,
            uint8_t ...TWidths >
  class SeqGraph< Dynamic, TCoordSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >
    : public DirectedGraph< Dynamic, Bidirected, TCoordSpec, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using dir_type = Bidirected;
    using base_type = DirectedGraph< spec_type, dir_type, TCoordSpec, TWidths... >;
    using node_prop_type = TNodeProp< spec_type, TWidths... >;
    using edge_prop_type = TEdgeProp< spec_type, dir_type, TWidths... >;
    using graph_prop_type = TGraphProp< spec_type, dir_type, TWidths... >;
    using typename base_type::id_type;
    using typename base_type::offset_type;
    using typename base_type::value_type;
    using typename base_type::size_type;
    using typename base_type::rank_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using typename base_type::string_type;
    using typename base_type::coordspec_type;
    using typename base_type::coordinate_type;
    using node_type = typename node_prop_type::node_type;
    using sequence_type = typename node_prop_type::sequence_type;
    using seq_const_reference = typename node_prop_type::seq_const_reference;
    using seq_reference = typename node_prop_type::seq_reference;
    using edge_type = typename edge_prop_type::edge_type;
    using path_type = typename graph_prop_type::path_type;

    template< typename TCSpec = void >
    using succinct_template = SeqGraph< Succinct, TCSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;

    template< typename TCSpec = coordspec_type >
    using dynamic_template = SeqGraph< Dynamic, TCSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;

    using succinct_type = succinct_template<>;
    using dynamic_type = dynamic_template<>;

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

    inline typename node_prop_type::const_reference
    get_node_prop( rank_type rank ) const
    {
      return this->node_prop( rank );
    }

    inline edge_prop_type const&
    get_edge_prop( ) const
    {
      return this->edge_prop;
    }

    inline graph_prop_type const&
    get_graph_prop( ) const
    {
      return this->graph_prop;
    }

    inline rank_type
    get_path_count( ) const
    {
      return this->graph_prop.get_path_count();
    }

    /* === OPERATORS === */
    SeqGraph& operator=( SeqGraph const& other ) = default;      /* copy assignment operator */
    SeqGraph& operator=( SeqGraph&& other ) noexcept = default;  /* move assignment operator */

    /* === METHODS === */
    inline rank_type
    path_id_to_rank( id_type id ) const
    {
      return this->graph_prop.id_to_rank( id );
    }

    inline id_type
    path_rank_to_id( rank_type rank ) const
    {
      return this->graph_prop.rank_to_id( rank );
    }

    inline id_type
    add_node( node_type node, id_type ext_id=0 )
    {
      this->node_prop.add_node( std::move( node ) );
      return base_type::add_node( ext_id );
    }

    inline id_type
    add_node( id_type ext_id=0 )
    {
      this->node_prop.add_node( node_type() );
      return base_type::add_node( ext_id );
    }

    template< typename TCallback = void(*)( id_type ) >
    inline void
    add_nodes( size_type count,
               TCallback callback = []( id_type ){} )
    {
      static_assert( std::is_invocable_v< TCallback, id_type >, "received a non-invocable as callback" );

      for ( size_type i = 0; i < count; ++i ) this->node_prop.add_node( node_type() );
      base_type::add_nodes( count, callback );
    }

    inline void
    update_node( id_type id, node_type node )
    {
      rank_type rank = base_type::id_to_rank( id );
      this->node_prop.update_node( rank, std::move( node ) );
    }

    inline void
    sort_nodes()
    {
      auto perm = base_type::sort_nodes();
      this->node_prop.sort_nodes( perm );
    }

    template< typename TContainer >
    inline void
    sort_nodes( TContainer const& perm )
    {
      base_type::sort_nodes( perm );
      this->node_prop.sort_nodes( perm );
    }

    inline void
    add_edge( link_type sides, edge_type edge=edge_type() )
    {
      assert( !this->has_edge( sides ) );
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

    inline id_type
    add_path( string_type name )
    {
      return this->graph_prop.add_path( std::move( name ) );
    }

    template< typename TIter, typename ...TArgs >
    inline id_type
    add_path( TIter nbegin, TIter nend, TArgs&&... args )
    {
      assert( std::all_of( nbegin, nend, [this]( id_type nid )
                                         { return this->has_node( nid ); } ) );
      return this->graph_prop.add_path( nbegin, nend, std::forward< TArgs >( args )... );
    }

    inline void
    extend_path( id_type pid, id_type nid, bool reversed=false )
    {
      assert( this->has_node( nid ) );
      this->graph_prop.extend_path( pid, nid, reversed );
    }

    template< typename TIter, typename ...TArgs >
    inline void
    extend_path( id_type id, TIter nbegin, TIter nend, TArgs&&... args )
    {
      assert( std::all_of( nbegin, nend, [this]( id_type nid )
                                         { return this->has_node( nid ); } ) );
      this->graph_prop.extend_path( id, nbegin, nend, std::forward< TArgs >( args )... );
    }

    inline bool
    has_path( id_type id ) const
    {
      return this->graph_prop.has_path( id );
    }

    template< typename TCallback >
    inline bool
    for_each_path( TCallback callback,
                   rank_type rank=1 ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, rank_type, id_type >, "received a non-invocable as callback" );

      return this->graph_prop.for_each_path( callback, rank );
    }

    inline seq_const_reference
    node_sequence( id_type id ) const
    {
      rank_type rank = base_type::id_to_rank( id );
      return this->node_prop( rank ).sequence;
    }

    inline offset_type
    node_length( id_type id ) const
    {
      return this->node_sequence( id ).size();
    }

    inline offset_type
    edge_overlap( link_type sides ) const
    {
      return this->edge_prop[ sides ].overlap;
    }

    inline offset_type
    edge_overlap( id_type from, id_type to,
                  linktype_type type=base_type::get_default_linktype() ) const
    {
      return this->edge_overlap( this->make_link( from, to, type ) );
    }

    inline offset_type
    edge_overlap( side_type from, side_type to ) const
    {
      return this->edge_overlap( this->make_link( from, to ) );
    }

    inline rank_type
    path_length( id_type id ) const
    {
      return this->graph_prop.path_length( id );
    }

    inline string_type
    path_name( id_type id ) const
    {
      return this->graph_prop.path_name( id );
    }

    inline path_type const&
    path( id_type id ) const
    {
      return this->graph_prop.path( id );
    }

    inline void
    clear( )
    {
      this->node_prop.clear();
      this->edge_prop.clear();
      this->graph_prop.clear();
      base_type::clear();
    }

    inline void
    shrink_to_fit( )
    {
      this->node_prop.shrink_to_fit();
      this->graph_prop.shrink_to_fit();
      base_type::shrink_to_fit();
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

    inline graph_prop_type&
    get_graph_prop( )
    {
      return this->graph_prop;
    }

  private:
    /* === DATA MEMBERS === */
    node_prop_type node_prop;
    edge_prop_type edge_prop;
    graph_prop_type graph_prop;
  };  /* --- end of template class SeqGraph --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_SEQGRAPH_DYNAMIC_HPP__ --- */
