/**
 *    @file  graph_traits_dynamic.hpp
 *   @brief  Traits definitions for Dynamic graphs
 *
 *  This is a header file for Dynamic graph traits definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  16:07
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_GRAPH_TRAITS_DYNAMIC_HPP__
#define GUM_GRAPH_TRAITS_DYNAMIC_HPP__

#include <string>
#include <vector>

#include <parallel_hashmap/phmap.h>

#include "basic_types.hpp"
#include "graph_traits_base.hpp"


namespace gum{
  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
  class GraphBaseTrait< Dynamic, TIdWidth, TOffsetWidth > {
  public:
    using id_type = integer_t< TIdWidth >;
    using offset_type = uinteger_t< TOffsetWidth >;
    using common_type = common< TIdWidth, TOffsetWidth >;
    using value_type = typename common_type::type;
    using nodes_type = std::vector< id_type >;
    using size_type = typename nodes_type::size_type;
    using rank_type = typename nodes_type::size_type;
    using rank_map_type = phmap::flat_hash_map< id_type, rank_type >;
    using string_type = std::string;  // for node and path names

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
    }
  };  /* --- end of template class GraphBaseTrait --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GRAPH_TRAITS_DYNAMIC_HPP__ --- */
