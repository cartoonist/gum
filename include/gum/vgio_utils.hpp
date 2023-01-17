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
    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, vg::Node const& node, TCoordinate&& coord={} )
    {
      update_node( graph, node, VGFormat{}, coord );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Node const& node, TCoordinate&& coord={}, bool force=false )
    {
      return add_node( graph, node, VGFormat{}, coord, force );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, vg::Edge const& edge, TCoordinate&& coord={}, bool force=false )
    {
      add_edge( graph, edge, VGFormat{}, coord, force );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, typename TGraph::id_type pid, vg::Path const& path,
            TCoordinate&& coord={}, bool force=false )
    {
      extend_path( graph, pid, path, VGFormat{}, coord, force );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, vg::Path const& path, TCoordinate&& coord={}, bool force=false,
         bool force_node=false )
    {
      return add_path( graph, path, VGFormat{}, coord, force, force_node );
    }

    template< typename TGraph,
              typename TCoordinate=VGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, vg::Graph& other, bool sort=false, TCoordinate&& coord={} )
    {
      extend_graph( graph, other, VGFormat{}, sort, coord );
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
