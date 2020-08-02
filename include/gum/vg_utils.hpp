/**
 *    @file  vg_utils.hpp
 *   @brief  vg utility functions and data types.
 *
 *  This header file is a part of the utility header files specialised for vg data type.
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
#include <istream>
#include <functional>

#include <vg/vg.pb.h>
#include <vg/io/stream.hpp>

#include "seqgraph.hpp"
#include "coordinate.hpp"
#include "basic_utils.hpp"


namespace gum {
  namespace util {
    struct VGFormat {
      inline static const std::string FILE_EXTENSION = ".vg";
      /**
       *  NOTE: The default coordinate system for vg graphs is
       *  `gum::coordinate::Identity`; i.e. the node IDs are identical to ones
       *  defined in vg graphs.
       */
      template< typename TGraph >
      using DefaultCoord = gum::CoordinateType< TGraph, gum::coordinate::Identity,
                                                decltype( vg::Node().id() ) >;
    };

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, vg::Node const& node, TCoordinate&& coord={} )
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
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Node const& node, TCoordinate&& coord={}, bool force=false )
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
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, vg::Edge const& edge, TCoordinate&& coord={}, bool force=false )
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
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, typename TGraph::id_type pid, vg::Path const& path,
            TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using mappings_type = std::decay_t< decltype( vg::Path().mapping() ) >;
      using mapping_rank_type = std::decay_t< decltype( vg::Path().mapping()[0].rank() ) >;
      using nodes_type = RandomAccessProxyContainer< mappings_type, id_type >;
      using orientations_type = RandomAccessProxyContainer< mappings_type, bool >;

      if ( !graph.has_path( pid ) ) {
        throw std::runtime_error( "extending a path with non-existent ID" );
      }

      mappings_type const& mappings = path.mapping();
      mapping_rank_type rank = graph.path_length( pid );
      for ( auto const& m : mappings ) {
        if ( ++rank != m.rank() ) {
          throw std::runtime_error( "extending a path with unordered node ranks" );
        }
      }

      auto get_id =
          [&graph, &coord, force]( auto const& m ) {
            id_type id = coord( m.position().node_id() );
            if ( !graph.has_node( id ) ) {
              if ( force ) {
                id = graph.add_node( id );
                coord( m.position().node_id(), id );
              }
              else throw std::runtime_error( "extending a path with non-existent nodes" );
            }
            return id;
          };
      auto get_orient =
          []( auto const& m ) {
            return m.position().is_reverse();
          };
      nodes_type nodes( &mappings, get_id );
      orientations_type orients( &mappings, get_orient );
      graph.extend_path( pid, nodes.begin(), nodes.end(),
                         orients.begin(), orients.end() );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Path const& path, TCoordinate&& coord={}, bool force=false,
         bool force_node=false )
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
      extend( graph, path_id, path, coord, force_node );
      return path_id;
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, vg::Graph& other, TCoordinate&& coord={} )
    {
      for ( auto const& node : other.node() ) {
        add( graph, node, coord, true );
      }
      for ( auto const& edge : other.edge() ) {
        add( graph, edge, coord, true );
      }
      for ( auto const& path : other.path() ) {
        add( graph, path, coord, true, true );
      }
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_vg( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      std::function< void( vg::Graph& ) > handle_chunks =
          [&]( vg::Graph& other ) {
            extend( graph, other, std::forward< TArgs >( args )... );
          };
      vg::io::for_each( in, handle_chunks );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_vg( TGraph& graph, std::string fname, TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_vg( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, VGFormat, TArgs&&... args )
    {
      extend_vg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, VGFormat, TArgs&&... args )
    {
      extend_vg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_vg( TGraph& graph, TArgs&&... args )
    {
      graph.clear();
      extend_vg( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::istream& in, VGFormat, TArgs&&... args )
    {
      load_vg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, VGFormat, TArgs&&... args )
    {
      load_vg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_VG_UTILS_HPP__ --- */
