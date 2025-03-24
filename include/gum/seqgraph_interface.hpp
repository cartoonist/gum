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

#include <cstddef>
#include <vector>
#include <algorithm>
#include <queue>

#include <sdsl/bit_vectors.hpp>

#include "iterators.hpp"
#include "stringset.hpp"


namespace gum {
  namespace util {
    template< class TGraph >
    inline typename TGraph::offset_type
    max_node_len( TGraph const& graph )
    {
      typedef TGraph graph_type;
      typedef typename graph_type::id_type id_type;
      typedef typename graph_type::rank_type rank_type;
      typedef typename graph_type::offset_type offset_type;

      offset_type max = 1;
      graph.for_each_node(
          [&graph, &max]( rank_type rank , id_type id ) {
            auto len = graph.node_length( id );
            if ( max < len ) max = len;
            return true;
          } );
      return max;
    }

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
    inline bool
    ranks_in_topological_order( TGraph& graph )
    {
      bool sorted = true;
      graph.for_each_node(
          [&]( auto from_rank, auto from ) {
            graph.for_each_edges_out(
                from,
                [&]( auto to, auto ) {
                  auto to_rank = graph.id_to_rank( to );
                  if ( from_rank < to_rank ) return true;
                  sorted = false;
                  return false;
                } );
            return sorted;
          } );
      return sorted;
    }

