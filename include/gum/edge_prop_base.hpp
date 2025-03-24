/**
 *    @file  edge_prop_base.hpp
 *   @brief  `EdgeProperty` base definitions
 *
 *  This header file includes basic definitions related to `EdgeProperty`
 *  class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  19:18
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_EDGE_PROP_BASE_HPP__
#define GUM_EDGE_PROP_BASE_HPP__

#include <cstdint>

#include "digraph_traits_base.hpp"


namespace gum {
  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class EdgePropertyTrait;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class EdgeProperty;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  using DefaultEdgeProperty = EdgeProperty< TSpec, TDir, TWidths... >;
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_EDGE_PROP_BASE_HPP__ --- */
