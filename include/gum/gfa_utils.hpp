/**
 *    @file  gfa_utils.hpp
 *   @brief  Utility functions and data types for working with GFA files.
 *
 *  This header file is a part of the utility header files specialised for working with
 *  GFA files.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Mon Aug 12, 2019  12:11
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_GFA_UTILS_HPP__
#define  GUM_GFA_UTILS_HPP__

#include <string>
#include <istream>

#include <gfakluge.hpp>

#include "coordinate.hpp"
#include "iterators.hpp"
#include "basic_types.hpp"


namespace gum {
  namespace util {
    struct GFAFormat {
      inline static const std::string FILE_EXTENSION = ".gfa";
      /**
       *  NOTE: The default coordinate system for GFA graphs is
       *  `gum::coordinate::Stoid`; i.e. the node IDs in the coordinate system
       *  are integer representation of GFA node names retrieved by string to
       *  integer conversion.
       */
      template< typename TGraph >
      using DefaultCoord = gum::CoordinateType< TGraph, gum::coordinate::Stoid >;
    };

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, gfak::sequence_elem const& elem, TCoordinate&& coord={} )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( elem.name );
      if ( graph.has_node( id ) ) {
        graph.update_node( id, node_type( elem.sequence, elem.name ) );
      } else {
        throw std::runtime_error( "updating a node with non-existent ID" );
      }
    }

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, gfak::sequence_elem const& elem, TCoordinate&& coord={},
         bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( elem.name );
      if ( !graph.has_node( id ) ) {
        id = graph.add_node( node_type( elem.sequence, elem.name ), id );
        coord( elem.name, id );
      }
      else if ( force ) {
        graph.update_node( id, node_type( elem.sequence, elem.name ) );
      }
      else throw std::runtime_error( "adding a node with duplicate ID" );
      return id;
    }

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, gfak::edge_elem const& elem, TCoordinate&& coord={},
         bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;

      id_type src_id = coord( elem.source_name );
      id_type sink_id = coord( elem.sink_name );
      if ( !force && !( graph.has_node( src_id ) && graph.has_node( sink_id ) ) ) {
        throw std::runtime_error( "adding an edge with non-existent adjacent node IDs" );
      }
      if ( !graph.has_node( src_id ) ) {
        src_id = graph.add_node( src_id );
        coord( elem.source_name, src_id );
      }
      if ( !graph.has_node( sink_id ) ) {
        sink_id = graph.add_node( sink_id );
        coord( elem.sink_name, sink_id );
      }
      auto source_len = graph.node_length( src_id );
      if ( ( elem.type != 1 && elem.type != 2 ) ||
           elem.sink_begin != 0 || elem.source_end != source_len ||
           elem.source_end - elem.source_begin != elem.sink_end ) {
        throw std::runtime_error( "only simple dovetail overlap is supported" );
      }
      graph.add_edge( link_type( src_id, elem.source_orientation_forward,
                                 sink_id, !elem.sink_orientation_forward ),
                      edge_type( elem.sink_end ) );
    }

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, typename TGraph::id_type pid, gfak::path_elem const& elem,
            TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using elem_segments_type = std::decay_t< decltype( elem.segment_names ) >;
      using elem_orients_type = std::decay_t< decltype( elem.orientations ) >;
      using nodes_type = RandomAccessProxyContainer< elem_segments_type, id_type >;
      using orientations_type = RandomAccessProxyContainer< elem_orients_type, bool >;

      if ( !graph.has_path( pid ) ) {
        throw std::runtime_error( "extending a path with non-existent ID" );
      }

      auto get_id =
          [&graph, &coord, force]( std::string const& name ) {
            id_type id = coord( name );
            if ( !graph.has_node( id ) ) {
              if ( force ) {
                id = graph.add_node( id );
                coord( name, id );
              }
              else throw std::runtime_error( "extending a path with non-existent nodes" );
            }
            return id;
          };
      auto get_orient = []( bool fwd ) { return !fwd; };
      nodes_type nodes( &elem.segment_names, get_id );
      orientations_type orients( &elem.orientations, get_orient );
      graph.extend_path( pid, nodes.begin(), nodes.end(),
                         orients.begin(), orients.end() );
    }

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, gfak::path_elem const& elem, TCoordinate&& coord={},
         bool force=false, bool force_node=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using rank_type = typename graph_type::rank_type;

      id_type path_id = 0;
      graph.for_each_path(
          [&graph, &elem, &path_id]( rank_type, id_type pid ) {
            if ( graph.path_name( pid ) == elem.name ) {
              path_id = pid;
              return false;
            }
            return true;
          } );
      if ( path_id == 0 ) {
        path_id = graph.add_path( elem.name );
      }
      else if ( !force ) throw std::runtime_error( "adding a duplicate path" );
      extend( graph, path_id, elem, coord, force_node );
      return path_id;
    }

    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, gfak::GFAKluge& other, bool sort=false, TCoordinate&& coord={} )
    {
      for ( auto const& rec : other.get_name_to_seq() ) {
        add( graph, rec.second, coord, true );
      }
      if ( sort ) graph.sort_nodes();
      for ( auto const& rec : other.get_seq_to_edges() ) {
        for ( auto const& elem : rec.second ) {
          add( graph, elem, coord, true );
        }
      }
      for ( auto const& rec : other.get_name_to_path() ) {
        add( graph, rec.second, coord, true, true );
      }
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      gfak::GFAKluge gg;
      gg.parse_gfa_file( in );
      extend( graph, gg, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::string fname, TArgs&&... args )
    {
      gfak::GFAKluge gg;
      gg.parse_gfa_file( fname );
      extend( graph, gg, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, GFAFormat, TArgs&&... args )
    {
      extend_gfa( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, GFAFormat, TArgs&&... args )
    {
      extend_gfa( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_gfa( TGraph& graph, Dynamic, TArgs&&... args )
    {
      graph.clear();
      extend_gfa( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_gfa( TGraph& graph, Succinct, TArgs&&... args )
    {
      typename TGraph::dynamic_type dyn_graph;
      extend_gfa( dyn_graph, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_gfa( TGraph& graph, TArgs&&... args )
    {
      _load_gfa( graph, typename TGraph::spec_type(), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::istream& in, GFAFormat, TArgs&&... args )
    {
      load_gfa( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, GFAFormat, TArgs&&... args )
    {
      load_gfa( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GFA_UTILS_HPP__ --- */
