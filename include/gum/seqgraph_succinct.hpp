/**
 *    @file  seqgraph_succinct.hpp
 *   @brief  Definitions for Succinct `SeqGraph`
 *
 *  This header file includes definitions for Succinct specification of
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

#ifndef GUM_SEQGRAPH_SUCCINCT_HPP__
#define GUM_SEQGRAPH_SUCCINCT_HPP__

#include "seqgraph_base.hpp"
#include "digraph_succinct.hpp"
#include "node_prop_succinct.hpp"
#include "edge_prop_succinct.hpp"
#include "graph_prop_succinct.hpp"


namespace gum {
  /**
   *  @brief  Bidirected sequence graph representation (succinct).
   *
   *  Represent a sequence graph (node-labeled bidirected graph).
   *
   *  NOTE: In contrast with xg graphs, the IDs in these graphs are handles. So,
   *  they can be used for efficient traversal directly.
   *
   *  NOTE: The default embedded coordinate system, which maps original (i.e.
   *  external) node IDs to local node IDs in the graph, is of type
   *  `gum::coordinate::Dense`. This coordinate class is most efficient when
   *  external node IDs are sequential. If it is not the case, consider using
   *  `gum::coordinate::Sparse` alternatively; i.e. use
   *  `SeqGraph< Succinct, gum::coordinate::Sparse >`.
   *  You can also drop this data structure completely by passing template
   *  parameter `gum::coordinate::Identity` instead saving some memory.
   */
  template< typename TCoordSpec,
            template< class, uint8_t ... > class TNodeProp,
            template< class, class, uint8_t ... > class TEdgeProp,
            template< class, class, uint8_t ... > class TGraphProp,
            uint8_t ...TWidths >
  class SeqGraph< Succinct, TCoordSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >
    : public DirectedGraph< Succinct, Bidirected, TCoordSpec, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Succinct;
    using dir_type = Bidirected;
    using base_type = DirectedGraph< spec_type, dir_type, TCoordSpec, TWidths... >;
    using node_prop_type = TNodeProp< spec_type, TWidths... >;
    using edge_prop_type = void;
    using graph_prop_type = TGraphProp< spec_type, dir_type, TWidths... >;
    using typename base_type::id_type;
    using typename base_type::offset_type;
    using typename base_type::value_type;
    using typename base_type::size_type;
    using typename base_type::rank_type;
    using typename base_type::string_type;
    using typename base_type::padding_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using typename base_type::coordspec_type;
    using typename base_type::coordinate_type;
    using node_type = typename node_prop_type::node_type;
    using sequence_type = typename node_prop_type::sequence_type;
    using seq_const_reference = typename node_prop_type::seq_const_reference;
    using seq_reference = typename node_prop_type::seq_reference;
    using edge_type = void;  // For compatibility with `SeqGraph< Dynamic >`
    using path_type = typename graph_prop_type::path_type;

    template< typename TCSpec = void >
    using dynamic_template = SeqGraph< Dynamic, TCSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;

    template< typename TCSpec = coordspec_type >
    using succinct_template = SeqGraph< Succinct, TCSpec, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;

    using dynamic_type = dynamic_template<>;
    using succinct_type = succinct_template<>;

    constexpr static padding_type NODE_PADDING = 2;
    constexpr static padding_type EDGE_PADDING = 1;

    constexpr static size_type NP_SEQSTART_OFFSET = 0;
    constexpr static size_type NP_SEQLEN_OFFSET = 1;

    constexpr static size_type EP_OVERLAP_OFFSET = 0;

    /* === LIFECYCLE === */
    /* constructor      */
    SeqGraph( )
      : base_type( SeqGraph::NODE_PADDING, SeqGraph::EDGE_PADDING )
    { }

    template< typename TCSpec >
    SeqGraph( dynamic_template< TCSpec > const& d_graph )
      : base_type( d_graph, SeqGraph::NODE_PADDING, SeqGraph::EDGE_PADDING ),
        node_prop( d_graph.get_node_prop( ) ),
        graph_prop( d_graph.get_graph_prop( ), this->get_coordinate() )
    {
      this->fill_properties( d_graph );
    }

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

    template< typename TCSpec >
    SeqGraph&
    operator=( dynamic_template< TCSpec > const& d_graph )
    {
      base_type::operator=( d_graph );
      this->node_prop = d_graph.get_node_prop( );
      this->fill_properties( d_graph );
      this->graph_prop = d_graph.get_graph_prop( );
      this->graph_prop.apply_coordinate( this->get_coordinate() );
      return *this;
    }

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
      size_type spos = this->get_np_value( id, SeqGraph::NP_SEQSTART_OFFSET );
      size_type len = this->get_np_value( id, SeqGraph::NP_SEQLEN_OFFSET );
      return this->node_prop.sequences()( spos, len );
    }

    inline offset_type
    node_length( id_type id ) const
    {
      return this->get_np_value( id, SeqGraph::NP_SEQLEN_OFFSET );
    }

    inline offset_type
    edge_overlap( id_type from, id_type to,
                  linktype_type type=base_type::get_default_linktype() ) const
    {
      auto fod = this->outdegree( from );
      auto tod = this->indegree( to );
      offset_type overlap = 0;
      bool success = false;
      (void)success;  // Silencing unused-but-set-variable warning.
      auto setoverlap =
          [this, &overlap]( id_type id, linktype_type type ) {
            return [this, id, type, &overlap]( size_type pos ) {
                     id_type adj_id = this->get_adj_id( pos );
                     linktype_type adj_type = this->get_adj_linktype( pos );
                     if ( adj_id == id && adj_type == type ) {
                       overlap = this->edge_overlap( pos );
                       return false;
                     }
                     return true;
                   };
          };
      if ( fod < tod ) {
        success = !this->for_each_edges_out_pos( from, setoverlap( to, type ) );
        assert( success );
      }
      else {
        success = !this->for_each_edges_in_pos( to, setoverlap( from, type ) );
        assert( success );
      }
      return overlap;
    }

    inline offset_type
    edge_overlap( side_type from, side_type to ) const
    {
      return this->edge_overlap( this->id_of( from ), this->id_of( to ),
                                 this->linktype( from, to ) );
    }

    inline offset_type
    edge_overlap( link_type sides ) const
    {
      return this->edge_overlap( this->from_id( sides ), this->to_id( sides ),
                                 this->linktype( sides ) );
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

    inline path_type
    path( id_type id ) const
    {
      return this->graph_prop.path( id );
    }

    inline void
    clear( )
    {
      this->node_prop.clear();
      this->graph_prop.clear();
      base_type::clear();
    }

  protected:
    /* === ACCESSORS === */
    inline node_prop_type&
    get_node_prop( )
    {
      return this->node_prop;
    }

    inline node_type
    get_node_prop( rank_type rank )
    {
      return this->node_prop( rank );
    }

    inline graph_prop_type&
    get_graph_prop( )
    {
      return this->graph_prop;
    }

    /* === METHODS === */
    inline size_type
    node_prop_pos( id_type id ) const
    {
      return id + this->header_core_len();
    }

    inline size_type
    edge_prop_pos( size_type pos ) const
    {
      return pos + this->edge_core_len();
    }

    inline value_type
    get_np_value( id_type id, size_type offset ) const
    {
      return this->get_nodes_at( this->node_prop_pos( id ) + offset );
    }

    inline void
    set_np_value( id_type id, size_type offset, value_type value )
    {
      this->set_nodes_at( this->node_prop_pos( id ) + offset, value );
    }

    inline value_type
    get_ep_value( size_type pos, size_type offset ) const
    {
      return this->get_nodes_at( this->edge_prop_pos( pos ) + offset );
    }

    inline void
    set_ep_value( size_type pos, size_type offset, value_type value )
    {
      this->set_nodes_at( this->edge_prop_pos( pos ) + offset, value );
    }

    inline offset_type
    edge_overlap( size_type pos ) const
    {
      return this->get_ep_value( pos, SeqGraph::EP_OVERLAP_OFFSET );
    }

  private:
    /* === DATA MEMBERS === */
    node_prop_type node_prop;
    graph_prop_type graph_prop;

    /* === METHODS === */
    template< typename TCSpec >
    inline void
    fill_properties( dynamic_template< TCSpec > const& d_graph )
    {
      this->for_each_node(
          [this, &d_graph]( rank_type rank, id_type id ) {
            this->set_np_value( id, SeqGraph::NP_SEQSTART_OFFSET,
                                this->node_prop.sequences().start_position( rank - 1 ) );
            this->set_np_value( id, SeqGraph::NP_SEQLEN_OFFSET,
                                this->node_prop.sequences().length( rank - 1 ) );
            id_type d_id = d_graph.rank_to_id( rank );
            this->for_each_edges_out_pos(
                id,
                [this, d_id, &d_graph]( size_type pos ) {
                  rank_type adj_rank = this->id_to_rank( this->get_adj_id( pos ) );
                  id_type d_adj_id = d_graph.rank_to_id( adj_rank );
                  link_type link = this->make_link( d_id,
                                                    d_adj_id,
                                                    this->get_adj_linktype( pos ) );
                  offset_type overlap = d_graph.edge_overlap( link );
                  this->set_ep_value( pos, SeqGraph::EP_OVERLAP_OFFSET, overlap );
                  return true;
                } );
            this->for_each_edges_in_pos(
                id,
                [this, d_id, &d_graph]( size_type pos ) {
                  rank_type adj_rank = this->id_to_rank( this->get_adj_id( pos ) );
                  id_type d_adj_id = d_graph.rank_to_id( adj_rank );
                  link_type link = this->make_link( d_adj_id,
                                                    d_id,
                                                    this->get_adj_linktype( pos ) );
                  offset_type overlap = d_graph.edge_overlap( link );
                  this->set_ep_value( pos, SeqGraph::EP_OVERLAP_OFFSET, overlap );
                  return true;
                } );
            return true;
          } );
    }
  };  /* --- end of template class SeqGraph --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_SEQGRAPH_SUCCINCT_HPP__ --- */