    template< typename TGraph,
              typename TCallback1,
              typename TCallback2 = void(*)( typename TGraph::rank_type, typename TGraph::id_type ),
              typename TCallback3 = void(*)( typename TGraph::rank_type, typename TGraph::id_type, bool ) >
    inline void
    dfs_traverse( TGraph const& graph,
                  TCallback1 on_finishing,
                  TCallback2 on_discovery = [](auto,auto){},
                  TCallback3 on_visited = [](auto,auto,auto){} )
    {
      using id_type = typename TGraph::id_type;
      using rank_type = typename TGraph::rank_type;
      using linktype_type = typename TGraph::linktype_type;
      using nodes_type = std::vector< std::pair< rank_type, id_type > >;
      using map_type = sdsl::bit_vector;

      static_assert( std::is_invocable_v< TCallback1, rank_type, id_type >, "received a non-invocable as callback" );
      static_assert( std::is_invocable_v< TCallback2, rank_type, id_type >, "received a non-invocable as callback" );
      static_assert( std::is_invocable_v< TCallback3, rank_type, id_type, bool >, "received a non-invocable as callback" );

      auto n = graph.get_node_count();

      nodes_type stack;
      map_type visited( 2*n+1, 0 );  // visited[rank*2] <- discovered | visited[rank*2-1] <- finished
      visited[0] = 1;  // dummy

      for_each_start_side(
          graph,
          [&stack]( auto rank, auto id ) -> bool {
            stack.push_back( { rank, id } );
            return true;
          } );

      typename map_type::size_type last_visited = 0;
      do {
        while ( !stack.empty() ) {
          const auto& node = stack.back();
          if ( visited[ node.first * 2 ] ) {         // discovered
            if ( !visited[ node.first * 2 - 1 ] ) {  // not finished
              visited[ node.first * 2 - 1 ] = 1;     // finish it!
              std::invoke( on_finishing, node.first, node.second );
            }
            stack.pop_back();
            continue;
          }
          visited[ node.first * 2 ] = 1;
          std::invoke( on_discovery, node.first, node.second );
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

    template< typename TGraph >
    inline std::pair< std::vector< std::pair< typename TGraph::rank_type, typename TGraph::id_type > >, bool >
    topological_sort_order( TGraph const& graph, bool reverse=false )
    {
      using graph_type = TGraph;
      using rank_type = typename graph_type::rank_type;
      using id_type = typename graph_type::id_type;

      std::vector< std::pair< rank_type, id_type > > finished;
      finished.reserve( graph.get_node_count() );

      bool dag = true;
      auto on_discovery = []( rank_type, id_type ){ };
      auto on_finishing = [&finished]( rank_type r, id_type id ) {
        finished.push_back( { r, id } );
      };
      auto on_visited = [&dag]( rank_type, id_type, bool finished ) {
        if ( dag && !finished ) dag = false;  // back edge
      };

      dfs_traverse( graph, on_finishing, on_discovery, on_visited );
      if ( !reverse ) std::reverse( finished.begin(), finished.end() );

      return { finished, dag };
    }

    template< typename TGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline bool
    topological_sort( TGraph& graph, bool force=false, bool reverse=false )
    {
      using graph_type = TGraph;
      using rank_type = typename graph_type::rank_type;
      using id_type = typename graph_type::id_type;

      auto [top_sort, dag] = topological_sort_order( graph, reverse );
      if ( dag || force ) {
        RandomAccessProxyContainer perm(
          &top_sort,
          []( std::pair< rank_type, id_type > const& p ) -> rank_type { return p.first - 1; } );
        graph.sort_nodes( perm );
      }

      return dag;
    }

    template< typename TGraph, typename TCallback1,
              typename TCompare = bool(*)( std::pair< typename TGraph::rank_type, typename TGraph::id_type >, std::pair< typename TGraph::rank_type, typename TGraph::id_type > ),
              typename TCallback2 = void(*)( typename TGraph::rank_type, typename TGraph::id_type, typename TGraph::rank_type ) >
    inline void
    bfs_traverse( TGraph const& graph,
                  TCallback1 on_finishing,
                  TCompare degree_cmp = [](auto a, auto b) -> bool { return a.first > b.first; },
                  TCallback2 on_discovery = []( auto, auto, auto ) {} )
    {
      using id_type = typename TGraph::id_type;
      using rank_type = typename TGraph::rank_type;
      using value_type = std::pair< rank_type, id_type >;
      using nodes_type = std::vector< value_type >;
      using map_type = sdsl::bit_vector;

      static_assert( std::is_invocable_v< TCallback1, rank_type, id_type >, "received a non-invocable as callback" );
      static_assert( std::is_invocable_v< TCompare, value_type, value_type >, "received a non-invocable as callback" );
      static_assert( std::is_invocable_v< TCallback2, rank_type, id_type, rank_type >, "received a non-invocable as callback" );

      auto n = graph.get_node_count();

      std::priority_queue< value_type, nodes_type, decltype(degree_cmp) > queue{ degree_cmp };
      map_type visited( n + 1, 0 );
      visited[ 0 ] = 1;  // dummy

      for_each_start_side(
          graph,
          [&]( auto rank, auto id ) -> bool {
            rank_type level = 0;
            if ( !visited[ rank ] ) {
              queue.push( { level, id } );
              visited[ rank ] = 1;
            }
            while ( !queue.empty() ) {
              std::tie( level, id ) = queue.top();
              level += 1;
              rank = graph.id_to_rank( id );
              queue.pop();

              std::invoke( on_finishing, rank, id );

              graph.for_each_edges_out(
                  id, [&]( auto to, auto ) -> bool {
                    auto rank = graph.id_to_rank( to );
                    if ( !visited[ rank ] ) {
                      std::invoke( on_discovery, rank, id, level );
                      queue.push( { level, to } );
                      visited[ rank ] = 1;
                    }
                    return true;
                  } );
            }
            return true;
          } );
    }

    template< typename TGraph >
    inline std::vector< std::pair< typename TGraph::rank_type, typename TGraph::id_type > >
    cuthill_mckee_order( TGraph& graph, bool reverse=true )
    {
      using id_type = typename TGraph::id_type;
      using rank_type = typename TGraph::rank_type;
      using value_type = std::pair< rank_type, id_type >;
      using nodes_type = std::vector< value_type >;

      nodes_type result;
      result.reserve( graph.get_node_count() );

      auto on_finishing = [&result]( rank_type rank, id_type id ) {
        result.push_back( { rank, id } );
      };

      auto degree_cmp = [&graph]( const auto& a, const auto& b ) {
        return graph.outdegree( a.second ) > graph.outdegree( b.second );
      };

      bfs_traverse( graph, on_finishing, degree_cmp );
      if ( reverse ) std::reverse( result.begin(), result.end() );

      return result;
    }

    template< typename TGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    cuthill_mckee_sort( TGraph& graph, bool reverse=true ) {
      using graph_type = TGraph;
      using rank_type = typename graph_type::rank_type;
      using id_type = typename graph_type::id_type;

      auto rcm = cuthill_mckee_order( graph, reverse );
      RandomAccessProxyContainer perm(
          &rcm, []( std::pair< rank_type, id_type > const& p ) -> rank_type {
            return p.first - 1;
          } );
      graph.sort_nodes( perm );
    }

    /**
     *  @brief  Compute a total order minimizing breaks in sibling node indices.
     *
     * This algorithm is based on BFS traversal with a slight modification
     * to minimise the number of breaks in the node indices of sibling nodes.
     * A break corresponding to each node is a discontinuity in the
     * sorted node indices of its children.
     *
     * NOTE: This algorithm assumes that the nodes are first sorted in topological
     * order.
     *
     *  @param[in]  graph The input graph.
     *  @param[in]  reverse If true, the ordering will be reversed.
     */
    template< typename TGraph >
    inline std::vector< std::pair< typename TGraph::rank_type, typename TGraph::id_type > >
    min_breaks_order( TGraph& graph, bool reverse=false )
    {
      using id_type = typename TGraph::id_type;
      using rank_type = typename TGraph::rank_type;
      using value_type = std::pair< rank_type, id_type >;
      using nodes_type = std::vector< value_type >;

      nodes_type result;
      result.reserve( graph.get_node_count() );

      auto on_finishing = [&result]( rank_type rank, id_type id ) {
        result.push_back( { rank, id } );
      };

      auto degree_cmp = [ &graph ]( const auto& a, const auto& b ) {
        auto min_parent_rank = [ &graph ]( auto const& id ) {
          if ( graph.indegree( id ) == 0 ) return graph.id_to_rank( id );

          rank_type min_rank = std::numeric_limits< rank_type >::max();
          graph.for_each_edges_in(
              id,
              [&]( id_type from, auto ) {
                auto rank = graph.id_to_rank( from );
                if ( min_rank > rank ) min_rank = rank;
                return true;
              } );
          return min_rank;
        };

        return min_parent_rank( a.second ) > min_parent_rank( b.second );
      };

      bfs_traverse( graph, on_finishing, degree_cmp );
      if ( reverse ) std::reverse( result.begin(), result.end() );

      return result;
    }

    /**
     *  @brief  Sort the nodes of the graph in a total order minimizing breaks in sibling node indices.
     *
     *  NOTE: This algorithm assumes that the nodes are first sorted in
     *  topological order.
     *
     *  @param[in]  graph The input graph.
     *  @param[in]  reverse If true, the ordering will be reversed.
     */
    template< typename TGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value > >
    inline void
    min_breaks_sort( TGraph& graph, bool reverse=false ) {
      using graph_type = TGraph;
      using rank_type = typename graph_type::rank_type;
      using id_type = typename graph_type::id_type;

      auto mbr = min_breaks_order( graph, reverse );
      RandomAccessProxyContainer perm(
          &mbr, []( std::pair< rank_type, id_type > const& p ) -> rank_type {
            return p.first - 1;
          } );
      graph.sort_nodes( perm );
    }

    template< typename TGraph >
    inline typename TGraph::offset_type
    _chargraph_bandwidth( TGraph const& graph, Dynamic )
    {
      using rank_type = typename TGraph::rank_type;
      using id_type = typename TGraph::id_type;
      using offset_type = typename TGraph::offset_type;

      std::vector< offset_type > char_orders( graph.get_node_count() + 1 );
      char_orders[ 0 ] = 0;  // dummy
      graph.for_each_node(
          [&]( rank_type rank, id_type id ) {
            char_orders[ rank ] = char_orders[ 0 ];
            char_orders[ 0 ] += graph.node_length( id );
            return true;
          } );

      offset_type bw = 0;
      graph.for_each_node(
          [&]( rank_type rank, id_type id ) {
            offset_type width = 0;
            auto from_order = char_orders[ rank ];
            from_order += graph.node_length( id ) - 1;
            graph.for_each_edges_out(
                id,
                [&]( id_type to, auto ) {
                  auto to_rank = graph.id_to_rank( to );
                  auto to_order = char_orders[ to_rank ];
                  offset_type diff = std::abs(
                      static_cast< ptrdiff_t >( to_order - from_order ) );
                  if ( width < diff ) width = diff;
                  return true;
                } );
            if ( bw < width ) bw = width;
            return true;
          } );

      return bw;
    }

    template< typename TGraph >
    inline typename TGraph::offset_type
    _chargraph_bandwidth( TGraph const& graph, Succinct )
    {
      using rank_type = typename TGraph::rank_type;
      using id_type = typename TGraph::id_type;
      using offset_type = typename TGraph::offset_type;

      offset_type bw = 0;
      graph.for_each_node( [&]( rank_type rank, id_type id ) {
        offset_type width = 0;
        auto from_order = id_to_charorder( graph, id );
        from_order += graph.node_length( id ) - 1;
        graph.for_each_edges_out( id, [&]( id_type to, auto ) {
          auto to_order = id_to_charorder( graph, to );
          offset_type diff
              = std::abs( static_cast< ptrdiff_t >( to_order - from_order ) );
          if ( width < diff ) width = diff;
          return true;
        } );
        if ( bw < width ) bw = width;
        return true;
      } );

      return bw;
    }

    /**
     * @brief Compute the character graph bandwidth of the given sequence graph
     *
     * NOTE: The bandwidth is NOT computed directly from the adjacency matrix.
     *
     * @param graph The input graph.
     *
     * @return The bandwidth of the adjacency matrix.
     */
    template< typename TGraph >
    inline typename TGraph::offset_type
    chargraph_bandwidth( TGraph const& graph )
    {
      return _chargraph_bandwidth( graph, typename TGraph::spec_type() );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_INTERFACE_HPP__ --- */
