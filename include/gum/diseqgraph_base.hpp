/**
 *    @file  diseqgraph_base.hpp
 *   @brief  Directed sequence graph base definitions
 *
 *  This is a base header file for Directed sequence graph class definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  20:32
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DISEQGRAPH_BASE_HPP__
#define GUM_DISEQGRAPH_BASE_HPP__

#include "digraph_traits_base.hpp"


namespace gum {
  template< typename TSpec,
            typename TCoordSpec = void,
            uint8_t ...TWidths >
  class DiSeqGraph;
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DISEQGRAPH_BASE_HPP__ --- */
