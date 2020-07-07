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
#include <random>

#include <gum/seqgraph.hpp>
#include <gum/io_utils.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Generic functionality of DirectedGraph", "[seqgraph][template]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, 32, 32 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 16, 64 > ),
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
        succinct_type sc_graph( graph );
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
        succinct_type sc_graph( graph );
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
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, 16, 64 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, 32, 32 > ) )
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
            edges[ i ] = link_type( graph.rank_to_id( graph.from_id( e ) ),
                                    graph.rank_to_id( graph.to_id( e ) ) );
          }
        };
    auto integrity_test =
        [&edges, abs_id]( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using side_type = typename graph_type::side_type;
          using linktype_type = typename graph_type::linktype_type;

          auto rtoi =
              [&graph]( rank_type rank ) {
                return graph.rank_to_id( rank );
              };

          REQUIRE( graph.get_edge_count() == edges.size() );
          for ( rank_type rank = 1; rank <= graph.get_node_count(); ++rank ) {
            graph.for_each_side(
                rtoi(rank),
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
          auto adjs = graph.adjacents_out( rtoi(2) );
          truth = { rtoi(5), rtoi(6), rtoi(7) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( rtoi(2), to_id_truth_check );
          graph.for_each_edges_out( side_type( rtoi(2) ), side_truth_check );
          adjs = graph.adjacents_out( rtoi(8) );
          truth = { rtoi(9) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( rtoi(8), to_id_truth_check );
          graph.for_each_edges_out( side_type( rtoi(8) ), side_truth_check );
          adjs = graph.adjacents_in( rtoi(2) );
          truth = { rtoi(1) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( rtoi(2), from_id_truth_check );
          graph.for_each_edges_in( side_type( rtoi(2) ), side_truth_check );
          adjs = graph.adjacents_in( rtoi(8) );
          truth = { rtoi(5), rtoi(6), rtoi(7), rtoi(4) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( rtoi(8), from_id_truth_check );
          graph.for_each_edges_in( side_type( rtoi(8) ), side_truth_check );
          REQUIRE( graph.outdegree( rtoi(1) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(2) ) == 3 );
          REQUIRE( graph.outdegree( rtoi(6) ) == 1 );
          REQUIRE( graph.outdegree( { rtoi(1) } ) == 2 );
          REQUIRE( graph.outdegree( { rtoi(2) } ) == 3 );
          REQUIRE( graph.outdegree( { rtoi(6) } ) == 1 );
          REQUIRE( graph.indegree( rtoi(9) ) == 1 );
          REQUIRE( graph.indegree( rtoi(8) ) == 4 );
          REQUIRE( graph.indegree( rtoi(6) ) == 1 );
          REQUIRE( graph.indegree( { rtoi(9) } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(8) } ) == 4 );
          REQUIRE( graph.indegree( { rtoi(6) } ) == 1 );
          REQUIRE( !graph.has_edges_in( rtoi(1) ) );
          REQUIRE( graph.has_edges_in( rtoi(2) ) );
          REQUIRE( graph.has_edges_in( rtoi(8) ) );
          REQUIRE( graph.has_edges_in( rtoi(9) ) );
          REQUIRE( !graph.has_edges_in( { rtoi(1) } ) );
          REQUIRE( graph.has_edges_in( { rtoi(2) } ) );
          REQUIRE( graph.has_edges_in( { rtoi(8) } ) );
          REQUIRE( graph.has_edges_in( { rtoi(9) } ) );
          REQUIRE( graph.has_edges_out( rtoi(1) ) );
          REQUIRE( graph.has_edges_out( rtoi(2) ) );
          REQUIRE( graph.has_edges_out( rtoi(8) ) );
          REQUIRE( !graph.has_edges_out( rtoi(9) ) );
          REQUIRE( graph.has_edges_out( { rtoi(1) } ) );
          REQUIRE( graph.has_edges_out( { rtoi(2) } ) );
          REQUIRE( graph.has_edges_out( { rtoi(8) } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(9) } ) );
          REQUIRE( graph.is_branch( rtoi(1) ) );
          REQUIRE( graph.is_branch( rtoi(2) ) );
          REQUIRE( !graph.is_branch( rtoi(6) ) );
          REQUIRE( graph.is_branch( { rtoi(1) } ) );
          REQUIRE( graph.is_branch( { rtoi(2) } ) );
          REQUIRE( !graph.is_branch( { rtoi(6) } ) );
          REQUIRE( !graph.is_merge( rtoi(9) ) );
          REQUIRE( graph.is_merge( rtoi(8) ) );
          REQUIRE( !graph.is_merge( rtoi(6) ) );
          REQUIRE( !graph.is_merge( { rtoi(9) } ) );
          REQUIRE( graph.is_merge( { rtoi(8) } ) );
          REQUIRE( !graph.is_merge( { rtoi(6) } ) );
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

TEMPLATE_SCENARIO( "Specialised functionality of Bidirected DirectedGraph", "[seqgraph]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 64, 16 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 16, 64 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 32, 32 > ),
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
            edges[ i ] = link_type( graph.rank_to_id( std::get< 0 >( e ) ),
                                    std::get< 1 >( e ),
                                    graph.rank_to_id( std::get< 2 >( e ) ),
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

          auto rtoi =
              [&graph]( rank_type rank ) {
                return graph.rank_to_id( rank );
              };

          REQUIRE( graph.get_edge_count() == edges.size() );
          for ( rank_type rank = 1; rank <= graph.get_node_count(); ++rank ) {
            bool sidetype = false;
            graph.for_each_side(
                rtoi(rank),
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
          auto adjs = graph.adjacents_out( { rtoi(2), true } );
          truth = { { rtoi(5), false }, { rtoi(6), true }, { rtoi(7), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(2), true }, side_truth_check );
          adjs = graph.adjacents_out( { rtoi(8), false } );
          truth = { { rtoi(9), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(8), false }, side_truth_check );
          adjs = graph.adjacents_out( { rtoi(8), true } );
          REQUIRE( adjs.empty() );
          graph.for_each_edges_out( rtoi(8), to_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(2), false } );
          truth = { { rtoi(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(2), false }, side_truth_check );
          adjs = graph.adjacents_in( { rtoi(2), true } );
          REQUIRE( adjs.empty() );
          graph.for_each_edges_in( rtoi(2), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(8), false } );
          truth = { { rtoi(5), true }, { rtoi(7), true }, { rtoi(4), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(8), false }, side_truth_check );
          truth.push_back( { rtoi(6), false } );
          graph.for_each_edges_in( rtoi(8), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(8), true } );
          truth = { { rtoi(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(8), true }, side_truth_check );
          REQUIRE( graph.outdegree( { rtoi(1), true } ) == 2 );
          REQUIRE( graph.outdegree( { rtoi(1), false } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(2), true } ) == 3 );
          REQUIRE( graph.outdegree( { rtoi(2), false } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(6), true } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(6), false } ) == 1 );
          REQUIRE( graph.outdegree( rtoi(1) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(2) ) == 3 );
          REQUIRE( graph.outdegree( rtoi(6) ) == 1 );
          REQUIRE( graph.indegree( { rtoi(9), true } ) == 0 );
          REQUIRE( graph.indegree( { rtoi(9), false } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(6), true } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(6), false } ) == 0 );
          REQUIRE( graph.indegree( { rtoi(8), true } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(8), false } ) == 3 );
          REQUIRE( graph.indegree( rtoi(9) ) == 1 );
          REQUIRE( graph.indegree( rtoi(6) ) == 1 );
          REQUIRE( graph.indegree( rtoi(8) ) == 4 );
          REQUIRE( !graph.has_edges_in( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(1), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(2), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(2), false } ) );
          REQUIRE( graph.has_edges_in( { rtoi(8), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(8), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(9), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(9), false } ) );
          REQUIRE( !graph.has_edges_in( rtoi(1) ) );
          REQUIRE( graph.has_edges_in( rtoi(2) ) );
          REQUIRE( graph.has_edges_in( rtoi(8) ) );
          REQUIRE( graph.has_edges_in( rtoi(9) ) );
          REQUIRE( graph.has_edges_out( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(1), false } ) );
          REQUIRE( graph.has_edges_out( { rtoi(2), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(2), false } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(8), true } ) );
          REQUIRE( graph.has_edges_out( { rtoi(8), false } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(9), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(9), false } ) );
          REQUIRE( graph.has_edges_out( rtoi(1) ) );
          REQUIRE( graph.has_edges_out( rtoi(2) ) );
          REQUIRE( graph.has_edges_out( rtoi(8) ) );
          REQUIRE( !graph.has_edges_out( rtoi(9) ) );
          REQUIRE( graph.is_branch( { rtoi(1), true } ) );
          REQUIRE( !graph.is_branch( { rtoi(1), false } ) );
          REQUIRE( graph.is_branch( { rtoi(2), true } ) );
          REQUIRE( !graph.is_branch( { rtoi(2), false } ) );
          REQUIRE( !graph.is_branch( { rtoi(6), true } ) );
          REQUIRE( !graph.is_branch( { rtoi(6), false } ) );
          REQUIRE( graph.is_branch( rtoi(1) ) );
          REQUIRE( graph.is_branch( rtoi(2) ) );
          REQUIRE( !graph.is_branch( rtoi(6) ) );
          REQUIRE( !graph.is_merge( { rtoi(9), true } ) );
          REQUIRE( !graph.is_merge( { rtoi(9), false } ) );
          REQUIRE( !graph.is_merge( { rtoi(6), true } ) );
          REQUIRE( !graph.is_merge( { rtoi(6), false } ) );
          REQUIRE( !graph.is_merge( { rtoi(8), true } ) );
          REQUIRE( graph.is_merge( { rtoi(8), false } ) );
          REQUIRE( !graph.is_merge( rtoi(9) ) );
          REQUIRE( !graph.is_merge( rtoi(6) ) );
          REQUIRE( graph.is_merge( rtoi(8) ) );
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
        []( auto const& graph ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using link_type = typename graph_type::link_type;
          using nodes_type = std::vector< std::pair< id_type, bool > >;
          using pathset_type = std::unordered_map< std::string, nodes_type >;

          auto rtoi =
              [&graph]( rank_type rank ) {
                return graph.rank_to_id( rank );
              };

          rank_type path_count = 2;
          std::string names[2] = { "x", "y" };
          pathset_type pathset =
              { { "x", { { 1, false }, { 2, false }, { 5, false }, { 8, false } } },
                { "y", { { 4, false }, { 5, true }, { 7, false } } } };

          REQUIRE( graph.node_sequence( rtoi(1) ) == "TGGTCAAC" );
          REQUIRE( graph.node_sequence( rtoi(2) ) == "T" );
          REQUIRE( graph.node_sequence( rtoi(3) ) == "GCC" );
          REQUIRE( graph.node_sequence( rtoi(4) ) == "A" );
          REQUIRE( graph.node_sequence( rtoi(5) ) == "CTTAAA" );
          REQUIRE( graph.node_sequence( rtoi(6) ) == "GCG" );
          REQUIRE( graph.node_sequence( rtoi(7) ) == "CTTTT" );
          REQUIRE( graph.node_sequence( rtoi(8) ) == "AAAT" );
          REQUIRE( graph.node_length( rtoi(1) ) == 8 );
          REQUIRE( graph.node_length( rtoi(2) ) == 1 );
          REQUIRE( graph.node_length( rtoi(3) ) == 3 );
          REQUIRE( graph.node_length( rtoi(4) ) == 1 );
          REQUIRE( graph.node_length( rtoi(5) ) == 6 );
          REQUIRE( graph.node_length( rtoi(6) ) == 3 );
          REQUIRE( graph.node_length( rtoi(7) ) == 5 );
          REQUIRE( graph.node_length( rtoi(8) ) == 4 );
          REQUIRE( graph.get_node_prop( 1 ).name == "1" );
          REQUIRE( graph.get_node_prop( 2 ).name == "2" );
          REQUIRE( graph.get_node_prop( 3 ).name == "3" );
          REQUIRE( graph.get_node_prop( 4 ).name == "4" );
          REQUIRE( graph.get_node_prop( 5 ).name == "5" );
          REQUIRE( graph.get_node_prop( 6 ).name == "6" );
          REQUIRE( graph.get_node_prop( 7 ).name == "7" );
          REQUIRE( graph.get_node_prop( 8 ).name == "8" );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(1), true, rtoi(2), false } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(1), true, rtoi(3), true } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(1), true, rtoi(4), false } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(2), true, rtoi(5), false } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(3), false, rtoi(5), false } ) ) == 1 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(4), true, rtoi(5), true } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(5), false, rtoi(6), false } ) ) == 1 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(5), false, rtoi(7), false } ) ) == 0 );
          REQUIRE( graph.edge_overlap( link_type( { rtoi(5), true, rtoi(8), false } ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(1), rtoi(2), graph.linktype( link_type( { rtoi(1), true, rtoi(2), false } ) ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(1), rtoi(3), graph.linktype( link_type( { rtoi(1), true, rtoi(3), true } )  ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(1), rtoi(4), graph.linktype( link_type( { rtoi(1), true, rtoi(4), false } ) ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(2), rtoi(5), graph.linktype( link_type( { rtoi(2), true, rtoi(5), false } ) ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(3), rtoi(5), graph.linktype( link_type( { rtoi(3), false, rtoi(5), false } ) ) ) == 1 );
          REQUIRE( graph.edge_overlap( rtoi(4), rtoi(5), graph.linktype( link_type( { rtoi(4), true, rtoi(5), true } ) ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(5), rtoi(6), graph.linktype( link_type( { rtoi(5), false, rtoi(6), false } ) ) ) == 1 );
          REQUIRE( graph.edge_overlap( rtoi(5), rtoi(7), graph.linktype( link_type( { rtoi(5), false, rtoi(7), false } ) ) ) == 0 );
          REQUIRE( graph.edge_overlap( rtoi(5), rtoi(8), graph.linktype( link_type( { rtoi(5), true, rtoi(8), false } ) ) ) == 0 );
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
                  REQUIRE( path.id_of( node ) == pathset[ name ][ nidx ].first );
                  REQUIRE( path.is_reverse( node ) == pathset[ name ][ nidx ].second );
                  ++nidx;
                }
                nidx = 0;
                path.for_each_node(
                    [&nidx, &name, &pathset]( id_type id, bool is_reverse ) {
                      REQUIRE( id == pathset[ name ][ nidx ].first );
                      REQUIRE( is_reverse == pathset[ name ][ nidx ].second );
                      ++nidx;
                      return true;
                    } );
                auto nodes = path.get_nodes();
                nidx = 0;
                for ( auto const& node : nodes ) {
                  REQUIRE( path.id_of( node ) == pathset[ name ][ nidx ].first );
                  REQUIRE( path.is_reverse( node ) == pathset[ name ][ nidx ].second );
                  ++nidx;
                }
                --path_count;
                return true;
              } );
          REQUIRE( !path_count );
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
      gum::util::extend( graph, test_data_dir + "/graph_simple_v2.gfa" );
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
      gum::util::load( sc_graph, test_data_dir + "/graph_simple_v2.gfa" );
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

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple.vg" );
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

    WHEN( "Loaded a Succinct SeqGraph from a file in vg format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/graph_simple.vg" );
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
