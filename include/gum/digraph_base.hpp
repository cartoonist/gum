/**
 *    @file  digraph_base.hpp
 *   @brief  `DirectedGraph` base definitions
 *
 *  This header file includes definitions related to `DirectedGraph` class
 *  representing bi/directed graphs.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  18:55
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_BASE_HPP__
#define GUM_DIGRAPH_BASE_HPP__

#include "digraph_traits_base.hpp"


namespace gum {
  template< typename TSpec,
            typename TDir = Bidirected,
            typename TCoordSpec = void,
            uint8_t ...TWidths >
  class DirectedGraph;
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_BASE_HPP__ --- */
