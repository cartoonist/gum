/**
 *    @file  test_seqgraph.h
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

#include "seqgraph.hpp"
#include "io_utils.hpp"

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Generic functionality of DirectedGraph", "[seqgraph][template]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Directed, 32, 32 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 64, 32 > ),
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected, 32, 64 > ),
                   ( gum::SeqGraph< gum::Dynamic > ) )
{
  GIVEN( "A DirectedGraph with some nodes and edges" )
  {
    using graph_type = TestType;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;

    graph_type graph;
    id_type abs_id = 66;
    std::vector< id_type > nodes;
    rank_type node_count = 9;
    auto integrity_test =
        [&nodes, node_count, abs_id]( graph_type const& graph ) {
          REQUIRE( graph.get_node_count() == node_count );
          for ( rank_type i = 1; i < graph.get_node_count(); ++i ) {
            REQUIRE( graph.id_to_rank( graph.rank_to_id( i ) ) == i );
            REQUIRE( graph.has_node( nodes[ i - 1 ] ) );
          }
          REQUIRE( !graph.has_node( abs_id ) );
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
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
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }
  }
}

SCENARIO( "Specialised functionality of DirectedGraph", "[seqgraph]" )
{
  GIVEN( "A DirectedGraph with some nodes and edges" )
  {
    using graph_type = gum::DirectedGraph< gum::Dynamic, gum::Directed >;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;
    using side_type = typename graph_type::side_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 66;
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
    auto rtoi =
        [&graph]( rank_type rank ) {
          return graph.rank_to_id( rank );
        };
    auto update_edges =
        [&graph, &nodes, &edges, rtoi]( ) {
          for ( std::size_t i = 0; i < edges.size(); ++i ) {
            link_type e = edges[ i ];
            edges[ i ] = link_type( rtoi( graph.from_id( e ) ),
                                    rtoi( graph.to_id( e ) ) );
          }
        };
    auto integrity_test =
        [&edges, abs_id, rtoi]( graph_type const& graph ) {
          REQUIRE( graph.get_edge_count() == edges.size() );
          for ( auto const& edge: edges ) {
            REQUIRE( graph.has_edge( edge.first, edge.second ) );
            REQUIRE( !graph.has_edge( edge.second, edge.first ) );
            REQUIRE( !graph.has_edge( abs_id, edge.second ) );
            REQUIRE( graph.has_edge( edge ) );
          }
          auto adjs = graph.adjacents_to( rtoi(2) );
          std::vector< side_type > truth = { rtoi(5), rtoi(6), rtoi(7) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_to( rtoi(8) );
          truth = { rtoi(9) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_to( abs_id );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_from( rtoi(2) );
          truth = { rtoi(1) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_from( rtoi(8) );
          truth = { rtoi(5), rtoi(6), rtoi(7), rtoi(4) };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          REQUIRE( graph.outdegree( rtoi(1) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(2) ) == 3 );
          REQUIRE( graph.outdegree( rtoi(6) ) == 1 );
          REQUIRE( graph.outdegree( abs_id ) == 0 );
          REQUIRE( graph.indegree( rtoi(9) ) == 1 );
          REQUIRE( graph.indegree( rtoi(8) ) == 4 );
          REQUIRE( graph.indegree( rtoi(6) ) == 1 );
          REQUIRE( graph.indegree( abs_id ) == 0 );
          REQUIRE( !graph.has_edges_from( rtoi(1) ) );
          REQUIRE( graph.has_edges_from( rtoi(2) ) );
          REQUIRE( graph.has_edges_from( rtoi(8) ) );
          REQUIRE( graph.has_edges_from( rtoi(9) ) );
          REQUIRE( graph.has_edges_to( rtoi(1) ) );
          REQUIRE( graph.has_edges_to( rtoi(2) ) );
          REQUIRE( graph.has_edges_to( rtoi(8) ) );
          REQUIRE( !graph.has_edges_to( rtoi(9) ) );
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
      update_edges();
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
      update_edges();
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }
  }
}

TEMPLATE_SCENARIO( "Specialised functionality of Bidirected DirectedGraph", "[seqgraph]",
                   ( gum::DirectedGraph< gum::Dynamic, gum::Bidirected > ),
                   ( gum::SeqGraph< gum::Dynamic > ) )
{
  GIVEN( "A Bidirected DirectedGraph with some nodes and edges" )
  {
    using graph_type = TestType;
    using id_type = typename graph_type::id_type;
    using rank_type = typename graph_type::rank_type;
    using side_type = typename graph_type::side_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 66;
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
    auto rtoi =
        [&graph]( rank_type rank ) {
          return graph.rank_to_id( rank );
        };
    auto update_edges =
        [&graph, &nodes, &edges, rtoi]( ) {
          for ( std::size_t i = 0; i < edges.size(); ++i ) {
            link_type e = edges[ i ];
            edges[ i ] = link_type( rtoi( std::get< 0 >( e ) ),
                                    std::get< 1 >( e ),
                                    rtoi( std::get< 2 >( e ) ),
                                    std::get< 3 >( e ) );
          }
        };
    auto integrity_test =
        [&edges, abs_id, rtoi]( graph_type const& graph ) {
          REQUIRE( graph.get_edge_count() == edges.size() );
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
          }
          auto adjs = graph.adjacents_to( { rtoi(2), true } );
          std::vector< side_type > truth =
              { { rtoi(5), false }, { rtoi(6), true }, { rtoi(7), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_to( { rtoi(8), false } );
          truth = { { rtoi(9), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_to( { rtoi(8), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_to( { abs_id, true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_from( { rtoi(2), false } );
          truth = { { rtoi(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_from( { rtoi(2), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_from( { rtoi(8), false } );
          truth = { { rtoi(5), true }, { rtoi(7), true }, { rtoi(4), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          adjs = graph.adjacents_from( { rtoi(8), true } );
          truth = { { rtoi(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          REQUIRE( graph.outdegree( { rtoi(1), true } ) == 2 );
          REQUIRE( graph.outdegree( { rtoi(1), false } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(2), true } ) == 3 );
          REQUIRE( graph.outdegree( { rtoi(2), false } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(6), true } ) == 0 );
          REQUIRE( graph.outdegree( { rtoi(6), false } ) == 1 );
          REQUIRE( graph.outdegree( { abs_id, true } ) == 0 );
          REQUIRE( graph.outdegree( { abs_id, false } ) == 0 );
          REQUIRE( graph.indegree( { rtoi(9), true } ) == 0 );
          REQUIRE( graph.indegree( { rtoi(9), false } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(6), true } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(6), false } ) == 0 );
          REQUIRE( graph.indegree( { rtoi(8), true } ) == 1 );
          REQUIRE( graph.indegree( { rtoi(8), false } ) == 3 );
          REQUIRE( graph.indegree( { abs_id, true } ) == 0 );
          REQUIRE( graph.indegree( { abs_id, false } ) == 0 );
          REQUIRE( !graph.has_edges_from( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_from( { rtoi(1), false } ) );
          REQUIRE( !graph.has_edges_from( { rtoi(2), true } ) );
          REQUIRE( graph.has_edges_from( { rtoi(2), false } ) );
          REQUIRE( graph.has_edges_from( { rtoi(8), true } ) );
          REQUIRE( graph.has_edges_from( { rtoi(8), false } ) );
          REQUIRE( !graph.has_edges_from( { rtoi(9), true } ) );
          REQUIRE( graph.has_edges_from( { rtoi(9), false } ) );
          REQUIRE( !graph.has_edges_from( { abs_id, true } ) );
          REQUIRE( !graph.has_edges_from( { abs_id, false } ) );
          REQUIRE( graph.has_edges_to( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_to( { rtoi(1), false } ) );
          REQUIRE( graph.has_edges_to( { rtoi(2), true } ) );
          REQUIRE( !graph.has_edges_to( { rtoi(2), false } ) );
          REQUIRE( !graph.has_edges_to( { rtoi(8), true } ) );
          REQUIRE( graph.has_edges_to( { rtoi(8), false } ) );
          REQUIRE( !graph.has_edges_to( { rtoi(9), true } ) );
          REQUIRE( !graph.has_edges_to( { rtoi(9), false } ) );
          REQUIRE( !graph.has_edges_to( { abs_id, true } ) );
          REQUIRE( !graph.has_edges_to( { abs_id, false } ) );
        };

    WHEN( "It is constructed incrementally" )
    {
      for ( rank_type i = 1; i <= node_count; ++i )
        nodes.push_back( graph.add_node( ) );
      update_edges();
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
      update_edges();
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "Adding existing edge" )
    {
      graph.add_nodes( node_count, [&nodes]( id_type id ) {
                                     nodes.push_back( id );
                                   } );
      update_edges();
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The method should silently ignore it" )
      {
        graph.add_edge( { rtoi(1), true, rtoi(2), false } );
        graph.add_edge( { rtoi(8), false, rtoi(9), false } );
        REQUIRE( graph.get_edge_count() == edges.size() );
      }
    }
  }
}

SCENARIO( "Specialised functionality of Dynamic SeqGraph", "[seqgraph]" )
{
  GIVEN( "A simple Dynamic SeqGraph" )
  {
    using graph_type = gum::SeqGraph< gum::Dynamic >;
    using rank_type = typename graph_type::rank_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    auto rtoi =
        [&graph]( rank_type rank ) {
          return graph.rank_to_id( rank );
        };
    auto integrity_test =
        [rtoi]( graph_type const& graph ) {
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
          REQUIRE( graph.get_node_prop( )[ 1 ].name == "1" );
          REQUIRE( graph.get_node_prop( )[ 2 ].name == "2" );
          REQUIRE( graph.get_node_prop( )[ 3 ].name == "3" );
          REQUIRE( graph.get_node_prop( )[ 4 ].name == "4" );
          REQUIRE( graph.get_node_prop( )[ 5 ].name == "5" );
          REQUIRE( graph.get_node_prop( )[ 6 ].name == "6" );
          REQUIRE( graph.get_node_prop( )[ 7 ].name == "7" );
          REQUIRE( graph.get_node_prop( )[ 8 ].name == "8" );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(1), true, rtoi(2), false } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(1), true, rtoi(3), true } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(1), true, rtoi(4), false } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(2), true, rtoi(5), false } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(3), false, rtoi(5), false } ) ].overlap == 1 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(4), true, rtoi(5), true } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(5), false, rtoi(6), false } ) ].overlap == 1 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(5), false, rtoi(7), false } ) ].overlap == 0 );
          REQUIRE( graph.get_edge_prop( )[ link_type( { rtoi(5), true, rtoi(8), false } ) ].overlap == 0 );
        };

    WHEN( "Loaded from a file in GFA 2.0 format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple_v2.gfa" );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "Loaded from a file in vg format" )
    {
      gum::util::extend( graph, test_data_dir + "/graph_simple.vg" );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }
  }
}
