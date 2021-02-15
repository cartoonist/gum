/**
 *    @file  seqgraph_interface.hpp
 *   @brief  Sequence graph interface function definitions
 *
 *  All `SeqGraph` interface functions are defined here.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Oct 15, 2020  19:01
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2020, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_SEQGRAPH_INTERFACE_HPP__
#define  GUM_SEQGRAPH_INTERFACE_HPP__

#include <vector>
#include <algorithm>


namespace gum {
  namespace util {
    template< typename TGraph, typename TCallback >
    inline void
    for_each_start_node( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.indegree( id ) && !callback( rank, id ) ) return false;
            return true;
          } );
    }

    template< typename TGraph, typename TCallback >
    inline void
    for_each_end_node( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.outdegree( id ) && !callback( rank, id ) ) return false;
            return true;
          } );
    }

    template< typename TGraph >
    inline typename TGraph::id_type
    _position_to_id( TGraph const& graph, typename TGraph::offset_type pos, Succinct )
    {
      auto rank = graph.get_node_prop().sequences().idx( pos ) + 1;
      return graph.rank_to_id( rank );
    }

    template< typename TGraph, typename ...TArgs >
    inline typename TGraph::id_type
    position_to_id( TGraph const& graph, typename TGraph::offset_type pos,
                    TArgs&&... args )
    {
      return _position_to_id( graph, pos, typename TGraph::spec_type(),
                              std::forward< TArgs >( args )... );
    }

    template< typename TGraph >
    inline typename TGraph::offset_type
    _position_to_offset( TGraph const& graph, typename TGraph::offset_type pos, Succinct )
    {
      auto idx = graph.get_node_prop().sequences().idx( pos );
      return pos - graph.get_node_prop().sequences().start_position( idx );
    }

    template< typename TGraph, typename ...TArgs >
    inline typename TGraph::offset_type
    position_to_offset( TGraph const& graph, typename TGraph::offset_type pos,
                        TArgs&&... args )
    {
      return _position_to_offset( graph, pos, typename TGraph::spec_type(),
                                  std::forward< TArgs >( args )... );
    }

    template< typename TGraph >
    inline typename TGraph::offset_type
    _id_to_position( TGraph const& graph, typename TGraph::id_type id, Succinct )
    {
      auto rank = graph.id_to_rank( id );
      return graph.get_node_prop().sequences().start_position( rank - 1 );
    }

    template< typename TGraph, typename ...TArgs >
    inline typename TGraph::offset_type
    id_to_position( TGraph const& graph, typename TGraph::id_type id, TArgs&&... args )
    {
      return _id_to_position( graph, id, typename TGraph::spec_type(),
                              std::forward< TArgs >( args )... );
    }

    template< typename TGraph >
    inline typename TGraph::offset_type
    _id_to_charorder( TGraph const& graph, typename TGraph::id_type id, Succinct )
    {
      auto idx = graph.id_to_rank( id ) - 1;
      return graph.get_node_prop().sequences().start_position( idx ) - idx;
    }

    template< typename TGraph, typename ...TArgs >
    inline typename TGraph::offset_type
    id_to_charorder( TGraph const& graph, typename TGraph::id_type id, TArgs&&... args )
    {
      return _id_to_charorder( graph, id, typename TGraph::spec_type(),
                               std::forward< TArgs >( args )... );
    }

    template< typename TGraph >
    inline bool
    ids_in_topological_order( TGraph& graph )
    {
      bool sorted = true;
      graph.for_each_node(
          [&]( auto, auto from ) {
            graph.for_each_edges_out(
                from,
                [&]( auto to, auto ) {
                  if ( from < to ) return true;
                  sorted = false;
                  return false;
                } );
            return sorted;
          } );
      return sorted;
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_INTERFACE_HPP__ --- */
