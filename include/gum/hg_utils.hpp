/**
 *    @file  hg_utils.hpp
 *   @brief  SeqGraphs template interface functions for loading/extending `HashGraph`s.
 *
 *  This header file includes interface function definitions specialised for `HashGraph`
 *  data type.
 *
 *  NOTE: The interface functions are template functions parameterised by data types
 *  defined in `libbdsg` (as template parameters). No specific data type from `libbdsg`
 *  is explicitly used in this header file to prevent imposing unwanted dependencies
 *  when `HashGraph` format is not used.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Wed Feb 10, 2021  23:19
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2021, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_HG_UTILS_HPP__
#define  GUM_HG_UTILS_HPP__

#include <string>

#include "coordinate.hpp"
#include "iterators.hpp"
#include "basic_types.hpp"
#include "seqgraph_interface.hpp"


namespace gum {
  namespace util {
    struct HGFormat {
      inline static const std::string FILE_EXTENSION = ".vg";
      /**
       *  NOTE: Node ID and offset type is defined independent of `handlegraph` library.
       */
      typedef long long int nid_t;
      typedef std::size_t off_t;
      /**
       *  NOTE: The default coordinate system for hash graphs is
       *  `gum::coordinate::Identity`; i.e. the node IDs are identical to ones
       *  defined in vg graphs.
       */
      template< typename TGraph >
      using DefaultCoord = gum::CoordinateType< TGraph, gum::coordinate::Identity, nid_t >;
    };

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update_node( TGraph& graph, HGFormat::nid_t eid, std::string seq, HGFormat, TCoordinate&& coord={} )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( eid );
      if ( graph.has_node( id ) ) {
        graph.update_node( id, node_type( std::move( seq ) ) );
      } else {
        throw std::runtime_error( "updating a node with non-existent ID" );
      }
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add_node( TGraph& graph, HGFormat::nid_t eid, std::string seq, HGFormat, TCoordinate&& coord={},
              bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using node_type = typename graph_type::node_type;

      id_type id = coord( eid );
      if ( !graph.has_node( id ) ) {
        id = graph.add_node( node_type( std::move( seq ) ), id );
        coord( eid, id );
      }
      else if ( force ) {
        graph.update_node( id, node_type( std::move( seq ) ) );
      }
      else throw std::runtime_error( "adding a node with duplicate ID" );
      return id;
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add_edge( TGraph& graph, HGFormat::nid_t from, bool from_start, HGFormat::nid_t to,
              bool to_end, HGFormat::off_t overlap, HGFormat, TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;

      id_type src = coord( from );
      id_type sink = coord( to );
      if ( !force && !( graph.has_node( src ) && graph.has_node( sink ) ) ) {
        throw std::runtime_error( "adding an edge with non-existent adjacent node IDs" );
      }
      if ( !graph.has_node( src ) ) {
        src = graph.add_node( src );
        coord( from, src );
      }
      if ( !graph.has_node( sink ) ) {
        sink = graph.add_node( sink );
        coord( to, sink );
      }
      graph.add_edge( link_type( src, !from_start, sink, to_end ), edge_type( overlap ) );
    }

    template< typename TGraph,
              typename THGGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_path( TGraph& graph, THGGraph const& other, HGFormat, TCoordinate&& coord={} )
    {
      using hg_path_handle_t = decltype( THGGraph{}.get_path_handle( "" ) );

      other.for_each_path_handle(
          [&]( hg_path_handle_t const& phandle ) -> bool {
            using graph_type = TGraph;
            using id_type = typename graph_type::id_type;
            using rank_type = typename graph_type::rank_type;

            auto path_name = other.get_path_name( phandle );
            id_type path_id = 0;
            graph.for_each_path(
                [&graph, &path_name, &path_id]( rank_type, id_type pid ) {
                  if ( graph.path_name( pid ) == path_name ) {
                    path_id = pid;
                    return false;
                  }
                  return true;
                } );
            if ( path_id == 0 ) path_id = graph.add_path( path_name );
            for ( auto const& handle : other.scan_path( phandle ) ) {
              id_type id = coord( other.get_id( handle ) );
              if ( !graph.has_node( id ) ) {
                throw std::runtime_error( "extending a path with non-existent nodes" );
              }
              graph.extend_path( path_id, id, other.get_is_reverse( handle ) );
            }
            return true;
          } );
    }

    template< typename TGraph,
              typename THGGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_graph( TGraph& graph, THGGraph const& other, HGFormat, bool sort=false,
                  TCoordinate&& coord={} )
    {
      using hg_handle_t = decltype( THGGraph{}.get_handle( HGFormat::nid_t{} ) );
      using hg_edge_t = decltype( THGGraph{}.edge_handle( hg_handle_t{}, hg_handle_t{} ) );

      other.for_each_handle(
          [&]( hg_handle_t const& handle ) -> bool {
            add_node( graph, other.get_id( handle ), other.get_sequence( handle ), HGFormat{},
                      coord, true );
            return true;
          } );
      other.for_each_edge(
          [&]( hg_edge_t const& edge ) -> bool {
            add_edge( graph, other.get_id( edge.first ), other.get_is_reverse( edge.first ),
                      other.get_id( edge.second ), other.get_is_reverse( edge.second ),
                      0 /* no overlap */, HGFormat{}, coord );
            return true;
          } );
      if ( sort ) {
        graph.sort_nodes();  // first, sort by ids
        gum::util::topological_sort( graph, true );
      }
      extend_path( graph, other, HGFormat{}, coord );
    }

    template< typename TGraph,
              typename THGGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    load_graph( TGraph& graph, THGGraph& other, HGFormat, TArgs&&... args )
    {
      graph.clear();
      extend_graph( graph, other, HGFormat{}, std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_HG_UTILS_HPP__ --- */
