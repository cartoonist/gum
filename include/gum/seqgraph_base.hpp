/**
 *    @file  seqgraph_base.hpp
 *   @brief  Sequence graph base definitions
 *
 *  This is a base header file for sequence graph class definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Feb 21, 2019  17:51
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_SEQGRAPH_BASE_HPP__
#define  GUM_SEQGRAPH_BASE_HPP__

#include "digraph_traits_base.hpp"
#include "node_prop_base.hpp"
#include "edge_prop_base.hpp"
#include "graph_prop_base.hpp"


namespace gum {
  template< typename TSpec,
            typename TCoordSpec = void,
            template< class, uint8_t ... > class TNodeProp = DefaultNodeProperty,
            template< class, class, uint8_t ... > class TEdgeProp = DefaultEdgeProperty,
            template< class, class, uint8_t ... > class TGraphProp = DefaultGraphProperty,
            uint8_t ...TWidths >
  class SeqGraph;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_BASE_HPP__ --- */
