/**
 *    @file  test_seqgraph.cpp
 *   @brief  Test cases for `seqgraph` module.
 *
 *  This source file includes test scenarios for `seqgraph` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Mon Aug 12, 2019  10:12
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include <random>

#include <gum/graph.hpp>
#include <gum/io_utils.hpp>
#include <gum/utils.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Generic functionality of DirectedGraph", "[seqgraph][template]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, void, 32, 32 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, void, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, void, 16, 64 > ),
                   ( gum::SeqGraph< gum::Dynamic > ) )
{
  GIVEN( "A DirectedGraph with some nodes and edges" )
  {
    using graph_type = TestType;
    using succinct_type = typename graph_type::succinct_type;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;

    graph_type graph;
    id_type abs_id = 999;
    std::vector< id_type > nodes;
    rank_type node_count = 9;
    auto integrity_test =
        [&nodes, node_count, abs_id]( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;

          REQUIRE( graph.get_node_count() == node_count );
          id_type successor = graph.rank_to_id( 1 );
          graph.for_each_node(
              [&graph, &successor]( rank_type rank, id_type id ) {
                REQUIRE( graph.rank_to_id( rank ) == id );
                REQUIRE( graph.id_to_rank( id ) == rank );
                REQUIRE( id == successor );
                successor = graph.successor_id( id );
                return true;
              } );
          unsigned int counter = 3;
          graph.for_each_node(
              [&counter]( rank_type rank, id_type ) {
                REQUIRE( rank == counter );
                ++counter;
                return true;
              },
              counter
            );
          REQUIRE( counter - 1 == node_count );
          for ( rank_type i = 1; i <= graph.get_node_count(); ++i ) {
            REQUIRE( graph.has_node( nodes[ i - 1 ] ) );
          }
          REQUIRE( !graph.has_node( abs_id ) );
        };

    auto empty_graph_test =
        [&nodes]( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;

          REQUIRE( graph.get_node_count() == 0 );
          rank_type counter = 0;
          graph.for_each_node(
              [&counter]( rank_type rank, id_type id ) {
                ++counter;
                return true;
              } );
          REQUIRE( counter == 0 );
          counter = 3;
          graph.for_each_node(
              [&counter]( rank_type rank, id_type id ) {
                ++counter;
                return true;
              }, counter );
          REQUIRE( counter == 3 );
          for ( id_type nid : nodes ) REQUIRE( !graph.has_node( nid ) );
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }
    }

    WHEN( "It is constructed by passing nodes list" )
    {
      graph.add_nodes( node_count, [&nodes]( id_type id ) {
                                     nodes.push_back( id );
                                   } );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "Nodes are added in an unordered way" )
    {
      std::vector< float > shuffle( node_count );
      std::uniform_real_distribution< float > distribution( 0, 1 );
      std::mt19937 engine;
      auto generator = std::bind( distribution, engine );
      std::generate_n( shuffle.begin(), node_count, generator );

      std::vector< id_type > unordered_nodes( node_count );
      std::iota( unordered_nodes.begin(), unordered_nodes.end(), 1 );
      gum::util::sort_zip( shuffle, unordered_nodes );

      for ( id_type n : unordered_nodes ) {
        graph.add_node( n );
        nodes.push_back( n );
      }

      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      AND_WHEN( "Nodes are sorted" )
      {
        graph.sort_nodes();

        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( graph );
        }
      }
    }

    WHEN( "A Succinct graph is constructed from Dynamic one" )
    {
      graph.add_nodes( node_count );
      succinct_type sc_graph( graph );
      sc_graph.for_each_node( [&nodes]( rank_type rank, id_type id ) {
                                nodes.push_back( id );
                                return true;
                              } );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }

    WHEN( "Node are added without specifying external IDs" )
    {
      std::size_t node_count = 20;
      for ( std::size_t i = 0; i < node_count / 2; ++i ) graph.add_node();
      graph.add_nodes( node_count / 2 );
      THEN( "The node IDs should be sequential" )
      {
        graph.for_each_node( []( rank_type rank, id_type id ) {
                               REQUIRE( rank == static_cast< rank_type >( id ) );
                               return true;
                             } );
      }

      AND_WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The node coordinate IDs should be sequential" )
        {
          graph.for_each_node(
              [&graph]( rank_type rank, id_type id ) {
                REQUIRE( rank == static_cast< rank_type >( graph.coordinate_id( id ) ) );
                return true;
              } );
        }
      }
    }

    WHEN( "Node are added by specifying random external IDs" )
    {
      std::random_device rd;     // Will be used to obtain a seed for the random number engine.
      std::mt19937 gen( rd() );  // Standard mersenne_twister_engine seeded with rd().
      std::uniform_int_distribution< id_type > dis( 1 );
      std::size_t node_count = 200;
      std::vector< id_type > ids;
      std::size_t i = 0;
      while ( i < node_count )
      {
        id_type candidate = dis( gen );
        if ( std::find( ids.begin(), ids.end(), candidate ) != ids.end() ) continue;
        ids.push_back( candidate );
        ++i;
      }
      for ( auto const& nid : ids ) graph.add_node( nid );
      THEN( "The nodes should be added by their specified IDs" )
      {
        graph.for_each_node( [&ids]( rank_type rank, id_type id ) {
                               REQUIRE( id == ids[ rank - 1 ] );
                               return true;
                             } );
      }

      AND_WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        gum::make_succinct_t< graph_type, gum::coordinate::Sparse > sc_graph( graph );
        THEN( "The node coordinate IDs should be sequential" )
        {
          graph.for_each_node(
              [&graph, &ids]( rank_type rank, id_type id ) {
                REQUIRE( graph.coordinate_id( id ) == ids[ rank - 1 ] );
                return true;
              } );
        }
      }
    }

    WHEN( "Node are added by mixed ID specification approach" )
    {
      std::random_device rd;     // Will be used to obtain a seed for the random number engine.
      std::mt19937 gen( rd() );  // Standard mersenne_twister_engine seeded with rd().
      std::uniform_int_distribution< id_type > dis( 1 );
      std::size_t node_count = 200;
      std::vector< id_type > ids;
      ids.push_back( dis( gen ) );
      graph.add_node( ids.back() );
      graph.add_nodes( 100 );
      std::size_t i = 0;
      id_type tmp = ids.back();
      for ( ; i < 100; ++i ) ids.push_back( ++tmp );
      i = ids.size();
      while ( i < node_count - 1 )
      {
        id_type candidate = dis( gen );
        if ( std::find( ids.begin(), ids.end(), candidate ) != ids.end() ) continue;
        ids.push_back( candidate );
        ++i;
      }
      for ( i = graph.get_node_count(); i < node_count - 1; ++i ) graph.add_node( ids[ i ] );
      ids.push_back( graph.add_node( ) );
      THEN( "The nodes should be added by their specified IDs" )
      {
        graph.for_each_node( [&ids]( rank_type rank, id_type id ) {
                               REQUIRE( id == ids[ rank - 1 ] );
                               return true;
                             } );
      }

      AND_WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        gum::make_succinct_t< graph_type, gum::coordinate::Sparse > sc_graph( graph );
        THEN( "The node coordinate IDs should be sequential" )
        {
          graph.for_each_node(
              [&graph, &ids]( rank_type rank, id_type id ) {
                REQUIRE( graph.coordinate_id( id ) == ids[ rank - 1 ] );
                return true;
              } );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO( "Specialised functionality of DirectedGraph", "[seqgraph][template]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, void, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, void, 16, 64 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, void, 32, 32 > ) )
{
  GIVEN( "A DirectedGraph with some nodes and edges" )
  {
    using graph_type = TestType;
    using succinct_type = typename graph_type::succinct_type;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 999;
    std::vector< id_type > nodes;
    rank_type node_count = 9;
    std::vector< link_type > edges = {
      { 1, 2 },
      { 1, 3 },
      { 2, 5 },
      { 2, 6 },
      { 2, 7 },
      { 3, 4 },
      { 5, 8 },
      { 6, 8 },
      { 7, 8 },
      { 4, 8 },
      { 8, 9 }
    };
    auto update_edges =
        [&edges]( auto const& graph ) {
          for ( std::size_t i = 0; i < edges.size(); ++i ) {
            link_type e = edges[ i ];
            edges[ i ] = link_type( graph.id_by_coordinate( graph.from_id( e ) ),
                                    graph.id_by_coordinate( graph.to_id( e ) ) );
          }
        };
    auto integrity_test =
        [&edges, abs_id]( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using side_type = typename graph_type::side_type;
          using linktype_type = typename graph_type::linktype_type;
          using lid_type = typename graph_type::coordinate_type::lid_type;

          auto ibyc =
            [&graph]( lid_type cid ) -> id_type {
              return graph.id_by_coordinate( cid );
            };

          REQUIRE( graph.get_edge_count() == edges.size() );
          for ( rank_type rank = 1; rank <= graph.get_node_count(); ++rank ) {
            graph.for_each_side(
                graph.rank_to_id( rank ),
                [&graph]( side_type side ) {
                  id_type id = graph.id_of( side );
                  REQUIRE( graph.start_side( id ) == side );
                  REQUIRE( graph.end_side( id ) == side );
                  REQUIRE( side == side_type( id ) );
                  return true;
                } );
          }
          for ( auto const& edge: edges ) {
            REQUIRE( graph.has_edge( edge.first, edge.second ) );
            REQUIRE( !graph.has_edge( edge.second, edge.first ) );
            REQUIRE( !graph.has_edge( abs_id, edge.second ) );
            REQUIRE( graph.has_edge( edge ) );
          }
          std::vector< side_type > truth;
          auto side_truth_check =
              [&truth]( side_type side ) {
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto to_id_truth_check =
              [&graph, &truth]( id_type id, linktype_type type ) {
                side_type side = graph.to_side( id, type );
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto from_id_truth_check =
              [&graph, &truth]( id_type id, linktype_type type ) {
                side_type side = graph.from_side( id, type );
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto adjs = graph.adjacents_out( ibyc(2) );
          truth = { ibyc(5), ibyc(6), ibyc(7) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( ibyc(2), to_id_truth_check );
          graph.for_each_edges_out( side_type( ibyc(2) ), side_truth_check );
          adjs = graph.adjacents_out( ibyc(8) );
          truth = { ibyc(9) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( ibyc(8), to_id_truth_check );
          graph.for_each_edges_out( side_type( ibyc(8) ), side_truth_check );
          adjs = graph.adjacents_in( ibyc(2) );
          truth = { ibyc(1) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( ibyc(2), from_id_truth_check );
          graph.for_each_edges_in( side_type( ibyc(2) ), side_truth_check );
          adjs = graph.adjacents_in( ibyc(8) );
          truth = { ibyc(5), ibyc(6), ibyc(7), ibyc(4) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( ibyc(8), from_id_truth_check );
          graph.for_each_edges_in( side_type( ibyc(8) ), side_truth_check );
          REQUIRE( graph.outdegree( ibyc(1) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(2) ) == 3 );
          REQUIRE( graph.outdegree( ibyc(6) ) == 1 );
          REQUIRE( graph.outdegree( { ibyc(1) } ) == 2 );
          REQUIRE( graph.outdegree( { ibyc(2) } ) == 3 );
          REQUIRE( graph.outdegree( { ibyc(6) } ) == 1 );
          REQUIRE( graph.indegree( ibyc(9) ) == 1 );
          REQUIRE( graph.indegree( ibyc(8) ) == 4 );
          REQUIRE( graph.indegree( ibyc(6) ) == 1 );
          REQUIRE( graph.indegree( { ibyc(9) } ) == 1 );
          REQUIRE( graph.indegree( { ibyc(8) } ) == 4 );
          REQUIRE( graph.indegree( { ibyc(6) } ) == 1 );
          REQUIRE( !graph.has_edges_in( ibyc(1) ) );
          REQUIRE( graph.has_edges_in( ibyc(2) ) );
          REQUIRE( graph.has_edges_in( ibyc(8) ) );
          REQUIRE( graph.has_edges_in( ibyc(9) ) );
          REQUIRE( !graph.has_edges_in( { ibyc(1) } ) );
          REQUIRE( graph.has_edges_in( { ibyc(2) } ) );
          REQUIRE( graph.has_edges_in( { ibyc(8) } ) );
          REQUIRE( graph.has_edges_in( { ibyc(9) } ) );
          REQUIRE( graph.has_edges_out( ibyc(1) ) );
          REQUIRE( graph.has_edges_out( ibyc(2) ) );
          REQUIRE( graph.has_edges_out( ibyc(8) ) );
          REQUIRE( !graph.has_edges_out( ibyc(9) ) );
          REQUIRE( graph.has_edges_out( { ibyc(1) } ) );
          REQUIRE( graph.has_edges_out( { ibyc(2) } ) );
          REQUIRE( graph.has_edges_out( { ibyc(8) } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(9) } ) );
          REQUIRE( graph.is_branch( ibyc(1) ) );
          REQUIRE( graph.is_branch( ibyc(2) ) );
          REQUIRE( !graph.is_branch( ibyc(6) ) );
          REQUIRE( graph.is_branch( { ibyc(1) } ) );
          REQUIRE( graph.is_branch( { ibyc(2) } ) );
          REQUIRE( !graph.is_branch( { ibyc(6) } ) );
          REQUIRE( !graph.is_merge( ibyc(9) ) );
          REQUIRE( graph.is_merge( ibyc(8) ) );
          REQUIRE( !graph.is_merge( ibyc(6) ) );
          REQUIRE( !graph.is_merge( { ibyc(9) } ) );
          REQUIRE( graph.is_merge( { ibyc(8) } ) );
          REQUIRE( !graph.is_merge( { ibyc(6) } ) );
        };

    auto empty_graph_test =
        [&edges]( auto const& graph ) {
          REQUIRE( graph.get_edge_count() == 0 );
          for ( auto const& edge: edges ) {
            REQUIRE( !graph.has_edge( edge.first, edge.second ) );
            REQUIRE( !graph.has_edge( edge.second, edge.first ) );
            REQUIRE( !graph.has_edge( edge ) );
          }
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
      update_edges( graph );
      for ( auto const& edge : edges ) graph.add_edge( edge.first, edge.second );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "It is constructed by passing nodes list" )
    {
      graph.add_nodes( node_count, [&nodes]( id_type id ) {
                                     nodes.push_back( id );
                                   } );
      update_edges( graph );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }
    }

    WHEN( "Nodes are added in an unordered way" )
    {
      std::vector< float > shuffle( node_count );
      std::uniform_real_distribution< float > distribution( 0, 1 );
      std::mt19937 engine;
      auto generator = std::bind( distribution, engine );
      std::generate_n( shuffle.begin(), node_count, generator );

      std::vector< id_type > unordered_nodes( node_count );
      std::iota( unordered_nodes.begin(), unordered_nodes.end(), 1 );
      gum::util::sort_zip( shuffle, unordered_nodes );

      for ( id_type n : unordered_nodes ) {
        graph.add_node( n );
        nodes.push_back( n );
      }

      THEN( "The resulting graph should pass integrity tests" )
      {
        update_edges( graph );
        for ( auto const& edge : edges ) graph.add_edge( edge.first, edge.second );

        integrity_test( graph );
      }

      AND_WHEN( "Nodes are sorted" )
      {
        graph.sort_nodes();
        update_edges( graph );
        for ( auto const& edge : edges ) graph.add_edge( edge.first, edge.second );

        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( graph );
        }
      }
    }

    WHEN( "A Succinct graph is constructed from Dynamic one" )
    {
      graph.add_nodes( node_count );
      auto tmp = edges;
      update_edges( graph );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      succinct_type sc_graph( graph );
      sc_graph.for_each_node( [&nodes]( rank_type rank, id_type id ) {
                                nodes.push_back( id );
                                return true;
                              } );
      edges = tmp;
      update_edges( sc_graph );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO( "Specialised functionality of Bidirected DirectedGraph", "[seqgraph][template]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, void, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, void, 16, 64 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, void, 32, 32 > ),
                   ( gum::SeqGraph< gum::Dynamic > ) )
{
  GIVEN( "A Bidirected DirectedGraph with some nodes and edges" )
  {
    using graph_type = TestType;
    using succinct_type = typename graph_type::succinct_type;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 999;
    std::vector< id_type > nodes;
    rank_type node_count = 9;
    std::vector< link_type > edges = {
      { 1, true, 2, false },
      { 1, true, 3, true },
      { 2, true, 5, false },
      { 2, true, 6, true },
      { 2, true, 7, false },
      { 3, false, 4, false },
      { 5, true, 8, false },
      { 6, false, 8, true },
      { 7, true, 8, false },
      { 4, true, 8, false },
      { 8, false, 9, false }
    };
    auto update_edges =
        [&edges]( auto const& graph ) {
          for ( std::size_t i = 0; i < edges.size(); ++i ) {
            link_type e = edges[ i ];
            edges[ i ] = link_type( graph.id_by_coordinate( std::get< 0 >( e ) ),
                                    std::get< 1 >( e ),
                                    graph.id_by_coordinate( std::get< 2 >( e ) ),
                                    std::get< 3 >( e ) );
          }
        };
    auto integrity_test =
        [&edges, abs_id]( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using side_type = typename graph_type::side_type;
          using linktype_type = typename graph_type::linktype_type;
          using lid_type = typename graph_type::coordinate_type::lid_type;

          auto ibyc =
            [&graph]( lid_type cid ) -> id_type {
              return graph.id_by_coordinate( cid );
            };

          REQUIRE( graph.get_edge_count() == edges.size() );
          for ( rank_type rank = 1; rank <= graph.get_node_count(); ++rank ) {
            bool sidetype = false;
            graph.for_each_side(
                graph.rank_to_id( rank ),
                [&graph, &sidetype]( side_type side ) {
                  id_type id = graph.id_of( side );
                  if ( sidetype ) REQUIRE( graph.end_side( id ) == side );
                  else REQUIRE( graph.start_side( id ) == side );
                  REQUIRE( side == side_type( id, sidetype ) );
                  sidetype = !sidetype;
                  return true;
                } );
          }
          for ( auto const& edge: edges ) {
            REQUIRE( graph.has_edge( graph.from_side( edge ), graph.to_side( edge ) ) );
            REQUIRE( !graph.has_edge( graph.to_side( edge ), graph.from_side( edge ) ) );
            REQUIRE( !graph.has_edge( { abs_id, false }, graph.to_side( edge ) ) );
            REQUIRE( !graph.has_edge( { abs_id, true }, graph.to_side( edge ) ) );
            REQUIRE( graph.has_edge( edge ) );
            REQUIRE( !graph.has_edge( graph.opposite_side( graph.from_side( edge ) ),
                                      graph.opposite_side( graph.to_side( edge ) ) ) );
            REQUIRE( !graph.has_edge( graph.from_side( edge ),
                                      graph.opposite_side( graph.to_side( edge ) ) ) );
            REQUIRE( !graph.has_edge( graph.opposite_side( graph.from_side( edge ) ),
                                      graph.to_side( edge ) ) );
            auto type = graph.linktype( edge );
            REQUIRE( graph.is_from_start( edge ) == !std::get<1>( edge ) );
            REQUIRE( graph.is_from_start( type ) == !std::get<1>( edge ) );
            REQUIRE( graph.is_to_end( edge ) == std::get<3>( edge ) );
            REQUIRE( graph.is_to_end( type ) == std::get<3>( edge ) );
          }
          std::vector< side_type > truth;
          auto side_truth_check =
              [&truth]( side_type side ) {
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto to_id_truth_check =
              [&graph, &truth]( id_type id, linktype_type type ) {
                side_type side = graph.to_side( id, type );
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto from_id_truth_check =
              [&graph, &truth]( id_type id, linktype_type type ) {
                side_type side = graph.from_side( id, type );
                REQUIRE( std::find( truth.begin(), truth.end(), side ) != truth.end() );
                return true;
              };
          auto adjs = graph.adjacents_out( { ibyc(2), true } );
          truth = { { ibyc(5), false }, { ibyc(6), true }, { ibyc(7), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(2), true }, side_truth_check );
          adjs = graph.adjacents_out( { ibyc(8), false } );
          truth = { { ibyc(9), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(8), false }, side_truth_check );
          adjs = graph.adjacents_out( { ibyc(8), true } );
          REQUIRE( adjs.empty() );
          graph.for_each_edges_out( ibyc(8), to_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(2), false } );
          truth = { { ibyc(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(2), false }, side_truth_check );
          adjs = graph.adjacents_in( { ibyc(2), true } );
          REQUIRE( adjs.empty() );
          graph.for_each_edges_in( ibyc(2), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(8), false } );
          truth = { { ibyc(5), true }, { ibyc(7), true }, { ibyc(4), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(8), false }, side_truth_check );
          truth.push_back( { ibyc(6), false } );
          graph.for_each_edges_in( ibyc(8), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(8), true } );
          truth = { { ibyc(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(8), true }, side_truth_check );
          REQUIRE( graph.outdegree( { ibyc(1), true } ) == 2 );
          REQUIRE( graph.outdegree( { ibyc(1), false } ) == 0 );
          REQUIRE( graph.outdegree( { ibyc(2), true } ) == 3 );
          REQUIRE( graph.outdegree( { ibyc(2), false } ) == 0 );
          REQUIRE( graph.outdegree( { ibyc(6), true } ) == 0 );
          REQUIRE( graph.outdegree( { ibyc(6), false } ) == 1 );
          REQUIRE( graph.outdegree( ibyc(1) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(2) ) == 3 );
          REQUIRE( graph.outdegree( ibyc(6) ) == 1 );
          REQUIRE( graph.indegree( { ibyc(9), true } ) == 0 );
          REQUIRE( graph.indegree( { ibyc(9), false } ) == 1 );
          REQUIRE( graph.indegree( { ibyc(6), true } ) == 1 );
          REQUIRE( graph.indegree( { ibyc(6), false } ) == 0 );
          REQUIRE( graph.indegree( { ibyc(8), true } ) == 1 );
          REQUIRE( graph.indegree( { ibyc(8), false } ) == 3 );
          REQUIRE( graph.indegree( ibyc(9) ) == 1 );
          REQUIRE( graph.indegree( ibyc(6) ) == 1 );
          REQUIRE( graph.indegree( ibyc(8) ) == 4 );
          REQUIRE( !graph.has_edges_in( { ibyc(1), true } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(1), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(2), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(2), false } ) );
          REQUIRE( graph.has_edges_in( { ibyc(8), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(8), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(9), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(9), false } ) );
          REQUIRE( !graph.has_edges_in( ibyc(1) ) );
          REQUIRE( graph.has_edges_in( ibyc(2) ) );
          REQUIRE( graph.has_edges_in( ibyc(8) ) );
          REQUIRE( graph.has_edges_in( ibyc(9) ) );
          REQUIRE( graph.has_edges_out( { ibyc(1), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(1), false } ) );
          REQUIRE( graph.has_edges_out( { ibyc(2), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(2), false } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(8), true } ) );
          REQUIRE( graph.has_edges_out( { ibyc(8), false } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(9), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(9), false } ) );
          REQUIRE( graph.has_edges_out( ibyc(1) ) );
          REQUIRE( graph.has_edges_out( ibyc(2) ) );
          REQUIRE( graph.has_edges_out( ibyc(8) ) );
          REQUIRE( !graph.has_edges_out( ibyc(9) ) );
          REQUIRE( graph.is_branch( { ibyc(1), true } ) );
          REQUIRE( !graph.is_branch( { ibyc(1), false } ) );
          REQUIRE( graph.is_branch( { ibyc(2), true } ) );
          REQUIRE( !graph.is_branch( { ibyc(2), false } ) );
          REQUIRE( !graph.is_branch( { ibyc(6), true } ) );
          REQUIRE( !graph.is_branch( { ibyc(6), false } ) );
          REQUIRE( graph.is_branch( ibyc(1) ) );
          REQUIRE( graph.is_branch( ibyc(2) ) );
          REQUIRE( !graph.is_branch( ibyc(6) ) );
          REQUIRE( !graph.is_merge( { ibyc(9), true } ) );
          REQUIRE( !graph.is_merge( { ibyc(9), false } ) );
          REQUIRE( !graph.is_merge( { ibyc(6), true } ) );
          REQUIRE( !graph.is_merge( { ibyc(6), false } ) );
          REQUIRE( !graph.is_merge( { ibyc(8), true } ) );
          REQUIRE( graph.is_merge( { ibyc(8), false } ) );
          REQUIRE( !graph.is_merge( ibyc(9) ) );
          REQUIRE( !graph.is_merge( ibyc(6) ) );
          REQUIRE( graph.is_merge( ibyc(8) ) );
        };

    auto empty_graph_test =
        [&edges]( auto const& graph ) {
          REQUIRE( graph.get_edge_count() == 0 );
          for ( auto const& edge: edges ) {
            REQUIRE( !graph.has_edge( graph.from_side( edge ), graph.to_side( edge ) ) );
            REQUIRE( !graph.has_edge( graph.to_side( edge ), graph.from_side( edge ) ) );
            REQUIRE( !graph.has_edge( edge ) );
          }
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
      update_edges( graph );
      for ( auto const& edge : edges ) {
        graph.add_edge( graph.from_side( edge ), graph.to_side( edge ) );
      }
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "It is constructed by passing nodes list" )
    {
      graph.add_nodes( node_count, [&nodes]( id_type id ) {
                                     nodes.push_back( id );
                                   } );
      update_edges( graph );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }
    }

    WHEN( "Nodes are added in an unordered way" )
    {
      std::vector< float > shuffle( node_count );
      std::uniform_real_distribution< float > distribution( 0, 1 );
      std::mt19937 engine;
      auto generator = std::bind( distribution, engine );
      std::generate_n( shuffle.begin(), node_count, generator );

      std::vector< id_type > unordered_nodes( node_count );
      std::iota( unordered_nodes.begin(), unordered_nodes.end(), 1 );
      gum::util::sort_zip( shuffle, unordered_nodes );

      for ( id_type n : unordered_nodes ) {
        graph.add_node( n );
        nodes.push_back( n );
      }

      THEN( "The resulting graph should pass integrity tests" )
      {
        update_edges( graph );
        for ( auto const& edge : edges ) {
          graph.add_edge( graph.from_side( edge ), graph.to_side( edge ) );
        }

        integrity_test( graph );
      }

      AND_WHEN( "Nodes are sorted" )
      {
        graph.sort_nodes();
        update_edges( graph );
        for ( auto const& edge : edges ) {
          graph.add_edge( graph.from_side( edge ), graph.to_side( edge ) );
        }

        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( graph );
        }
      }
    }

    WHEN( "A Succinct graph is constructed from Dynamic one" )
    {
      graph.add_nodes( node_count );
      auto tmp = edges;
      update_edges( graph );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      succinct_type sc_graph( graph );
      sc_graph.for_each_node( [&nodes]( rank_type rank, id_type id ) {
                                nodes.push_back( id );
                                return true;
                              } );
      edges = tmp;
      update_edges( sc_graph );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }
  }
}

SCENARIO( "Specialised functionality of SeqGraph", "[seqgraph]" )
{
  GIVEN( "A simple SeqGraph" )
  {
    using graph_type = gum::SeqGraph< gum::Dynamic >;
    using succinct_type = typename graph_type::succinct_type;

    graph_type graph;
    auto integrity_test =
        []( auto const& graph, bool check_name=true, bool check_overlap=true ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using link_type = typename graph_type::link_type;
          using nodes_type = std::vector< std::pair< id_type, bool > >;
          using pathset_type = std::unordered_map< std::string, nodes_type >;

          auto ibyc =
              [&graph]( auto cid ) {
                return graph.id_by_coordinate( cid );
              };

          gum::util::for_each_start_node(
              graph,
              [ibyc]( rank_type rank, id_type id ) {
                REQUIRE( rank == 1 );
                REQUIRE( id == ibyc( 1 ) );
                return true;
              } );

          id_type end_id = 6;
          gum::util::for_each_end_node(
              graph,
              [ibyc, &end_id]( rank_type, id_type id ) {
                REQUIRE( id == ibyc( end_id++ ) );
                return true;
              } );

          rank_type path_count = 2;
          std::string names[2] = { "x", "y" };
          pathset_type pathset =
              { { "x", { { 1, false }, { 2, false }, { 5, false }, { 8, false } } },
                { "y", { { 4, false }, { 5, true }, { 7, false } } } };

          REQUIRE( graph.node_sequence( ibyc(1) ) == "TGGTCAAC" );
          REQUIRE( graph.node_sequence( ibyc(2) ) == "T" );
          REQUIRE( graph.node_sequence( ibyc(3) ) == "GCC" );
          REQUIRE( graph.node_sequence( ibyc(4) ) == "A" );
          REQUIRE( graph.node_sequence( ibyc(5) ) == "CTTAAA" );
          REQUIRE( graph.node_sequence( ibyc(6) ) == "GCG" );
          REQUIRE( graph.node_sequence( ibyc(7) ) == "CTTTT" );
          REQUIRE( graph.node_sequence( ibyc(8) ) == "AAAT" );
          REQUIRE( graph.node_length( ibyc(1) ) == 8 );
          REQUIRE( graph.node_length( ibyc(2) ) == 1 );
          REQUIRE( graph.node_length( ibyc(3) ) == 3 );
          REQUIRE( graph.node_length( ibyc(4) ) == 1 );
          REQUIRE( graph.node_length( ibyc(5) ) == 6 );
          REQUIRE( graph.node_length( ibyc(6) ) == 3 );
          REQUIRE( graph.node_length( ibyc(7) ) == 5 );
          REQUIRE( graph.node_length( ibyc(8) ) == 4 );
          if ( check_name ) {
            REQUIRE( graph.get_node_prop( 1 ).name == "1" );
            REQUIRE( graph.get_node_prop( 2 ).name == "2" );
            REQUIRE( graph.get_node_prop( 3 ).name == "3" );
            REQUIRE( graph.get_node_prop( 4 ).name == "4" );
            REQUIRE( graph.get_node_prop( 5 ).name == "5" );
            REQUIRE( graph.get_node_prop( 6 ).name == "6" );
            REQUIRE( graph.get_node_prop( 7 ).name == "7" );
            REQUIRE( graph.get_node_prop( 8 ).name == "8" );
          }
          if ( check_overlap ) {
            REQUIRE( graph.edge_overlap( link_type( { ibyc(1), true, ibyc(2), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(1), true, ibyc(3), true } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(1), true, ibyc(4), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(2), true, ibyc(5), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(3), false, ibyc(5), false } ) ) == 1 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(4), true, ibyc(5), true } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(5), false, ibyc(6), false } ) ) == 1 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(5), false, ibyc(7), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(5), true, ibyc(8), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(1), ibyc(2), graph.linktype( link_type( { ibyc(1), true, ibyc(2), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(1), ibyc(3), graph.linktype( link_type( { ibyc(1), true, ibyc(3), true } )  ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(1), ibyc(4), graph.linktype( link_type( { ibyc(1), true, ibyc(4), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(2), ibyc(5), graph.linktype( link_type( { ibyc(2), true, ibyc(5), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(3), ibyc(5), graph.linktype( link_type( { ibyc(3), false, ibyc(5), false } ) ) ) == 1 );
            REQUIRE( graph.edge_overlap( ibyc(4), ibyc(5), graph.linktype( link_type( { ibyc(4), true, ibyc(5), true } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(5), ibyc(6), graph.linktype( link_type( { ibyc(5), false, ibyc(6), false } ) ) ) == 1 );
            REQUIRE( graph.edge_overlap( ibyc(5), ibyc(7), graph.linktype( link_type( { ibyc(5), false, ibyc(7), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(5), ibyc(8), graph.linktype( link_type( { ibyc(5), true, ibyc(8), false } ) ) ) == 0 );
          }
          REQUIRE( graph.get_path_count() == path_count );
          REQUIRE( !graph.has_path( 0 ) );
          REQUIRE( !graph.has_path( -1 ) );
          REQUIRE( !graph.has_path( path_count + 1 ) );
          graph.for_each_path(
              [&graph, &path_count, &names, &pathset]( rank_type rank, id_type id ) {
                std::string const& name = names[ rank - 1 ];
                REQUIRE( graph.has_path( id ) );
                REQUIRE( graph.path_rank_to_id( rank ) == id );
                REQUIRE( graph.path_id_to_rank( id ) == rank );
                REQUIRE( graph.path_name( id ) == name );
                REQUIRE( graph.path_length( id ) == pathset[ name ].size() );
                auto path = graph.path( id );
                REQUIRE( path.get_id() == id );
                REQUIRE( path.get_name() == name );
                REQUIRE( path.size() == pathset[ name ].size() );
                rank_type nidx = 0;
                for ( auto const& node : path ) {
                  REQUIRE( path.id_of( node ) == graph.id_by_coordinate( pathset[ name ][ nidx ].first ) );
                  REQUIRE( path.is_reverse( node ) == pathset[ name ][ nidx ].second );
                  ++nidx;
                }
                nidx = 0;
                path.for_each_node(
                    [&graph, &nidx, &name, &pathset]( id_type id, bool is_reverse ) {
                      REQUIRE( graph.coordinate_id( id ) == pathset[ name ][ nidx ].first );
                      REQUIRE( is_reverse == pathset[ name ][ nidx ].second );
                      ++nidx;
                      return true;
                    } );
                auto nodes = path.get_nodes();
                nidx = 0;
                for ( auto const& node : nodes ) {
                  REQUIRE( graph.coordinate_id( path.id_of( node ) ) == pathset[ name ][ nidx ].first );
                  REQUIRE( path.is_reverse( node ) == pathset[ name ][ nidx ].second );
                  ++nidx;
                }
                --path_count;
                return true;
              } );
          REQUIRE( !path_count );
        };

    auto succinct_test =
        []( succinct_type const& graph ) {
          auto ibyc =
              [&graph]( auto cid ) {
                return graph.id_by_coordinate( cid );
              };

          REQUIRE( gum::util::position_to_id( graph, 0 ) == ibyc( 1 ) );
          REQUIRE( gum::util::position_to_id( graph, 7 ) == ibyc( 1 ) );
          REQUIRE( gum::util::position_to_id( graph, 8 ) == ibyc( 1 ) );
          REQUIRE( gum::util::position_to_id( graph, 9 ) == ibyc( 2 ) );
          REQUIRE( gum::util::position_to_id( graph, 10 ) == ibyc( 2 ) );
          REQUIRE( gum::util::position_to_id( graph, 11 ) == ibyc( 3 ) );
          REQUIRE( gum::util::position_to_id( graph, 14 ) == ibyc( 3 ) );
          REQUIRE( gum::util::position_to_id( graph, 15 ) == ibyc( 4 ) );
          REQUIRE( gum::util::position_to_id( graph, 16 ) == ibyc( 4 ) );
          REQUIRE( gum::util::position_to_id( graph, 17 ) == ibyc( 5 ) );
          REQUIRE( gum::util::position_to_id( graph, 23 ) == ibyc( 5 ) );
          REQUIRE( gum::util::position_to_id( graph, 24 ) == ibyc( 6 ) );
          REQUIRE( gum::util::position_to_id( graph, 27 ) == ibyc( 6 ) );
          REQUIRE( gum::util::position_to_id( graph, 28 ) == ibyc( 7 ) );
          REQUIRE( gum::util::position_to_id( graph, 33 ) == ibyc( 7 ) );
          REQUIRE( gum::util::position_to_id( graph, 34 ) == ibyc( 8 ) );
          REQUIRE( gum::util::position_to_id( graph, 38 ) == ibyc( 8 ) );
          REQUIRE( gum::util::position_to_offset( graph, 0 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 7 ) == 7 );
          REQUIRE( gum::util::position_to_offset( graph, 9 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 10 ) == 1 );
          REQUIRE( gum::util::position_to_offset( graph, 11 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 14 ) == 3 );
          REQUIRE( gum::util::position_to_offset( graph, 15 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 16 ) == 1 );
          REQUIRE( gum::util::position_to_offset( graph, 17 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 23 ) == 6 );
          REQUIRE( gum::util::position_to_offset( graph, 24 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 27 ) == 3 );
          REQUIRE( gum::util::position_to_offset( graph, 28 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 33 ) == 5 );
          REQUIRE( gum::util::position_to_offset( graph, 34 ) == 0 );
          REQUIRE( gum::util::position_to_offset( graph, 38 ) == 4 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 1 ) ) == 0 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 2 ) ) == 9 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 3 ) ) == 11 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 4 ) ) == 15 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 5 ) ) == 17 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 6 ) ) == 24 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 7 ) ) == 28 );
          REQUIRE( gum::util::id_to_position( graph, ibyc( 8 ) ) == 34 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 1 ) ) == 0 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 2 ) ) == 8 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 3 ) ) == 9 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 4 ) ) == 12 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 5 ) ) == 13 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 6 ) ) == 19 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 7 ) ) == 22 );
          REQUIRE( gum::util::id_to_charorder( graph, ibyc( 8 ) ) == 27 );
          REQUIRE( gum::util::ids_in_topological_order( graph ) );
        };

    auto empty_graph_test =
        []( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;

          REQUIRE( graph.get_path_count() == 0 );
          REQUIRE( !graph.has_path( 0 ) );
          REQUIRE( !graph.has_path( 1 ) );
          rank_type counter = 0;
          graph.for_each_path(
              [&counter]( rank_type rank, id_type id ) {
                ++counter;
                return true;
              } );
          REQUIRE( counter == 0 );
          counter = 3;
          graph.for_each_path(
              [&counter]( rank_type rank, id_type id ) {
                ++counter;
                return true;
              }, counter );
          REQUIRE( counter == 3 );
        };

    WHEN( "Loaded a Dynamic SeqGraph from a file in GFA 2.0 format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple_v2.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph );
          succinct_test( sc_graph );
        }

        AND_WHEN( "It is cleared" )
        {
          sc_graph.clear();
          THEN( "The graph should be empty" )
          {
            empty_graph_test( sc_graph );
          }
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in GFA 2.0 format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/graph_simple_v2.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
        succinct_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg/Protobuf format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple.pb.vg", gum::util::VGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph, false, false );
          succinct_test( sc_graph );
        }

        AND_WHEN( "It is cleared" )
        {
          sc_graph.clear();
          THEN( "The graph should be empty" )
          {
            empty_graph_test( sc_graph );
          }
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in vg/Protobuf format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/graph_simple.pb.vg", gum::util::VGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph, false, false );
        succinct_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg/HashGraph format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple.hg.vg", gum::util::HGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }

      AND_WHEN( "It is cleared" )
      {
        graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( graph );
        }
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph, false, false );
          succinct_test( sc_graph );
        }

        AND_WHEN( "It is cleared" )
        {
          sc_graph.clear();
          THEN( "The graph should be empty" )
          {
            empty_graph_test( sc_graph );
          }
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in vg/HashGraph format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/graph_simple.hg.vg", gum::util::HGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph, false, false );
        succinct_test( sc_graph );
      }

      AND_WHEN( "It is cleared" )
      {
        sc_graph.clear();
        THEN( "The graph should be empty" )
        {
          empty_graph_test( sc_graph );
        }
      }
    }
  }
}

TEMPLATE_SCENARIO( "Get graph statistics", "[seqgraph][template]",
                   ( gum::SeqGraph< gum::Dynamic > ),
                   ( gum::SeqGraph< gum::Succinct > ) )
{
  typedef TestType graph_type;

  GIVEN( "A tiny variation graph" )
  {
    std::string filepath = test_data_dir + "/tiny.gfa";
    graph_type graph;
    gum::util::load( graph, filepath, true );

    WHEN( "Total number of loci in the graph is counted" )
    {
      auto nof_loci = gum::util::total_nof_loci( graph );
      THEN( "It should be equal to the sum of node label lengths" )
      {
        REQUIRE( nof_loci == 55 );
      }
    }

    WHEN( "Total number of loci in a subgraph is counted" )
    {
      auto nof_loci = gum::util::total_nof_loci( graph, 5, 10 );
      THEN( "It should be equal to the sum of node label lengths of the subgraph" )
      {
        REQUIRE( nof_loci == 25 );
      }
    }
  }

  GIVEN( "A complex variation graph" )
  {
    std::string filepath = test_data_dir + "/complex_v2.gfa";
    graph_type graph;
    gum::util::load( graph, filepath );

    WHEN( "Iterating over start nodes" )
    {
      std::vector< typename graph_type::id_type > starts;
      gum::util::for_each_start_node(
        graph,
        [&starts]( auto rank, auto id ) {
          starts.push_back( id );
          return true;
        } );

      THEN( "It should gives the all start nodes" )
      {
        REQUIRE( starts.size() == 2 );
        CHECK( graph.coordinate_id( starts[ 0 ] ) == 1 );
        CHECK( graph.coordinate_id( starts[ 1 ] ) == 6 );
      }
    }

    WHEN( "Iterating over end nodes" )
    {
      std::vector< typename graph_type::id_type > ends;
      gum::util::for_each_end_node(
        graph,
        [&ends]( auto rank, auto id ) {
          ends.push_back( id );
          return true;
        } );

      THEN( "It should gives the all end nodes" )
      {
        REQUIRE( ends.size() == 3 );
        CHECK( graph.coordinate_id( ends[ 0 ] ) == 5 );
        CHECK( graph.coordinate_id( ends[ 1 ] ) == 9 );
        CHECK( graph.coordinate_id( ends[ 2 ] ) == 12 );
      }
    }

    WHEN( "Iterating over start sides" )
    {
      std::vector< typename graph_type::id_type > starts;
      gum::util::for_each_start_side(
        graph,
        [&starts]( auto rank, auto id ) {
          starts.push_back( id );
          return true;
        } );

      THEN( "It should gives the all start sides" )
      {
        REQUIRE( starts.size() == 4 );
        CHECK( graph.coordinate_id( starts[ 0 ] ) == 1 );
        CHECK( graph.coordinate_id( starts[ 1 ] ) == 3 );
        CHECK( graph.coordinate_id( starts[ 2 ] ) == 5 );
        CHECK( graph.coordinate_id( starts[ 3 ] ) == 6 );
      }
    }

    WHEN( "Iterating over end sides" )
    {
      std::vector< typename graph_type::id_type > ends;
      gum::util::for_each_end_side(
        graph,
        [&ends]( auto rank, auto id ) {
          ends.push_back( id );
          return true;
        } );

      THEN( "It should gives the all end sides" )
      {
        REQUIRE( ends.size() == 4 );
        CHECK( graph.coordinate_id( ends[ 0 ] ) == 3 );
        CHECK( graph.coordinate_id( ends[ 1 ] ) == 5 );
        CHECK( graph.coordinate_id( ends[ 2 ] ) == 9 );
        CHECK( graph.coordinate_id( ends[ 3 ] ) == 12 );
      }
    }
  }
}

TEMPLATE_SCENARIO( "DFS traversal", "[seqgraph][template]",
                   ( gum::SeqGraph< gum::Dynamic > ),
                   ( gum::SeqGraph< gum::Succinct > ) )
{
  using graph_type = TestType;
  using id_type = typename graph_type::id_type;
  using rank_type = typename graph_type::rank_type;

  GIVEN( "A DAG" )
  {
    std::string filepath = test_data_dir + "/dfs_dag_v2.gfa";
    graph_type graph;
    gum::util::load( graph, filepath );

    auto on_finishing = []( rank_type, id_type ){ };
    auto on_discovery = []( rank_type, id_type ){ };
    bool dag = true;
    auto on_visited = [&dag]( rank_type, id_type, bool finished ) {
      if ( dag && !finished ) dag = false;
    };

    WHEN( "It is traversed by DFS algorithm with callbacks for all events" )
    {
      gum::util::dfs_traverse( graph, on_finishing, on_discovery, on_visited );

      THEN( "The DFS should have found no back edges" )
      {
        REQUIRE( dag );
      }
    }
  }

  GIVEN( "A cyclic graph" )
  {
    std::string filepath = test_data_dir + "/dfs_cyclic_v2.gfa";
    graph_type graph;
    gum::util::load( graph, filepath );

    std::vector< rank_type > discovered;
    std::vector< rank_type > finished;
    discovered.reserve( graph.get_node_count() );
    finished.reserve( graph.get_node_count() );

    auto on_discovery = [&discovered]( rank_type r, id_type ) {
      discovered.push_back( r );
    };

    auto on_finishing = [&finished]( rank_type r, id_type ) {
      finished.push_back( r );
    };

    bool dag = true;
    auto on_visited = [&dag]( rank_type, id_type, bool finished ) {
      if ( dag && !finished ) dag = false;
    };

    const std::vector< rank_type > truth_dsc_ranks
      = { 1, 3, 6, 8, 5, 7, 2, 4, 9, 13, 14, 15, 10, 11, 12 };

    const std::vector< rank_type > truth_fin_ranks
      = { 8, 7, 5, 6, 3, 4, 2, 1, 15, 14, 13, 12, 11, 10, 9 };

    WHEN( "It is traversed by DFS algorithm with callbacks for all events" )
    {
      gum::util::dfs_traverse( graph, on_finishing, on_discovery, on_visited );

      THEN( "The order of nodes by their finishing times should be correct" )
      {
        REQUIRE( std::equal( finished.begin(), finished.end(), truth_fin_ranks.begin() ) );
      }

      THEN( "The order of nodes by their discovery times should be correct" )
      {
        REQUIRE( std::equal( discovered.begin(), discovered.end(), truth_dsc_ranks.begin() ) );
      }

      THEN( "The DFS should have found at least one back edge" )
      {
        REQUIRE( !dag );
      }
    }


    WHEN( "It is traversed by DFS algorithm with callbacks for both finishing and discovery" )
    {
      gum::util::dfs_traverse( graph, on_finishing, on_discovery );

      THEN( "The order of nodes by their finishing times should be correct" )
      {
        REQUIRE( std::equal( finished.begin(), finished.end(), truth_fin_ranks.begin() ) );
      }

      THEN( "The order of nodes by their discovery times should be correct" )
      {
        REQUIRE( std::equal( discovered.begin(), discovered.end(), truth_dsc_ranks.begin() ) );
      }
    }

    WHEN( "It is traversed by DFS algorithm with callback for finishing" )
    {
      gum::util::dfs_traverse( graph, on_finishing );

      THEN( "The order of nodes by their finishing times should be correct" )
      {
        REQUIRE( std::equal( finished.begin(), finished.end(), truth_fin_ranks.begin() ) );
      }
    }
  }
}

SCENARIO( "Topological sort", "[seqgraph]" )
{
  using graph_type = gum::SeqGraph< gum::Dynamic >;

  GIVEN( "A DAG" )
  {
    std::string filepath = test_data_dir + "/dfs_dag_v2.gfa";
    graph_type graph;
    gum::util::load( graph, filepath );

    WHEN( "Node ranks are not topologically sorted" )
    {
      REQUIRE( !gum::util::ranks_in_topological_order( graph ) );

      AND_WHEN( "It is topologically sorted" )
      {
        bool dag = gum::util::topological_sort( graph );

        THEN( "Node ranks are in topological order" )
        {
          REQUIRE( gum::util::ranks_in_topological_order( graph ) );
        }

        THEN( "No back edges are found" )
        {
          REQUIRE( dag );
        }
      }
    }
  }

  GIVEN( "A cyclic graph" )
  {
    std::string filepath = test_data_dir + "/dfs_cyclic_v2.gfa";
    graph_type graph;
    gum::util::load( graph, filepath );

    WHEN( "Node ranks are not topologically sorted" )
    {
      REQUIRE( !gum::util::ranks_in_topological_order( graph ) );

      AND_WHEN( "It is topologically sorted" )
      {
        bool dag = gum::util::topological_sort( graph );

        THEN( "Node ranks cannot be in topological order" )
        {
          REQUIRE( !gum::util::ranks_in_topological_order( graph ) );
        }

        THEN( "There should be some back edges" )
        {
          REQUIRE( !dag );
        }
      }
    }
  }
}
