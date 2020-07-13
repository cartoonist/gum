/**
 *    @file  main.cpp
 *   @brief  Benchmark program.
 *
 *  This program runs a benchmark for the library.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Wed Nov 13, 2019  10:25
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

#include <gum/seqgraph.hpp>
#include <gum/io_utils.hpp>
//#include <xg/xg.hpp>


using namespace std::chrono_literals;

int
main( int argc, char* argv[] )
{
  if ( argc != 2 ) {
    std::cout << "Too few arguments. Expected 1 argument as input graph file path." << std::endl;
    return EXIT_FAILURE;
  }

  std::string graph_path = argv[1];

  clock_t t;
  /*
  xg::XG xgraph;

  auto for_each_sequence =
      [&dgraph]( auto callback ) {
        dgraph.for_each_node(
            [&]( auto rank, auto id ) {
              callback( dgraph.node_sequence( id ), id );
              return true;
            });
      };

  auto for_each_edge =
      [&dgraph]( auto callback ) {
        dgraph.for_each_node(
            [&]( auto rank, auto from ) {
              dgraph.for_each_edges_out(
                  from,
                  [&]( auto to, auto type ) {
                    callback( from, dgraph.is_from_start( type ),
                              to, dgraph.is_to_end( type ) );
                    return true;
                  });
              return true;
            });
      };

  auto for_each_path =
      [&dgraph]( auto callback ) {
      };
  */

  auto total_sequence_length =
      []( auto graph ) {
        unsigned long long int total_length = 0;
        graph.for_each_node(
            [&total_length, &graph]( auto rank, auto id ) {
              total_length += graph.node_length( id );
              return true;
            }
          );
        return total_length;
      };

  std::cout << "Loading the graph from file '" << graph_path << "'..." << std::endl;

  {
    t = clock();
    gum::SeqGraph< gum::Dynamic > dgraph;
    gum::util::load( dgraph, graph_path );
    auto d = static_cast< float >( clock() - t ) / CLOCKS_PER_SEC;
    std::cerr << "[dgraph/load] " << std::setprecision(3) << d << " sec" << std::endl;

    std::cout << "No. of nodes: " << dgraph.get_node_count() << std::endl;
    std::cout << "No. of edges: " << dgraph.get_edge_count() << std::endl;
    std::cout << "No. of paths: " << dgraph.get_path_count() << std::endl;
    t = clock();
    auto tlen = total_sequence_length( dgraph );
    d = static_cast< float >( clock() - t ) / CLOCKS_PER_SEC;
    std::cout << "Total sequence length: " << tlen << std::endl;
    std::cerr << "[dgraph/tlen] " << std::setprecision(3) << d << " sec" << std::endl;

    dgraph.shrink_to_fit();

    std::this_thread::sleep_for(25s);

    t = clock();
    gum::SeqGraph< gum::Succinct > sgraph( dgraph );
    d = static_cast< float >( clock() - t ) / CLOCKS_PER_SEC;
    std::cerr << "[sgraph/index] " << std::setprecision(3) << d << " sec" << std::endl;

    std::cout << "No. of nodes: " << sgraph.get_node_count() << std::endl;
    std::cout << "No. of edges: " << sgraph.get_edge_count() << std::endl;
    std::cout << "No. of paths: " << sgraph.get_path_count() << std::endl;
    t = clock();
    tlen = total_sequence_length( sgraph );
    d = static_cast< float >( clock() - t ) / CLOCKS_PER_SEC;
    std::cout << "Total sequence length: " << tlen << std::endl;
    std::cerr << "[sgraph/tlen] " << std::setprecision(3) << d << " sec" << std::endl;
  }

  //t = clock();
  //xgraph.from_enumerators( for_each_sequence, for_each_edge, for_each_path );
  //d = static_cast< float >( clock() - t ) / CLOCKS_PER_SEC;
  //std::cerr << "[xgraph/load] " << std::setprecision(3) << d << " sec" << std::endl;

  return EXIT_SUCCESS;
}
