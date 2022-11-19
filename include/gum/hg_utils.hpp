/**
 *    @file  hg_utils.hpp
 *   @brief  `HashGraph` utility functions and data types.
 *
 *  This header file is a part of the utility header files specialised for `HashGraph`
 *  data type.
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
#include <istream>

#include <bdsg/hash_graph.hpp>
#include <vg/io/vpkg.hpp>

#include "coordinate.hpp"
#include "iterators.hpp"
#include "basic_types.hpp"


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
    update( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={} )
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
    add( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={},
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
    add( TGraph& graph, HGFormat::nid_t from, bool from_start, HGFormat::nid_t to,
         bool to_end, HGFormat::off_t overlap, TCoordinate&& coord={}, bool force=false )
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
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, handlegraph::HandleGraph const& other, bool sort=false,
            TCoordinate&& coord={} )
    {
      other.for_each_handle(
          [&]( handlegraph::handle_t const& handle ) -> bool {
            add( graph, other.get_id( handle ), other.get_sequence( handle ), coord, true );
            return true;
          } );
      if ( sort ) graph.sort_nodes();
      other.for_each_edge(
          [&]( handlegraph::edge_t const& edge ) -> bool {
            add( graph, other.get_id( edge.first ), other.get_is_reverse( edge.first ),
                 other.get_id( edge.second ), other.get_is_reverse( edge.second ),
                 0 /* no overlap */, coord );
            return true;
          } );
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, handlegraph::PathHandleGraph const& other, bool sort=false,
            TCoordinate&& coord={} )
    {
      extend( graph, static_cast< handlegraph::HandleGraph const& >( other ), sort, coord );

      other.for_each_path_handle(
          [&]( handlegraph::path_handle_t const& phandle ) -> bool {
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

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_hg( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      bdsg::HashGraph other( in );
      extend( graph, other, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_hg( TGraph& graph, std::string fname, TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_hg( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, HGFormat, TArgs&&... args )
    {
      extend_hg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, HGFormat, TArgs&&... args )
    {
      extend_hg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_hg( TGraph& graph, Dynamic, TArgs&&... args )
    {
      graph.clear();
      extend_hg( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_hg( TGraph& graph, Succinct, TArgs&&... args )
    {
      typename TGraph::dynamic_type dyn_graph;
      extend_hg( dyn_graph, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_hg( TGraph& graph, TArgs&&... args )
    {
      _load_hg( graph, typename TGraph::spec_type(), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::istream& in, HGFormat, TArgs&&... args )
    {
      load_hg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, HGFormat, TArgs&&... args )
    {
      load_hg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_HG_UTILS_HPP__ --- */
