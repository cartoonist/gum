/**
 *    @file  io_utils.hpp
 *   @brief  Utility functions and data types for IO.
 *
 *  This header file is a part of the utility header files specialised for working with
 *  files and input streams of any file format.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Aug 20, 2019  15:44
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_IO_UTILS_HPP__
#define  GUM_IO_UTILS_HPP__

#include "vg_utils.hpp"
#include "gfa_utils.hpp"


namespace gum {
  namespace util {
    /**
     *  @brief  Extend a Dynamic SeqGraph from a file.
     *
     *  It extends the given `Dynamic` graph by the graph defined in the input
     *  file. The file format of the input file will be determined by the file
     *  extension.
     *
     *  @param  graph The `Dynamic` graph.
     *  @param  fname The input file path.
     *  @param  args The parameters forwarded to lower-level functions (see
     *               `gum::util::extend_gfa` or `gum::util::extend_vg`).
     */
    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
            std::string fname,
            TArgs&&... args )
    {
      if ( util::ends_with( fname, VGFormat::FILE_EXTENSION ) ) {
        extend_vg( graph, fname, std::forward< TArgs >( args )... );
      }
      else if ( util::ends_with( fname, GFAFormat::FILE_EXTENSION ) ) {
        extend_gfa( graph, fname, std::forward< TArgs >( args )... );
      }
      else throw std::runtime_error( "unsupported input file format" );
    }

    /**
     *  @brief  Load a Dynamic SeqGraph from a file.
     *
     *  It constructs the given `Dynamic` graph from the input file. The file
     *  format of the input file will be determined by the file extension.
     *
     *  @param  graph The `Dynamic` graph.
     *  @param  fname The input file path.
     *  @param  args The parameters forwarded to lower-level functions (see
     *               `gum::util::load_gfa` or `gum::util::load_vg`).
     */
    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    load( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
          std::string fname,
          TArgs&&... args )
    {
      if ( util::ends_with( fname, VGFormat::FILE_EXTENSION ) ) {
        load_vg( graph, fname, std::forward< TArgs >( args )... );
      }
      else if ( util::ends_with( fname, GFAFormat::FILE_EXTENSION ) ) {
        load_gfa( graph, fname, std::forward< TArgs >( args )... );
      }
      else throw std::runtime_error( "unsupported input file format" );
    }

    /**
     *  @brief  Load a Succinct SeqGraph from a file.
     *
     *  It constructs the given `Succinct` graph from the input file. The file
     *  format of the input file will be determined by the file extension.
     *
     *  NOTE: The `Succinct` specialisation of `SeqGraph` cannot be extended as
     *        it is immutable.
     *
     *  @param  graph The `Succinct` graph.
     *  @param  fname The input file path.
     *  @param  args The parameters forwarded to lower-level functions (see
     *               `gum::util::load_gfa` or `gum::util::load_vg`).
     */
    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    load( SeqGraph< Succinct, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
          std::string fname,
          TArgs&&... args )
    {
      using graph_type = std::decay_t< decltype( graph ) >;
      typename graph_type::dynamic_type dyn_graph;
      extend( dyn_graph, fname, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_IO_UTILS_HPP__ --- */
