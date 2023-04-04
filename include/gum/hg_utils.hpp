/**
 *    @file  hg_utils.hpp
 *   @brief  SeqGraphs template interface functions for converting `HashGraph`s.
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

#define GUM_INCLUDED_HG


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

    /**
     *  @brief  Update an exisiting node in the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  eid External node id
     *  @param  seq External node sequence
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node id to graph local id
     *
     *  This function is a part of `update_node` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      update_node( graph_type, node_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `node_type` is an external type; for example `gfak::sequence_elem`, or `vg::Node`,
     *  - `format_tag` specifies the type of the external graph whose node is processed.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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

    /**
     *  @brief  Add a node to the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  eid External node id
     *  @param  seq External node sequence
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node id to graph local id
     *  @param  force Force node update if node already exists in the graph
     *
     *  This function is a part of `add_node` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      add_node( graph_type, node_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `node_type` is an external type; for example `gfak::sequence_elem`,
     *  - `format_tag` specifies the type of the external graph whose node is processed;
     *    e.g. `GFAFormat` or `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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

    /**
     *  @brief  Add an edge to the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  from External edge 'from' id
     *  @param  from_start External edge from side
     *  @param  to External edge 'to' id
     *  @param  to_end External edge to side
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force node creation if any adjacent node does not exist in the graph
     *
     *  The direction of added edge is indicated by two boolean values: `from_start` and
     *  `to_end`. A regular edge starts from the end of a node ('from' node) to the
     *  start of the adjacent node ('to' node). In this case, both `from_start` and
     *  `to_end` is `false` since the edge does not start from end of the first node and
     *  will not go to the end of the adjacent node.
     *
     *  This function is a part of `add_edge` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      add_edge( graph_type, edge_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `edge_type` is an external type; for example `gfak::edge_elem`, or `vg::Edge`,
     *  - `format_tag` specifies the type of the external graph whose edge is processed;
     *    e.g. `GFAFormat` or `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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

    /**
     *  @brief  Extend a set of paths in the graph with an external set of paths (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  other External set of path in the form of `HashGraph`
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *
     *  This function is a part of `extend_path` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      1. extend_path( graph_type, id_type, path_type, format_tag, arg_types... )
     *      2. extend_path( graph_type, external_graph_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `id_type` is path id type; i.e. id type defined by the graph_type (1),
     *  - `path_type` is an external type; for example `gfak::path_elem`, or `vg::Path` (1),
     *  - or a set of paths in the form of a graph with type `external_graph_type` (2),
     *  - `format_tag` specifies the type of the external graph whose path is processed;
     *    e.g. `GFAFormat` or `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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

    /**
     *  @brief  Extend a native graph with an external one (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  other External graph
     *  @param  tag Format specifier tag
     *  @param  sort Sort node ranks in topological order
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *
     *  This function is a part of `extend_graph` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      extend_graph( graph_type, external_graph_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `external_graph_type` is an external graph type; for example
     *    `gfak::graph_elem`, or `vg::Graph`,
     *  - `format_tag` specifies the type of the external graph; e.g. `GFAFormat` or
     *    `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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

    /**
     *  @brief  Load a native graph with an external one (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  other External graph
     *  @param  tag Format specifier tag
     *  @param  args Arguments passed to `extend_graph`
     *
     *  This function is a part of `load_graph` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      load_graph( graph_type, external_graph_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `external_graph_type` is an external graph type; for example
     *    `gfak::graph_elem`, or `vg::Graph`,
     *  - `format_tag` specifies the type of the external graph; e.g. `GFAFormat` or
     *    `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
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
