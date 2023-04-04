/**
 *    @file  io_utils_hg.hpp
 *   @brief  SeqGraphs io-related template interface functions for loading HashGraph graphs.
 *
 *  This header file includes interface function definitions specialised for HashGraph data
 *  type when it comes with dealing with IO.
 *
 *  NOTE: Do not include this file directly; use `io_utils.hpp` or `bdsg_utils.hpp`.
 *
 *  NOTE: The interface functions are template functions parameterised by data types
 *  defined in `libbdsg` (as template parameters). No specific data type from `libbdsg`
 *  is explicitly used in this header file to prevent imposing unwanted dependencies
 *  when `HashGraph` format is not used.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Apr 4, 2023  19:07
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_IO_UTILS_HG_HPP__
#define GUM_IO_UTILS_HG_HPP__

#include <istream>

#include "hg_utils.hpp"


namespace gum {
  namespace util {
    /**
     *  @brief  Extend a native graph with an external one using an `ExternalLoader` (HashGraph overload).
     *
     *  @param  graph Graph of any native type with Dynamic spec tag
     *  @param  in Input stream
     *  @param  loader An instance of `ExternalLoader`
     *  @param  args Arguments passed to `extend_graph`
     *
     *  This overload uses parsing mechanism provided as input argument. Any calls to
     *  `load`/`extend`/`load_{gfa|vg|hg}`/`extend_{gfa|vg|hg}` with `ExternalLoader`
     *  will be delegated to this overload instead of using the bundled dependencies to
     *  parse the input graph stream.
     */
    template< typename TGraph, typename THGGraph, typename ...TArgs >
    inline void
    extend_hg( TGraph& graph, std::istream& in, ExternalLoader< THGGraph > loader, TArgs&&... args )
    {
      THGGraph other = loader( in );
      extend_graph( graph, other, HGFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_hg( TGraph& graph, std::string fname, TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_hg( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_hg( TGraph& graph, Dynamic, TArgs&&... args )
    {
      graph.clear();
      extend_hg( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_hg( TGraph& graph, Succinct, TArgs&&... args )
    {
      typename TGraph::dynamic_type dyn_graph;
      extend_hg( dyn_graph, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_hg( TGraph& graph, TArgs&&... args )
    {
      _load_hg( graph, typename TGraph::spec_type(), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, HGFormat, TArgs&&... args )
    {
      extend_hg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, HGFormat, TArgs&&... args )
    {
      extend_hg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::istream& in, HGFormat, TArgs&&... args )
    {
      load_hg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, HGFormat, TArgs&&... args )
    {
      load_hg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif // GUM_IO_UTILS_HG_HPP__
