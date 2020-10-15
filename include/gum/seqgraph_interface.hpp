/**
 *    @file  seqgraph_interface.hpp
 *   @brief  Sequence graph interface function definitions
 *
 *  All `SeqGraph` interface functions are defined here.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Oct 15, 2020  19:01
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2020, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_SEQGRAPH_INTERFACE_HPP__
#define  GUM_SEQGRAPH_INTERFACE_HPP__

#include <vector>
#include <algorithm>


namespace gum {
  namespace util {
    template< typename TGraph, typename TCallback >
    inline void
    for_each_start_node( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.indegree( id ) && !callback( rank, id ) ) return false;
            return true;
          } );
    }

    template< typename TGraph, typename TCallback >
    inline void
    for_each_end_node( TGraph const& graph, TCallback callback )
    {
      graph.for_each_node(
          [&]( auto rank, auto id ) {
            if ( !graph.outdegree( id ) && !callback( rank, id ) ) return false;
            return true;
          } );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_INTERFACE_HPP__ --- */
