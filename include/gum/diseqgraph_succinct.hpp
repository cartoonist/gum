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
