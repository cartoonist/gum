/**
 *    @file  diseqgraph_dynamic.hpp
 *   @brief  Definitions for Dynamic `DiSeqGraph`
 *
 *  This header file includes definitions for Dynamic specification of
 *  `DiSeqGraph` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  18:57
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DISEQGRAPH_DYNAMIC_HPP__
#define GUM_DISEQGRAPH_DYNAMIC_HPP__

#include "diseqgraph_base.hpp"
#include "digraph_dynamic.hpp"
#include "node_prop_dynamic.hpp"
#include "edge_prop_dynamic.hpp"
#include "graph_prop_dynamic.hpp"

namespace gum {
  /**
   *  @brief  Directed sequence graph representation (dynamic).
   *
   *  Represent a directed sequence graph (node-labeled directed graph).
   */
  template< typename TCoordSpec,
            uint8_t ...TWidths >
  class DiSeqGraph< Dynamic, TCoordSpec, TWidths... >
    : public DirectedGraph< Dynamic, Directed, TCoordSpec, TWidths... > {
  };
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DISEQGRAPH_DYNAMIC_HPP__ --- */
