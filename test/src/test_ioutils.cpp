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
          using linktype_type = typename graph_type::linktype_type;
          using nodes_type = std::vector< std::pair< id_type, bool > >;
          using pathset_type = std::unordered_map< std::string, nodes_type >;

          auto rtoi =
              [&graph]( rank_type rank ) {
                return graph.rank_to_id( rank );
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
          for ( auto const& e: edges ) {
            link_type edge = graph.make_link( rtoi( graph.from_id(e) ),
                                              rtoi( graph.to_id(e) ),
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
          auto adjs = graph.adjacents_out( { rtoi(1), true } );
          truth = { { rtoi(2), false }, { rtoi(3), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(1), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(1), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(1), false } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_out( { rtoi(2), true } );
          truth = { { rtoi(4), false }, { rtoi(5), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(2), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(2), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(3), true } );
          truth = { { rtoi(4), false }, { rtoi(5), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(3), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(3), to_id_truth_check );
          REQUIRE( graph.is_branch( rtoi(2) ) );
          REQUIRE( !graph.is_merge( rtoi(2) ) );
          adjs = graph.adjacents_out( { rtoi(4), true } );
          truth = { { rtoi(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(4), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(4), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(5), true } );
          truth = { { rtoi(6), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(5), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(5), to_id_truth_check );
          REQUIRE( !graph.is_branch( rtoi(5) ) );
          REQUIRE( graph.is_merge( rtoi(5) ) );
          adjs = graph.adjacents_out( { rtoi(6), true } );
          truth = { { rtoi(7), false }, { rtoi(8), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(6), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(6), to_id_truth_check );
          REQUIRE( graph.is_branch( rtoi(6) ) );
          REQUIRE( graph.is_merge( rtoi(6) ) );
          adjs = graph.adjacents_out( { rtoi(12), true } );
          truth = { { rtoi(13), false }, { rtoi(14), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(12), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(12), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(13), true } );
          truth = { { rtoi(15), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(13), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(13), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(14), true } );
          truth = { { rtoi(15), false } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_out( { rtoi(14), true }, side_truth_check );
          graph.for_each_edges_out(  rtoi(14), to_id_truth_check );
          adjs = graph.adjacents_out( { rtoi(15), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { rtoi(1), false } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { rtoi(1), true } );
          REQUIRE( adjs.empty() );
          adjs = graph.adjacents_in( { rtoi(2), false } );
          truth = { { rtoi(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(2), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(2), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(3), false } );
          truth = { { rtoi(1), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(3), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(3), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(4), false } );
          truth = { { rtoi(2), true }, { rtoi(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(4), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(4), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(5), false } );
          truth = { { rtoi(2), true }, { rtoi(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(5), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(5), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(5), false } );
          truth = { { rtoi(2), true }, { rtoi(3), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(5), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(5), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(6), false } );
          truth = { { rtoi(4), true }, { rtoi(5), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(6), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(6), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(7), false } );
          truth = { { rtoi(6), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(7), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(7), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(8), false } );
          truth = { { rtoi(6), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(8), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(8), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(9), false } );
          truth = { { rtoi(7), true }, { rtoi(8), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(9), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(9), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(10), false } );
          truth = { { rtoi(9), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(10), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(10), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(11), false } );
          truth = { { rtoi(9), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(11), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(11), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(12), false } );
          truth = { { rtoi(10), true }, { rtoi(11), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(12), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(12), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(13), false } );
          truth = { { rtoi(12), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(13), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(13), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(14), false } );
          truth = { { rtoi(12), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(14), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(14), from_id_truth_check );
          adjs = graph.adjacents_in( { rtoi(15), false } );
          truth = { { rtoi(13), true }, { rtoi(14), true } };
          REQUIRE( adjs.size() == truth.size() );
          for ( auto const& side : truth ) {
            REQUIRE( std::find( adjs.begin(), adjs.end(), side ) != adjs.end() );
          }
          graph.for_each_edges_in( { rtoi(15), false }, side_truth_check );
          graph.for_each_edges_in(  rtoi(15), from_id_truth_check );
          REQUIRE( graph.outdegree( rtoi(1) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(2) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(3) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(4) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(5) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(6) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(7) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(8) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(9) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(10) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(11) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(12) ) == 2 );
          REQUIRE( graph.outdegree( rtoi(13) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(14) ) == 1 );
          REQUIRE( graph.outdegree( rtoi(15) ) == 0 );
          REQUIRE( graph.indegree( rtoi(1) ) == 0 );
          REQUIRE( graph.indegree( rtoi(2) ) == 1 );
          REQUIRE( graph.indegree( rtoi(3) ) == 1 );
          REQUIRE( graph.indegree( rtoi(4) ) == 2 );
          REQUIRE( graph.indegree( rtoi(5) ) == 2 );
          REQUIRE( graph.indegree( rtoi(6) ) == 2 );
          REQUIRE( graph.indegree( rtoi(7) ) == 1 );
          REQUIRE( graph.indegree( rtoi(8) ) == 1 );
          REQUIRE( graph.indegree( rtoi(9) ) == 2 );
          REQUIRE( graph.indegree( rtoi(10) ) == 1 );
          REQUIRE( graph.indegree( rtoi(11) ) == 1 );
          REQUIRE( graph.indegree( rtoi(12) ) == 2 );
          REQUIRE( graph.indegree( rtoi(13) ) == 1 );
          REQUIRE( graph.indegree( rtoi(14) ) == 1 );
          REQUIRE( graph.indegree( rtoi(15) ) == 2 );
          REQUIRE( !graph.has_edges_in( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(1), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(2), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(2), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(8), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(8), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(9), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(9), false } ) );
          REQUIRE( !graph.has_edges_in( { rtoi(15), true } ) );
          REQUIRE( graph.has_edges_in( { rtoi(15), false } ) );
          REQUIRE( !graph.has_edges_in( rtoi(1) ) );
          REQUIRE( graph.has_edges_in( rtoi(2) ) );
          REQUIRE( graph.has_edges_in( rtoi(8) ) );
          REQUIRE( graph.has_edges_in( rtoi(9) ) );
          REQUIRE( graph.has_edges_out( { rtoi(1), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(1), false } ) );
          REQUIRE( graph.has_edges_out( { rtoi(2), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(2), false } ) );
          REQUIRE( graph.has_edges_out( { rtoi(8), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(8), false } ) );
          REQUIRE( graph.has_edges_out( { rtoi(9), true } ) );
          REQUIRE( !graph.has_edges_out( { rtoi(9), false } ) );
          REQUIRE( graph.has_edges_out( rtoi(1) ) );
          REQUIRE( graph.has_edges_out( rtoi(2) ) );
          REQUIRE( graph.has_edges_out( rtoi(8) ) );
          REQUIRE( graph.has_edges_out( rtoi(9) ) );
          REQUIRE( !graph.has_edges_out( rtoi(15) ) );
          REQUIRE( graph.node_sequence( rtoi(1) ) == "CAAATAAG" );
          REQUIRE( graph.node_sequence( rtoi(2) ) == "A" );
          REQUIRE( graph.node_sequence( rtoi(3) ) == "G" );
          REQUIRE( graph.node_sequence( rtoi(4) ) == "T" );
          REQUIRE( graph.node_sequence( rtoi(5) ) == "C" );
          REQUIRE( graph.node_sequence( rtoi(6) ) == "TTG" );
          REQUIRE( graph.node_sequence( rtoi(7) ) == "A" );
          REQUIRE( graph.node_sequence( rtoi(8) ) == "G" );
          REQUIRE( graph.node_sequence( rtoi(9) ) == "AAATTTTCTGGAGTTCTAT" );
          REQUIRE( graph.node_sequence( rtoi(10) ) == "A" );
          REQUIRE( graph.node_sequence( rtoi(11) ) == "T" );
          REQUIRE( graph.node_sequence( rtoi(12) ) == "ATAT" );
          REQUIRE( graph.node_sequence( rtoi(13) ) == "A" );
          REQUIRE( graph.node_sequence( rtoi(14) ) == "T" );
          REQUIRE( graph.node_sequence( rtoi(15) ) == "CCAACTCTCTG" );
          REQUIRE( graph.node_length( rtoi(1) ) == 8 );
          REQUIRE( graph.node_length( rtoi(2) ) == 1 );
          REQUIRE( graph.node_length( rtoi(3) ) == 1 );
          REQUIRE( graph.node_length( rtoi(4) ) == 1 );
          REQUIRE( graph.node_length( rtoi(5) ) == 1 );
          REQUIRE( graph.node_length( rtoi(6) ) == 3 );
          REQUIRE( graph.node_length( rtoi(7) ) == 1 );
          REQUIRE( graph.node_length( rtoi(8) ) == 1 );
          REQUIRE( graph.node_length( rtoi(9) ) == 19 );
          REQUIRE( graph.node_length( rtoi(10) ) == 1 );
          REQUIRE( graph.node_length( rtoi(11) ) == 1 );
          REQUIRE( graph.node_length( rtoi(12) ) == 4 );
          REQUIRE( graph.node_length( rtoi(13) ) == 1 );
          REQUIRE( graph.node_length( rtoi(14) ) == 1 );
          REQUIRE( graph.node_length( rtoi(15) ) == 11 );
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
            REQUIRE( graph.edge_overlap( link_type( { rtoi(1), true, rtoi(2), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(1), true, rtoi(3), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(2), true, rtoi(4), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(2), true, rtoi(5), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(3), true, rtoi(4), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(3), true, rtoi(5), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(4), true, rtoi(6), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(5), true, rtoi(6), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(6), true, rtoi(7), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(6), true, rtoi(8), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(7), true, rtoi(9), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(8), true, rtoi(9), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(9), true, rtoi(10), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(9), true, rtoi(11), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(10), true, rtoi(12), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(11), true, rtoi(12), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(12), true, rtoi(13), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(12), true, rtoi(14), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(13), true, rtoi(15), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( link_type( { rtoi(14), true, rtoi(15), false } ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(1), rtoi(2), graph.linktype( link_type( { rtoi(1), true, rtoi(2), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(1), rtoi(3), graph.linktype( link_type( { rtoi(1), true, rtoi(3), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(2), rtoi(4), graph.linktype( link_type( { rtoi(2), true, rtoi(4), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(2), rtoi(5), graph.linktype( link_type( { rtoi(2), true, rtoi(5), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(3), rtoi(4), graph.linktype( link_type( { rtoi(3), true, rtoi(4), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(3), rtoi(5), graph.linktype( link_type( { rtoi(3), true, rtoi(5), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(4), rtoi(6), graph.linktype( link_type( { rtoi(4), true, rtoi(6), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(5), rtoi(6), graph.linktype( link_type( { rtoi(5), true, rtoi(6), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(6), rtoi(7), graph.linktype( link_type( { rtoi(6), true, rtoi(7), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(6), rtoi(8), graph.linktype( link_type( { rtoi(6), true, rtoi(8), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(7), rtoi(9), graph.linktype( link_type( { rtoi(7), true, rtoi(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(8), rtoi(9), graph.linktype( link_type( { rtoi(8), true, rtoi(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(8), rtoi(9), graph.linktype( link_type( { rtoi(8), true, rtoi(9), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(9), rtoi(10), graph.linktype( link_type( { rtoi(9), true, rtoi(10), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(9), rtoi(11), graph.linktype( link_type( { rtoi(9), true, rtoi(11), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(10), rtoi(12), graph.linktype( link_type( { rtoi(10), true, rtoi(12), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(11), rtoi(12), graph.linktype( link_type( { rtoi(11), true, rtoi(12), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(12), rtoi(13), graph.linktype( link_type( { rtoi(12), true, rtoi(13), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(12), rtoi(14), graph.linktype( link_type( { rtoi(12), true, rtoi(14), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(13), rtoi(15), graph.linktype( link_type( { rtoi(13), true, rtoi(15), false } ) ) ) == 0 );
            REQUIRE( graph.edge_overlap( rtoi(14), rtoi(15), graph.linktype( link_type( { rtoi(14), true, rtoi(15), false } ) ) ) == 0 );
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
      gum::util::load( graph, test_data_dir + "/tiny.gfa", true );
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
      gum::util::load( sc_graph, test_data_dir + "/tiny.gfa", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( sc_graph );
      }
    }

    WHEN( "Loaded a Dynamic SeqGraph from a file in vg/Protobuf format" )
    {
      gum::util::load( graph, test_data_dir + "/tiny.pb.vg", true );
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
      gum::util::load( sc_graph, test_data_dir + "/tiny.pb.vg", true );
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
      gum::util::load( graph, test_data_dir + "/tiny_p1.pb.vg", true );
      gum::util::extend( graph, test_data_dir + "/tiny_p2.pb.vg", true );
      THEN( "The resulting graph should pass integrity tests" )
      {
        integrity_test( graph, false, false );
      }
    }
  }
}
