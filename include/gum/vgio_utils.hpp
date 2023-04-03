/**
 *    @file  vgio_utils.hpp
 *   @brief  vg utility functions and data types.
 *
 *  This header file is a part of the io utility header files specialised for vg file
 *  format.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali@ghaffaari.com>
 *
 *  @internal
 *       Created:  Mon Dec 26, 2022  22:35
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2022, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_VGIO_UTILS_HPP__
#define  GUM_VGIO_UTILS_HPP__

#include <istream>

#include <vg/vg.pb.h>
#include <vg/io/stream.hpp>

#include "vg_utils.hpp"


namespace gum {
  namespace util {
    /**
     *  @brief  Update an exisiting node in the graph (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  node External node
     *  @param  coord Coorindate system converting the given node id to graph local id
     *
     *  This is a part of `update` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      update( graph_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `element_type` is type of the external element to be updated; e.g. node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, vg::Node const& node, TCoordinate&& coord={} )
    {
      update_node( graph, node, VGFormat{}, coord );
    }

    /**
     *  @brief  Add a node to the graph (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  node External node
     *  @param  coord Coorindate system converting the given node id to graph local id
     *  @param  force Force node update if node already exists in the graph
     *
     *  This is a part of `add` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      add( graph_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `element_type` is type of the external element to be added; e.g. node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Node const& node, TCoordinate&& coord={}, bool force=false )
    {
      return add_node( graph, node, VGFormat{}, coord, force );
    }

    /**
     *  @brief  Add an edge to the graph (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  edge External edge
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force node creation if any adjacent node does not exist in the graph
     *
     *  This is a part of `add` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      add( graph_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `element_type` is type of the external element to be added; e.g. node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, vg::Edge const& edge, TCoordinate&& coord={}, bool force=false )
    {
      add_edge( graph, edge, VGFormat{}, coord, force );
    }

    /**
     *  @brief  Extend a path in the graph with an external path (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  pid Path id to extend
     *  @param  path External path
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force node creation if any node in the path does not exist
     *
     *  This is a part of `extend` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      1. extend( graph_type, element_type, args_type... )
     *      2. extend( graph_type, id_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `id_type` is the graph's id type indicating the id of the graph entity to be
     *    extended unless it is the graph itself (1),
     *  - `element_type` is type of the external element to be used for extension; e.g.
     *    node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, typename TGraph::id_type pid, vg::Path const& path,
            TCoordinate&& coord={}, bool force=false )
    {
      extend_path( graph, pid, path, VGFormat{}, coord, force );
    }

    /**
     *  @brief  Add a path to the graph (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  path External path
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force adding the given path even if it already exists
     *  @param  force_node Force node creation if any node in the path does not exist
     *
     *  This is a part of `add` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      add( graph_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `element_type` is type of the external element to be added; e.g. node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Path const& path, TCoordinate&& coord={}, bool force=false,
         bool force_node=false )
    {
      return add_path( graph, path, VGFormat{}, coord, force, force_node );
    }

    /**
     *  @brief  Extend a native graph with an external one (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  other External graph
     *  @param  sort Sort node ranks in topological order
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *
     *  This is a part of `extend` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      1. extend( graph_type, element_type, args_type... )
     *      2. extend( graph_type, id_type, element_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `id_type` is the graph's id type indicating the id of the graph entity to be
     *    extended unless it is the graph itself (1),
     *  - `element_type` is type of the external element to be used for extension; e.g.
     *    node, etc.
     */
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, vg::Graph& other, bool sort=false, TCoordinate&& coord={} )
    {
      extend_graph( graph, other, VGFormat{}, sort, coord );
    }

    /**
     *  @brief  Load a native graph with an external one (vg overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  other External graph
     *  @param  args Arguments passed to `load_graph`
     *
     *  This is a part of `load` family of interface functions with this signature
     *  overloaded for different external data structures:
     *
     *  @code
     *      load( graph_type, external_graph_type, args_type... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `external_graph_type` is type of the external graph to be loaded.
     */
    template< typename TGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    load( TGraph& graph, vg::Graph& other, TArgs&&... args )
    {
      load_graph( graph, other, VGFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_vg( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      vg::Graph merged;
      std::function< void( vg::Graph& ) > handle_chunks =
          [&]( vg::Graph& other ) {
            merge_vg( merged, static_cast< vg::Graph const& >( other ) );
          };
      vg::io::for_each( in, handle_chunks );
      extend( graph, merged, std::forward< TArgs >( args )... );
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
    _load_vg( TGraph& graph, Dynamic, TArgs&&... args )
    {
      graph.clear();
      extend_vg( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_vg( TGraph& graph, Succinct, TArgs&&... args )
    {
      typename TGraph::dynamic_type dyn_graph;
      extend_vg( dyn_graph, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_vg( TGraph& graph, TArgs&&... args )
    {
      _load_vg( graph, typename TGraph::spec_type(), std::forward< TArgs >( args )... );
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

#endif  /* --- #ifndef GUM_VGIO_UTILS_HPP__ --- */
