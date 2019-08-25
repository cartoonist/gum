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
    std::vector< id_type > nodes( { 1, 2, 3, 72, 23, 6, 401, 10344, 92 } );
    auto integrity_test = [&nodes, abs_id]( graph_type const& graph ) {
      REQUIRE( graph.get_node_count() == nodes.size() );
      REQUIRE( graph.get_max_node_rank( ) == nodes.size() );
      for ( rank_type i = 1; i < graph.get_max_node_rank(); ++i ) {
        REQUIRE( graph.id_to_rank( graph.rank_to_id( i ) ) == i );
        REQUIRE( graph.has_node( nodes[ i - 1 ] ) );
      }
      REQUIRE( !graph.has_node( abs_id ) );
    };

    WHEN( "It is constructed incrementally" )
    {
      for ( auto const& id : nodes ) graph.add_node( id );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "It is constructed by passing nodes list" )
    {
      graph.set_nodes( nodes );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "Adding non-positive node ID" )
    {
      THEN( "The method should throw an exception" )
      {
        REQUIRE_THROWS( graph.add_node( 0 ) );
        REQUIRE_THROWS( graph.add_node( -1 ) );
      }
    }

    WHEN( "Adding existing node ID" )
    {
      graph.set_nodes( nodes );
      THEN( "The method should silently ignore it" )
      {
        graph.add_node( nodes[ 0 ] );
        graph.add_node( nodes[ 2 ] );
        graph.add_node( nodes[ 5 ] );
        REQUIRE( graph.get_node_count() == nodes.size() );
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
    using side_type = typename graph_type::side_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 66;
    std::vector< id_type > nodes( { 1, 2, 3, 72, 23, 6, 401, 10344, 92 } );
    std::vector< link_type > edges = {
      { 1, 2 },
      { 1, 3 },
      { 2, 23 },
      { 2, 6 },
      { 2, 401 },
      { 3, 72 },
      { 23, 10344 },
      { 6, 10344 },
      { 401, 10344 },
      { 72, 10344 },
      { 10344, 92 }
    };
    auto integrity_test = [&edges, abs_id]( graph_type const& graph ) {
      REQUIRE( graph.get_edge_count() == edges.size() );
      for ( auto const& edge: edges ) {
        REQUIRE( graph.has_edge( edge.first, edge.second ) );
        REQUIRE( !graph.has_edge( edge.second, edge.first ) );
        REQUIRE( !graph.has_edge( abs_id, edge.second ) );
        REQUIRE( graph.has_edge( edge ) );
      }
      auto adjs = graph.get_adjacents_to( 2 );
      std::vector< side_type > truth = { 23, 6, 401 };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_to( 10344 );
      truth = { 92 };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_to( abs_id );
      REQUIRE( adjs.empty() );
      adjs = graph.get_adjacents_from( 2 );
      truth = { 1 };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_from( 10344 );
      truth = { 23, 6, 401, 72 };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      REQUIRE( graph.outdegree( 1 ) == 2 );
      REQUIRE( graph.outdegree( 2 ) == 3 );
      REQUIRE( graph.outdegree( 6 ) == 1 );
      REQUIRE( graph.outdegree( abs_id ) == 0 );
      REQUIRE( graph.indegree( 92 ) == 1 );
      REQUIRE( graph.indegree( 10344 ) == 4 );
      REQUIRE( graph.indegree( 6 ) == 1 );
      REQUIRE( graph.indegree( abs_id ) == 0 );
      REQUIRE( !graph.has_edges_from( 1 ) );
      REQUIRE( graph.has_edges_from( 2 ) );
      REQUIRE( graph.has_edges_from( 10344 ) );
      REQUIRE( graph.has_edges_from( 92 ) );
      REQUIRE( graph.has_edges_to( 1 ) );
      REQUIRE( graph.has_edges_to( 2 ) );
      REQUIRE( graph.has_edges_to( 10344 ) );
      REQUIRE( !graph.has_edges_to( 92 ) );
    };

    WHEN( "It is constructed incrementally" )
    {
      for ( auto const& id : nodes ) graph.add_node( id );
      for ( auto const& edge : edges ) graph.add_edge( edge.first, edge.second );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "It is constructed by passing nodes list" )
    {
      graph.set_nodes( nodes );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }
  }
}

