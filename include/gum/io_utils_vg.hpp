/**
 *    @file  io_utils_vg.hpp
 *   @brief  SeqGraphs io-related template interface functions for loading vg graphs.
 *
 *  This header file includes interface function definitions specialised for vg data
 *  type when it comes with dealing with IO.
 *
 *  NOTE: Do not include this file directly; use `io_utils.hpp` or `vgio_utils.hpp`.
 *
 *  NOTE: The interface functions are template functions parameterised by vg data types
 *  (as template parameters). No specific data type from vg is explicitly used in this
 *  header file to prevent imposing unwanted dependencies especially when vg format is
 *  not used.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Apr 4, 2023  18:45
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_IO_UTILS_VG_HPP__
#define GUM_IO_UTILS_VG_HPP__

#include "vg_utils.hpp"


namespace gum {
  namespace util {
    /**
     *  @brief  Extend a native graph with an external one using an `ExternalLoader` (vg overload).
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
    template< typename TGraph,
              typename TVGGraph,
              typename=std::enable_if_t< std::is_same< typename TGraph::spec_type, Dynamic >::value >,
              typename ...TArgs >
    inline void
    extend_vg( TGraph& graph, std::istream& in, ExternalLoader< TVGGraph > loader, TArgs&&... args )
    {
      TVGGraph other = loader( in );
      extend_graph( graph, other, VGFormat{}, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend_vg( TGraph& graph, std::string fname, TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_vg( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_vg( TGraph& graph, Dynamic, TArgs&&... args )
    {
      graph.clear();
      extend_vg( graph, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    _load_vg( TGraph& graph, Succinct, TArgs&&... args )
    {
      typename TGraph::dynamic_type dyn_graph;
      extend_vg( dyn_graph, std::forward< TArgs >( args )... );
      graph = dyn_graph;
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load_vg( TGraph& graph, TArgs&&... args )
    {
      _load_vg( graph, typename TGraph::spec_type(), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::istream& in, VGFormat, TArgs&&... args )
    {
      extend_vg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    extend( TGraph& graph, std::string fname, VGFormat, TArgs&&... args )
    {
      extend_vg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::istream& in, VGFormat, TArgs&&... args )
    {
      load_vg( graph, in, std::forward< TArgs >( args )... );
    }

    template< typename TGraph, typename ...TArgs >
    inline void
    load( TGraph& graph, std::string fname, VGFormat, TArgs&&... args )
    {
      load_vg( graph, std::move( fname ), std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */


#endif // GUM_IO_UTILS_VG_HPP__
