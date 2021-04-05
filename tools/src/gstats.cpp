/**
 *    @file  gstats.cpp
 *   @brief  Report some statistics of input sequence graphs.
 *
 *  This auxiliary tool computes some basic statistics for given sequence graphs mainly
 *  used for checking if parsing external files with different formats works.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Sun Apr 04, 2021  16:05
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#include <cstdlib>
#include <iostream>
#include <string>

#include <cxxopts.hpp>
#include <gum/graph.hpp>
#include <gum/io_utils.hpp>
#include <gum/utils.hpp>


using namespace gum;

/* ====== Constants ====== */
constexpr const char* const LONG_DESC = "Report some basic statistics of input sequence graphs";

void
config_parser( cxxopts::Options& options )
{
  options.positional_help( "GRAPH" );
  options.add_options()
      ( "f, format", "input file format (gfa, vg, hg)", cxxopts::value< std::string >()->default_value( "" ) )
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
  if ( !util::readable( result[ "graph" ].as< std::string >() ) ) {
    throw cxxopts::OptionParseException( "Graph file not found" );
  }

  return result;
}

  int
main( int argc, char* argv[] )
{
  cxxopts::Options options( argv[0], LONG_DESC );
  config_parser( options );

  try {
    auto res = parse_opts( options, argc, argv );

    std::string graph_path = res[ "graph" ].as< std::string >();
    std::string format = res[ "format" ].as< std::string >();
    SeqGraph< Succinct > graph;

    if ( format == "gfa" ) {
      util::load_gfa( graph, graph_path, true );
    }
    else if ( format == "vg" ) {
      util::load_vg( graph, graph_path, true );
    }
    else if ( format == "hg" ) {
      util::load_hg( graph, graph_path, true );
    }
    else if ( format == "" ) {
      util::load( graph, graph_path, true );
    }
    else throw std::runtime_error( "unknown file format '" + format + "'" );

    std::string sort_status = util::ids_in_topological_order( graph ) ? "" : "not ";
    std::cout << "Input graph node IDs are " << sort_status << "in topological sort order."
              << std::endl;

    std::cout << "Number of nodes: " << graph.get_node_count() << std::endl;
    std::cout << "Number of edges: " << graph.get_edge_count() << std::endl;
    std::cout << "Number of paths: " << graph.get_path_count() << std::endl;
    std::cout << "Total number of characters: " << util::total_nof_loci( graph ) << std::endl;
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
