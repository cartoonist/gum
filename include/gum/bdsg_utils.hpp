/**
 *    @file  bdsg_utils.hpp
 *   @brief  `HashGraph` utility functions and data types.
 *
 *  This header file is a part of the utility header files specialised for `HashGraph`
 *  data type.
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

#ifndef GUM_BDSG_UTILS_HPP_
#define GUM_BDSG_UTILS_HPP_

#include <istream>

#include <bdsg/hash_graph.hpp>

#include "hg_utils.hpp"


namespace gum {
  namespace util {
    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    update( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={} )
    {
      update_node( graph, eid, std::move( seq ), HGFormat{}, coord );
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline typename TGraph::id_type
    add( TGraph& graph, HGFormat::nid_t eid, std::string seq, TCoordinate&& coord={},
         bool force=false )
    {
      return add_node( graph, eid, std::move( seq ), HGFormat{}, coord, force );
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    add( TGraph& graph, HGFormat::nid_t from, bool from_start, HGFormat::nid_t to,
         bool to_end, HGFormat::off_t overlap, TCoordinate&& coord={}, bool force=false )
    {
      add_edge( graph, from, from_start, to, to_end, overlap, HGFormat{}, coord, force );
    }

    template< typename TGraph,
              typename TCoordinate=HGFormat::DefaultCoord< TGraph >,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    extend( TGraph& graph, bdsg::HashGraph const& other, bool sort=false,
            TCoordinate&& coord={} )
    {
      extend_graph( graph, other, HGFormat{}, sort, coord );
    }

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

#endif  /* --- #ifndef GUM_BDSG_UTILS_HPP_ --- */
