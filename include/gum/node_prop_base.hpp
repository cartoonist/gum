/**
 *    @file  node_prop_base.hpp
 *   @brief  `NodeProperty` base definitions
 *
 *  This header file includes basic definitions related to `NodeProperty`
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

#ifndef GUM_NODE_PROP_BASE_HPP__
#define GUM_NODE_PROP_BASE_HPP__

#include <cstdint>
#include <utility>

#include "graph_traits_base.hpp"


namespace gum {
  template< typename TSequence, typename TString >
  class Node {
  public:
    /* === TYPEDEFS === */
    using sequence_type = TSequence;
    using string_type = TString;
    /* === LIFECYCLE === */
    Node( sequence_type s="", string_type n="" )  /* constructor */
      : sequence( std::move( s ) ), name( std::move( n ) ) { }
    /* === DATA MEMBERS === */
    sequence_type sequence;
    string_type name;
  };  /* --- end of class Node --- */

  template< typename TSpec, uint8_t ...TWidths >
  class NodePropertyTrait;

  template< typename TSpec, uint8_t ...TWidths >
  class NodeProperty;

  template< typename TSpec, uint8_t ...TWidths >
  using DefaultNodeProperty = NodeProperty< TSpec, TWidths... >;
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_NODE_PROP_BASE_HPP__ --- */
