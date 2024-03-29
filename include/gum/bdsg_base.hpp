/**
 *    @file  bdsg_base.hpp
 *   @brief  Base header file defining interface functions and data types for HashGraph/vg files.
 *
 *  This header file is the core part of the bdsg utility module.
 *
 *  NOTE: Do not include this file directly; use `io_utils.hpp` or `bdsg_utils.hpp`.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Apr 4, 2023  20:42
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_BDSG_BASE_HPP__
#define GUM_BDSG_BASE_HPP__

#include <bdsg/hash_graph.hpp>


namespace gum {
  namespace util {
    /**
     *  @brief  Update an exisiting node in the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  eid External node id
     *  @param  seq External node sequence
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
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={} )
    {
      update_node( graph, eid, std::move( seq ), HGFormat{}, coord );
    }

    /**
     *  @brief  Add a node to the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  eid External node id
     *  @param  seq External node sequence
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
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={},
         bool force=false )
    {
      return add_node( graph, eid, std::move( seq ), HGFormat{}, coord, force );
    }

    /**
     *  @brief  Add an edge to the graph (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  from External edge 'from' id
     *  @param  from_start External edge from side
     *  @param  to External edge 'to' id
     *  @param  to_end External edge to side
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
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, HGFormat::nid_t from, bool from_start, HGFormat::nid_t to,
         bool to_end, HGFormat::off_t overlap, TCoordinate&& coord={}, bool force=false )
    {
      add_edge( graph, from, from_start, to, to_end, overlap, HGFormat{}, coord, force );
    }

    /**
     *  @brief  Extend a native graph with an external one (HashGraph overload).
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
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, bdsg::HashGraph const& other, bool sort=false,
            TCoordinate&& coord={} )
    {
      extend_graph( graph, other, HGFormat{}, sort, coord );
    }

    /**
     *  @brief  Load a native graph with an external one (HashGraph overload).
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
    load( TGraph& graph, bdsg::HashGraph const& other, TArgs&&... args )
    {
      load_graph( graph, other, HGFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_hg( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      bdsg::HashGraph other( in );
      extend( graph, other, std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif // GUM_BDSG_BASE_HPP__
