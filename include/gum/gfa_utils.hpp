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
#include "seqgraph_interface.hpp"


namespace gum {
  namespace util {
    struct GFAFormat {
      /* === TYPE MEMBERS === */
      union Version {
        double m_d;
      };
      /* === STATIC MEMBERS === */
      inline static const std::string FILE_EXTENSION = ".gfa";
      /* === TYPE MEMBERS === */
      /**
       *  NOTE: The default coordinate system for GFA graphs is
       *  `gum::coordinate::Stoid`; i.e. the node IDs in the coordinate system
       *  are integer representation of GFA node names retrieved by string to
       *  integer conversion.
       */
      template< typename TGraph >
      using DefaultCoord = gum::CoordinateType< TGraph, gum::coordinate::Stoid >;
      /* === LIFECYCLE === */
      GFAFormat( Version ver={ 1.0 } ) : m_v( ver ) { }
      /* === SETTERS === */
      inline void
      set_version( Version ver )
      {
        this->m_v = ver;
      }

      inline void
      set_version( double ver )
      {
        this->m_v.m_d = ver;
      }
      /* === GETTERS === */
      inline Version
      version() const
      {
        return this->m_v;
      }

      inline double
      version_d() const
      {
        return this->m_v.m_d;
      }
    private:
      Version m_v;
    };

    inline static const GFAFormat GFA1{ GFAFormat::Version{ 1.0 } };
    inline static const GFAFormat GFA1_1{ GFAFormat::Version{ 1.1 } };
    inline static const GFAFormat GFA2{ GFAFormat::Version{ 2.0 } };

    /**
     *  @brief  Update an exisiting node in the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External node
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
              typename TGFAKNode,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update_node( TGraph& graph, TGFAKNode const& elem, GFAFormat, TCoordinate&& coord={} )
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

    /**
     *  @brief  Update an exisiting node in the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External node
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
     *  - `element_type` is an external type of the element to be updated; e.g. node, etc
     */
    template< typename TGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, gfak::sequence_elem const& elem, TCoordinate&& coord={} )
    {
      update_node( graph, elem, GFAFormat{}, coord );
    }

