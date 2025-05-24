/**
 *    @file  test_ioutils.cpp
 *   @brief  Test cases for `io_utils` module.
 *
 *  This source file includes test scenarios for `io_utils` module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sat Mar 7, 2020  09:36
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <vector>
#include <utility>

#include <gum/seqgraph.hpp>
#include <gum/io_utils.hpp>

#include "test_base.hpp"


TEMPLATE_SCENARIO( "Sanity checks for serialising a SeqGraph", "[ioutils][serialise]",
                   ( gum::SeqGraph< gum::Dynamic > ),
                   ( gum::SeqGraph< gum::Succinct > ) )
{
  using graph_type = TestType;
  using id_type = typename graph_type::id_type;
  using rank_type = typename graph_type::rank_type;

  auto compare_graph = []( auto const& g1, auto const& g2 ) -> bool {
    if ( g1.get_node_count() != g2.get_node_count() ) { return false; }
    if ( g1.get_edge_count() != g2.get_edge_count() ) { return false; }
    if ( g1.get_path_count() != g2.get_path_count() ) { return false; }

    auto res = g1.for_each_node( [&]( rank_type rank, id_type id ) {
      if ( !g2.has_node( id ) ) return false;

      auto res = g1.for_each_side( id, [&]( auto from ) {
        auto res = g1.for_each_edges_out( from, [&]( auto to ) {
          if ( !g2.has_edge( from, to ) ) return false;
          return true;
        } );
        if ( !res ) return false;
        return true;
      } );

      if ( !res ) return false;
      return true;
    } );

    if ( !res ) return false;

    return g1.for_each_path( [&]( auto, auto pid1 ) {
      auto pname = g1.path_name( pid1 );
      id_type pid2 = 0;
      g2.for_each_path( [&]( auto, auto _pid2 ) {
        if ( g2.path_name( _pid2 ) == pname ) {
          pid2 = _pid2;
          return false;
        }
        return true;
      } );

      if ( pid2 == 0 ) return false;
      auto path1 = g1.path( pid1 );
      auto path2 = g2.path( pid2 );

      return std::equal( path1.begin(), path1.end(), path2.begin(),
                         path2.end() );
    } );
  };

  GIVEN( "Variation graph in GFA1" )
  {
    std::string basename_v1 = GENERATE(
        "/complex_v1.gfa", "/dfs_dag_v1.gfa", "/graph_simple_v1.gfa",
        "/tiny_v1.gfa", "/tiny_p1.gfa", "/dfs_cyclic_v1.gfa",
        "/hprc-v1.0-minigraph-chm13-subgraph_v1.gfa" );
    std::string other_basename = "/tiny_p2.gfa";
    graph_type other;
    gum::util::load( other, test_data_dir + other_basename, false );

    AND_GIVEN( "Input graph '" + basename_v1.substr( 1 ) + "'" )
    {
      graph_type graph;
      auto bname = basename_v1;

      WHEN( "Loaded a Dynamic SeqGraph from a file" )
      {
        gum::util::load( graph, test_data_dir + bname, gum::util::GFA1,
                         false );

        THEN( "The graph should be equal to itself" )
        {
          REQUIRE( compare_graph( graph, graph ) );
        }

        THEN( "The graph should not be equal to a different graph" )
        {
          REQUIRE( !compare_graph( graph, other ) );
        }

        AND_WHEN( "Serialised the graph to a GFA1 file" )
        {
          std::string fname = gum::util::get_tmpfile();
          gum::util::write( graph, fname, gum::util::GFA1, true );

          THEN( "The serialised graph should be equal to the original" )
          {
            graph_type graph2;
            gum::util::load( graph2, fname, gum::util::GFA1, false );
            REQUIRE( compare_graph( graph, graph2 ) );
          }
        }
      }
    }
  }

  GIVEN( "Variation graph in GFA2" )
  {
    std::string basename_v2 = GENERATE(
        "/complex_v2.gfa", "/dfs_dag_v2.gfa", "/graph_simple_v2.gfa",
        "/tiny_v2.gfa", "/tiny_p2.gfa", "/dfs_cyclic_v2.gfa",
        "/hprc-v1.0-minigraph-chm13-subgraph_v2.gfa" );
    std::string other_basename = "/tiny_p1.gfa";
    graph_type other;
    gum::util::load( other, test_data_dir + other_basename, false );

    AND_GIVEN( "Input graph '" + basename_v2.substr( 1 ) + "'" )
    {
      graph_type graph;
      auto bname = basename_v2;

      WHEN( "Loaded a Dynamic SeqGraph from a file" )
      {
        gum::util::load( graph, test_data_dir + bname, gum::util::GFA2,
                         false );

        THEN( "The graph should be equal to itself" )
        {
          REQUIRE( compare_graph( graph, graph ) );
        }

        THEN( "The graph should not be equal to a different graph" )
        {
          REQUIRE( !compare_graph( graph, other ) );
        }

        AND_WHEN( "Serialised the graph to a GFA2 file" )
        {
          std::string fname = gum::util::get_tmpfile();
          gum::util::write( graph, fname, gum::util::GFA2, true );

          THEN( "The serialised graph should be equal to the original" )
          {
            graph_type graph2;
            gum::util::load( graph2, fname, gum::util::GFA2, false );
            REQUIRE( compare_graph( graph, graph2 ) );
          }
        }
      }
    }
  }
}

SCENARIO( "Sanity check for deserialising a SeqGraph", "[ioutils]" )
{
  GIVEN( "The 'tiny' variation graph" )
  {
    using graph_type = gum::SeqGraph< gum::Dynamic >;
    using succinct_type = typename graph_type::succinct_type;

    graph_type graph;
    auto integrity_test =
        []( auto const& graph, bool check_name=true, bool check_overlap=true ) {
          using graph_type = std::decay_t< decltype( graph ) >;
          using id_type = typename graph_type::id_type;
          using rank_type = typename graph_type::rank_type;
          using side_type = typename graph_type::side_type;
          using link_type = typename graph_type::link_type;
          using lid_type = typename graph_type::coordinate_type::lid_type;
          using linktype_type = typename graph_type::linktype_type;
          using nodes_type = std::vector< std::pair< id_type, bool > >;
          using pathset_type = std::unordered_map< std::string, nodes_type >;

          auto ibyc =
            [&graph]( lid_type cid ) -> id_type {
              return graph.id_by_coordinate( cid );
            };

          id_type abs_id = 999;
          rank_type node_count = 15;
          std::vector< link_type > edges = {
            { 1, true, 2, false },
            { 1, true, 3, false },
            { 2, true, 4, false },
            { 2, true, 5, false },
            { 3, true, 4, false },
            { 3, true, 5, false },
            { 4, true, 6, false },
            { 5, true, 6, false },
            { 6, true, 7, false },
            { 6, true, 8, false },
            { 7, true, 9, false },
            { 8, true, 9, false },
            { 9, true, 10, false },
            { 9, true, 11, false },
            { 10, true, 12, false },
            { 11, true, 12, false },
            { 12, true, 13, false },
            { 12, true, 14, false },
            { 13, true, 15, false },
            { 14, true, 15, false }
          };
          rank_type path_count = 1;
          std::string names[1] = { "x" };
          pathset_type pathset =
              { { "x", { { 1, false }, { 3, false }, { 5, false }, { 6, false },
                         { 8, false }, { 9, false }, { 11, false }, { 12, false },
                         { 14, false }, { 15, false } } } };

          REQUIRE( graph.get_node_count() == node_count );
          REQUIRE( graph.get_edge_count() == edges.size() );

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
          REQUIRE( !graph.has_node( abs_id ) );

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
          for ( auto const& e: edges ) {
            link_type edge = graph.make_link( ibyc( graph.from_id(e) ),
                                              ibyc( graph.to_id(e) ),
                                              graph.linktype( e ) );
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
          auto adjs = graph.adjacents_out( { ibyc(1), true } );
          truth = { { ibyc(2), false }, { ibyc(3), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(1), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(1), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(1), false } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_out( { ibyc(2), true } );
          truth = { { ibyc(4), false }, { ibyc(5), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(2), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(2), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(3), true } );
          truth = { { ibyc(4), false }, { ibyc(5), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(3), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(3), to_id_truth_check );
          REQUIRE( graph.is_branch( ibyc(2) ) );
          REQUIRE( !graph.is_merge( ibyc(2) ) );
          adjs = graph.adjacents_out( { ibyc(4), true } );
          truth = { { ibyc(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(4), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(4), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(5), true } );
          truth = { { ibyc(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(5), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(5), to_id_truth_check );
          REQUIRE( !graph.is_branch( ibyc(5) ) );
          REQUIRE( graph.is_merge( ibyc(5) ) );
          adjs = graph.adjacents_out( { ibyc(6), true } );
          truth = { { ibyc(7), false }, { ibyc(8), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(6), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(6), to_id_truth_check );
          REQUIRE( graph.is_branch( ibyc(6) ) );
          REQUIRE( graph.is_merge( ibyc(6) ) );
          adjs = graph.adjacents_out( { ibyc(12), true } );
          truth = { { ibyc(13), false }, { ibyc(14), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(12), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(12), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(13), true } );
          truth = { { ibyc(15), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(13), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(13), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(14), true } );
          truth = { { ibyc(15), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { ibyc(14), true }, side_truth_check );
          graph.for_each_edges_out(  ibyc(14), to_id_truth_check );
          adjs = graph.adjacents_out( { ibyc(15), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { ibyc(1), false } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { ibyc(1), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { ibyc(2), false } );
          truth = { { ibyc(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(2), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(2), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(3), false } );
          truth = { { ibyc(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(3), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(3), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(4), false } );
          truth = { { ibyc(2), true }, { ibyc(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(4), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(4), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(5), false } );
          truth = { { ibyc(2), true }, { ibyc(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(5), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(5), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(5), false } );
          truth = { { ibyc(2), true }, { ibyc(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(5), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(5), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(6), false } );
          truth = { { ibyc(4), true }, { ibyc(5), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(6), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(6), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(7), false } );
          truth = { { ibyc(6), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(7), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(7), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(8), false } );
          truth = { { ibyc(6), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(8), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(8), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(9), false } );
          truth = { { ibyc(7), true }, { ibyc(8), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(9), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(9), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(10), false } );
          truth = { { ibyc(9), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(10), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(10), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(11), false } );
          truth = { { ibyc(9), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(11), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(11), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(12), false } );
          truth = { { ibyc(10), true }, { ibyc(11), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(12), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(12), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(13), false } );
          truth = { { ibyc(12), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(13), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(13), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(14), false } );
          truth = { { ibyc(12), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(14), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(14), from_id_truth_check );
          adjs = graph.adjacents_in( { ibyc(15), false } );
          truth = { { ibyc(13), true }, { ibyc(14), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { ibyc(15), false }, side_truth_check );
          graph.for_each_edges_in(  ibyc(15), from_id_truth_check );
          REQUIRE( graph.outdegree( ibyc(1) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(2) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(3) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(4) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(5) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(6) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(7) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(8) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(9) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(10) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(11) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(12) ) == 2 );
          REQUIRE( graph.outdegree( ibyc(13) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(14) ) == 1 );
          REQUIRE( graph.outdegree( ibyc(15) ) == 0 );
          REQUIRE( graph.indegree( ibyc(1) ) == 0 );
          REQUIRE( graph.indegree( ibyc(2) ) == 1 );
          REQUIRE( graph.indegree( ibyc(3) ) == 1 );
          REQUIRE( graph.indegree( ibyc(4) ) == 2 );
          REQUIRE( graph.indegree( ibyc(5) ) == 2 );
          REQUIRE( graph.indegree( ibyc(6) ) == 2 );
          REQUIRE( graph.indegree( ibyc(7) ) == 1 );
          REQUIRE( graph.indegree( ibyc(8) ) == 1 );
          REQUIRE( graph.indegree( ibyc(9) ) == 2 );
          REQUIRE( graph.indegree( ibyc(10) ) == 1 );
          REQUIRE( graph.indegree( ibyc(11) ) == 1 );
          REQUIRE( graph.indegree( ibyc(12) ) == 2 );
          REQUIRE( graph.indegree( ibyc(13) ) == 1 );
          REQUIRE( graph.indegree( ibyc(14) ) == 1 );
          REQUIRE( graph.indegree( ibyc(15) ) == 2 );
          REQUIRE( !graph.has_edges_in( { ibyc(1), true } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(1), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(2), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(2), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(8), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(8), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(9), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(9), false } ) );
          REQUIRE( !graph.has_edges_in( { ibyc(15), true } ) );
          REQUIRE( graph.has_edges_in( { ibyc(15), false } ) );
          REQUIRE( !graph.has_edges_in( ibyc(1) ) );
          REQUIRE( graph.has_edges_in( ibyc(2) ) );
          REQUIRE( graph.has_edges_in( ibyc(8) ) );
          REQUIRE( graph.has_edges_in( ibyc(9) ) );
          REQUIRE( graph.has_edges_out( { ibyc(1), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(1), false } ) );
          REQUIRE( graph.has_edges_out( { ibyc(2), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(2), false } ) );
          REQUIRE( graph.has_edges_out( { ibyc(8), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(8), false } ) );
          REQUIRE( graph.has_edges_out( { ibyc(9), true } ) );
          REQUIRE( !graph.has_edges_out( { ibyc(9), false } ) );
          REQUIRE( graph.has_edges_out( ibyc(1) ) );
          REQUIRE( graph.has_edges_out( ibyc(2) ) );
          REQUIRE( graph.has_edges_out( ibyc(8) ) );
          REQUIRE( graph.has_edges_out( ibyc(9) ) );
          REQUIRE( !graph.has_edges_out( ibyc(15) ) );
          REQUIRE( graph.node_sequence( ibyc(1) ) == "CAAATAAG" );
          REQUIRE( graph.node_sequence( ibyc(2) ) == "A" );
          REQUIRE( graph.node_sequence( ibyc(3) ) == "G" );
          REQUIRE( graph.node_sequence( ibyc(4) ) == "T" );
          REQUIRE( graph.node_sequence( ibyc(5) ) == "C" );
          REQUIRE( graph.node_sequence( ibyc(6) ) == "TTG" );
          REQUIRE( graph.node_sequence( ibyc(7) ) == "A" );
          REQUIRE( graph.node_sequence( ibyc(8) ) == "G" );
          REQUIRE( graph.node_sequence( ibyc(9) ) == "AAATTTTCTGGAGTTCTAT" );
          REQUIRE( graph.node_sequence( ibyc(10) ) == "A" );
          REQUIRE( graph.node_sequence( ibyc(11) ) == "T" );
          REQUIRE( graph.node_sequence( ibyc(12) ) == "ATAT" );
          REQUIRE( graph.node_sequence( ibyc(13) ) == "A" );
          REQUIRE( graph.node_sequence( ibyc(14) ) == "T" );
          REQUIRE( graph.node_sequence( ibyc(15) ) == "CCAACTCTCTG" );
          REQUIRE( graph.node_length( ibyc(1) ) == 8 );
          REQUIRE( graph.node_length( ibyc(2) ) == 1 );
          REQUIRE( graph.node_length( ibyc(3) ) == 1 );
          REQUIRE( graph.node_length( ibyc(4) ) == 1 );
          REQUIRE( graph.node_length( ibyc(5) ) == 1 );
          REQUIRE( graph.node_length( ibyc(6) ) == 3 );
          REQUIRE( graph.node_length( ibyc(7) ) == 1 );
          REQUIRE( graph.node_length( ibyc(8) ) == 1 );
          REQUIRE( graph.node_length( ibyc(9) ) == 19 );
          REQUIRE( graph.node_length( ibyc(10) ) == 1 );
          REQUIRE( graph.node_length( ibyc(11) ) == 1 );
          REQUIRE( graph.node_length( ibyc(12) ) == 4 );
          REQUIRE( graph.node_length( ibyc(13) ) == 1 );
          REQUIRE( graph.node_length( ibyc(14) ) == 1 );
          REQUIRE( graph.node_length( ibyc(15) ) == 11 );
          if ( check_name ) {
            REQUIRE( graph.get_node_prop( 1 ).name == "1" );
            REQUIRE( graph.get_node_prop( 2 ).name == "2" );
            REQUIRE( graph.get_node_prop( 3 ).name == "3" );
            REQUIRE( graph.get_node_prop( 4 ).name == "4" );
            REQUIRE( graph.get_node_prop( 5 ).name == "5" );
            REQUIRE( graph.get_node_prop( 6 ).name == "6" );
            REQUIRE( graph.get_node_prop( 7 ).name == "7" );
            REQUIRE( graph.get_node_prop( 8 ).name == "8" );
            REQUIRE( graph.get_node_prop( 9 ).name == "9" );
            REQUIRE( graph.get_node_prop( 10 ).name == "10" );
            REQUIRE( graph.get_node_prop( 11 ).name == "11" );
            REQUIRE( graph.get_node_prop( 12 ).name == "12" );
            REQUIRE( graph.get_node_prop( 13 ).name == "13" );
            REQUIRE( graph.get_node_prop( 14 ).name == "14" );
            REQUIRE( graph.get_node_prop( 15 ).name == "15" );
          }
          if ( check_overlap ) {
            REQUIRE( graph.edge_overlap( link_type( { ibyc(1), true, ibyc(2), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(1), true, ibyc(3), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(2), true, ibyc(4), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(2), true, ibyc(5), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(3), true, ibyc(4), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(3), true, ibyc(5), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(4), true, ibyc(6), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(5), true, ibyc(6), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(6), true, ibyc(7), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(6), true, ibyc(8), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(7), true, ibyc(9), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(8), true, ibyc(9), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(9), true, ibyc(10), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(9), true, ibyc(11), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(10), true, ibyc(12), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(11), true, ibyc(12), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(12), true, ibyc(13), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(12), true, ibyc(14), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(13), true, ibyc(15), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { ibyc(14), true, ibyc(15), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(1), ibyc(2), graph.linktype( link_type( { ibyc(1), true, ibyc(2), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(1), ibyc(3), graph.linktype( link_type( { ibyc(1), true, ibyc(3), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(2), ibyc(4), graph.linktype( link_type( { ibyc(2), true, ibyc(4), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(2), ibyc(5), graph.linktype( link_type( { ibyc(2), true, ibyc(5), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(3), ibyc(4), graph.linktype( link_type( { ibyc(3), true, ibyc(4), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(3), ibyc(5), graph.linktype( link_type( { ibyc(3), true, ibyc(5), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(4), ibyc(6), graph.linktype( link_type( { ibyc(4), true, ibyc(6), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(5), ibyc(6), graph.linktype( link_type( { ibyc(5), true, ibyc(6), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(6), ibyc(7), graph.linktype( link_type( { ibyc(6), true, ibyc(7), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(6), ibyc(8), graph.linktype( link_type( { ibyc(6), true, ibyc(8), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(7), ibyc(9), graph.linktype( link_type( { ibyc(7), true, ibyc(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(8), ibyc(9), graph.linktype( link_type( { ibyc(8), true, ibyc(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(8), ibyc(9), graph.linktype( link_type( { ibyc(8), true, ibyc(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(9), ibyc(10), graph.linktype( link_type( { ibyc(9), true, ibyc(10), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(9), ibyc(11), graph.linktype( link_type( { ibyc(9), true, ibyc(11), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(10), ibyc(12), graph.linktype( link_type( { ibyc(10), true, ibyc(12), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(11), ibyc(12), graph.linktype( link_type( { ibyc(11), true, ibyc(12), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(12), ibyc(13), graph.linktype( link_type( { ibyc(12), true, ibyc(13), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(12), ibyc(14), graph.linktype( link_type( { ibyc(12), true, ibyc(14), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(13), ibyc(15), graph.linktype( link_type( { ibyc(13), true, ibyc(15), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( ibyc(14), ibyc(15), graph.linktype( link_type( { ibyc(14), true, ibyc(15), false } ) ) ) == 0 );
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

    WHEN( "Loaded a Dynamic SeqGraph from a file in GFA 2.0 format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny_v2.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph );
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in GFA 2.0 format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/tiny_v2.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
      }
    }

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg/Protobuf format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny.pb.vg", gum::util::VGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph, false, false );
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in vg/Protobuf format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/tiny.pb.vg", gum::util::VGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph, false, false );
      }
    }

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg/HashGraph format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny.hg.vg", gum::util::HGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph, false, false );
        }
      }
    }

    WHEN( "Loaded a Succinct SeqGraph from a file in vg/HashGraph format" )
    {
      succinct_type sc_graph;
      gum::util::load( sc_graph, test_data_dir + "/tiny.hg.vg", gum::util::HGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph, false, false );
      }
    }

    WHEN( "Extend a Dynamic SeqGraph by another graph in GFA 2.0 format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny_p1.gfa", true );
      gum::util::extend( graph, test_data_dir + "/tiny_p2.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph );
      }
    }

    WHEN( "Extend a Dynamic SeqGraph by another graph in vg/Protobuf format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny_p1.pb.vg", gum::util::VGFormat(), true );
      gum::util::extend( graph, test_data_dir + "/tiny_p2.pb.vg", gum::util::VGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }
    }

    WHEN( "Extend a Dynamic SeqGraph by another graph in vg/HashGraph format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny_p1.hg.vg", gum::util::HGFormat(), true );
      gum::util::extend( graph, test_data_dir + "/tiny_p2.hg.vg", gum::util::HGFormat(), true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }
    }

    WHEN( "Load a Dynamic SeqGraph from a vg file without passing the format tag" )
    {
#ifdef GUM_IO_PROTOBUF_VG
      gum::util::load( graph, test_data_dir + "/tiny.pb.vg", true );
#else
      gum::util::load( graph, test_data_dir + "/tiny.hg.vg", true );
#endif

      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }

      WHEN( "A Succinct graph is constructed from Dynamic one" )
      {
        succinct_type sc_graph( graph );
        THEN( "The resulting graph should pass integrity tests" )
        {
          integrity_test( sc_graph, false, false );
        }
      }
    }
  }
}
