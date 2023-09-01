/**
 *    @file  gbenchmark.cpp
 *   @brief  GUM benchmarking tool
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Thu Aug 24, 2023  13:42
 *  Organization:  Universität Bielefeld
 *     Copyright:  Copyright (c) 2023, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <__config>
#include <cstdlib>
#include <ios>
#include <iostream>

#include <cxxopts.hpp>
#include <gum/graph.hpp>
#include <gum/io_utils.hpp>
#include <gum/timer.hpp>


/* ====== Constants ====== */
constexpr const char* const LONG_DESC = "GUM benchmarking tool";

void
config_parser( cxxopts::Options& options )
{
  options.positional_help( "GRAPH" );
  options.add_options()
      ( "i, interactive", "Wait for user confirmation after each step" )
      ( "f, format", "Input file format (gfa, vg, hg)", cxxopts::value< std::string >()->default_value( "" ) )
      ( "h, help", "Print this message and exit" )
      ;

  options.add_options( "positional" )
      ( "graph", "input graph", cxxopts::value< std::string >() )
      ;
  options.parse_positional( { "graph" } );
}

cxxopts::ParseResult
parse_opts( cxxopts::Options& options, int& argc, char**& argv )
{
  auto result = options.parse( argc, argv );

  if ( result.count( "help" ) ) {
    std::cout << options.help( { "" } ) << std::endl;
    throw EXIT_SUCCESS;
  }

  if ( !result.count( "graph" ) ) {
    throw cxxopts::OptionParseException( "Graph must be specified" );
  }
  if ( !gum::util::readable( result[ "graph" ].as< std::string >() ) ) {
    throw cxxopts::OptionParseException( "Graph file not found" );
  }

  return result;
}

template< typename TGraph >
auto
compute_cg_count( TGraph const& graph )
{
  long long unsigned int cgc = 0;
  graph.for_each_node( [&]( auto rank, auto id ) {
    auto seq = graph.node_sequence( id );
    for ( auto const& c : seq ) {
      if ( c == 'C' || c == 'G' ) ++cgc;
    }
    return true;
  } );
  return cgc;
}

template< typename TGraph >
auto
compute_cg10_count( TGraph const& graph )
{
  long long unsigned int cgc10 = 0;
  graph.for_each_node( [&]( auto rank, auto id ) {
    auto const& seq = graph.node_sequence( id );
    std::size_t i = 0;
    for ( auto&& c : seq ) {
      if ( i++ == 10 ) break;
      if ( c == 'C' || c == 'G' ) ++cgc10;
    }
    return true;
  } );
  return cgc10;
}

int
main( int argc, char* argv[] )
{
  using timer_type = gum::Timer<>;

  cxxopts::Options options( argv[0], LONG_DESC );
  config_parser( options );

  try {
    auto res = parse_opts( options, argc, argv );

    std::string graph_path = res[ "graph" ].as< std::string >();
    std::string format = res[ "format" ].as< std::string >();
    bool interactive = res[ "interactive" ].as< bool >();

    std::cout << "Graph file: " << graph_path << std::endl;

    if (interactive) {
      std::cout << "Running in interactive mode..." << std::endl;
      std::cout << "Press the Enter key to continue." << std::endl;
      std::cin.ignore();
    }

    gum::SeqGraph< gum::Succinct > graph;

    {
      gum::SeqGraph< gum::Dynamic > d_graph;

      std::cout << "Loading input file into a Dynamic graph..." << std::endl;
      {
        auto timer = timer_type( "load-dynamic" );

        if ( format == "gfa" ) {
          gum::util::load_gfa( d_graph, graph_path, true );
        }
        else if ( format == "vg" ) {
          gum::util::load_vg( d_graph, graph_path, true );
        }
        else if ( format == "hg" ) {
          gum::util::load_hg( d_graph, graph_path, true );
        }
        else if ( format == "" ) {
          gum::util::load( d_graph, graph_path, true );
        }
        else throw std::runtime_error( "unknown file format '" + format + "'" );
      }

      if ( interactive ) {
        std::cout << "Press the Enter key to continue." << std::endl;
        std::cin.ignore();
      }

      std::cout << "Building a Succinct graph from the Dynamic one..." << std::endl;
      {
        auto timer = timer_type( "build-succinct" );
        graph = d_graph;
      }

      if ( interactive ) {
        std::cout << "Press the Enter key to continue." << std::endl;
        std::cin.ignore();
      }
    }

    std::cout << "Computing CG content..." << std::endl;
    auto total_chars = gum::util::total_nof_loci( graph );
    decltype(compute_cg_count(graph)) cgc = 0;
    {
      auto timer = timer_type( "compute-cgc" );
      cgc = compute_cg_count( graph );
    }
    std::cout << "CG Content: " << cgc / static_cast<double>(total_chars)
              << " (" << cgc << ")" << std::endl;

    if ( interactive ) {
     std::cout << "Press the Enter key to continue." << std::endl;
     std::cin.ignore();
    }

    std::cout << "Computing CG content of the first 10 bases..." << std::endl;
    decltype(compute_cg_count(graph)) cgc10 = 0;
    {
      auto timer = timer_type( "compute-cgc-10" );
      cgc10 = compute_cg10_count( graph );
    }
    std::cout << "CG10 Content: " << cgc10 / static_cast<double>(total_chars)
              << " (" << cgc10 << ")" << std::endl;

    if ( interactive ) {
     std::cout << "Press the Enter key to continue." << std::endl;
     std::cin.ignore();
    }

    std::cout << "Number of nodes: " << graph.get_node_count() << std::endl;
    std::cout << "Number of edges: " << graph.get_edge_count() << std::endl;
    std::cout << "Number of paths: " << graph.get_path_count() << std::endl;
    std::cout << "Total number of characters: " << total_chars << std::endl;

    std::cout << "All Timers" << std::endl;
    std::cout << "----------" << std::endl;
    for ( const auto& timer : timer_type::get_timers() ) {
      std::cout << timer.first << ": " << timer.second.str() << std::endl;
    }
  }
  catch ( const cxxopts::OptionException& e ) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch ( const int& rv ) {
    return rv;
  }

  return EXIT_SUCCESS;
}