    /**
     *  @brief  Add a node to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External node
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
              typename TGFAKNode,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add_node( TGraph& graph, TGFAKNode const& elem, GFAFormat, TCoordinate&& coord={}, bool force=false )
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

    /**
     *  @brief  Add a node to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External node
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
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, gfak::sequence_elem const& elem, TCoordinate&& coord={},
         bool force=false )
    {
      return add_node( graph, elem, GFAFormat{}, coord, force );
    }

    /**
     *  @brief  Add an edge to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External edge
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force node creation if any adjacent node does not exist in the graph
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
              typename TGFAKEdge,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add_edge( TGraph& graph, TGFAKEdge const& elem, GFAFormat, TCoordinate&& coord={},
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

    /**
     *  @brief  Add an edge to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External edge
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
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, gfak::edge_elem const& elem, TCoordinate&& coord={},
         bool force=false )
    {
      add_edge( graph, elem, GFAFormat{}, coord, force );
    }

    /**
     *  @brief  Extend a path in the graph with an external path (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  pid Path id to extend
     *  @param  elem External path
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force node creation if any node in the path does not exist
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
              typename TGFAKPath,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_path( TGraph& graph, typename TGraph::id_type pid, TGFAKPath const& elem, GFAFormat,
                 TCoordinate&& coord={}, bool force=false )
    {
      using graph_type = TGraph;
      using id_type = typename graph_type::id_type;

      if ( !graph.has_path( pid ) ) {
        throw std::runtime_error( "extending a path with non-existent ID" );
      }

      auto get_id =
          [&graph, &coord, force]( std::string const& name ) -> id_type {
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
      auto get_orient = []( bool fwd ) -> bool { return !fwd; };
      RandomAccessProxyContainer nodes( &elem.segment_names, get_id );
      RandomAccessProxyContainer orients( &elem.orientations, get_orient );
      graph.extend_path( pid, nodes.begin(), nodes.end(),
                         orients.begin(), orients.end() );
    }

    /**
     *  @brief  Extend a path in the graph with an external path (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  pid Path id to extend
     *  @param  elem External path
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
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, typename TGraph::id_type pid, gfak::path_elem const& elem,
            TCoordinate&& coord={}, bool force=false )
    {
      extend_path( graph, pid, elem, GFAFormat{}, coord, force );
    }

    /**
     *  @brief  Add a path to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External path
     *  @param  tag Format specifier tag
     *  @param  coord Coorindate system converting the given node ids to graph local ids
     *  @param  force Force adding the given path even if it already exists
     *  @param  force_node Force node creation if any node in the path does not exist
     *
     *  This function is a part of `add_path` family of interface functions with this
     *  signature for different external graph data structures:
     *
     *  @code
     *      add_path( graph_type, path_type, format_tag, arg_types... )
     *  @endcode
     *
     *  where:
     *  - `graph_type` is native graph type (i.e. `SeqGraph`),
     *  - `path_type` is an external type; for example `gfak::path_elem`, or `vg::Path`,
     *  - `format_tag` specifies the type of the external graph whose path is processed;
     *    e.g. `GFAFormat` or `VGFormat`.
     *
     *  In this family of functions, all external types (for nodes, edges, and graphs)
     *  are specified by template parameters. That is why a tag is needed as a format
     *  specifier.
     */
    template< typename TGraph,
              typename TGFAKPath,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add_path( TGraph& graph, TGFAKPath const& elem, GFAFormat, TCoordinate&& coord={},
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

    /**
     *  @brief  Add a path to the graph (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  elem External path
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
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, gfak::path_elem const& elem, TCoordinate&& coord={},
         bool force=false, bool force_node=false )
    {
      return add_path( graph, elem, GFAFormat{}, coord, force, force_node );
    }

    /**
     *  @brief  Extend a native graph with an external one (GFA overload).
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
              typename TGFAKGraph,
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend_graph( TGraph& graph, TGFAKGraph& other, GFAFormat, bool sort=false,
                  TCoordinate&& coord={} )
    {
      for ( auto const& rec : other.get_name_to_seq() ) {
        add( graph, rec.second, coord, true );
      }
      for ( auto const& rec : other.get_seq_to_edges() ) {
        for ( auto const& elem : rec.second ) {
          add( graph, elem, coord, true );
        }
      }
      if ( sort ) {
        graph.sort_nodes();  // first, sort by ids
        gum::util::topological_sort( graph, true );
      }
      for ( auto const& rec : other.get_name_to_path() ) {
        add( graph, rec.second, coord, true, true );
      }
    }

    /**
     *  @brief  Extend a native graph with an external one (GFA overload).
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
              typename TCoordinate=GFAFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, gfak::GFAKluge& other, bool sort=false, TCoordinate&& coord={} )
    {
      extend_graph( graph, other, GFAFormat{}, sort, coord );
    }

    /**
     *  @brief  Load a native graph with an external one (GFA overload).
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
              typename TGFAKGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    load_graph( TGraph& graph, TGFAKGraph& other, GFAFormat, TArgs&&... args )
    {
      graph.clear();
      extend_graph( graph, other, GFAFormat{}, std::forward< TArgs >( args )... );
    }

    /**
     *  @brief  Load a native graph with an external one (GFA overload).
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
    load( TGraph& graph, gfak::GFAKluge& other, TArgs&&... args )
    {
      load_graph( graph, other, GFAFormat{}, std::forward< TArgs >( args )... );
    }

    /**
     *  @brief  Extend a native graph with an external one using an `ExternalLoader` (GFA overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  in Input stream
     *  @param  loader An instance of `ExternalLoader`
     *  @param  args Arguments passed to `extend_graph`
     *
     *  This overload uses parsing mechanism provided as input argument. Any calls to
     *  `load`/`extend`/`load_{gfa|vg|hg}`/`extend_{gfa|vg|hg}` with `ExternalLoader`
     *  will be delegated to this overload instead of using the bundled dependencies to
     *  parse the input graph stream.
     */
    template< typename TGraph,
              typename TGFAKGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::istream& in, ExternalLoader< TGFAKGraph > loader, TArgs&&... args )
    {
      TGFAKGraph other = loader( in );
      extend_graph( graph, other, GFAFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::istream& in, GFAFormat fmt, TArgs&&... args )
    {
      gfak::GFAKluge gg;
      gg.set_version( fmt.version_d() );
      gg.parse_gfa_file( in );
      extend( graph, gg, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::istream& in, TArgs&&... args )
    {
      extend_gfa( graph, in, GFAFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_gfa( TGraph& graph, std::string fname, TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_gfa( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, GFAFormat fmt, TArgs&&... args )
    {
      extend_gfa( graph, in, fmt, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, GFAFormat fmt, TArgs&&... args )
    {
      extend_gfa( graph, std::move( fname ), fmt, std::forward< TArgs >( args )... );
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
    load( TGraph& graph, std::istream& in, GFAFormat fmt, TArgs&&... args )
    {
      load_gfa( graph, in, fmt, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, GFAFormat fmt, TArgs&&... args )
    {
      load_gfa( graph, std::move( fname ), fmt, std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GFA_UTILS_HPP__ --- */