SCENARIO( "Specialised functionality of Bidirected DirectedGraph", "[seqgraph]" )
{
  GIVEN( "A Bidirected DirectedGraph with some nodes and edges" )
  {
    using graph_type = gum::DirectedGraph< gum::Dynamic, gum::Bidirected >;
    using id_type = typename graph_type::id_type;
    using side_type = typename graph_type::side_type;
    using link_type = typename graph_type::link_type;

    graph_type graph;
    id_type abs_id = 66;
    std::vector< id_type > nodes( { 1, 2, 3, 72, 23, 6, 401, 10344, 92 } );
    std::vector< link_type > edges = {
      { 1, true, 2, false },
      { 1, true, 3, true },
      { 2, true, 23, false },
      { 2, true, 6, true },
      { 2, true, 401, false },
      { 3, false, 72, false },
      { 23, true, 10344, false },
      { 6, false, 10344, true },
      { 401, true, 10344, false },
      { 72, true, 10344, false },
      { 10344, false, 92, false }
    };
    auto integrity_test = [&edges, abs_id]( graph_type const& graph ) {
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
      auto adjs = graph.get_adjacents_to( { 2, true } );
      std::vector< side_type > truth = { { 23, false }, { 6, true }, { 401, false } };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_to( { 10344, false } );
      truth = { { 92, false } };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_to( { 10344, true } );
      REQUIRE( adjs.empty() );
      adjs = graph.get_adjacents_to( { abs_id, true } );
      REQUIRE( adjs.empty() );
      adjs = graph.get_adjacents_from( { 2, false } );
      truth = { { 1, true } };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_from( { 2, true } );
      REQUIRE( adjs.empty() );
      adjs = graph.get_adjacents_from( { 10344, false } );
      truth = { { 23, true }, { 401, true }, { 72, true } };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      adjs = graph.get_adjacents_from( { 10344, true } );
      truth = { { 6, false } };
      REQUIRE( adjs.size() == truth.size() );
      for ( auto const& side : truth ) {
        REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
      }
      REQUIRE( graph.outdegree( { 1, true } ) == 2 );
      REQUIRE( graph.outdegree( { 1, false } ) == 0 );
      REQUIRE( graph.outdegree( { 2, true } ) == 3 );
      REQUIRE( graph.outdegree( { 2, false } ) == 0 );
      REQUIRE( graph.outdegree( { 6, true } ) == 0 );
      REQUIRE( graph.outdegree( { 6, false } ) == 1 );
      REQUIRE( graph.outdegree( { abs_id, true } ) == 0 );
      REQUIRE( graph.outdegree( { abs_id, false } ) == 0 );
      REQUIRE( graph.indegree( { 92, true } ) == 0 );
      REQUIRE( graph.indegree( { 92, false } ) == 1 );
      REQUIRE( graph.indegree( { 6, true } ) == 1 );
      REQUIRE( graph.indegree( { 6, false } ) == 0 );
      REQUIRE( graph.indegree( { 10344, true } ) == 1 );
      REQUIRE( graph.indegree( { 10344, false } ) == 3 );
      REQUIRE( graph.indegree( { abs_id, true } ) == 0 );
      REQUIRE( graph.indegree( { abs_id, false } ) == 0 );
      REQUIRE( !graph.has_edges_from( { 1, true } ) );
      REQUIRE( !graph.has_edges_from( { 1, false } ) );
      REQUIRE( !graph.has_edges_from( { 2, true } ) );
      REQUIRE( graph.has_edges_from( { 2, false } ) );
      REQUIRE( graph.has_edges_from( { 10344, true } ) );
      REQUIRE( graph.has_edges_from( { 10344, false } ) );
      REQUIRE( !graph.has_edges_from( { 92, true } ) );
      REQUIRE( graph.has_edges_from( { 92, false } ) );
      REQUIRE( !graph.has_edges_from( { abs_id, true } ) );
      REQUIRE( !graph.has_edges_from( { abs_id, false } ) );
      REQUIRE( graph.has_edges_to( { 1, true } ) );
      REQUIRE( !graph.has_edges_to( { 1, false } ) );
      REQUIRE( graph.has_edges_to( { 2, true } ) );
      REQUIRE( !graph.has_edges_to( { 2, false } ) );
      REQUIRE( !graph.has_edges_to( { 10344, true } ) );
      REQUIRE( graph.has_edges_to( { 10344, false } ) );
      REQUIRE( !graph.has_edges_to( { 92, true } ) );
      REQUIRE( !graph.has_edges_to( { 92, false } ) );
      REQUIRE( !graph.has_edges_to( { abs_id, true } ) );
      REQUIRE( !graph.has_edges_to( { abs_id, false } ) );
    };

    WHEN( "It is constructed incrementally" )
    {
      for ( auto const& id : nodes ) graph.add_node( id );
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
      graph.set_nodes( nodes );
      for ( auto const& edge : edges ) graph.add_edge( edge );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }
  }
}
