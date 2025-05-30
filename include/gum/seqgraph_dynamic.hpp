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
    using str_const_reference = typename node_prop_type::str_const_reference;
    using str_reference = typename node_prop_type::str_reference;
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

    template< typename TGraph >
    class NodeFlipper {
      public:
        /* === TYPE MEMBERS === */
        using graph_type = TGraph;
        using id_type = typename graph_type::id_type;
        using function_type = std::function< void( std::string const& ) >;
        using container_type = phmap::flat_hash_set< id_type >;
        /* === LIFECYCLE === */
        NodeFlipper( TGraph* ptr, bool annotate = false, bool lazy = false,
                     function_type info = nullptr,
                     function_type warn = nullptr )
            : m_ptr( ptr ), m_annotate( annotate ), m_lazy( lazy ),
              m_info( info ), m_warn( warn )
        { }

        ~NodeFlipper() noexcept
        {
          this->flush();
        }
        /* === METHOD === */
        inline bool
        flip_orientation( id_type id )
        {
          if ( !this->m_lazy ) {
            if ( !this->_flip( id ) ) return false;
          }
          this->_stash( id );
          return true;
        }

        inline void
        flush() noexcept
        {
          if ( this->m_lazy ) {
            for ( auto id : this->m_stash ) this->_flip( id );
          }
          this->m_ptr->graph_prop.flip_orientation( this->m_stash );
          this->m_stash.clear();
        }

        inline void
        discard() noexcept
        {
          this->m_stash.clear();
        }

      private:
        /* === DATA MEMBERS === */
        TGraph* m_ptr;
        bool m_annotate;
        bool m_lazy;
        function_type m_info;
        function_type m_warn;
        container_type m_stash;
        /* === METHODS === */
        inline bool
        _flip( id_type id )
        {
          if ( !this->m_ptr->_flip_orientation_impl( id, this->m_annotate ) ) {
            if ( this->m_warn ) {
              this->m_warn( "cannot flip the orientation of node '"
                            + std::to_string( id ) + "'" );
            }
            return false;
          }
          return true;
        }

        inline void
        _stash( id_type id )
        {
          auto itr = this->m_stash.find( id );
          if ( itr != this->m_stash.end() ) {
            if ( this->m_info ) {
              this->m_info( "double flipping orientation of node '"
                            + std::to_string( id ) + "'" );
            }
            this->m_stash.erase( itr );
          }
          else {
            this->m_stash.insert( id );
          }
        }
    };

    template< typename ...TArgs >
    inline auto
    get_node_flipper( TArgs&&... args )
    {
      return NodeFlipper( this, std::forward< TArgs >( args )... );
    }

    /**
     *  @brief Flip the orientation of a node.
     *
     *  @param id       Node ID.
     *  @param annotate If true, the node's name will be appended by "-". If
     *                  already flipped, the annotation will be removed.
     *
     *  NOTE: Flipping a node also requires updating its orientation in all
     *  embedded paths to maintain the integrity of the graph. However,
     *  flipping nodes one at a time is inefficient. For better performance,
     *  you can use a `NodeFlipper` by calling `get_node_flipper`. It offers an
     *  RAII design to make sure that paths are compliant with the graph
     *  topology.
     */
    inline bool
    flip_orientation( id_type id, bool annotate=false )
    {
      if ( !this->_flip_orientation_impl( id, annotate ) ) return false;
      this->graph_prop.flip_orientation( id );
      return true;
    }

    template< typename TSet >
    inline bool
    flip_orientation( TSet const& node_set, bool annotate=false )
    {
      std::size_t cnt = 0;
      for ( auto id : node_set ) {
        if ( this->_flip_orientation_impl( id, annotate ) ) cnt++;
      }
      if ( cnt > 0 ) this->graph_prop.flip_orientation( node_set );
      return cnt > 0;
    }

    template< typename TGraph >
    class EdgeFlipper {
      public:
        /* === TYPE MEMBERS === */
        using graph_type = TGraph;
        using side_type = typename graph_type::side_type;
        using function_type = std::function< void( std::string const& ) >;
        using container_type = phmap::flat_hash_set< std::pair< side_type, side_type > >;
        /* === LIFECYCLE === */
        EdgeFlipper( TGraph* ptr, bool swap = false, bool lazy = false,
                     function_type info = nullptr,
                     function_type warn = nullptr )
            : m_ptr( ptr ), m_swap( swap ), m_lazy( lazy ), m_info( info ),
              m_warn( warn )
        { }

        ~EdgeFlipper() noexcept
        {
          this->flush();
        }
        /* === METHOD === */
        inline bool
        flip_edge( side_type from, side_type to )
        {
          if ( !this->m_lazy ) {
            return this->_flip( from, to );
          }
          this->_stash( from, to );
          return true;
        }

        inline void
        flush() noexcept
        {
          for ( auto const& [ from, to ] : this->m_stash ) {
            this->_flip( from, to );
          }
          this->m_stash.clear();
        }

        inline void
        discard() noexcept
        {
          this->m_stash.clear();
        }
        /* === STATIC METHODS === */
        inline std::string
        edge_to_str( side_type from, side_type to ) const
        {
          return "(" + std::to_string( this->m_ptr->id_of( from ) )
                 + ( this->m_ptr->is_end_side( from ) ? "" : "-" ) + ", "
                 + std::to_string( this->m_ptr->id_of( to ) )
                 + ( this->m_ptr->is_start_side( to ) ? "" : "-" ) + ")";
        }

      private:
        /* === DATA MEMBERS === */
        TGraph* m_ptr;
        bool m_swap;
        bool m_lazy;
        function_type m_info;
        function_type m_warn;
        container_type m_stash;
        /* === METHOD === */
        inline bool
        _flip( side_type from, side_type to )
        {
          if ( !this->m_ptr->flip_edge( from, to, this->m_swap ) ) {
            if ( this->m_warn ) {
              this->m_info( "cannot flip edge "
                            + this->edge_to_str( from, to ) );
            }
            return false;
          }
          return true;
        }

        inline void
        _stash( side_type from, side_type to )
        {
          auto itr = this->m_stash.find( { from, to } );
          if ( itr != this->m_stash.end() ) {
            if ( this->m_info ) {
              this->m_info( "double flipping of edge "
                            + this->edge_to_str( from, to ) );
            }
            this->m_stash.erase( itr );
          }
          else {
            this->m_stash.insert( { from, to } );
          }
        }
    };

    template< typename ...TArgs >
    inline auto
    get_edge_flipper( TArgs&&... args )
    {
      return EdgeFlipper( this, std::forward< TArgs >( args )... );
    }

    inline bool
    flip_edge( side_type from, side_type to, bool swap = false )
    {
      auto old_link = this->make_link( from, to );
      auto new_link = this->make_link( to, from );
      auto res = base_type::flip_edge( from, to, swap );
      if ( res ) {
        res = this->edge_prop.change_edge( old_link, new_link, swap );
        if ( !res ) base_type::flip_edge( to, from, swap );  // revert
      }
      return res;
    }

    inline bool
    flip_edge( link_type link, bool swap = false )
    {
      return this->flip_edge( this->from_side( link ), this->to_side( link ), swap );
    }

    /**
     *  @brief Make edges canonical by arranging node orientations.
     *
     *  NOTE: The initial node ranks are crucial for effectiveness of final
     *  graph. Ranking nodes based on the semi-topological order or their node
     *  ids when indicates the structure of sequences encoded in the graph,
     *  would be a good start.
     */
    inline void
    make_edges_canonical( std::function< void( std::string const& ) > info=nullptr,
                          std::function< void( std::string const& ) > warn=nullptr )
    {
      if ( warn && this->has_any_parallel_edge() ) {
        warn( "graph has parallel edges" );
      }

      sdsl::bit_vector visited( this->get_node_count() + 1, 0 );
      this->for_each_node( [ & ]( rank_type rank, id_type id ) {
        visited[ rank ] = 1;  // assume forward

        if ( this->_is_unambiguously_forward( id, visited ) ) {
          bool r;
          r = this->_make_outgoing_edges_canonical( id, visited, info, warn );
          assert( r );
          assert( this->indegree( this->end_side( id ) ) == 0 );
          r = this->_make_incoming_edges_canonical( id, visited, info, warn );
          assert( r );
          assert( this->outdegree( this->start_side( id ) ) == 0 );
        }
        else {
          if ( warn ) {
            warn( "cannot unambiguously determine orientation of node '"
                  + std::to_string( id ) + "'" );
          }
        }

        return true;
      } );
    }

    inline bool
    _is_unambiguously_forward( id_type id, sdsl::bit_vector& visited ) const
    {
      auto start = this->start_side( id );
      auto end = this->end_side( id );
      phmap::flat_hash_set< side_type > fwd_adjs;  // to detect common adjacents

      return this->for_each_edges_in( end, [&]( side_type from ) {
        auto f_id = this->id_of( from );
        auto f_rank = this->id_to_rank( f_id );
        fwd_adjs.insert( from );
        if ( this->is_end_side( from ) && visited[ f_rank ] == 1 ) {
          return false;
        }
        return true;
      } ) && this->for_each_edges_out( end, [&]( side_type to ) {
        auto t_id = this->id_of( to );
        auto t_rank = this->id_to_rank( t_id );
        fwd_adjs.insert( to );
        if ( this->is_end_side( to ) && visited[ t_rank ] == 1 ) {
          return false;
        }
        return true;
      } ) && this->for_each_edges_out( start, [&]( side_type to ) {
        auto t_id = this->id_of( to );
        auto t_rank = this->id_to_rank( t_id );
        if ( ( this->is_start_side( to ) && visited[ t_rank ] == 1 )
             || fwd_adjs.count( to ) ) {  // both sides have a common adjacent
          return false;
        }
        return true;
      } );
    }

    inline bool
    _make_outgoing_edges_canonical(
        id_type id, sdsl::bit_vector& visited,
        std::function< void( std::string const& ) > info,
        std::function< void( std::string const& ) > warn )
    {
      auto node_flipper = this->get_node_flipper(
          /*annotate*/ false, /*lazy*/ true, info, warn );
      auto edge_flipper = this->get_edge_flipper(
          /*swap*/ false, /*lazy*/ true, info, warn );

      // find edges to this end side, canonicalise edges, and adjust
      // adjacent nodes' orientation
      auto end = this->end_side( id );
      bool unanimously_fwd = true;
      this->for_each_edges_in( end, [&]( side_type from ) {
        auto from_id = this->id_of( from );
        auto from_rank = this->id_to_rank( from_id );
        if ( this->is_end_side( from ) ) {
          if ( visited[ from_rank ] == 1 ) {
            unanimously_fwd = false;
            return true;  // continue
          }
          // flip adjacent nodes that are reversed from this node's
          // perspective
          if ( info ) {
            info( "flipping node '" + std::to_string( from_id ) + "'" );
          }
          // mark to flip
          node_flipper.flip_orientation( from_id );
          from = this->start_side( from_id );  // node has flipped
        }
        // assume adjacents' orientation
        visited[ from_rank ] = 1;
        if ( info ) {
          info( "flipping edge " + edge_flipper.edge_to_str( from, end ) );
        }
        // mark to flip
        edge_flipper.flip_edge( from, end );
        return true;
      } );

      node_flipper.flush();
      edge_flipper.flush();

      // Assume adjacents' orientation forward
      this->for_each_edges_out( end, [&]( side_type to ) {
        auto to_id = this->id_of( to );
        auto to_rank = this->id_to_rank( to_id );
        if ( this->is_end_side( to ) ) {
          if ( visited[ to_rank ] == 1 ) {
            unanimously_fwd = false;
            return true;  // continue
          }
          if ( info ) {
            info( "flipping node '" + std::to_string( to_id ) + "'" );
          }
          node_flipper.flip_orientation( to_id );
        }
        visited[ to_rank ] = 1;
        return true;
      } );

      return unanimously_fwd;
    }

    inline bool
    _make_incoming_edges_canonical(
        id_type id, sdsl::bit_vector& visited,
        std::function< void( std::string const& ) > info,
        std::function< void( std::string const& ) > warn )
    {
      auto node_flipper = this->get_node_flipper(
          /*annotate*/ false, /*lazy*/ true, info, warn );
      auto edge_flipper = this->get_edge_flipper(
          /*swap*/ false, /*lazy*/ true, info, warn );

      // find edges to this end side, canonicalise edges, and adjust
      // adjacent nodes' orientation
      auto start = this->start_side( id );
      bool unanimously_fwd = true;
      this->for_each_edges_out( start, [&]( side_type to ) {
        auto to_id = this->id_of( to );
        auto to_rank = this->id_to_rank( to_id );
        if ( this->is_start_side( to ) ) {
          if ( visited[ to_rank ] == 1 ) {
            unanimously_fwd = false;
            return true;  // continue
          }
          // flip adjacent nodes that are reversed from this node's
          // perspective
          if ( info ) {
            info( "flipping node '" + std::to_string( to_id ) + "'" );
          }
          // mark to flip
          node_flipper.flip_orientation( to_id );
          to = this->end_side( to_id );  // node has flipped
        }
        // assume adjacents' orientation
        visited[ to_rank ] = 1;
        if ( info ) {
          info( "flipping edge " + edge_flipper.edge_to_str( start, to ) );
        }
        // mark to flip
        edge_flipper.flip_edge( start, to );
        return true;
      } );

      node_flipper.flush();
      edge_flipper.flush();

      return unanimously_fwd;
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

    inline str_const_reference
    node_name( id_type id ) const
    {
      rank_type rank = base_type::id_to_rank( id );
      return this->node_prop( rank ).name;
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
    /* === METHODS === */
    inline bool
    _flip_orientation_impl( id_type id, bool annotate=false )
    {
      rank_type rank = base_type::id_to_rank( id );
      if ( rank == 0 ) return false;
      this->_flip_edge_props( id );
      this->node_prop.flip_orientation( rank, annotate );
      auto res = base_type::flip_orientation( id );
      assert( res );  // should always succeed
      return res;
    }

    inline void
    _flip_edge_props( id_type id )
    {
      phmap::flat_hash_set< side_type > visited;  // to avoid double flipping
      this->for_each_side( id, [&]( auto from ) {
        return this->for_each_edges_out( from, [&]( auto to ) {
          if ( visited.count( to ) ) return true;
          visited.insert( to );
          auto opposite = this->opposite_side( from );
          auto old_link = this->make_link( from, to );
          auto new_link = this->make_link( opposite, to );
          this->edge_prop.change_edge( old_link, new_link, /*swap=*/true );
          return true;
        } );
      } );

      visited.clear();
      this->for_each_side( id, [&]( auto to ) {
        return this->for_each_edges_in( to, [&]( auto from ) {
          if ( visited.count( from ) ) return true;
          visited.insert( from );
          auto opposite = this->opposite_side( to );
          auto old_link = this->make_link( from, to );
          auto new_link = this->make_link( from, opposite );
          this->edge_prop.change_edge( old_link, new_link, /*swap=*/true );
          return true;
        } );
      } );
    }
  };  /* --- end of template class SeqGraph --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_SEQGRAPH_DYNAMIC_HPP__ --- */
