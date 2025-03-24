/**
 *    @file  diseqgraph_succinct.hpp
 *   @brief  Definitions for Succinct `DiSeqGraph`
 *
 *  This header file includes definitions for Succinct specification of
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

#ifndef GUM_DISEQGRAPH_SUCCINCT_HPP__
#define GUM_DISEQGRAPH_SUCCINCT_HPP__

#include "diseqgraph_base.hpp"
#include "digraph_succinct.hpp"
#include "node_prop_succinct.hpp"
#include "edge_prop_succinct.hpp"
#include "graph_prop_succinct.hpp"


namespace gum {
  /**
   *  @brief  Directed sequence graph representation (succinct).
   *
   *  Represent a directed sequence graph (node-labeled directed graph).
   */
  template< typename TCoordSpec,
            uint8_t ...TWidths >
  class DiSeqGraph< Succinct, TCoordSpec, TWidths... >
    : public DirectedGraph< Succinct, Directed, TCoordSpec, TWidths... > {
  };
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DISEQGRAPH_SUCCINCT_HPP__ --- */
