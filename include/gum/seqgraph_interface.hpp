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

#include <sdsl/bit_vectors.hpp>


namespace gum {
  namespace util {
    /**
     *  @brief  Compute total number of loci in the subgraph indicated by node ranks
     *          [lower, upper).
     *
     *  @param[in]  graph The input graph.
     *  @param[in]  lower The minimum node rank in the subgraph.
     *  @param[in]  upper The maximum node rank in the subgraph.
     */
    template< class TGraph >
    inline typename TGraph::offset_type
    total_nof_loci( TGraph const& graph, typename TGraph::rank_type lower,
                    typename TGraph::rank_type upper=0 )
    {
      typedef TGraph graph_type;
      typedef typename graph_type::id_type id_type;
      typedef typename graph_type::rank_type rank_type;
      typedef typename graph_type::offset_type offset_type;

      offset_type total = 0;
      graph.for_each_node(
          [&graph, &total, &upper]( rank_type rank, id_type id ) {
            total += graph.node_length( id );
            if ( rank + 1 == upper ) return false;
            return true;
          },
          lower );
      return total;
    }

    template< class TGraph >
    inline typename TGraph::offset_type
    _total_nof_loci( TGraph const& graph, Dynamic )
    {
      return total_nof_loci( graph, 1 /* from the first node to the end */ );
    }

    template< class TGraph >
    inline typename TGraph::offset_type
    _total_nof_loci( TGraph const& graph, Succinct )
    {
      return length_sum( graph.get_node_prop().sequences() );
    }

    template< class TGraph >
    inline typename TGraph::offset_type
    total_nof_loci( TGraph const& graph )
    {
      return _total_nof_loci( graph, typename TGraph::spec_type() );
    }

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
    for_each_start_side( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.indegree( graph.start_side( id ) ) && !callback( rank, id ) ) return false;
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

    template< typename TGraph, typename TCallback >
    inline void
    for_each_end_side( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.outdegree( graph.end_side( id ) ) && !callback( rank, id ) ) return false;
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

    template< typename TGraph >
    inline void
    dfs_traverse( TGraph const& graph,
                  std::function< void( typename TGraph::rank_type, typename TGraph::id_type ) > on_finishing,
                  std::function< void( typename TGraph::rank_type, typename TGraph::id_type ) > on_discovery = [](auto,auto){},
                  std::function< void( typename TGraph::rank_type, typename TGraph::id_type, bool ) > on_visited = [](auto,auto,auto){} )
    {
      using id_type = typename TGraph::id_type;
      using rank_type = typename TGraph::rank_type;
      using linktype_type = typename TGraph::linktype_type;
      using nodes_type = std::vector< std::pair< rank_type, id_type > >;
      using map_type = typename sdsl::bit_vector;

      auto call = []( std::pair< rank_type, id_type > node,
                      std::function< void( rank_type, id_type ) > function ){
        function( node.first, node.second );
      };

      auto n = graph.get_node_count();

      nodes_type stack;
      map_type visited( 2*n+1, 0 );  // visited[rank*2] <- discovered | visited[rank*2-1] <- finished
      visited[0] = 1;  // dummy

      for_each_start_node(
          graph,
          [&stack]( auto rank, auto id ) -> bool {
            stack.push_back( { rank, id } );
            return true;
          } );
      std::reverse( stack.begin(), stack.end() );

      typename map_type::size_type last_visited = 0;
      do {
        while ( !stack.empty() ) {
          const auto& node = stack.back();
          if ( visited[ node.first * 2 ] ) {
            visited[ node.first * 2 - 1 ] = 1;
            call( node, on_finishing );
            stack.pop_back();
            continue;
          }
          visited[ node.first * 2 ] = 1;
          call( node, on_discovery );
          graph.for_each_edges_out(
            node.second,
            [&stack, &graph, &visited, &on_visited]( id_type to, linktype_type ) -> bool {
              auto rank = graph.id_to_rank( to );
              if ( !visited[ rank * 2 ] ) stack.push_back( { rank, to } );
              else on_visited( rank, to, visited[ rank * 2 - 1 ] );
              return true;
            }
          );
        }

        last_visited = bv_ifind( visited, false, last_visited );
        if ( last_visited == visited.size() ) break;
        auto last_rank = ( last_visited + 1 ) / 2;
        stack.push_back( { last_rank, graph.rank_to_id( last_rank )} );
      } while ( true );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_INTERFACE_HPP__ --- */
