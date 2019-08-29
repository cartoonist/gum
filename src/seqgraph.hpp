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
        /* === TYPEDEFS  === */
        using spec_type = Dynamic;
        using trait_type = DirectedGraphTrait< spec_type, TDir, TWidths... >;
        using id_type = typename trait_type::id_type;
        using offset_type = typename trait_type::offset_type;
        using rank_type = typename trait_type::rank_type;
        using nodes_type = typename trait_type::nodes_type;
        using rank_map_type = typename trait_type::rank_map_type;
        using side_type = typename trait_type::side_type;
        using link_type = typename trait_type::link_type;
        using adjs_type = typename trait_type::adjs_type;
        using adj_map_type = typename trait_type::adj_map_type;

        /* === LIFECYCLE === */
        DirectedGraph( ) {                                          /* constructor      */
          trait_type::init_rank_map( this->node_rank );
          trait_type::init_adj_map( this->adj_from );
          trait_type::init_adj_map( this->adj_to );
          this->max_rank = 0;
          this->edge_count = 0;
        }

        DirectedGraph( DirectedGraph const& other ) = default;      /* copy constructor */
        DirectedGraph( DirectedGraph&& other ) noexcept = default;  /* move constructor */
        ~DirectedGraph() noexcept = default;                        /* destructor       */

        /* === ACCESSORS === */
          inline nodes_type const&
        get_nodes( ) const
        {
          return this->nodes;
        }  /* -----  end of method DirectedGraph::get_nodes  ----- */

          inline rank_type
        get_max_node_rank( ) const
        {
          assert( this->max_rank == this->get_node_count() );
          return this->max_rank;
        }  /* -----  end of method DirectedGraph::get_max_node_rank  ----- */

          inline rank_type
        get_node_count( ) const
        {
          return this->nodes.size();
        }  /* -----  end of method DirectedGraph::get_node_count  ----- */

          inline rank_type
        get_edge_count( ) const
        {
          return this->edge_count;
        }  /* -----  end of method DirectedGraph::get_edge_count  ----- */

        /* === MUTATORS === */
          inline void
        set_nodes( nodes_type value )
        {
          this->nodes = std::move( value );
          this->set_rank();
        }  /* -----  end of method DirectedGraph::set_nodes  ----- */

        /* === OPERATORS === */
        DirectedGraph& operator=( DirectedGraph const& other ) = default;      /* copy assignment operator */
        DirectedGraph& operator=( DirectedGraph&& other ) noexcept = default;  /* move assignment operator */

        /* === METHODS === */
          inline rank_type
        id_to_rank( id_type id ) const
        {
          if ( id <= 0 ) throw std::runtime_error( "non-positive node ID");
          auto found = this->node_rank.find( id );
          if ( found == this->node_rank.end() ) return 0;
          return found->second;
        }  /* -----  end of method DirectedGraph::id_to_rank  ----- */

          inline id_type
        rank_to_id( rank_type rank ) const
        {
          if ( rank <= 0 ) throw std::runtime_error( "non-positive node rank");
          return this->nodes[ rank - 1 ];
        }  /* -----  end of method DirectedGraph::rank_to_id  ----- */

          inline void
        add_node( id_type id )
        {
          this->add_node_imp( id );
        }  /* -----  end of method DirectedGraph::add_node  ----- */

          inline bool
        has_node( id_type id ) const
        {
          return this->id_to_rank( id ) != 0;
        }  /* -----  end of method DirectedGraph::has_node  ----- */

          constexpr inline side_type
        from_side( link_type sides ) const
        {
          return trait_type::from_side( sides );
        }  /* -----  end of method DirectedGraph::from_side  ----- */

          constexpr inline side_type
        to_side( link_type sides ) const
        {
          return trait_type::to_side( sides );
        }  /* -----  end of method DirectedGraph::to_side  ----- */

          constexpr inline link_type
        merge_sides( side_type from, side_type to ) const
        {
          return trait_type::merge_sides( from, to );
        }  /* -----  end of method DirectedGraph::merge_sides  ----- */

          constexpr inline side_type
        opposite_side( side_type side ) const
        {
          return trait_type::opposite_side( side );
        }

          inline void
        add_edge( side_type from, side_type to )
        {
          this->add_edge_imp( from, to );
        }  /* -----  end of method DirectedGraph::add_edge  ----- */

          inline void
        add_edge( link_type sides )
        {
          this->add_edge( this->from_side( sides ), this->to_side( sides ) );
        }  /* -----  end of method DirectedGraph::add_edge  ----- */

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
        }  /* -----  end of method DirectedGraph::has_edge  ----- */

          inline bool
        has_edge( link_type sides ) const
        {
          return this->has_edge( this->from_side( sides ), this->to_side( sides ) );
        }  /* -----  end of method DirectedGraph::has_edge  ----- */

          inline adjs_type
        get_adjacents_to( side_type from ) const
        {
          auto found = this->adj_to.find( from );
          if ( found == this->adj_to.end() ) return adjs_type();
          return found->second;
        }  /* -----  end of method DirectedGraph::get_adjacents_to  ----- */

          inline adjs_type
        get_adjacents_from( side_type to ) const
        {
          auto found = this->adj_from.find( to );
          if ( found == this->adj_from.end() ) return adjs_type();
          return found->second;
        }  /* -----  end of method DirectedGraph::get_adjacents_from  ----- */

          inline rank_type
        outdegree( side_type side ) const
        {
          auto found = this->adj_to.find( side );
          if ( found == this->adj_to.end() ) return 0;
          return found->second.size();
        }  /* -----  end of method DirectedGraph::outdegree  ----- */

          inline rank_type
        indegree( side_type side ) const
        {
          auto found = this->adj_from.find( side );
          if ( found == this->adj_from.end() ) return 0;
          return found->second.size();
        }  /* -----  end of method DirectedGraph::indegree  ----- */

          inline bool
        has_edges_from( side_type side ) const
        {
          return this->indegree( side ) != 0;
        }  /* -----  end of method DirectedGraph::has_edges_from  ----- */

          inline bool
        has_edges_to( side_type side ) const
        {
          return this->outdegree( side ) != 0;
        }  /* -----  end of method DirectedGraph::has_edges_to  ----- */

      protected:
        /* === ACCESSORS === */
          inline nodes_type&
        get_nodes( )
        {
          return this->nodes;
        }  /* -----  end of method DirectedGraph::get_nodes  ----- */

        /* === METHODS === */
          inline void
        add_node_imp( id_type id, bool safe=true )
        {
          if ( id <= 0 ) throw std::runtime_error( "non-positive node ID");
          if ( safe && this->has_node( id ) ) return;
          this->nodes.push_back( id );
          this->set_last_rank();
        }  /* -----  end of method DirectedGraph::add_node_imp  ----- */

          inline void
        add_edge_imp( side_type from, side_type to, bool safe=true )
        {
          if ( safe && this->has_edge( from, to ) ) return;
          this->adj_to[ from ].push_back( to );
          this->adj_from[ to ].push_back( from );
          ++this->edge_count;
        }  /* -----  end of method DirectedGraph::add_edge_imp  ----- */

          inline void
        add_edge_imp( link_type sides, bool safe=true )
        {
          this->add_edge_imp( this->from_side( sides ), this->to_side( sides ), safe );
        }  /* -----  end of method DirectedGraph::add_edge_imp  ----- */

      private:
        /* === DATA MEMBERS === */
        nodes_type nodes;
        rank_map_type node_rank;
        adj_map_type adj_to;
        adj_map_type adj_from;
        rank_type edge_count;
        rank_type max_rank;

        /* === METHODS === */
          inline void
        set_rank( typename nodes_type::const_iterator begin,
            typename nodes_type::const_iterator end )
        {
          assert( end - begin + this->max_rank == this->nodes.size() );
          for ( ; begin != end; ++begin ) {
            bool inserted;
            std::tie( std::ignore, inserted ) =
              this->node_rank.insert( { *begin, ++this->max_rank } );
            assert( inserted );  // avoid duplicate insersion from upstream.
          }
        }  /* -----  end of method DirectedGraph::set_rank  ----- */

          inline void
        set_rank( )
        {
          this->set_rank( this->nodes.begin(), this->nodes.end() );
        }  /* -----  end of method DirectedGraph::set_rank  ----- */

          inline void
        set_last_rank( )
        {
          this->set_rank( this->nodes.end() - 1, this->nodes.end() );
        }  /* -----  end of method DirectedGraph::set_last_rank  ----- */
    };  /* -----  end of template class DirectedGraph  ----- */

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
        /* === TYPEDEFS  === */
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
        }  /* -----  end of method NodeProperty::get_nodes  ----- */

        /* === OPERATORS === */
        NodeProperty& operator=( NodeProperty const& other ) = default;      /* copy assignment operator */
        NodeProperty& operator=( NodeProperty&& other ) noexcept = default;  /* move assignment operator */

          inline value_type const&
        operator[]( rank_type rank ) const
        {
          return this->nodes[ rank - 1 ];
        }

        /* === METHODS === */
          inline void
        add_node( value_type node )
        {
          this->nodes.push_back( std::move( node ) );
        }  /* -----  end of method NodeProperty::add_node  ----- */

      private:
        /* === DATA MEMBERS === */
        container_type nodes;
    };  /* ----------  end of template class NodeProperty  ---------- */

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
        /* === TYPEDEFS  === */
        using spec_type = Dynamic;
        using trait_type = EdgePropertyTrait< spec_type, TDir, TWidths... >;
        using id_type = typename trait_type::id_type;
        using offset_type = typename trait_type::offset_type;
        using edge_type = typename trait_type::edge_type;
        using key_type = typename trait_type::key_type;
        using value_type = typename trait_type::value_type;
        using container_type = typename trait_type::container_type;

        /* === LIFECYCLE === */
        EdgeProperty( ) {                                         /* constructor      */
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
        }  /* -----  end of method EdgeProperty::get_edges  ----- */

        /* === OPERATORS === */
        EdgeProperty& operator=( EdgeProperty const& other ) = default;      /* copy assignment operator */
        EdgeProperty& operator=( EdgeProperty&& other ) noexcept = default;  /* move assignment operator */

        /* === METHODS === */
          inline void
        add_edge( key_type sides, value_type edge )
        {
          this->edges[ sides ] = edge;
        }  /* -----  end of method EdgeProperty::add_edge  ----- */

          inline bool
        has_edge( key_type sides ) const
        {
          return this->edges.find( sides ) != this->edges.end();
        }  /* -----  end of method EdgeProperty::has_edge  ----- */

      private:
        /* === DATA MEMBERS === */
        container_type edges;
    };  /* -----  end of template class EdgeProperty  ----- */

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
        /* === TYPEDEFS  === */
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
        }  /* -----  end of method SeqGraph::get_node_prop  ----- */

          inline edge_prop_type const&
        get_edge_prop( ) const
        {
          return this->edge_prop;
        }  /* -----  end of method SeqGraph::get_edge_prop  ----- */

        /* === OPERATORS === */
        SeqGraph& operator=( SeqGraph const& other ) = default;      /* copy assignment operator */
        SeqGraph& operator=( SeqGraph&& other ) noexcept = default;  /* move assignment operator */

        /* === METHODS === */
          inline void
        add_node( id_type id, node_type node=node_type() )
        {
          if ( this->has_node( id ) ) return;
          base_type::add_node_imp( id, false );
          this->node_prop.add_node( node );
        }  /* -----  end of method SeqGraph::add_node  ----- */

          inline void
        add_edge( link_type sides, edge_type edge=edge_type() )
        {
          if ( this->has_edge( sides ) ) return;
          base_type::add_edge_imp( sides, false );
          this->edge_prop.add_edge( sides, edge );
        }  /* -----  end of method SeqGraph::add_edge  ----- */

          inline void
        add_edge( side_type from, side_type to, edge_type edge=edge_type() )
        {
          this->add_edge( base_type::merge_sides( from, to ), edge );
        }  /* -----  end of method SeqGraph::add_edge  ----- */

          inline bool
        has_edge( link_type sides ) const
        {
          return this->edge_prop.has_edge( sides );
        }  /* -----  end of method SeqGraph::has_edge  ----- */

          inline bool
        has_edge( side_type from, side_type to ) const
        {
          return this->has_edge( base_type::merge_sides( from, to ) );
        }  /* -----  end of method SeqGraph::has_edge  ----- */

          inline typename node_type::sequence_type
        node_sequence( id_type id ) const
        {
          rank_type rank = base_type::id_to_rank( id );
          return this->node_prop[ rank ].sequence;
        }  /* -----  end of method SeqGraph::node_sequence  ----- */

          inline typename node_type::sequence_type::size_type
        node_length( id_type id ) const
        {
          return this->node_sequence( id ).size();
        }  /* -----  end of method SeqGraph::node_length  ----- */

      protected:
        /* === ACCESSORS === */
          inline node_prop_type&
        get_node_prop( )
        {
          return this->node_prop;
        }  /* -----  end of method SeqGraph::get_node_prop  ----- */

          inline edge_prop_type&
        get_edge_prop( )
        {
          return this->edge_prop;
        }  /* -----  end of method SeqGraph::get_edge_prop  ----- */

      private:
        /* === DATA MEMBERS === */
        node_prop_type node_prop;
        edge_prop_type edge_prop;
    };  /* -----  end of template class SeqGraph  ----- */

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
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_SEQGRAPH_HPP__  ----- */
