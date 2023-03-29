/**
 *    @file  vg_utils.hpp
 *   @brief  SeqGraphs template interface functions for loading/extending vg graphs.
 *
 *  This header file includes interface function definitions specialised for vg data
 *  type.
 *
 *  NOTE: The interface functions are template functions parameterised by vg data types
 *  (as template parameters). No specific data type from vg is explicitly used in this
 *  header file to prevent imposing unwanted dependencies especially when vg format is
 *  not used.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Jul 30, 2019  17:02
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_VG_UTILS_HPP__
#define  GUM_VG_UTILS_HPP__

#include <string>
#include <functional>

#include "coordinate.hpp"
#include "iterators.hpp"
#include "basic_types.hpp"
#include "seqgraph_interface.hpp"


namespace gum {
  namespace util {
    struct VGFormat {
      inline static const std::string FILE_EXTENSION = ".vg";
      /**
       *  NOTE: Node ID and offset type is defined independent of `libvgio` library.
       */
      typedef int64_t nid_t;  // decltype( vg::Node().id() )
      typedef std::size_t off_t;
      /**
       *  NOTE: The default coordinate system for vg graphs is
       *  `gum::coordinate::Identity`; i.e. the node IDs are identical to ones
       *  defined in vg graphs.
       */
      template< typename TGraph >
      using DefaultCoord = gum::CoordinateType< TGraph, gum::coordinate::Identity, nid_t >;
    };

    template< typename TGraph,
              typename TVGNode,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update_node( TGraph& graph, TVGNode const& node, VGFormat, TCoordinate&& coord={} )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( node.id() );
      if ( graph.has_node( id ) ) {
        graph.update_node( id, node_type( node.sequence(), node.name() ) );
      } else {
        throw std::runtime_error( "updating a node with non-existent ID" );
      }
    }

    template< typename TGraph,
              typename TVGNode,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add_node( TGraph& graph, TVGNode const& node, VGFormat, TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( node.id() );
      if ( !graph.has_node( id ) ) {
        id = graph.add_node( node_type( node.sequence(), node.name() ), id );
        coord( node.id(), id );
      }
      else if ( force ) {
        graph.update_node( id, node_type( node.sequence(), node.name() ) );
      }
      else throw std::runtime_error( "adding a node with duplicate ID" );
      return id;
    }

    template< typename TGraph,
              typename TVGEdge,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add_edge( TGraph& graph, TVGEdge const& edge, VGFormat, TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;

      id_type src_id = coord( edge.from() );
      id_type sink_id = coord( edge.to() );
      if ( !force && !( graph.has_node( src_id ) && graph.has_node( sink_id ) ) ) {
        throw std::runtime_error( "adding an edge with non-existent adjacent node IDs" );
      }
      if ( !graph.has_node( src_id ) ) {
        src_id = graph.add_node( src_id );
        coord( edge.from(), src_id );
      }
      if ( !graph.has_node( sink_id ) ) {
        sink_id = graph.add_node( sink_id );
        coord( edge.to(), sink_id );
      }
      graph.add_edge(
          link_type( src_id, !edge.from_start(), sink_id, edge.to_end() ),
          edge_type( edge.overlap() ) );
    }

    template< typename TGraph,
              typename TVGPath,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_path( TGraph& graph, typename TGraph::id_type pid, TVGPath const& path, VGFormat,
                 TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using mappings_type = std::decay_t< decltype( TVGPath().mapping() ) >;
      using mapping_rank_type = std::decay_t< decltype( TVGPath().mapping()[0].rank() ) >;
      using orientations_type = RandomAccessProxyContainer< mappings_type, bool >;

      if ( !graph.has_path( pid ) ) {
        throw std::runtime_error( "extending a path with non-existent ID" );
      }

      mappings_type const& mappings = path.mapping();
      auto compare_rank
          = []( auto const& left, auto const& right ) {
            return left.rank() < right.rank();
          };
      // sort mappings by their ranks
      auto perm = util::sort_permutation( mappings, compare_rank );

      // validate if this chunk comes immediately after previously extended one without any gap in ranks
      mapping_rank_type min_rank = path.mapping( perm[ 0 ] ).rank();
      mapping_rank_type max_rank = path.mapping( perm[ perm.size() - 1 ] ).rank();
      mapping_rank_type last_rank = graph.path_length( pid );
      if ( min_rank != last_rank + 1 ||
           max_rank - min_rank + 1 != static_cast< mapping_rank_type >( mappings.size() ) ) {
        throw std::runtime_error( "embedded path in graph has invalid Mapping ranks" );
      }

      std::vector< id_type > nodes;
      nodes.reserve( mappings.size() );
      for ( auto const& m : mappings ) {
        id_type id = coord( m.position().node_id() );
        if ( !graph.has_node( id ) ) {
          if ( force ) {
            id = graph.add_node( id );
            coord( m.position().node_id(), id );
          }
          else throw std::runtime_error( "extending a path with non-existent nodes" );
        }
        nodes.push_back( id );
      }
      util::permute( perm, nodes );

      auto get_orient =
          []( auto const& m ) {
            return m.position().is_reverse();
          };
      orientations_type orients( &mappings, get_orient );
      graph.extend_path( pid, nodes.begin(), nodes.end(),
                         orients.begin(), orients.end() );
    }

    template< typename TGraph,
              typename TVGPath,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add_path( TGraph& graph, TVGPath const& path, VGFormat, TCoordinate&& coord={},
              bool force=false, bool force_node=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using rank_type = typename graph_type::rank_type;

      id_type path_id = 0;
      graph.for_each_path(
          [&graph, &path, &path_id]( rank_type, id_type pid ) {
            if ( graph.path_name( pid ) == path.name() ) {
              path_id = pid;
              return false;
            }
            return true;
          } );
      if ( path_id == 0 ) {
        path_id = graph.add_path( path.name() );
      }
      else if ( !force ) throw std::runtime_error( "adding a duplicate path" );
      extend_path( graph, path_id, path, VGFormat{}, coord, force_node );
      return path_id;
    }

    template< typename TGraph,
              typename TVGGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_graph( TGraph& graph, TVGGraph& other, VGFormat, bool sort=false, TCoordinate&& coord={} )
    {
      for ( auto const& node : other.node() ) {
        add_node( graph, node, VGFormat{}, coord, true );
      }
      for ( auto const& edge : other.edge() ) {
        add_edge( graph, edge, VGFormat{}, coord, true );
      }
      if ( sort ) {
        graph.sort_nodes();  // first, sort by ids
        gum::util::topological_sort( graph, true );
      }
      for ( auto const& path : other.path() ) {
        add_path( graph, path, VGFormat{}, coord, true, true );
      }
    }

    template< typename TGraph,
              typename TVGGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    load_graph( TGraph& graph, TVGGraph& other, VGFormat, TArgs&&... args )
    {
      graph.clear();
      extend_graph( graph, other, VGFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TVGPath >
    inline void
    _extend_path( TVGPath* output, TVGPath const& chunk, VGFormat )
    {
      for ( auto const& m : chunk.mapping() ) {
        auto mapping = output->add_mapping();
        auto pos = mapping->mutable_position();
        pos->set_node_id( m.position().node_id() );
        pos->set_offset( m.position().offset() );
        pos->set_is_reverse( m.position().is_reverse() );
        pos->set_name( m.position().name() );
        mapping->set_rank( m.rank() );
        for ( auto const& e : m.edit() ) {
          auto edit = mapping->add_edit();
          edit->set_from_length( e.from_length() );
          edit->set_to_length( e.to_length() );
          edit->set_sequence( e.sequence() );
        }
      }
    }

    template< typename TVGGraph >
    inline void
    merge_vg( TVGGraph& output, TVGGraph const& chunk )
    {
      using size_type = decltype( chunk.node_size() );

      for ( size_type i = 0; i < chunk.node_size(); ++i ) {
        auto node = output.add_node();
        node->set_id( chunk.node( i ).id() );
        node->set_sequence( chunk.node( i ).sequence() );
        node->set_name( chunk.node( i ).name() );
      }

      for ( size_type i = 0; i < chunk.edge_size(); ++i ) {
        auto edge = output.add_edge();
        edge->set_from( chunk.edge( i ).from() );
        edge->set_to( chunk.edge( i ).to() );
        edge->set_from_start( chunk.edge( i ).from_start() );
        edge->set_to_end( chunk.edge( i ).to_end() );
        edge->set_overlap( chunk.edge( i ).overlap() );
      }

      for ( auto const& p : chunk.path() ) {
        size_type i = 0;
        for ( ; i < output.path_size(); ++i ) {
          if ( output.path( i ).name() == p.name() ) break;
        }
        if ( i == output.path_size() ) {  // new path
          auto path = output.add_path();
          path->set_name( p.name() );
          path->set_is_circular( p.is_circular() );
          path->set_length( p.length() );
        }
        _extend_path( output.mutable_path( i ), p, VGFormat{} );
      }
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_VG_UTILS_HPP__ --- */
